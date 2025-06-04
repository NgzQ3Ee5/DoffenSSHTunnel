#ifndef ATSKELETONWINDOW_H
#define ATSKELETONWINDOW_H

#include "CustomAction.h"
#include "ManagedProcess.h"
#include "ui_ATSkeleton.h"

class CustomActionsEditWidget;
class ATMainWindow_c;
class ATSkeletonWindow;
class Tunnel_c;
class ExportHostsDialog;
class ImportHostsDialog;
class BackupInfo;

#define TUNNEL_TYPE_TUNNEL	(0)
#define TUNNEL_TYPE_TUNNEL_SSH (1)
#define TUNNEL_TYPE_TUNNEL_SSM (2)
#define TUNNEL_TYPE_FOLDER	(5)

#define IMPORT_AFTER	(0)
#define IMPORT_CHILD	(1)


struct VariableStatTunnelStruct
{
    enum {FIELD_SSH_HOST, FIELD_USER_NAME, FIELD_PASSWORD, FIELD_SSH_KEY_FILE,
        FIELD_KEY_PASSWORD, FIELD_DESTINATION_HOST, FIELD_EXTRA_ARGUMENTS, FIELD_SSM_REGION,FIELD_SSM_PROFILE, FIELD_CHILDNODES_COMMAND};
	QUuid   uUidVariable;	//points to the VariableStruct. Below we have the overview of where that variable are used.
	QList<int> listFields;
	QList<CustomActionStruct> listCustomActions;
	VariableStatTunnelStruct(QUuid uuidVariable=QUuid())
	{
		uUidVariable = uuidVariable;
	}
	int totalUsageCount() { return listFields.size() + listCustomActions.size(); }
};

Q_DECLARE_METATYPE(VariableStatTunnelStruct)

/*
	NOTE! The streaming operators gets declared in main.cpp main() method like this:
	qRegisterMetaTypeStreamOperators<VariableStatTunnelStruct>("VariableStatTunnelStruct");
	qRegisterMetaType<VariableStatTunnelStruct>("VariableStatTunnelStruct");
*/

inline QDataStream &operator>>(QDataStream &s, VariableStatTunnelStruct &p)
{
	QUuid uUidVariable;
	QList<int> listFields;
	QList<CustomActionStruct> listCustomActions;
	s >> uUidVariable >> listFields >> listCustomActions;
	p = VariableStatTunnelStruct(uUidVariable);
	p.listFields = listFields;
	p.listCustomActions = listCustomActions;
	return s;
}

inline QDataStream &operator<<(QDataStream &s, const VariableStatTunnelStruct &p)
{
	s << p.uUidVariable << p.listFields << p.listCustomActions;
	return s;
}

struct VariableStatStruct
{
	QUuid   uUidVariable;	//points to the VariableStruct. Below we have the overview of where this variable are used.
	QList<QUuid> listExecVarables;	//List of uuid of variables in QList<VariableStruct> m_listExecutableVariables
	QList<QUuid> listUserDefVarables; //List of uuid of variables in QList<VariableStruct> m_listUserDefinedVariables
	QList<VariableStatTunnelStruct> listTunnelStats; //List of tunnels that use the variable (found in ui.treeTunnels)
	VariableStatStruct(QUuid uuidVariable=QUuid())
	{
		uUidVariable = uuidVariable;
	}
	int totalUsageCount() { 
		int totalUsageCount = 0;
		totalUsageCount = totalUsageCount + listExecVarables.size();
		totalUsageCount = totalUsageCount + listUserDefVarables.size();
		for(int i=0;i<listTunnelStats.size();i++) {
			VariableStatTunnelStruct varStatTunnel = listTunnelStats.at(i);
			totalUsageCount = totalUsageCount + varStatTunnel.totalUsageCount();
		}
		return totalUsageCount;
	};
};

Q_DECLARE_METATYPE(VariableStatStruct)

/*
	NOTE! The streaming operators gets declared in main.cpp main() method like this:
	qRegisterMetaTypeStreamOperators<VariableStatStruct>("VariableStatStruct");
	qRegisterMetaType<VariableStatStruct>("VariableStatStruct");
*/

