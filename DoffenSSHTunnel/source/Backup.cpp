#include "Backup.h"
#include "pch.h"
#include <QtCore/QSettings>
#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QDateTime>

BackupInfo::BackupInfo()
{
}

BackupInfo::BackupInfo(const BackupInfo &other)
{
	m_name = other.m_name;
	m_info = other.m_info;
	m_args = other.m_args;
}

BackupInfo::BackupInfo(const QString &name, const QString &info)
{
	m_name = name;
	m_info = info;
}

BackupInfo::BackupInfo(const QString &name, const QString &info, const QString &arg1)
{
	m_name = name;
	m_info = info;
	m_args << arg1;
}

BackupInfo::BackupInfo(const QString &name, const QString &info, const QString &arg1, const QString &arg2)
{
	m_name = name;
	m_info = info;
	m_args << arg1 << arg2;
}

BackupInfo::BackupInfo(const QString &name, const QString &info, const QString &arg1, const QString &arg2, const QString &arg3)
{
	m_name = name;
	m_info = info;
	m_args << arg1 << arg2 << arg3;
}

BackupInfo::~BackupInfo(void)
{
}

QString BackupInfo::getName() const
{
	return m_name;
}

QString BackupInfo::getInfo() const
{
	return m_info;
}

QList<QString> BackupInfo::getArgs() const
{
	return m_args;
}

//static
QString BackupInfo::formatDateTime(const QSettings& settings)
{
	return settings.value( "DateTime", "" ).toString();
}

