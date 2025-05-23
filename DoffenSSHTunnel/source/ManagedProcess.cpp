// ManagedProcess.cpp
#include "ManagedProcess.h"
#include <QDebug>

#ifdef Q_OS_WIN
#include <windows.h>
#elif defined(Q_OS_UNIX)
#include <signal.h>
#include <unistd.h>
#endif

ManagedProcess::ManagedProcess(QObject* parent)
    : QProcess(parent)
{
#ifdef Q_OS_WIN
    setupJobObject();
#endif
}

ManagedProcess::~ManagedProcess()
{
    try {
        killProcess();
    } catch (...) {
        // Never throw in destructor
    }
#ifdef Q_OS_WIN
    if (jobHandle != nullptr) {
        CloseHandle(jobHandle);
        jobHandle = nullptr;
    }
#endif
}

bool ManagedProcess::startWithJob(const QString& program, const QStringList& arguments)
{
#ifdef Q_OS_UNIX
    this->setChildProcessModifier([] {
        ::setpgid(0, 0); // Start in a new process group
    });
#endif

    this->start(program, arguments);
    if (!this->waitForStarted()) {
        throw ManagedProcessException("Failed to start process: " + program.toStdString());
    }

#ifdef Q_OS_WIN
    attachToJobObject();
#endif
    return true;
}

bool ManagedProcess::waitForCleanExit(int timeoutMs)
{
    return this->state() == NotRunning || this->waitForFinished(timeoutMs);
}

void ManagedProcess::killProcess()
{
#ifdef Q_OS_WIN
    if (jobAssigned && jobHandle != nullptr) {
        CloseHandle(jobHandle);
        jobHandle = nullptr;
        jobAssigned = false;
        return;
    }
    qint64 pid = this->processId();
    if (pid > 0) {
        int result = QProcess::execute("taskkill", {"/pid", QString::number(pid), "/t", "/f"});
        if (result != 0) {
            throw ManagedProcessException("taskkill failed for PID " + std::to_string(pid));
        }
    } else {
        this->kill();
    }
#elif defined(Q_OS_UNIX)
    if (this->processId() > 0) {
        if (::kill(-this->processId(), SIGKILL) != 0) {
            throw ManagedProcessException("Failed to kill process group");
        }
    } else {
        this->kill();
    }
#else
    this->kill();
#endif
}


#ifdef Q_OS_WIN
void ManagedProcess::setupJobObject()
{
    jobHandle = CreateJobObject(nullptr, nullptr);
    if (jobHandle == nullptr) {
        throw ManagedProcessException("CreateJobObject failed with code " + std::to_string(GetLastError()));
    }

    JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = {};
    jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;

    if (!SetInformationJobObject(jobHandle, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli))) {
        CloseHandle(jobHandle);
        jobHandle = nullptr;
        throw ManagedProcessException("SetInformationJobObject failed with code " + std::to_string(GetLastError()));
    }
}

void ManagedProcess::attachToJobObject()
{
    if (!jobHandle || this->processId() <= 0) {
        throw ManagedProcessException("Invalid job handle or process ID");
    }

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, this->processId());
    if (hProcess == nullptr) {
        throw ManagedProcessException("OpenProcess failed with code " + std::to_string(GetLastError()));
    }

    if (!AssignProcessToJobObject(jobHandle, hProcess)) {
        CloseHandle(hProcess);
        throw ManagedProcessException("AssignProcessToJobObject failed with code " + std::to_string(GetLastError()));
    }

    jobAssigned = true;
    CloseHandle(hProcess);
}
#endif