inline QDataStream &operator>>(QDataStream &s, VariableStatStruct &p)
{
	QUuid uUidVariable;
	QList<QUuid> listExecVarables;
	QList<QUuid> listUserDefVarables;
	QList<VariableStatTunnelStruct> listTunnelStats;
	s >> uUidVariable >> listExecVarables >> listUserDefVarables >> listTunnelStats;
	p = VariableStatStruct(uUidVariable);
	p.listExecVarables = listExecVarables;
	p.listUserDefVarables = listUserDefVarables;
	p.listTunnelStats = listTunnelStats;
	return s;
}

inline QDataStream &operator<<(QDataStream &s, const VariableStatStruct &p)
{
	s << p.uUidVariable << p.listExecVarables << p.listUserDefVarables << p.listTunnelStats;
	return s;
}

struct VariableStruct
{
    enum {NONE,USERDEFINED,EDITPANE,PASSWORDS,EXECUTABLE};
	QUuid   uUid;			//Unique identifier {626b2afd-0b57-40f6-83a4-68ea6344055b}		
	int nType;				//NONE,USERDEFINED,EDITPANE,PASSWORDS,EXECUTABLE
	QString strDescription; //Firefox
	QString strName;		//firefox
	QString strValue;		//C:\Program Files\Mozilla Firefox\firefox.exe
    QString strArgs;		//Commandline args
    VariableStruct(QUuid uuid=QUuid(), QString descr="", QString name="", QString value="", QString args="", int type=NONE )
	{
		uUid = uuid;
		nType = type;
		strDescription = descr;
		strName = name;
		strValue = value;
        strArgs = args;
	}
	bool operator==(const VariableStruct &other) const 
	{
		if(nType != other.nType) { return false; }
		if(strDescription != other.strDescription) { return false; }
		if(strName != other.strName) { return false; }
		if(strValue != other.strValue) { return false; }
        if(strArgs != other.strArgs) { return false; }
		return true;
    }
	bool operator!=(const VariableStruct &other) const 
	{
		return !(*this == other);
    }
	QString toString() {
        return QString("type=[%1] name=[%2] value=[%3] value=[%4] description=[%5] uuid=[%6]").arg(nType).arg(strName).arg(strValue).arg(strArgs).arg(strDescription).arg(uUid.toString());
    }
    static bool stdComparatorNameReverse(const VariableStruct& first, const VariableStruct& second) {
        if (first.strName.toLower() > second.strName.toLower()) {
            return true;
        }
        return false;
    }
};

Q_DECLARE_METATYPE(VariableStruct)

/*
	NOTE! The streaming operators gets declared in main.cpp main() method like this:
	qRegisterMetaTypeStreamOperators<VariableStruct>("VariableStruct");
	qRegisterMetaType<CustomActionStruct>("VariableStruct");
*/

inline QDataStream &operator>>(QDataStream &s, VariableStruct &p)
{
	quint32 nType;
	QString strDescription;
	QString strName;
	QString strValue;
    QString strArgs;
	QUuid uUid;
    s >> uUid >> nType >> strDescription >> strName >> strValue >> strArgs;
    p = VariableStruct(uUid, strDescription,strName,strValue,strArgs,nType);
	return s;
}

inline QDataStream &operator<<(QDataStream &s, const VariableStruct &p)
{
    s << p.uUid << quint32(p.nType) << p.strDescription << p.strName << p.strValue << p.strArgs;
	return s;
}


