#pragma once

#include <QProcess>
#include <stdexcept>
#include <windows.h>

class ManagedProcessException : public std::runtime_error {
public:
    explicit ManagedProcessException(const std::string& message)
        : std::runtime_error(message) {}
};

class ManagedProcess : public QProcess {
    Q_OBJECT

public:
    explicit ManagedProcess(QObject* parent = nullptr);
    ~ManagedProcess();

    // Throws ManagedProcessException on failure
    bool startWithJob(const QString& program, const QStringList& arguments);
    bool waitForCleanExit(int timeoutMs);
    void killProcess();  // Throws ManagedProcessException on failure

private:
#ifdef Q_OS_WIN
    void setupJobObject();              // Throws on error
    void attachToJobObject();          // Throws on error
    HANDLE jobHandle = nullptr;
    bool jobAssigned = false;
#endif
};