//static
QString BackupInfo::formatInfo(const QSettings& settings)
{
	QString name = settings.value( "Name", "" ).toString();
	QString info = "";
	if(name == BACKUP_MANUAL) {
		info = settings.value( "Info", "" ).toString(); //user might have entered a description
	} else if(name == BACKUP_BEFORE_APPSTOP) {
		info = "Before writing settings at application shutdown";
	} else if(name == BACKUP_AFTER_APPSTOP) {
		info = "Application shutdown";
	} else if(name == BACKUP_BEFORE_APPSTART) {
		info = "Application start";
	} else if(name == BACKUP_AFTER_UPGRADE) {
		info = "Upgraded settings to new version [%1]";
	} else if(name == BACKUP_BEFORE_MOVEUP) {
		info = "Hosts tree: Before move host up [%1]"; //deprecated
	} else if(name == BACKUP_BEFORE_MOVE_FOLDER_UP) {
		info = "Hosts tree: Before move folder up [%1]";
	} else if(name == BACKUP_BEFORE_MOVE_TUNNEL_UP) {
		info = "Hosts tree: Before move host up [%1]";
	} else if(name == BACKUP_BEFORE_MOVEDOWN) {
		info = "Hosts tree: Before move host down [%1]"; //deprecated
	} else if(name == BACKUP_BEFORE_MOVE_FOLDER_DOWN) {
		info = "Hosts tree: Before move folder down [%1]";
	} else if(name == BACKUP_BEFORE_MOVE_TUNNEL_DOWN) {
		info = "Hosts tree: Before move host down [%1]";
	} else if(name == BACKUP_BEFORE_MOVELEFT) {
		info = "Hosts tree: Before move host left [%1]"; //deprecated
	} else if(name == BACKUP_BEFORE_MOVE_FOLDER_LEFT) {
		info = "Hosts tree: Before move folder left [%1]";
	} else if(name == BACKUP_BEFORE_MOVE_TUNNEL_LEFT) {
		info = "Hosts tree: Before move host left [%1]";
	} else if(name == BACKUP_BEFORE_MOVERIGHT) {
		info = "Hosts tree: Before move host right [%1]"; //deprecated
	} else if(name == BACKUP_BEFORE_MOVE_FOLDER_RIGHT) {
		info = "Hosts tree: Before move folder right [%1]";
	} else if(name == BACKUP_BEFORE_MOVE_TUNNEL_RIGHT) {
		info = "Hosts tree: Before move host right [%1]";
	} else if(name == BACKUP_BEFORE_DRAG_TUNNEL) {
		info = "Hosts tree: Before drag&drop host [%1]";
	} else if(name == BACKUP_BEFORE_DRAG_FOLDER) {
		info = "Hosts tree: Before drag&drop folder [%1]";
	} else if(name == BACKUP_BEFORE_EDITTUNNEL) {	
		info = "Before edit host settings/custom actions [%1]";
	} else if(name == BACKUP_BEFORE_RENAMETUNNEL) {
		info = "Hosts tree: Before rename host form [%1] to [%2]";
    } else if(name == BACKUP_BEFORE_EDITFOLDER) {
        info = "Before edit folder settings [%1]";
	} else if(name == BACKUP_BEFORE_RENAMEFOLDER) {
		info = "Hosts tree: Before rename folder from [%1] to [%2]";
	} else if(name == BACKUP_BEFORE_DUPLICATE) {
		info = "Hosts tree: Before duplicate [%1]";
	} else if(name == BACKUP_BEFORE_INSERTCHILDFOLDER) {
		info = "Hosts tree: Before insert child folder at [%1]";
	} else if(name == BACKUP_BEFORE_INSERTFOLDERAFTER) {
		info = "Hosts tree: Before insert folder after [%1]";
	} else if(name == BACKUP_BEFORE_INSERTFOLDERBEFORE) {
		info = "Hosts tree: Before insert folder before [%1]";
	} else if(name == BACKUP_BEFORE_INSERTCHILDTUNNEL) {
		info = "Hosts tree: Before insert child host at [%1]";
	} else if(name == BACKUP_BEFORE_INSERTTUNNELAFTER) {
		info = "Hosts tree: Before insert host after [%1]";
	} else if(name == BACKUP_BEFORE_INSERTTUNNELBEFORE) {
		info = "Hosts tree: Before insert host before [%1]";
	} else if(name == BACKUP_BEFORE_PASTECHILD) {
		info = "Hosts tree: Before paste child at [%1]";
	} else if(name == BACKUP_BEFORE_PASTEAFTER) {
		info = "Hosts tree: Before paste after [%1]";
	} else if(name == BACKUP_BEFORE_DELETE) {
		info = "Hosts tree: Before delete [%1]";
	} else if(name == BACKUP_BEFORE_CHANGE_VAR) {
		info = "Before edit variables";
	} else if(name == BACKUP_BEFORE_RESTORE) {
		info = "Before restore [%1]";
	} else if(name == BACKUP_AFTER_RESTORE) {
		info = "Restored [%1]";
	} else if(name == BACKUP_BEFORE_IMPORT_HOSTS) {
		info = "Hosts tree: Before import file [%1]";
	} else {
		info = settings.value( "Info", "Unkown" ).toString();
	}

	QString arg1 = settings.value( "Arg1", "" ).toString();
	QString arg2 = settings.value( "Arg2", "" ).toString();
	QString arg3 = settings.value( "Arg3", "" ).toString();

	if(info.contains("%1")) {
		info = info.arg(arg1);
	}
	if(info.contains("%2")) {
		info = info.arg(arg2);
	}
	if(info.contains("%3")) {
		info = info.arg(arg3);
	}

	return info;
}