struct PortForwardStruct {
    enum {LOCAL,REMOTE,DYNAMIC};
	QUuid   uUid;			//Unique identifier {626b2afd-0b57-40f6-83a4-68ea6344055b}		
	bool bEnabled;
	int nType;				//LOCAL,REMOTE,DYNAMIC
	QString strName;
	QString strLocalIP;
	int nLocalPort;
	QString strDestinationHost;
	int nDestinationPort;
	QString strDescription;
	PortForwardStruct(QUuid uuid=QUuid(), bool enabled=true, int type=LOCAL, QString name="", QString lIP="127.0.0.1", int lPort=0, QString dHost="", int dPort=0, QString descr=""  )
	{
		uUid = uuid;
		bEnabled = enabled;
		nType = type;
		strName = name;
		strLocalIP = lIP;
		nLocalPort = lPort;
		strDestinationHost = dHost;
		nDestinationPort = dPort;
		strDescription = descr;
	}
	bool operator==(const PortForwardStruct &other) const 
	{
		if(bEnabled != other.bEnabled) { return false; }
		if(nType != other.nType) { return false; }
		if(strName != other.strName) { return false; }
		if(strLocalIP != other.strLocalIP) { return false; }
        //LocalPort might be auto assigned so cannot compare this
        //if(nLocalPort != other.nLocalPort) { return false; }
		if(strDestinationHost != other.strDestinationHost) { return false; }
		if(nDestinationPort != other.nDestinationPort) { return false; }
		if(strDescription != other.strDescription) { return false; }
		return true;
	};
	bool operator!=(const PortForwardStruct &other) const 
	{
		return !(*this == other);
	};
	QString toString() {
		return QString("enabled=[%1] type=[%2] name=[%3] lip=[%4] lport=[%5] dhost=[%6] dport=[%7] descr=[%8] uuid=[%9]").arg(bEnabled).arg(nType).arg(strName).arg(strLocalIP).arg(nLocalPort).arg(strDestinationHost).arg(nDestinationPort).arg(strDescription).arg(uUid.toString());
	};
};

Q_DECLARE_METATYPE(PortForwardStruct)

/*
	NOTE! The streaming operators gets declared in main.cpp main() method like this:
	qRegisterMetaTypeStreamOperators<PortForwardStruct>("PortForwardStruct");
	qRegisterMetaType<PortForwardStruct>("PortForwardStruct");
*/

inline QDataStream &operator>>(QDataStream &s, PortForwardStruct &p)
{
	QUuid uUid;
	bool bEnabled;
	quint32 nType;
	QString strName;
	QString strLocalIP;
	quint32 nLocalPort;
	QString strDestinationHost;
	quint32 nDestinationPort;
	QString strDescription;
	
	s >> uUid >> bEnabled >> nType >> strName >> strLocalIP >> nLocalPort >> strDestinationHost >> nDestinationPort >> strDescription;
	p = PortForwardStruct(uUid, bEnabled, nType, strName, strLocalIP, nLocalPort, strDestinationHost, nDestinationPort, strDescription);
	return s;
}

inline QDataStream &operator<<(QDataStream &s, const PortForwardStruct &p)
{
	s << p.uUid << p.bEnabled << quint32(p.nType) << p.strName << p.strLocalIP << quint32(p.nLocalPort) << p.strDestinationHost << quint32(p.nDestinationPort) << p.strDescription;
	return s;
}

class ATTunnelConnector_c : public QObject
{
	Q_OBJECT

public:
	ATTunnelConnector_c( ATSkeletonWindow *pParent, QTreeWidgetItem *pTreeTunnelsItem );

	ATSkeletonWindow *m_pParent;
	QTreeWidgetItem *m_pTreeTunnelsItem;

public slots:
	void slotProcessReadStandardOutput();
	void slotProcessReadStandardError();
    void slotProcessError(ManagedProcess::ProcessError error);
    void slotProcessFinished(int exitCode, ManagedProcess::ExitStatus exitStatus);

signals:
	void finished( Tunnel_c * );
	void signalConnected( QTreeWidgetItem* );
    void signalKillConnection( QTreeWidgetItem* );

protected:
    void processPlinkOutput( QString &str );
    void processSSMOutput( QString &str );
};

class ATPopulateChildNodesConnector_c : public QObject
{
    Q_OBJECT

public:
    ATPopulateChildNodesConnector_c( ATSkeletonWindow *pParent, QTreeWidgetItem *pTreeTunnelsItem );
    ~ATPopulateChildNodesConnector_c();
    QString getOutput() { return m_strOutput; }
    int getExitCode() { return m_nExitCode; }
    bool isCanceled() { return m_bCancelled; }

private:
    ATSkeletonWindow *m_pParent;
    QTreeWidgetItem *m_pTreeTunnelsItem;
    QString m_strOutput;
    int m_nExitCode;
    bool m_bCancelled;

public slots:
    void slotProcessReadStandardOutput();
    void slotProcessReadStandardError();
    void slotProcessError(ManagedProcess::ProcessError error);
    void slotProcessFinished(int exitCode, ManagedProcess::ExitStatus exitStatus);
    void slotCancel();

signals:
    void finished( Tunnel_c * );

protected:
    void processCommandOutput( const QByteArray &b );
};

