#ifndef __BACKUP_H__
#define __BACKUP_H__

#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QtGlobal>
#include <QtCore/QMetaType>
class QSettings;

#define BACKUP_MANUAL ("manual")
#define BACKUP_BEFORE_APPSTOP ("bappstop")
#define BACKUP_AFTER_APPSTOP ("appstop")
#define BACKUP_BEFORE_APPSTART ("appstart")
#define BACKUP_AFTER_UPGRADE ("aupgrade")
#define BACKUP_BEFORE_MOVEUP ("bmoveup") //deprecated
#define BACKUP_BEFORE_MOVE_TUNNEL_UP ("bmovetunnelup")
#define BACKUP_BEFORE_MOVE_FOLDER_UP ("bmovefolderup")
#define BACKUP_BEFORE_MOVEDOWN ("bmovedown") //deprecated
#define BACKUP_BEFORE_MOVE_TUNNEL_DOWN ("bmovetunneldown")
#define BACKUP_BEFORE_MOVE_FOLDER_DOWN ("bmovefolderdown")
#define BACKUP_BEFORE_MOVELEFT ("bmoveleft") //deprecated
#define BACKUP_BEFORE_MOVE_TUNNEL_LEFT ("bmovetunnelleft")
#define BACKUP_BEFORE_MOVE_FOLDER_LEFT ("bmovefolderleft")
#define BACKUP_BEFORE_MOVERIGHT ("bmoveright") //deprecated
#define BACKUP_BEFORE_MOVE_TUNNEL_RIGHT ("bmovetunnelright")
#define BACKUP_BEFORE_MOVE_FOLDER_RIGHT ("bmovefolderright")
#define BACKUP_BEFORE_DRAG_TUNNEL ("bdragtunnel")
#define BACKUP_BEFORE_DRAG_FOLDER ("bdragfolder")
#define BACKUP_BEFORE_EDITTUNNEL ("bedittunnel")
#define BACKUP_BEFORE_RENAMETUNNEL ("brenametunnel")
#define BACKUP_BEFORE_EDITFOLDER ("beditfolder")
#define BACKUP_BEFORE_RENAMEFOLDER ("brenamefolder")
#define BACKUP_BEFORE_DUPLICATE ("bduplicate")
#define BACKUP_BEFORE_INSERTCHILDFOLDER ("bchildfolder")
#define BACKUP_BEFORE_INSERTFOLDERAFTER ("bfolderafter")
#define BACKUP_BEFORE_INSERTFOLDERBEFORE ("bfolderbefore")
#define BACKUP_BEFORE_INSERTCHILDTUNNEL ("bchildtunnel")
#define BACKUP_BEFORE_INSERTTUNNELAFTER ("btunnelafter")
#define BACKUP_BEFORE_INSERTTUNNELBEFORE ("btunnelbefore")
#define BACKUP_BEFORE_PASTECHILD ("bpastechild")
#define BACKUP_BEFORE_PASTEAFTER ("bpasteafter")
#define BACKUP_BEFORE_DELETE ("bdelete")
#define BACKUP_BEFORE_RESTORE ("brestore")
#define BACKUP_AFTER_RESTORE ("restore")
#define BACKUP_BEFORE_CHANGE_VAR ("bchangevar")
#define BACKUP_BEFORE_IMPORT_HOSTS ("bimporthosts")

#define DEFAULT_BACKUP_DIRNAME ("DoffenSSHTunnelBackup")

#define BACKUP_TYPE_MANUAL (1)				//00000001
#define BACKUP_TYPE_AUTOMATIC (2)			//00000010
#define BACKUP_TYPE_AUTOMATIC_SPECIAL (4)	//00000100

struct BackupStatus
{
	bool bIsOK;
	QString sMsg;
};

struct RestoreStatus
{
	bool bIsOK;
	QString sMsg;
	QString backupDirPath;
	QString backupFileTunnelsPath;
	QString backupFileAppPath;
	QString iniFileTunnelsPath;
	QString iniFileAppPath;
};

class BackupInfo
{
public:
	BackupInfo();
	BackupInfo(const BackupInfo &other);
	BackupInfo(const QString &name, const QString &info);
	BackupInfo(const QString &name, const QString &info, const QString &arg1);
	BackupInfo(const QString &name, const QString &info, const QString &arg1, const QString &arg2);
	BackupInfo(const QString &name, const QString &info, const QString &arg1, const QString &arg2, const QString &arg3);
	~BackupInfo(void);

	QString getName() const;
	QString getInfo() const;
	QList<QString> getArgs() const;

	static int getBackupType(QString name);
	static QString formatDateTime(const QSettings& settings);
	static QString formatInfo(const QSettings& settings);

private:
	QString m_name;
	QString m_info;
	QList<QString> m_args;
};

Q_DECLARE_METATYPE(BackupInfo)

class Backup
{
public:

public:
	Backup(void);
	~Backup(void);

	BackupStatus backup(BackupInfo backupInfo);
	RestoreStatus canRestore(const QString& backupInfoFilePath);
	RestoreStatus restore(const QString& backupInfoFilePath);
	void setBackupDirName(QString name);
	QString getBackupDirName();

private:
	bool restoreFile(QString fromPath, QString toPath, RestoreStatus &status);

private:
	QString m_backupDirName;

};

#endif