//static
int BackupInfo::getBackupType(QString name)
{
	if(name == BACKUP_MANUAL)							return BACKUP_TYPE_MANUAL;
	else if(name == BACKUP_BEFORE_MOVEUP)				return BACKUP_TYPE_AUTOMATIC;
	else if(name == BACKUP_BEFORE_MOVE_TUNNEL_UP)		return BACKUP_TYPE_AUTOMATIC;
	else if(name == BACKUP_BEFORE_MOVE_FOLDER_UP)		return BACKUP_TYPE_AUTOMATIC;
	else if(name == BACKUP_BEFORE_MOVEDOWN)				return BACKUP_TYPE_AUTOMATIC;
	else if(name == BACKUP_BEFORE_MOVE_TUNNEL_DOWN)		return BACKUP_TYPE_AUTOMATIC;
	else if(name == BACKUP_BEFORE_MOVE_FOLDER_DOWN)		return BACKUP_TYPE_AUTOMATIC;
	else if(name == BACKUP_BEFORE_MOVELEFT)				return BACKUP_TYPE_AUTOMATIC;
	else if(name == BACKUP_BEFORE_MOVE_TUNNEL_LEFT)		return BACKUP_TYPE_AUTOMATIC;
	else if(name == BACKUP_BEFORE_MOVE_FOLDER_LEFT)		return BACKUP_TYPE_AUTOMATIC;
	else if(name == BACKUP_BEFORE_MOVERIGHT)			return BACKUP_TYPE_AUTOMATIC;
	else if(name == BACKUP_BEFORE_MOVE_TUNNEL_RIGHT)	return BACKUP_TYPE_AUTOMATIC;
	else if(name == BACKUP_BEFORE_MOVE_FOLDER_RIGHT)	return BACKUP_TYPE_AUTOMATIC;
	else if(name == BACKUP_BEFORE_DRAG_TUNNEL)			return BACKUP_TYPE_AUTOMATIC;
	else if(name == BACKUP_BEFORE_DRAG_FOLDER)			return BACKUP_TYPE_AUTOMATIC;
	else if(name == BACKUP_BEFORE_EDITTUNNEL)			return BACKUP_TYPE_AUTOMATIC;
	else if(name == BACKUP_BEFORE_RENAMETUNNEL)			return BACKUP_TYPE_AUTOMATIC;
	else if(name == BACKUP_BEFORE_EDITFOLDER)			return BACKUP_TYPE_AUTOMATIC;
	else if(name == BACKUP_BEFORE_RENAMEFOLDER)			return BACKUP_TYPE_AUTOMATIC;
	else if(name == BACKUP_BEFORE_DUPLICATE)			return BACKUP_TYPE_AUTOMATIC;
	else if(name == BACKUP_BEFORE_INSERTCHILDFOLDER)	return BACKUP_TYPE_AUTOMATIC;
	else if(name == BACKUP_BEFORE_INSERTFOLDERAFTER)	return BACKUP_TYPE_AUTOMATIC;
	else if(name == BACKUP_BEFORE_INSERTFOLDERBEFORE)	return BACKUP_TYPE_AUTOMATIC;
	else if(name == BACKUP_BEFORE_INSERTCHILDTUNNEL)	return BACKUP_TYPE_AUTOMATIC;
	else if(name == BACKUP_BEFORE_INSERTTUNNELAFTER)	return BACKUP_TYPE_AUTOMATIC;
	else if(name == BACKUP_BEFORE_INSERTTUNNELBEFORE)	return BACKUP_TYPE_AUTOMATIC;
	else if(name == BACKUP_BEFORE_PASTECHILD)			return BACKUP_TYPE_AUTOMATIC;
	else if(name == BACKUP_BEFORE_PASTEAFTER)			return BACKUP_TYPE_AUTOMATIC;
	else if(name == BACKUP_BEFORE_DELETE)				return BACKUP_TYPE_AUTOMATIC;
	else if(name == BACKUP_BEFORE_RESTORE)				return BACKUP_TYPE_AUTOMATIC;
	else if(name == BACKUP_BEFORE_CHANGE_VAR)			return BACKUP_TYPE_AUTOMATIC;
	else if(name == BACKUP_BEFORE_IMPORT_HOSTS)			return BACKUP_TYPE_AUTOMATIC;
	else												return BACKUP_TYPE_AUTOMATIC_SPECIAL;
}


Backup::Backup(void)
{
	m_backupDirName = DEFAULT_BACKUP_DIRNAME;
}

Backup::~Backup(void)
{
}

void Backup::setBackupDirName(QString name)
{
	m_backupDirName = name;
}

QString Backup::getBackupDirName()
{
	return m_backupDirName;
}