class TunnelLog
{
public:
    TunnelLog();
    void append(const QString &str);
    int size();
    void trimToSize(int size);
    void clear();
    QString toHtml();
    static QString toHtml(const QString &str);
private:
    QString m_strLog;
};

class Tunnel_c: public QObject
{
    Q_OBJECT

public:
	Tunnel_c();
	Tunnel_c(const Tunnel_c* other);
	~Tunnel_c();
	// Use default copy contructor and assignment operator

	void init();
	void copyFrom( const Tunnel_c *orig );
    bool isConnectionDetailsEqual( const Tunnel_c *other );

	void setSSHHostList( const QString& sshHostList );
	QString getSSHHostList( );
	QString getSelectedSSHHost();
	void setSelectedSSHHost( const QString& sshHost );
	void setSelectedSSHHost( int sshHostListIndex );

	void setRemoteHostList( const QString& sshHostList );
	QString getRemoteHostList( );
	QString getSelectedRemoteHost();
	void setSelectedRemoteHost( const QString& sshHost );
	void setSelectedRemoteHost( int sshHostListIndex );

    void startTimerDelayRetryConnect(int intervalSeconds) {
        stopTimerDelayRetryConnect();
        m_timerDelayRetryConnect.setInterval(intervalSeconds*1000);
        m_timerDelayRetryConnect.start();
    }
    void stopTimerDelayRetryConnect() {
        m_timerDelayRetryConnect.stop();
    }
    bool isActiveTimerDelayRetryConnect() {
        return m_timerDelayRetryConnect.isActive();
    }

	//--------------------------------------------------
	// REMEMBER TO UPDATE ATSkeletonWindow::changeTunnelVarNames(QList<QPair<QString,QString>> & varList)
	// IF YOU ADD NEW VARIABLES HERE

	QUuid   uUid;
    QString strExtID; //External ID. I.e. Amazon EC2 instance ID
	QString strName;
    QString strDescription;
    int		iType;   //TUNNEL_TYPE_TUNNEL or TUNNEL_TYPE_FOLDER
    int		iType2;  //TUNNEL_TYPE_TUNNEL_SSH or TUNNEL_TYPE_TUNNEL_SSM
	QStringList strSSHHostList;
	int		iSSHHost;
	QString strLocalIP;
    int     iLocalPort;
	QStringList strRemoteHostList;
	int		iRemoteHost;
	int     iRemotePort;
	QString strUsername;
	QString strPassword;
	QList<CustomActionStruct> customActionList;
	QList<PortForwardStruct> portForwardList;
	int		iLevel;
	QString strSSHKeyFile;
	QString strSSHKeyFilePassword;
	int     iDirection;
	bool    bAutoConnect;
	bool    bDoKeepAlivePing;
	bool    bAutoReconnect;
	bool	bIsExpanded;	//expanded in the tree ?
	QString strExtraArguments;

    ManagedProcess *pProcess;
    TunnelLog log;
	ATTunnelConnector_c *pConnector;

    int iReconnectRetries;
    bool bShouldReconnect;
	int iPasswordRetries;
	int iKeyPasswordRetries;
	int iChallengeResponseRetries;
	int iConnectStatus;
	QTreeWidgetItem *twi;

    QString strChildNodesCommand;
    bool bChildNodesCommandEnabled;
    bool bFolderActivateChildNodesCommand;
    ManagedProcess *pPopulateChildNodesProcess;
    ATPopulateChildNodesConnector_c *pPopulateChildNodesConnector;
    bool bChildNodesCommandType; //returned by childnodes command (e.g nodejs script retrieved hosts from aws)

    QString strFgColor;
    QString strBgColor;

    bool bActivateDisconnects;

    QString strSSMRegion;
    QString strSSMProfile;

protected:
    QTimer m_timerDelayRetryConnect;

signals:
    void signalTimerDelayRetryConnect(Tunnel_c*);

private slots:
    void slotTimerDelayRetryConnectTimeout() {
        stopTimerDelayRetryConnect();
        emit signalTimerDelayRetryConnect(this);
    }
};
/*
to create a QVariant 
Tunnel_c* tunnel = new Tunnel_c();
QVariant var = qVariantFromValue(tunnel);
to get the value back 
Tunnel_c* tunnel = var.value<Tunnel_c*>();
*/
Q_DECLARE_METATYPE( Tunnel_c* ) //allows Tunnel_c to be used as a QVariant

