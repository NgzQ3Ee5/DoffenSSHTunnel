
#include "ATLog.h"
#include "ATMainWindow.h"
#include "ATSkeleton.h"
#include "CustomAction.h"
#include "PasswordDb.h"
#include <QApplication>
#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include "pch.h"

// For the about box
const char *APP_VERSION = "0.9.69";
const char *APP_NICE_NAME = "Doffen SSH Tunnel";

// For the registry
const char *SETTINGS_COMPANY = "doffen";
const char *SETTINGS_APP = "DoffenSSHTunnel";

const char *argv0=""; //set in main() below. The name of the exeuctable.

ATLog atlog;
QString g_strAppDirectoryPath;
QString g_strDataDirectoryPath;
QString g_strAppIniFile;
QString g_strTunnelIniFile;
QString g_strPwdFile;
bool g_bPwdFileEnabled;
QString g_strWindowTitle;

static void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type)
    {
    case QtDebugMsg:
        #ifdef QT_DEBUG
            atlog.addDebugEntry( msg );
            fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        #endif
        break;
    case QtWarningMsg:
        if(!msg.contains("known incorrect sRGB profile")) {
            atlog.addWarningEntry( msg );
        }
        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtCriticalMsg:
        atlog.addCriticalEntry( msg );
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtFatalMsg:
        atlog.addFatalEntry( msg );
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        abort(); // deliberately core dump
    default:
        fprintf(stderr, "Type %d: %s (%s:%u, %s)\n", type, localMsg.constData(), context.file, context.line, context.function);
/*
#ifdef WIN32
        {
            QString strDbg;
            strDbg.sprintf( "Type %d: %s (%s:%u, %s)\n", type, localMsg.constData(), context.file, context.line, context.function );
            OutputDebugStringA( strDbg.toUtf8().data() );
        }
#endif
*/
    }
}