RestoreStatus Backup::canRestore(const QString& backupInfoFilePath)
{
    qDebug() << Q_FUNC_INFO << backupInfoFilePath;

	RestoreStatus status;
	status.bIsOK = false;

	//./DoffenSSHTunnelBackup/20120502_000011_816#bedittunnel#DoffenSSHTunnel.txt
	QFileInfo backupInfoFileInfo(backupInfoFilePath);
	if(!backupInfoFileInfo.exists()) {
		status.sMsg = QString("File not found [%1]").arg(backupInfoFilePath);
        qDebug() << Q_FUNC_INFO << status.sMsg;
		return status;
	}
	//./DoffenSSHTunnelBackup
	status.backupDirPath = backupInfoFileInfo.absolutePath();
	QDir backupDir(status.backupDirPath);

	QSettings settings( backupInfoFilePath, QSettings::IniFormat);

	//20120502_000011_816#bedittunnel#DoffenSSHTunnel.ini
	QString backupFileNameTunnels = settings.value("BackupFileName","").toString().trimmed();
	if(backupFileNameTunnels.isEmpty()) {
		status.sMsg = QString("Setting BackupFilename not found in %1").arg(backupInfoFilePath);
        qDebug() << Q_FUNC_INFO << status.sMsg;
		return status;
	}
	//./DoffenSSHTunnelBackup/20120502_000011_816#bedittunnel#DoffenSSHTunnel.ini
	QFileInfo backupFileTunnelsInfo(backupDir, backupFileNameTunnels);
	status.backupFileTunnelsPath = backupFileTunnelsInfo.absoluteFilePath();
	if(!backupFileTunnelsInfo.exists()) {
		status.sMsg = QString("File not found [%1]").arg(status.backupFileTunnelsPath);
        qDebug() << Q_FUNC_INFO << status.sMsg;
		return status;	
	}

	//20120502_000011_816#bedittunnel#DoffenSSHTunnelApp.ini
	QString appBackupFileName = settings.value("AppBackupFileName","").toString().trimmed();
	if(!appBackupFileName.isEmpty()) {
		QFileInfo appBackupFileInfo(backupDir,appBackupFileName);
		status.backupFileAppPath = appBackupFileInfo.absoluteFilePath();
		if(!appBackupFileInfo.exists()) {
			status.sMsg = QString("File not found [%1]").arg(status.backupFileAppPath);
            qDebug() << Q_FUNC_INFO << status.sMsg;
			return status;
		}
	}

	//./DoffenSSHTunnel.ini
	QFileInfo iniFileTunnelsInfo(g_strTunnelIniFile);
	status.iniFileTunnelsPath = iniFileTunnelsInfo.absoluteFilePath();

	//./DoffenSSHTunnelApp.ini
    QFileInfo iniFileAppInfo(g_strAppIniFile);
	status.iniFileAppPath = iniFileAppInfo.absoluteFilePath();

    status.bIsOK = true;
	return status;
}

RestoreStatus Backup::restore(const QString& backupInfoFilePath)
{
    qDebug() << Q_FUNC_INFO << backupInfoFilePath;

	RestoreStatus status = canRestore(backupInfoFilePath);
	if(!status.bIsOK) {
		return status;
	}

	status.bIsOK = false;

	//./DoffenSSHTunnelBackup
    QDir backupDir(status.backupDirPath);

	//BACKUP FILE: ./DoffenSSHTunnelBackup/20120502_000011_816#bedittunnel#DoffenSSHTunnel.ini
	QString backupFileTunnelsPath = status.backupFileTunnelsPath;
	// |
	//RESTORES TO: ./DoffenSSHTunnel.ini
	QString iniFileTunnelsPath = status.iniFileTunnelsPath;

	//BACKUP FILE: ./DoffenSSHTunnelBackup/20120502_000011_816#bedittunnel#DoffenSSHTunnelApp.ini
	QString backupFileAppPath = status.backupFileAppPath;
	// |
	//RESTORES TO: ./DoffenSSHTunnel.ini
	QString iniFileAppPath = status.iniFileAppPath;

	//RESTORE ./DoffenSSHTunnelBackup/20120502_000011_816#bedittunnel#DoffenSSHTunnel.ini
	//  -> ./DoffenSSHTunnel.ini
	if(!restoreFile(backupFileTunnelsPath, iniFileTunnelsPath, status)) {
		return status;
	}

	//RESTORE ./DoffenSSHTunnelBackup/20120502_000011_816#bedittunnel#DoffenSSHTunnelApp.ini
	//  -> ./DoffenSSHTunnelApp.ini
	if(!backupFileAppPath.isEmpty()) {
		if(!restoreFile(backupFileAppPath, iniFileAppPath, status)) {
			return status;
		}
	}

	status.bIsOK = true;
	return status;
}