class ATSkeletonWindow : public QWidget
{
    Q_OBJECT
	typedef std::list<Tunnel_c>::iterator TunnelInterator;

public:
	ATSkeletonWindow(QWidget *parent = 0);
	~ATSkeletonWindow();

	bool onClose();
    void AddToLog( Tunnel_c &tunnel, const QString &str );
    void setTunnelDataFromEditPane(Tunnel_c *pt);
	QString replaceVars( Tunnel_c &tunnel, QString str );
    QString replaceBuiltinVars( QString str );
	QString replaceBuiltinVars( Tunnel_c *pt, QString str );
	QString replacePasswordVars( QString str );
    QString replaceExecutableVariables( QString str );
	QString replaceUserDefVars( QString str );
	QString replaceVarsLog( Tunnel_c &tunnel, QString str );
    QStringList replaceVars( Tunnel_c &tunnel, QStringList strList );
    QStringList replaceVarsLog( Tunnel_c &tunnel, QStringList strList );
    static QString addQuotesIfNeeded(QString str);
    static QStringList addQuotesIfNeeded(QStringList strList);
    static QString removeQuotes(QString str);

	//Used for statistics.
	//QUuid: VariableStruct.uUid
	//VariableStatStruct: The stat values
	QMap<QUuid,VariableStatStruct> getVariableStatistics();
	QList<VariableStruct> getPasswordVariables();
	static QList<VariableStruct> getVariablesUsedInTunnels(QTreeWidget *tunnelTreeWidget, QList<VariableStruct> &varCheckList);
	static QList<VariableStruct> getVariablesUsedInTunnels(QList<Tunnel_c*> &tunnelList, QList<VariableStruct> &varCheckList);
	static VariableStatTunnelStruct getVariableUsageInTunnel(Tunnel_c *pt, QString varName);
	static bool variableNameExists(VariableStruct var, QList<VariableStruct> &varList);
	static bool variableNameExists(const QString& name, QList<VariableStruct> &varList);
	bool isDuplicateVariableName(const QString& name); //Passwords, Executables, Userdefined
	bool isDuplicateBuiltinVariableName(const QString& name);
    bool isDuplicatePortForwardVariableName(const QString& name, bool checkGlobally, QTableWidgetItem *ignore=NULL);
    void replaceExecutableVariables(QList<VariableStruct> &varList);
	static void resolveVariable(VariableStruct var, QList<VariableStruct> &addToList, QList<VariableStruct> &checkList);
    void setNewLocalPort(QTreeWidgetItem *twi, bool alsoUpdateChildren);
    void setNewLocalPort(QTreeWidgetItem *twi, bool alsoUpdateChildren, QList<int> &excludePorts);
	int proposeNewLocalPort(QTreeWidgetItem *twi);
    int proposeNewLocalPort(QTreeWidgetItem *twi, QList<int> &excludePorts);
	QList<int> getLocalTunnelPortsInUse(Tunnel_c *pt);
	QMap<int,int> getAllLocalTunnelPortsInUse(Tunnel_c *ptIgnore=NULL);
    QVersionNumber getPlinkVersion(const QString& plinkPath, Tunnel_c &tunnel);

//override
protected:
	

public slots:
	void slotLoadIcons();
	void slotEnableEditingToggled(bool);
	void slotTreeTunnelExportCurrent();
	void slotTreeTunnelExportAll();
	void slotTreeTunnelImportAfter();
	void slotTreeTunnelImportChild();
	//void slotTreeTunnelEdit();
	void slotTreeTunnelRename();
	void slotTreeTunnelItemChanged(QTreeWidgetItem*,int);
	void slotTreeTunnelDuplicate();
	void slotTreeTunnelInsertChildFolder();
	void slotTreeTunnelInsertFolderBefore();
	void slotTreeTunnelInsertFolderAfter();
	void slotTreeTunnelInsertChild();
	void slotTreeTunnelInsertNodeBefore();
	void slotTreeTunnelInsertNodeAfter();
	void slotTreeTunnelCutNode();
	void slotTreeTunnelCopyNode();
	void slotTreeTunnelPasteNodeChild();
	void slotTreeTunnelPasteNodeAfter();
	void slotTreeTunnelDeleteNode();
	void slotTreeTunnelClearNodeLog();
    void slotTreeTunnelExpandAll();
    void slotTreeTunnelCollapseAll();
	void slotTreeTunnelMoveLeft();
	void slotTreeTunnelMoveRight();
	void slotTreeTunnelMoveUp();
	void slotTreeTunnelMoveDown();
	void slotTreeTunnelExpanded(QTreeWidgetItem *twi);
	void slotTreeTunnelCollapsed(QTreeWidgetItem *twi);
	void slotDragMoveStarted(QTreeWidgetItem *twi);
	void slotDragMoveFinished(QTreeWidgetItem *twi);
	void slotCustomActionExec(const CustomActionStruct& cas);
	void slotSelectTunnel();
    void slotSave();
	void slotCancel();
    void slotComboTunnelTypeSelectionChanged(int index);
	void slotComboPasswordSelectSelectionChanged(int index);
	void slotComboKeyPasswordSelectSelectionChanged(int index);
	void slotEditSSHHost();
	void slotEditRemoteHost();
	void slotTabChanged();
	void slotFocusChanged(QWidget* oldFocus, QWidget* newFocus);
	void slotConnect();
	void slotDisconnect();
	void slotDisconnectAllTunnels();
	void slotConnectorFinished( Tunnel_c* );
	void slotAutoConnect( Tunnel_c* );
	void slotBrowseKeyFile();
	void slotItemActivated();