int main(int argc, char *argv[])
{
    /*
    QApplication::setDesktopSettingsAware(true);
    QApplication::setEffectEnabled(Qt::UI_AnimateMenu,true);
    QApplication::setEffectEnabled(Qt::UI_FadeMenu,true);
    QApplication::setEffectEnabled(Qt::UI_AnimateCombo,true);
    QApplication::setEffectEnabled(Qt::UI_AnimateTooltip,true);
    QApplication::setEffectEnabled(Qt::UI_FadeTooltip,true);
    QApplication::setEffectEnabled(Qt::UI_AnimateToolBox,true);
    */

    qRegisterMetaType<CustomActionStruct>();
    qRegisterMetaType<VariableStatStruct>();
    qRegisterMetaType<VariableStruct>();
    qRegisterMetaType<VariableStatTunnelStruct>();
    qRegisterMetaType<PortForwardStruct>();

    QApplication app(argc, argv);
    // ATASSERT( app.connect(&app, &QApplication::lastWindowClosed, &app, &QApplication::quit) );
    app.setApplicationVersion(APP_VERSION);
    app.setApplicationName(SETTINGS_APP);
    app.setApplicationDisplayName(QString("%1 %2").arg(APP_NICE_NAME, APP_VERSION));

    g_strAppDirectoryPath = app.applicationDirPath();

    qInstallMessageHandler(myMessageOutput);

    QCommandLineParser commandLineParser;
    commandLineParser.setApplicationDescription(APP_NICE_NAME);
    commandLineParser.addHelpOption();
    commandLineParser.addVersionOption(); // adds the --version and -v options and handles their behavior automatically — including printing the version and exiting.
    // --data-dir
    QCommandLineOption datadirOption(QStringList() << "d" << "data-dir",
        QCoreApplication::translate("main",
            "Directory where application data is stored.\n"
            "Defaults to:\n"
            "-Windows:\n"
            "  directory of DoffenSSHTunnel.exe\n"
            "-macOS:\n"
            "  ~/Library/Application Support/DoffenSSHTunnel\n"
            "-Linux:\n"
            "  ~/.local/share/DoffenSSHTunnel"),
        QCoreApplication::translate("main", "directory"), "");
    commandLineParser.addOption(datadirOption);

    // --pwd-file
    QCommandLineOption passwordsOption(QStringList() << "p" << "pwd-file",
        QCoreApplication::translate("main",
            "Path to the password database file.\n"
            "Use 'disabled' to disable password db usage entirely."),
        QCoreApplication::translate("main", "file"), "");
    commandLineParser.addOption(passwordsOption);

    // --title
    QCommandLineOption windowTitleOption(QStringList() << "t" << "title",
        QCoreApplication::translate("main",
            "Set the main window title.\n"
            "Defaults to the name of the data-file."),
        QCoreApplication::translate("main", "title"), "");
    commandLineParser.addOption(windowTitleOption);

    // positional argument: data-file
    commandLineParser.addPositionalArgument("data-file",
        QCoreApplication::translate("main",
            "Optional INI file to read/write tunnel data.\n"
            "Defaults to \"DoffenSSHTunnel.ini\".\n"
            "If only a filename is given (no path), it will be stored inside the "
            "data-dir."),
        "[data-file]");

    commandLineParser.process(app);

    qDebug() << "-------------------------------------------------------------------------------------";
    qDebug() << "---------------------- DoffenSSHTunnel STARTED --------------------------------------";
    qDebug() << "-------------------------------------------------------------------------------------";

    argv0 = argv[0]; //Global - name of exeuctable
    qDebug() << "main() argv0 = " << argv0;

    g_strDataDirectoryPath = commandLineParser.value(datadirOption); //default empty string
    if(g_strDataDirectoryPath.isEmpty()) {
#ifdef Q_OS_WIN
            //DoffenSSHTunnel.exe's directory
            g_strDataDirectoryPath = QFileInfo("").absoluteFilePath();
#else
            QStringList locations = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation);
            if(locations.size() > 0) {
                //macOS: /Users/<username>/Library/Application Support/DoffenSSHTunnel.app
                //Linux: /home/<username>/.local/share/DoffenSSHTunnel-v0.9.34-x86_64.AppImage
                QString strLocation = locations.at(0);
                qDebug() << "main() strLocation = " << strLocation;
                if(strLocation.length() > 0) {
                    //macOS: /Users/<username>/Library/Application Support
                    //Linux: /home/<username>/.local/share/
                    QDir dirParent = QFileInfo(strLocation).dir();
                    //home/<username>/.local/share/DoffenSSHTunnel
                    g_strDataDirectoryPath = QFileInfo(dirParent, SETTINGS_APP).absoluteFilePath();
                }
            }
#endif

        //Create g_strDataDirectoryPath it does not already exist
        qDebug() << "main() g_strDataDirectoryPath = " << g_strDataDirectoryPath;
        QDir dirLocation = QDir(g_strDataDirectoryPath);
        if(!dirLocation.exists()) {
            QDir dirParent = QFileInfo(g_strDataDirectoryPath).dir();
            qDebug() << "main() dirParent = " << dirParent.absolutePath();
            if(dirParent.mkpath(dirLocation.dirName())) {
                qDebug() << "main() created data dir [" << dirLocation.absolutePath() << "]";
            } else {
                qCritical() << "main() could not create data dir [" << dirLocation.absolutePath() << "] dirParent.mkpath returned false";
            }
        }
    } //end - if(g_strDataDirectoryPath.isEmpty()) {
    else {
        if(!QFileInfo(g_strDataDirectoryPath).isDir()) {
            fprintf(stderr, "Directory not found: %s\n", g_strDataDirectoryPath.toLocal8Bit().constData());
            return 1;
        }
        g_strDataDirectoryPath = QFileInfo(g_strDataDirectoryPath).absoluteFilePath();
    }

    QString strLogFileName = QString("%1.log").arg(SETTINGS_APP);
    QString strLogFile = QFileInfo(g_strDataDirectoryPath, strLogFileName).absoluteFilePath();
    atlog.setOutputFile( strLogFile );
    atlog.clear();

    QString strTunnelIniFileName = QString("%1.ini").arg(SETTINGS_APP);
    g_strTunnelIniFile = QFileInfo(g_strDataDirectoryPath, strTunnelIniFileName).absoluteFilePath();
    QString strAppIniFileName = QString("%1App.ini").arg(SETTINGS_APP);
    g_strAppIniFile = QFileInfo(g_strDataDirectoryPath, strAppIniFileName).absoluteFilePath();
    QString strPwdFileName = QString("%1.pwd").arg(SETTINGS_APP);
    g_strPwdFile = QFileInfo(g_strDataDirectoryPath, strPwdFileName).absoluteFilePath();

    const QStringList args = commandLineParser.positionalArguments();
    if(args.size() > 0) {
        QString argTunnelIniFile = QString( "%1" ).arg( args.at(0) );
        if(!argTunnelIniFile.isEmpty()) {
            QFileInfo fileInfo(argTunnelIniFile);
            if (fileInfo.isAbsolute() || fileInfo.path() != ".") {
                g_strTunnelIniFile = QString( "%1" ).arg( args.at(0) );
            } else {
                g_strTunnelIniFile = QFileInfo(g_strDataDirectoryPath, argTunnelIniFile).absoluteFilePath();
            }
        }
    }

    g_bPwdFileEnabled = true;
    if(!commandLineParser.value(passwordsOption).isEmpty()) {
        g_strPwdFile = commandLineParser.value(passwordsOption);
        if(g_strPwdFile == "disabled") {
            g_bPwdFileEnabled = false;
        }
    }


    QString strWindowTitle = commandLineParser.value(windowTitleOption);
    if(!strWindowTitle.isEmpty()) {
        g_strWindowTitle = strWindowTitle;
    } else {
        g_strWindowTitle = QFileInfo(g_strTunnelIniFile).fileName();
    }

    qDebug() << "main() logFile = " << strLogFile;
    qDebug() << "main() g_strDataDirectoryPath = " << g_strDataDirectoryPath;
    qDebug() << "main() g_strTunnelIniFile = " << g_strTunnelIniFile;
    qDebug() << "main() g_strAppIniFile = " << g_strAppIniFile;
    qDebug() << "main() g_strPwdFile = " << g_strPwdFile;
    qDebug() << "main() g_strWindowTitle = " << g_strWindowTitle;

    ATMainWindow_c mainWindow;
    mainWindow.setObjectName("ATMainWindow_c");
    mainWindow.show();

    return app.exec();
}