//private
bool Backup::restoreFile(QString fromPath, QString toPath, RestoreStatus &status)
{
	//DELETE toPath
	if(QFile::exists(toPath)) {
		if(QFile::remove(toPath)) {
            qDebug() << Q_FUNC_INFO << QString("Deleted current file [%1]").arg(toPath);
		} else {
			status.bIsOK = false;
			status.sMsg = QString("File not found [%1]").arg(toPath);
            qDebug() << Q_FUNC_INFO << status.sMsg;
			return false;
		}
	} else {
        qDebug() << Q_FUNC_INFO << QString("Current file not found [%1]").arg(toPath);
	}	

	//COPY fromPath -> toPath
	if(QFile::copy(fromPath,toPath)) {
        qDebug() << Q_FUNC_INFO << QString("Copied file [%1] to [%2]").arg(fromPath).arg(toPath);
	} else {
		status.bIsOK = false;
		status.sMsg = QString("QFile::copy() returned false [%1] -> [%2]").arg(fromPath).arg(toPath);
        qDebug() << Q_FUNC_INFO << status.sMsg;
		return false;
	}

	return true;
}

BackupStatus Backup::backup(BackupInfo backupInfoData)
{
    qDebug( "backup() g_strTunnelIniFile: %s", qPrintable( g_strTunnelIniFile ) );
    qDebug( "backup() g_strAppIniFile: %s", qPrintable( g_strAppIniFile ) );

	BackupStatus status;
	status.bIsOK = false;

	QString backupName = backupInfoData.getName();
	QString backupInfo = backupInfoData.getInfo();
	QList<QString> backupArgs = backupInfoData.getArgs();

	//DoffenSSHTunnel.ini
	QFileInfo iniFileInfo(g_strTunnelIniFile);
	QString iniDirPath = iniFileInfo.absolutePath();			
	QString iniFilePath = iniFileInfo.absoluteFilePath();
	QDir iniDir(iniDirPath);

	//DoffenSSHTunnelApp.ini
	QFileInfo iniAppFileInfo(g_strAppIniFile);
    QString iniAppFilePath = iniAppFileInfo.absoluteFilePath();
	QFile iniAppFile(iniAppFilePath);	

	if(!iniAppFile.exists()) {
		status.sMsg = "File not found. Ignore it. [" + iniAppFilePath + "]";
        qDebug() << Q_FUNC_INFO << status.sMsg;
	}
	
	//./DoffenSSHTunnelBackup
	QFileInfo backupDirInfo(iniDirPath,m_backupDirName);
	QString backupDirPath = backupDirInfo.absoluteFilePath();			
	QDir backupDir(backupDirPath);


	QDateTime now = QDateTime::currentDateTime();

	//20120502_000011_816#bedittunnel#DoffenSSHTunnel.ini
	QString backupFileName = now.toString("yyyyMMdd_hhmmss_zzz") + "#" + backupName + "#" + iniFileInfo.baseName() + ".ini";
	//./DoffenSSHTunnelBackup/20120502_000011_816#bedittunnel#DoffenSSHTunnel.ini
	QFileInfo backupFileInfo(backupDir, backupFileName);
	QString backupFilePath = backupFileInfo.absoluteFilePath();
	QFile backupFile(backupFilePath);

	//20120502_000011_816#bedittunnel#DoffenSSHTunnelApp.ini
	QString backupAppFileName = now.toString("yyyyMMdd_hhmmss_zzz") + "#" + backupName + "#" + iniAppFileInfo.baseName() + ".ini";
	//./DoffenSSHTunnelBackup/20120502_000011_816#bedittunnel#DoffenSSHTunnelApp.ini
	QFileInfo backupAppFileInfo(backupDir, backupAppFileName);
	QString backupAppFilePath = backupAppFileInfo.absoluteFilePath();
	QFile backupAppFile(backupAppFilePath);

	//20120502_000011_816#bedittunnel#DoffenSSHTunnel.txt
	QString backupInfoFileName = backupFileInfo.baseName() + ".txt";
	//./DoffenSSHTunnelBackup/20120502_000011_816#bedittunnel#DoffenSSHTunnel.txt
	QFileInfo backupInfoFileInfo(backupDir, backupInfoFileName);
	QString backupInfoFilePath = backupInfoFileInfo.absoluteFilePath();
	QFile backupInfoFile(backupInfoFilePath);

	//CREATE ./DoffenSSHTunnelBackup
	if(!backupDir.exists()) {
        if(iniDir.mkdir(m_backupDirName)) {
			qDebug() << "backupTunnelSettings() created backup dir [" + backupDirPath + "]";
		} else {
			status.sMsg = "backupDir.mkdir() returned false [" + backupDirPath + "]";
            qDebug() << Q_FUNC_INFO << status.sMsg;
			return status;
		}
	}

	//CHECK ./DoffenSSHTunnelBackup/20120502_000011_816#bedittunnel#DoffenSSHTunnel.txt
	QSettings infoIni(backupInfoFilePath, QSettings::IniFormat);
	if(!infoIni.isWritable()) {
		status.sMsg = "Backup directory is not writable [" + backupDirPath + "]";
        qDebug() << Q_FUNC_INFO << status.sMsg;
		return status;
	}
	
	//REMOVE ./DoffenSSHTunnelBackup/20120502_000011_816#bedittunnel#DoffenSSHTunnel.ini
	if(backupFile.exists()) {
		if(backupFile.remove()) {
			qDebug() << "backupTunnelSettings() deleted backup file [" + backupFilePath + "]";
		} else {
			status.sMsg = "backupTunnelSettings() backupFile.remove() returned false [" + backupFilePath + "]";
            qDebug() << Q_FUNC_INFO << status.sMsg;
			return status;
		}
	}

	//REMOVE ./DoffenSSHTunnelBackup/20120502_000011_816#bedittunnel#DoffenSSHTunnelApp.ini
	if(backupAppFile.exists()) {
		if(backupAppFile.remove()) {
			qDebug() << "backupTunnelSettings() deleted backup file [" + backupAppFilePath + "]";
		} else {
			status.sMsg = "backupTunnelSettings() backupAppFile.remove() returned false [" + backupAppFilePath + "]";
            qDebug() << Q_FUNC_INFO << status.sMsg;
			return status;
		}
	}

	//REMOVE ./DoffenSSHTunnelBackup/20120502_000011_816#bedittunnel#DoffenSSHTunnel.txt
	if(backupInfoFile.exists()) {
		if(backupInfoFile.remove()) {
			qDebug() << "backupTunnelSettings() deleted backup info file [" + backupInfoFilePath + "]";
		} else {
			status.sMsg = "backupTunnelSettings() backupFileInfo.remove() returned false [" + backupInfoFilePath + "]";
            qDebug() << Q_FUNC_INFO << status.sMsg;
			return status;
		}
	}

	//COPY DoffenSSHTunnel.ini -> ./DoffenSSHTunnelBackup/20120502_000011_816#bedittunnel#DoffenSSHTunnel.ini
	if(QFile::copy(iniFilePath, backupFilePath)) {
		qDebug() << "backupTunnelSettings() copied tunnels ini file [" + iniFilePath + "] -> [" + backupFilePath + "]";
	} else {
		status.sMsg = "backupTunnelSettings() QFile::copy returned false [" + iniFilePath + "] -> [" + backupFilePath + "]";
        qDebug() << Q_FUNC_INFO << status.sMsg;
		return status;
	}

	//COPY DoffenSSHTunnelApp.ini -> ./DoffenSSHTunnelBackup/20120502_000011_816#bedittunnel#DoffenSSHTunnelApp.ini
	if(QFile::copy(iniAppFilePath, backupAppFilePath)) {
		qDebug() << "backupTunnelSettings() copied app ini file [" + iniAppFilePath + "] -> [" + backupAppFilePath + "]";
	} else {
		status.sMsg = "backupTunnelSettings() QFile::copy returned false [" + iniAppFilePath + "] -> [" + backupAppFilePath + "]";
        qDebug() << Q_FUNC_INFO << status.sMsg;
		return status;
	}

	//CREATE ./DoffenSSHTunnelBackup/20120502_000011_816#bedittunnel#DoffenSSHTunnel.txt
	infoIni.setValue( "Version", APP_VERSION );
	infoIni.setValue( "DateTime", now.toString("yyyy-MM-dd hh:mm:ss zzz") );
	infoIni.setValue( "BackupFileDir", backupDirPath );
	infoIni.setValue( "BackupFileName", backupFileName );
	infoIni.setValue( "AppBackupFileName", backupAppFileName );
	infoIni.setValue( "Name", backupName );
	infoIni.setValue( "Info", backupInfo );
	for(int i=0;i<backupArgs.size();i++) {
		infoIni.setValue( QString("Arg%1").arg(i+1), backupArgs.at(i) );
	}
	infoIni.sync();
	qDebug() << "backupTunnelSettings() wrote info ["+backupInfo+"] to file [" + backupInfoFilePath + "]";

	status.bIsOK = true;
	return status;
}