	void slotDelayWriteSettings();
	void slotUpdateUIFromTWI();

	void slotDisableTreeTunnelsPaste() { enableTreeTunnelsPaste(false); }

	void slotPasswordDialog();

	void slotManualBackupSelected();
	void slotCleanBackup();
	void slotCleanBackupFinished();

	void slotConnected( QTreeWidgetItem *twi );
    void slotKillConnection( QTreeWidgetItem *twi );

    void slotDelayRetryConnect( Tunnel_c* );

	void slotHostModified();

	void slotUpdateClearLogButtons();

    void slotConnectorPopulateChildNodesWithExternalCommandFinished( Tunnel_c* );

    void slotUpdateTrayIcon();

    void slotTunnelNameFgColorDialog();
    void slotTunnelNameBgColorDialog();
    void slotFolderNameFgColorDialog();
    void slotFolderNameBgColorDialog();

    void slotTunnelNameFgColorReset();
    void slotFolderNameFgColorReset();

    void slotTunnelNameBgColorReset();
    void slotFolderNameBgColorReset();

signals:
	void signalSetTrayIcon( int iIndex );
	void signalAutoConnect( Tunnel_c* );
	void signalReadOptions();
	void signalPasswordDialog();
	void signalCleanBackup();

private:
	Ui::ATSkeletonWindowClass ui;

	void wireSignals();
	
	void readSettings();	//Read DoffenSSHTunnel.ini
	static Tunnel_c* readSettingsHost(QSettings &settings); //Create tunnel from settings [Host_xx]   
    static Tunnel_c* readJsonSettingsHost(QJsonObject &json);
    static QJsonObject buildJsonSettingsHost(Tunnel_c *tunnel);
    static QJsonObject buildJsonSettings(QTreeWidgetItem *twi);
    void addHostsRecursively(const QJsonArray& jsonHosts, QTreeWidgetItem* parentItem, int levelOffset, const QList<QTreeWidgetItem*>& toRemove, QList<int>& excludePorts);
    static VariableStruct readSettingsVariable(QSettings &settings);
	static QList<VariableStruct> readSettingsUserDefinedVariables(QSettings &settings); 
	static QList<VariableStruct> readSettingsExecutableVariables(QSettings &settings); 
	void readAppSettings(); //Called by RESTORE ONLY. Read DoffenSSHTunnelApp.ini
	void delayWriteSettings();
	void writeSettings();
	void importSettingsTunnel(QTreeWidgetItem *twiImportAt, int importMode);
	void importTunnels(QList<Tunnel_c*> &tunnelList, QTreeWidgetItem *twiImportAt, int importMode);
	void importVariables(QList<VariableStruct> &varList);
	void exportSettingsTunnelList(QString filePath, QList<Tunnel_c*> &tunnelList);
	static void writeSettingsTreeWidget(QSettings &settings, QTreeWidget *treeWidget);
	static QList<Tunnel_c*> getTunnelList(QTreeWidget *treeWidget);
	static void writeSettingsTunnelList(QSettings &settings, QList<Tunnel_c*> &tunnelList);
	static void writeSettingsTunnel(QSettings &settings, Tunnel_c *it);
	static void writeSettingsUserDefinedVariables(QSettings &settings, QList<VariableStruct> &varList);
	static void writeSettingsExecutableVariables(QSettings &settings, QList<VariableStruct> &varList);
	static void writeSettingsVariable(QSettings &settings, VariableStruct &var);
	
	void autoBackupSettings(const BackupInfo &backupInfoData);
	bool m_bAutoBackupSettingsDisabled;
	void backupSettings(const BackupInfo &backupInfoData);
	void restoreBackup(QString backupFilePath);

	QFutureWatcher<void> m_cleanBackupFutureWatcher;
	void cleanBackup(QString iniFilePath);
	void deleteBackup(const QDir& backupDir, const QString& backupFileName);
	QTimer m_timerDelayAutoBackupSettings;
	
	void updateControls();
	void treeTunnelUpdateMoveControls();
	void updateControlsTunnel();
	void updateControlsTunnel(Tunnel_c *pt);
	void updateControlsFolder();

	void launchCustomCommand( Tunnel_c *pt, QString label, QString command );
	QTreeWidgetItem* markConnect( QTreeWidgetItem* twi );
	void crawlConnect( QTreeWidgetItem* twi );
	void connectTunnel( Tunnel_c &tunnel );
    void connectSSHTunnel( Tunnel_c &tunnel );
    void connectSSMTunnel( Tunnel_c &tunnel );
	void disconnectTunnel( Tunnel_c &tunnel );
	void disconnectTunnelSilent( QTreeWidgetItem* twi );
	void disconnectAllTunnelsSilent();
	int getConnectionCount();	//get total number of connected tunnels
	int getConnectionCountChildren(QTreeWidgetItem* twi, bool countFolders); 
    int getDelayRetryConnectCountChildren(QTreeWidgetItem* twi);

    void populateChildNodesWithExternalCommand( QTreeWidgetItem* twi );
	
    void saveEditPane();
    bool confirmSaveEditPane();

	bool askforPassword( Tunnel_c &tunnel );
	QString askforPassword( QString label );

	void populateEditUIFromTwi( QTreeWidgetItem *twi );
	void delayUpdateUIFromTWI();
	QTimer m_timerDelayUpdateUIFromTWI;
	void populateConnectUIFromTwi( QTreeWidgetItem *twi );

	int getItemLevel(QTreeWidgetItem* item);
	static Tunnel_c* getTunnel(QTreeWidgetItem* twi);
	static void setTunnel(QTreeWidgetItem* twi, Tunnel_c* pt);

    void delayRetryConnect( Tunnel_c*, int autoReconnectIntervalSeconds );

	QTreeWidgetItem *m_pTreeTunnelsItemEdit;
	QTreeWidgetItem *m_pTreeTunnelsPaste;
	int m_pTreeTunnelsPasteMode; //Copy or Cut

	QMenu *m_pTreeTunnelsContextMenu;
	QAction *m_pTreeTunnelsActionExport;
	QAction *m_pTreeTunnelsActionImportAfter;
	QAction *m_pTreeTunnelsActionImportChild;
	QAction *m_pTreeTunnelsActionRename;
	QAction *m_pTreeTunnelsActionDuplicate;
	QAction *m_pTreeTunnelsActionInsertChildFolder;
	QAction *m_pTreeTunnelsActionInsertFolderAfter;
	QAction *m_pTreeTunnelsActionInsertFolderBefore;
	QAction *m_pTreeTunnelsActionInsertChild;
	QAction *m_pTreeTunnelsActionInsertNodeAfter;
	QAction *m_pTreeTunnelsActionInsertNodeBefore;
	QAction *m_pTreeTunnelsActionCutNode;
	QAction *m_pTreeTunnelsActionCopyNode;
	QAction *m_pTreeTunnelsActionPasteNodeChild;
	QAction *m_pTreeTunnelsActionPasteNodeAfter;
	QAction *m_pTreeTunnelsActionDeleteNode;
	QAction *m_pTreeTunnelsActionClearLog;
    QAction *m_pTreeTunnelsActionExpandAll;
    QAction *m_pTreeTunnelsActionCollapseAll;

	void enableTreeTunnelsPaste(bool enable);
	void enableTreeTunnelDragDrop(bool enable);
	QTreeWidgetItem* treeTunnelCreateNew();
	QTreeWidgetItem* treeTunnelCreateNew(QTreeWidgetItem *twiInsertAt, int insertMode);
	QTreeWidgetItem* treeTunnelCreateNewFolder(QTreeWidgetItem *twiInsertAt, int insertMode);
	QTreeWidgetItem* treeTunnelPaste(QTreeWidgetItem *twiPasteFrom, QTreeWidgetItem *twiPasteTo, int pasteMode, int insertMode);
	void treeTunnelInsert(QTreeWidgetItem *twiInsertAt, QTreeWidgetItem *twiInsert, int insertMode);
	bool treeTunnelIsDuplicateByName(QTreeWidgetItem* twi);
	void treeTunnelUpdateFromParent(QTreeWidgetItem* twi);
    void treeTunnelSetIcon(QTreeWidgetItem *twi, QPixmap pm);

	QTreeWidgetItem* findParentTunnelNode(QTreeWidgetItem *twi); //find first real parent tunnel 
	QTreeWidgetItem* findParentFolderNode(QTreeWidgetItem *twi); //find first parent folder
	QList<QTreeWidgetItem*> findChildTunnelNodes(QTreeWidgetItem *twi); //find real child tunnels, traverse child folders

	void updateParentFoldersIcon(QTreeWidgetItem *twi);
    void updateCurrentAndParentFoldersIcon(QTreeWidgetItem *twi);

	void recursiveReplaceTunnelWithCopy( QTreeWidgetItem* twi );
	void recursiveDisconnectTunnel( QTreeWidgetItem* twi );
    void recursiveDisconnectTunnelSilent( QTreeWidgetItem* twi );
	void recursiveDisconnectTunnelSignals( QTreeWidgetItem* twi );
	void recursiveDeleteTunnel( QTreeWidgetItem* twi );

    QTimer m_timerDelayWriteSettings;

	void editSetSSHHost(const QString& host);
	void editSetSelectedSSHHost(const QString& host);
	QString editGetSSHHost();
	QString editGetSelectedSSHHost();

	void editSetRemoteHost(const QString& host);
	void editSetSelectedRemoteHost(const QString& host);
	QString editGetRemoteHost();
	QString editGetSelectedRemoteHost();
	void clearNodeLog(QTreeWidgetItem *twi);
	void clearTunnelLog(Tunnel_c *pt);

	bool m_bEditingEnabled;
	
	QList<VariableStruct> m_listExecutableVariables;	//DoffenSSHTunnelApp.ini
	QList<VariableStruct> m_listUserDefinedVariables;	//DoffenSSHTunnel.ini

	//QPair.first = original varname
	//QPair.second = new varname
    void changeTunnelVarNames(QList<QPair<QString,QString> > & varList);

	ExportHostsDialog *m_pExportHostsDialog;
	ImportHostsDialog *m_pImportHostsDialog;

    QMenu *m_pTunnelNameMenu;
    QAction *m_pTunnelNameFgColorAction;
    QAction *m_pTunnelNameBgColorAction;
    QAction *m_pTunnelNameFgColorResetAction;
    QAction *m_pTunnelNameBgColorResetAction;
    QMenu *m_pFolderNameMenu;
    QAction *m_pFolderNameFgColorAction;
    QAction *m_pFolderNameBgColorAction;
    QAction *m_pFolderNameFgColorResetAction;
    QAction *m_pFolderNameBgColorResetAction;

    QMenu *m_pLogMenu;
    QAction *m_pLogMenuClearAction;

	ATMainWindow_c *m_pMainWindow;

	friend class ATTunnelConnector_c;
	friend class ATMainWindow_c;
	friend class CustomActionsEditWidget;
	friend class CustomButtonDialog;
	friend class ExportHostsDialog;
	friend class ImportHostsDialog;
	friend class PreferencesDialog;
	friend class MoreTunnelsEditWidget;
    friend class TreeSearchWidget;
    friend class SearchWidget;
    friend class ATPopulateChildNodesConnector_c;
};

#endif 
