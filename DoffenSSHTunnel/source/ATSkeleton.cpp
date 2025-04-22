#include "ATSkeleton.h"
#include "Backup.h"
#include "PasswordDialog.h"
#include "CustomActionsEditWidget.h"
#include "MoreTunnelsEditWidget.h"
#include "CreatePasswordDialog.h"
#include "CustomButtonDialog.h"
#include "ExportHostsDialog.h"
#include "ImportHostsDialog.h"
#include "SSHHostDialog.h"
#include "ATMainWindow.h"
#include "PreferencesDialog.h"
#include "VariableSettingsDialog.h"
#include "Images.h"
#include "PasswordDb.h"
#include "exceptions.h"
#include "searchwidget.h"
#include "treesearchwidget.h"
#include "OutputWindow.h"

#include <QtConcurrent/QtConcurrent>


#define PAGE_CONNECT	(0)
#define PAGE_EDIT		(1)

#define LOG_MAX_BUFFER_SIZE (8*1024)
#define WAIT_FOR_FINISHED_TIMEOUT (100)
#define CONNECTION_RETRIES (10)
#define PASSWORD_RETRIES (0)
#define KEY_PASSWORD_RETRIES (0)
#define CHALLENGE_RESPONSE_RETRIES (0)

#define INVALID_PASSWORD ""

#ifdef Q_OS_WIN
    #define CUSTOM_BUTTON_1_LABEL "SSH Term"
    #define CUSTOM_BUTTON_1_CMD "${putty} -l ${uid} -P ${port} ${ip}"
    #define CUSTOM_BUTTON_2_LABEL "SSH FTP"
    #define CUSTOM_BUTTON_2_CMD "${winscp} sftp://${uid}@${ip}:${port}"
#endif

#ifdef Q_OS_MACOS
    #define CUSTOM_BUTTON_1_LABEL "SSH Term"
    #define CUSTOM_BUTTON_1_CMD "${ssh} -l ${uid} -p ${port} ${ip}"
    #define CUSTOM_BUTTON_2_LABEL "SSH FTP"
    #define CUSTOM_BUTTON_2_CMD "${filezilla} sftp://${uid}@${ip}:${port}"
#endif

#ifdef Q_OS_LINUX
    #define CUSTOM_BUTTON_1_LABEL "SSH Term"
    #define CUSTOM_BUTTON_1_CMD "${ssh} -l ${uid} -p ${port} ${ip}"
    #define CUSTOM_BUTTON_2_LABEL "SSH FTP"
    #define CUSTOM_BUTTON_2_CMD "${filezilla} sftp://${uid}@${ip}:${port}"
#endif

#define PASTE_MODE_COPY (1)
#define PASTE_MODE_CUT (2)

#define INSERT_MODE_BEFORE (1)
#define INSERT_MODE_AFTER (2)
#define INSERT_MODE_CHILD (3)

#define DISCONNECTED (0)
#define CONNECTING (1)
#define CONNECTED (2)
#define MARKCONNECT (3)

ATSkeletonWindow::ATSkeletonWindow(QWidget *parent)
: QWidget(parent)
{
	m_pMainWindow = static_cast<ATMainWindow_c*>(parent);

	m_pTreeTunnelsItemEdit = NULL;
	m_pTreeTunnelsPaste = NULL;
	m_pTreeTunnelsPasteMode = 0;
	m_bAutoBackupSettingsDisabled = false;
	m_timerDelayWriteSettings.setInterval( 500 );
	m_timerDelayUpdateUIFromTWI.setSingleShot(true);
	m_timerDelayUpdateUIFromTWI.setInterval( 200 );

	ui.setupUi(this);

	ui.tabWidget->setCurrentIndex( PAGE_CONNECT );

	m_pExportHostsDialog = new ExportHostsDialog(this, this);
	m_pImportHostsDialog = new ImportHostsDialog(this, this);

	ui.treeTunnels->setAllowItemsToBeParents(true);

	// TREEVIEW CONTEXT MENU
	//x,m,u,f,u,t,c,p,d,l
	ui.treeTunnels->setContextMenuPolicy(Qt::ActionsContextMenu);
	m_pTreeTunnelsContextMenu = new QMenu(ui.treeTunnels);
    m_pTreeTunnelsActionExport = new QAction("E&xport...",m_pTreeTunnelsContextMenu);
    m_pTreeTunnelsActionImportAfter = new QAction("Import After...",m_pTreeTunnelsContextMenu);
	m_pTreeTunnelsActionImportChild = new QAction("Import as Children...",m_pTreeTunnelsContextMenu);
	m_pTreeTunnelsActionRename = new QAction("Rena&me",m_pTreeTunnelsContextMenu);
	m_pTreeTunnelsActionDuplicate = new QAction("D&uplicate",m_pTreeTunnelsContextMenu);
	m_pTreeTunnelsActionInsertFolderAfter = new QAction("Insert &Folder After",m_pTreeTunnelsContextMenu);
	m_pTreeTunnelsActionInsertFolderBefore = new QAction("Insert Folder Before",m_pTreeTunnelsContextMenu);
	m_pTreeTunnelsActionInsertChildFolder = new QAction("Insert Folder Child",m_pTreeTunnelsContextMenu);
    m_pTreeTunnelsActionInsertNodeAfter = new QAction("&Insert SSH Host After",m_pTreeTunnelsContextMenu);
#ifdef Q_OS_MACOS
    m_pTreeTunnelsActionInsertNodeAfter->setText(m_pTreeTunnelsActionInsertNodeAfter->text()+" (Cmd+N)");
#else
    m_pTreeTunnelsActionInsertNodeAfter->setText(m_pTreeTunnelsActionInsertNodeAfter->text()+" (Ctrl+N)");
#endif
	m_pTreeTunnelsActionInsertNodeBefore = new QAction("Insert SSH Host Before",m_pTreeTunnelsContextMenu);
	m_pTreeTunnelsActionInsertChild = new QAction("Insert SSH Host Child",m_pTreeTunnelsContextMenu);
    m_pTreeTunnelsActionCutNode = new QAction("Cu&t",m_pTreeTunnelsContextMenu);
#ifdef Q_OS_MACOS
    m_pTreeTunnelsActionCutNode->setText(m_pTreeTunnelsActionCutNode->text()+" (Cmd+X)");
#else
    m_pTreeTunnelsActionCutNode->setText(m_pTreeTunnelsActionCutNode->text()+" (Ctrl+X)");
#endif
    m_pTreeTunnelsActionCopyNode = new QAction("&Copy",m_pTreeTunnelsContextMenu);
#ifdef Q_OS_MACOS
    m_pTreeTunnelsActionCopyNode->setText(m_pTreeTunnelsActionCopyNode->text()+" (Cmd+C)");
#else
    m_pTreeTunnelsActionCopyNode->setText(m_pTreeTunnelsActionCopyNode->text()+" (Ctrl+C)");
#endif
    m_pTreeTunnelsActionPasteNodeAfter = new QAction("&Paste After",m_pTreeTunnelsContextMenu);
#ifdef Q_OS_MACOS
    m_pTreeTunnelsActionPasteNodeAfter->setText(m_pTreeTunnelsActionPasteNodeAfter->text()+" (Cmd+V)");
#else
    m_pTreeTunnelsActionPasteNodeAfter->setText(m_pTreeTunnelsActionPasteNodeAfter->text()+" (Ctrl+V)");
#endif
	m_pTreeTunnelsActionPasteNodeChild = new QAction("Paste Child",m_pTreeTunnelsContextMenu);
	m_pTreeTunnelsActionDeleteNode = new QAction("&Delete",m_pTreeTunnelsContextMenu);
	m_pTreeTunnelsActionClearLog = new QAction("Clear &Log",m_pTreeTunnelsContextMenu);
	enableTreeTunnelsPaste(false);

#ifdef Q_OS_MACOS
    ui.btnDeleteTunnel->setToolTip(ui.btnDeleteTunnel->toolTip() + " (Fn+Backspace)");
    ui.btnAddTunnel->setToolTip(ui.btnAddTunnel->toolTip() + " (Cmd+N)");
    ui.btnCustomActionDelete->setToolTip(ui.btnCustomActionDelete->toolTip() + " (Fn+Backspace)");
    ui.btnCustomActionAdd->setToolTip(ui.btnCustomActionAdd->toolTip() + " (Cmd+N)");
#else
    ui.btnLeftTunnel->setToolTip("Move Left (Ctrl+Left)");
    ui.btnRightTunnel->setToolTip("Move Right (Ctrl+Right)");
    ui.btnUpTunnel->setToolTip("Move Up (Ctrl+Up)");
    ui.btnDownTunnel->setToolTip("Move Down (Ctrl+Down)");
    ui.btnDeleteTunnel->setToolTip("Delete (Ctrl+Del)");
    ui.btnAddTunnel->setToolTip("Add a new host (Ctrl+N)");
    ui.btnCustomActionLeft->setToolTip("Move left (Ctrl+Left)");
    ui.btnCustomActionRight->setToolTip("Move Right (Ctrl+Right)");
    ui.btnCustomActionUp->setToolTip("Move Up (Ctrl+Up)");
    ui.btnCustomActionDown->setToolTip("Move Down (Ctrl+Down)");
    ui.btnCustomActionDelete->setToolTip("Delete (Ctrl+Del)");
    ui.btnCustomActionAdd->setToolTip("Add a new action (Ctrl+N)");
#endif

QAction *sep0 = new QAction(m_pTreeTunnelsContextMenu);
	sep0->setSeparator(true);
	QAction *sep1 = new QAction(m_pTreeTunnelsContextMenu);
	sep1->setSeparator(true);
	QAction *sep2 = new QAction(m_pTreeTunnelsContextMenu);
	sep2->setSeparator(true);
	QAction *sep3 = new QAction(m_pTreeTunnelsContextMenu);
	sep3->setSeparator(true);
	QAction *sep4 = new QAction(m_pTreeTunnelsContextMenu);
	sep4->setSeparator(true);
	QAction *sep5 = new QAction(m_pTreeTunnelsContextMenu);
	sep5->setSeparator(true);
	QAction *sep6 = new QAction(m_pTreeTunnelsContextMenu);
	sep6->setSeparator(true);

	ui.treeTunnels->addAction(m_pTreeTunnelsActionClearLog);
	ui.treeTunnels->addAction(sep0);
	ui.treeTunnels->addAction(m_pTreeTunnelsActionRename);
	ui.treeTunnels->addAction(sep1);
	ui.treeTunnels->addAction(m_pTreeTunnelsActionExport);
	ui.treeTunnels->addAction(m_pTreeTunnelsActionImportAfter);
	ui.treeTunnels->addAction(m_pTreeTunnelsActionImportChild);
	ui.treeTunnels->addAction(sep2);
	ui.treeTunnels->addAction(m_pTreeTunnelsActionDuplicate);
	ui.treeTunnels->addAction(sep3);
	ui.treeTunnels->addAction(m_pTreeTunnelsActionInsertFolderAfter);
	ui.treeTunnels->addAction(m_pTreeTunnelsActionInsertFolderBefore);
	ui.treeTunnels->addAction(m_pTreeTunnelsActionInsertChildFolder);
	ui.treeTunnels->addAction(sep4);
	ui.treeTunnels->addAction(m_pTreeTunnelsActionInsertNodeAfter);
	ui.treeTunnels->addAction(m_pTreeTunnelsActionInsertNodeBefore);
	ui.treeTunnels->addAction(m_pTreeTunnelsActionInsertChild);
	ui.treeTunnels->addAction(sep5);
	ui.treeTunnels->addAction(m_pTreeTunnelsActionCutNode);
	ui.treeTunnels->addAction(m_pTreeTunnelsActionCopyNode);
	ui.treeTunnels->addAction(m_pTreeTunnelsActionPasteNodeAfter);
	ui.treeTunnels->addAction(m_pTreeTunnelsActionPasteNodeChild);
	ui.treeTunnels->addAction(sep6);
	ui.treeTunnels->addAction(m_pTreeTunnelsActionDeleteNode);

	ui.widgetCustomActions->setup(this);
	ui.widgetMoreTunnels->setup(this);
    ui.widgetTreeTunnelSearch_2->setup(this);

    //Tunnel name menu
    m_pTunnelNameMenu = new QMenu(ui.btnTunnelNameMenu);
    m_pTunnelNameFgColorAction = new QAction("Set foreground color",ui.btnTunnelNameMenu);
    m_pTunnelNameBgColorAction = new QAction("Set background color",ui.btnTunnelNameMenu);
    m_pTunnelNameFgColorResetAction = new QAction("Reset foreground color",ui.btnTunnelNameMenu);
    m_pTunnelNameBgColorResetAction = new QAction("Reset background color",ui.btnTunnelNameMenu);
    m_pTunnelNameMenu->addAction(m_pTunnelNameFgColorAction);
    m_pTunnelNameMenu->addAction(m_pTunnelNameBgColorAction);
    m_pTunnelNameMenu->addAction(m_pTunnelNameFgColorResetAction);
    m_pTunnelNameMenu->addAction(m_pTunnelNameBgColorResetAction);
    ui.btnTunnelNameMenu->setMenu(m_pTunnelNameMenu);

    //Folder name menu
    m_pFolderNameMenu = new QMenu(ui.btnFolderNameMenu);
    m_pFolderNameFgColorAction = new QAction("Set foreground color...",ui.btnFolderNameMenu);
    m_pFolderNameBgColorAction = new QAction("Set background color...",ui.btnFolderNameMenu);
    m_pFolderNameFgColorResetAction = new QAction("Reset foreground color",ui.btnFolderNameMenu);
    m_pFolderNameBgColorResetAction = new QAction("Reset background color",ui.btnFolderNameMenu);
    m_pFolderNameMenu->addAction(m_pFolderNameFgColorAction);
    m_pFolderNameMenu->addAction(m_pFolderNameBgColorAction);
    m_pFolderNameMenu->addAction(m_pFolderNameFgColorResetAction);
    m_pFolderNameMenu->addAction(m_pFolderNameBgColorResetAction);
    ui.btnFolderNameMenu->setMenu(m_pFolderNameMenu);

    //Log button menu
    m_pLogMenu = new QMenu(ui.widgetLog);
    m_pLogMenuClearAction = new QAction("Clear", ui.widgetLog);
    m_pLogMenu->addAction(m_pLogMenuClearAction);
    ui.btnLogMenu->setMenu(m_pLogMenu);

	// SHOW PASSWORD DIALOG
    ATVERIFY( connect( this, &ATSkeletonWindow::signalPasswordDialog, this, &ATSkeletonWindow::slotPasswordDialog, Qt::QueuedConnection ) );
    emit signalPasswordDialog();
}

ATSkeletonWindow::~ATSkeletonWindow()
{
	qDebug("ATSkeletonWindow::~ATSkeletonWindow()");

	m_cleanBackupFutureWatcher.cancel();
	m_cleanBackupFutureWatcher.waitForFinished();

    QList<QTreeWidgetItem*> items = ui.treeTunnels->findItems(".*", Qt::MatchFlags(Qt::MatchRegularExpression | Qt::MatchRecursive), 0);
	for(int i=0;i<items.size();i++) {
		Tunnel_c* pt = getTunnel(items[i]);
		disconnectTunnelSilent(items[i]);
		delete pt;
	}

    PasswordDb::kill();
}


void ATSkeletonWindow::wireSignals()
{
    ATVERIFY( connect(qApp, &QApplication::focusChanged, this, &ATSkeletonWindow::slotFocusChanged) );

	//Clean backup
    ATVERIFY( connect( &m_cleanBackupFutureWatcher, &QFutureWatcher<void>::finished, this, &ATSkeletonWindow::slotCleanBackupFinished ) );
    ATVERIFY( connect( this, &ATSkeletonWindow::signalCleanBackup, this, &ATSkeletonWindow::slotCleanBackup, Qt::QueuedConnection ) );

	// Organize tree
    ATVERIFY( connect( ui.btnLeftTunnel,  &QToolButton::clicked, this, &ATSkeletonWindow::slotTreeTunnelMoveLeft, Qt::UniqueConnection ) );
    ATVERIFY( connect( ui.btnRightTunnel, &QToolButton::clicked, this, &ATSkeletonWindow::slotTreeTunnelMoveRight, Qt::UniqueConnection ) );
    ATVERIFY( connect( ui.btnUpTunnel,    &QToolButton::clicked, this, &ATSkeletonWindow::slotTreeTunnelMoveUp, Qt::UniqueConnection ) );
    ATVERIFY( connect( ui.btnDownTunnel,  &QToolButton::clicked, this, &ATSkeletonWindow::slotTreeTunnelMoveDown, Qt::UniqueConnection ) );
    ATVERIFY( connect( ui.treeTunnels,	  &TunnelTreeWidget::signalKeyCtrlLeftPressed,  this, &ATSkeletonWindow::slotTreeTunnelMoveLeft, Qt::UniqueConnection ) );
    ATVERIFY( connect( ui.treeTunnels,	  &TunnelTreeWidget::signalKeyCtrlRightPressed, this, &ATSkeletonWindow::slotTreeTunnelMoveRight, Qt::UniqueConnection ) );
    ATVERIFY( connect( ui.treeTunnels,	  &TunnelTreeWidget::signalKeyCtrlUpPressed,	this, &ATSkeletonWindow::slotTreeTunnelMoveUp, Qt::UniqueConnection ) );
    ATVERIFY( connect( ui.treeTunnels,	  &TunnelTreeWidget::signalKeyCtrlDownPressed,	this, &ATSkeletonWindow::slotTreeTunnelMoveDown, Qt::UniqueConnection ) );

    ATVERIFY( connect( ui.treeTunnels,	  &TunnelTreeWidget::signalDragMoveStarted, this, &ATSkeletonWindow::slotDragMoveStarted ) );
    ATVERIFY( connect( ui.treeTunnels,	  &TunnelTreeWidget::signalDragMoveFinished,this, &ATSkeletonWindow::slotDragMoveFinished ) );

    // Edit host widget
    ATVERIFY( connect( ui.btnEditSSHHost,		&QToolButton::clicked, this, &ATSkeletonWindow::slotEditSSHHost ) );
    ATVERIFY( connect( ui.btnEditRemoteHost,	&QToolButton::clicked, this, &ATSkeletonWindow::slotEditRemoteHost ) );
    ATVERIFY( connect( ui.btnSave,				&QToolButton::clicked, this, &ATSkeletonWindow::slotSave ) );
    ATVERIFY( connect( ui.btnCancel,			&QToolButton::clicked, this, &ATSkeletonWindow::slotCancel ) );
    ATVERIFY( connect( ui.comboPasswordSelect,	  &QComboBox::currentIndexChanged, this, &ATSkeletonWindow::slotComboPasswordSelectSelectionChanged ) );
    ATVERIFY( connect( ui.comboKeyPasswordSelect, &QComboBox::currentIndexChanged, this, &ATSkeletonWindow::slotComboKeyPasswordSelectSelectionChanged ) );
	ui.widgetEditTunnel->wireSignals();
    ATVERIFY( connect( ui.widgetEditTunnel,		&Widget::signalChildWidgetModified, this, &ATSkeletonWindow::slotHostModified ) );
	
    // Edit host widget - returnPressed() - capture return key and save
    ATVERIFY( connect( ui.editTunnelName,			&QLineEdit::returnPressed, this, &ATSkeletonWindow::slotSave ) );
    ATVERIFY( connect( ui.editSSHHost,				&QLineEdit::returnPressed, this, &ATSkeletonWindow::slotSave ) );
    ATVERIFY( connect( ui.editUsername,				&QLineEdit::returnPressed, this, &ATSkeletonWindow::slotSave ) );
    ATVERIFY( connect( ui.editPassword,				&QLineEdit::returnPressed, this, &ATSkeletonWindow::slotSave ) );
    ATVERIFY( connect( ui.editSSHKeyFile,			&QLineEdit::returnPressed, this, &ATSkeletonWindow::slotSave ) );
    ATVERIFY( connect( ui.editSSHKeyFilePassword,	&QLineEdit::returnPressed, this, &ATSkeletonWindow::slotSave ) );
    ATVERIFY( connect( ui.editLocalIP,				&QLineEdit::returnPressed, this, &ATSkeletonWindow::slotSave ) );
    ATVERIFY( connect( ui.editLocalPort,			&QLineEdit::returnPressed, this, &ATSkeletonWindow::slotSave ) );
    ATVERIFY( connect( ui.editRemoteHost,			&QLineEdit::returnPressed, this, &ATSkeletonWindow::slotSave ) );
    ATVERIFY( connect( ui.editRemotePort,			&QLineEdit::returnPressed, this, &ATSkeletonWindow::slotSave ) );
    ATVERIFY( connect( ui.editExtraArguments,		&QLineEdit::returnPressed, this, &ATSkeletonWindow::slotSave ) );

    // Edit folder widget
    ATVERIFY( connect( ui.btnSaveFolder,			&QAbstractButton::clicked, this, &ATSkeletonWindow::slotSave ) );
    ATVERIFY( connect( ui.btnCancelFolder,			&QAbstractButton::clicked, this, &ATSkeletonWindow::slotCancel ) );
    ui.widgetEditFolder->wireSignals();
    ATVERIFY( connect( ui.widgetEditFolder,         &Widget::signalChildWidgetModified, this, &ATSkeletonWindow::slotHostModified ) );

    // Edit host widget - returnPressed() - capture return key and save
    ATVERIFY( connect( ui.editFolderName,               &QLineEdit::returnPressed, this, &ATSkeletonWindow::slotSave ) );
    ATVERIFY( connect( ui.editFolderChildNodesCommand,	&QLineEdit::returnPressed, this, &ATSkeletonWindow::slotSave ) );

	// Connect buttons
    ATVERIFY( connect( ui.btnAddFolder,    &QAbstractButton::clicked, this, &ATSkeletonWindow::slotTreeTunnelInsertFolderAfter ) );
    ATVERIFY( connect( ui.btnAddTunnel,    &QAbstractButton::clicked, this, &ATSkeletonWindow::slotTreeTunnelInsertNodeAfter ) );
    ATVERIFY( connect( ui.btnDuplicate,    &QAbstractButton::clicked, this, &ATSkeletonWindow::slotTreeTunnelDuplicate ) );
    ATVERIFY( connect( ui.treeTunnels,	   &TunnelTreeWidget::signalKeyDeletePressed, this, &ATSkeletonWindow::slotTreeTunnelDeleteNode, Qt::UniqueConnection ) );
    ATVERIFY( connect( ui.btnDeleteTunnel, &QAbstractButton::clicked,     this, &ATSkeletonWindow::slotTreeTunnelDeleteNode ) );
    ATVERIFY( connect( ui.btnBrowseSSHKeyFile, &QAbstractButton::clicked, this, &ATSkeletonWindow::slotBrowseKeyFile ) );

	// Connect tab widget
    ATVERIFY( connect( ui.btnConnect,                           &QAbstractButton::clicked, this, &ATSkeletonWindow::slotConnect ) );
    ATVERIFY( connect( ui.btnDisconnect,                        &QAbstractButton::clicked, this, &ATSkeletonWindow::slotDisconnect ) );
    ATVERIFY( connect( ui.tabWidget,                          &QTabWidget::currentChanged, this, &ATSkeletonWindow::slotTabChanged ) );
    ATVERIFY( connect( ui.widgetCustomActions, &CustomActionsEditWidget::signalActionExec, this, &ATSkeletonWindow::slotCustomActionExec ) );
    ATVERIFY( connect( ui.textBrowser,                         &QTextBrowser::textChanged, this, &ATSkeletonWindow::slotUpdateClearLogButtons ) );

	// Connect tree
    ATVERIFY( connect( &m_timerDelayUpdateUIFromTWI,               &QTimer::timeout, this, &ATSkeletonWindow::slotUpdateUIFromTWI, Qt::QueuedConnection ) );
    ATVERIFY( connect( &m_timerDelayWriteSettings,                 &QTimer::timeout, this, &ATSkeletonWindow::slotDelayWriteSettings, Qt::UniqueConnection ) );
    ATVERIFY( connect( ui.treeTunnels,      &TunnelTreeWidget::itemSelectionChanged, this, &ATSkeletonWindow::slotSelectTunnel, Qt::UniqueConnection ) );
    ATVERIFY( connect( ui.treeTunnels,                 &TunnelTreeWidget::activated, this, &ATSkeletonWindow::slotItemActivated, Qt::UniqueConnection ) );
    ATVERIFY( connect( m_pTreeTunnelsActionExport,				&QAction::triggered, this, &ATSkeletonWindow::slotTreeTunnelExportCurrent, Qt::UniqueConnection ) );
    ATVERIFY( connect( m_pTreeTunnelsActionImportAfter,			&QAction::triggered, this, &ATSkeletonWindow::slotTreeTunnelImportAfter, Qt::UniqueConnection ) );
    ATVERIFY( connect( m_pTreeTunnelsActionImportChild,			&QAction::triggered, this, &ATSkeletonWindow::slotTreeTunnelImportChild, Qt::UniqueConnection ) );
    ATVERIFY( connect( m_pTreeTunnelsActionRename,				&QAction::triggered, this, &ATSkeletonWindow::slotTreeTunnelRename, Qt::UniqueConnection ) );
    ATVERIFY( connect( ui.treeTunnels,               &TunnelTreeWidget::itemChanged, this, &ATSkeletonWindow::slotTreeTunnelItemChanged, Qt::UniqueConnection ) );
    ATVERIFY( connect( m_pTreeTunnelsActionDuplicate,			&QAction::triggered, this, &ATSkeletonWindow::slotTreeTunnelDuplicate, Qt::UniqueConnection ) );
    ATVERIFY( connect( m_pTreeTunnelsActionInsertChildFolder,	&QAction::triggered, this, &ATSkeletonWindow::slotTreeTunnelInsertChildFolder, Qt::UniqueConnection ) );
    ATVERIFY( connect( m_pTreeTunnelsActionInsertFolderAfter,	&QAction::triggered, this, &ATSkeletonWindow::slotTreeTunnelInsertFolderAfter, Qt::UniqueConnection ) );
    ATVERIFY( connect( m_pTreeTunnelsActionInsertFolderBefore,	&QAction::triggered, this, &ATSkeletonWindow::slotTreeTunnelInsertFolderBefore, Qt::UniqueConnection ) );
    ATVERIFY( connect( ui.treeTunnels,     &TunnelTreeWidget::signalKeyCtrlNPressed, this, &ATSkeletonWindow::slotTreeTunnelInsertNodeAfter, Qt::UniqueConnection ) );
    ATVERIFY( connect( m_pTreeTunnelsActionInsertChild,			&QAction::triggered, this, &ATSkeletonWindow::slotTreeTunnelInsertChild, Qt::UniqueConnection ) );
    ATVERIFY( connect( m_pTreeTunnelsActionInsertNodeAfter,		&QAction::triggered, this, &ATSkeletonWindow::slotTreeTunnelInsertNodeAfter, Qt::UniqueConnection ) );
    ATVERIFY( connect( m_pTreeTunnelsActionInsertNodeBefore,	&QAction::triggered, this, &ATSkeletonWindow::slotTreeTunnelInsertNodeBefore, Qt::UniqueConnection ) );
    ATVERIFY( connect( ui.treeTunnels,    &TunnelTreeWidget::signalKeyEscapePressed, this, &ATSkeletonWindow::slotDisableTreeTunnelsPaste, Qt::UniqueConnection ) );
    ATVERIFY( connect( ui.treeTunnels,     &TunnelTreeWidget::signalKeyCtrlXPressed, this, &ATSkeletonWindow::slotTreeTunnelCutNode, Qt::UniqueConnection ) );
    ATVERIFY( connect( ui.treeTunnels,     &TunnelTreeWidget::signalKeyCtrlCPressed, this, &ATSkeletonWindow::slotTreeTunnelCopyNode, Qt::UniqueConnection ) );
    ATVERIFY( connect( ui.treeTunnels,     &TunnelTreeWidget::signalKeyCtrlVPressed, this, &ATSkeletonWindow::slotTreeTunnelPasteNodeAfter, Qt::UniqueConnection ) );
    ATVERIFY( connect( m_pTreeTunnelsActionCutNode,				&QAction::triggered, this, &ATSkeletonWindow::slotTreeTunnelCutNode, Qt::UniqueConnection ) );
    ATVERIFY( connect( m_pTreeTunnelsActionCopyNode,			&QAction::triggered, this, &ATSkeletonWindow::slotTreeTunnelCopyNode, Qt::UniqueConnection ) );
    ATVERIFY( connect( m_pTreeTunnelsActionPasteNodeAfter,		&QAction::triggered, this, &ATSkeletonWindow::slotTreeTunnelPasteNodeAfter, Qt::UniqueConnection ) );
    ATVERIFY( connect( m_pTreeTunnelsActionPasteNodeChild,		&QAction::triggered, this, &ATSkeletonWindow::slotTreeTunnelPasteNodeChild, Qt::UniqueConnection ) );
    ATVERIFY( connect( m_pTreeTunnelsActionDeleteNode,			&QAction::triggered, this, &ATSkeletonWindow::slotTreeTunnelDeleteNode, Qt::UniqueConnection ) );
    ATVERIFY( connect( m_pTreeTunnelsActionClearLog,			&QAction::triggered, this, &ATSkeletonWindow::slotTreeTunnelClearNodeLog, Qt::UniqueConnection ) );
    ATVERIFY( connect( ui.treeTunnels,             &TunnelTreeWidget::itemCollapsed, this, &ATSkeletonWindow::slotTreeTunnelCollapsed ) );
    ATVERIFY( connect( ui.treeTunnels,              &TunnelTreeWidget::itemExpanded, this, &ATSkeletonWindow::slotTreeTunnelExpanded ) );

    //Tunnel/Folder color
    ATVERIFY( connect( m_pTunnelNameFgColorAction,		&QAction::triggered, this, &ATSkeletonWindow::slotTunnelNameFgColorDialog, Qt::UniqueConnection ) );
    ATVERIFY( connect( m_pTunnelNameBgColorAction,		&QAction::triggered, this, &ATSkeletonWindow::slotTunnelNameBgColorDialog, Qt::UniqueConnection ) );
    ATVERIFY( connect( m_pTunnelNameFgColorResetAction,	&QAction::triggered, this, &ATSkeletonWindow::slotTunnelNameFgColorReset, Qt::UniqueConnection ) );
    ATVERIFY( connect( m_pTunnelNameBgColorResetAction,	&QAction::triggered, this, &ATSkeletonWindow::slotTunnelNameBgColorReset, Qt::UniqueConnection ) );
    ATVERIFY( connect( m_pFolderNameFgColorAction,		&QAction::triggered, this, &ATSkeletonWindow::slotFolderNameFgColorDialog, Qt::UniqueConnection ) );
    ATVERIFY( connect( m_pFolderNameBgColorAction,		&QAction::triggered, this, &ATSkeletonWindow::slotFolderNameBgColorDialog, Qt::UniqueConnection ) );
    ATVERIFY( connect( m_pFolderNameFgColorResetAction,	&QAction::triggered, this, &ATSkeletonWindow::slotFolderNameFgColorReset, Qt::UniqueConnection ) );
    ATVERIFY( connect( m_pFolderNameBgColorResetAction,	&QAction::triggered, this, &ATSkeletonWindow::slotFolderNameBgColorReset, Qt::UniqueConnection ) );

    //Log
    ATVERIFY( connect( m_pLogMenuClearAction,	        &QAction::triggered, this, &ATSkeletonWindow::slotTreeTunnelClearNodeLog, Qt::UniqueConnection ) );
 
	// Connect signals
    ATVERIFY( connect( this,           &ATSkeletonWindow::signalAutoConnect, this, &ATSkeletonWindow::slotAutoConnect, Qt::QueuedConnection ) );
}

void ATSkeletonWindow::slotHostModified()
{
	qDebug() << "MODIFIED MODIFIED MODIFIED MODIFIED MODIFIED MODIFIED MODIFIED MODIFIED";
	m_pMainWindow->setWindowModified(true);
}

void ATSkeletonWindow::readSettings()
{
    qDebug( "%s", Q_FUNC_INFO );
    //qDebug() << "readSettings()";

	QSettings settings( g_strTunnelIniFile, QSettings::IniFormat);

	bool bUpdateIniFile = false;

	//Upgrade: removed in v0.9.15. Moved to new ini file DoffenSSHTunnelApp.ini
	if(settings.contains("pos")) { settings.remove("pos"); bUpdateIniFile = true; }
	if(settings.contains("size")) {	settings.remove("size"); bUpdateIniFile = true; }
	if(settings.contains("ConfirmOnQuit")) { settings.remove("ConfirmOnQuit"); bUpdateIniFile = true; }
	if(settings.contains("splitterPos")) { settings.remove("splitterPos"); bUpdateIniFile = true; }
	if(settings.contains("maximized")) { settings.remove("maximized"); bUpdateIniFile = true; }
	if(settings.contains("qtstyle")) { settings.remove("qtstyle"); bUpdateIniFile = true; }
	if(settings.contains("hotkey_mod1")) { settings.remove("hotkey_mod1"); bUpdateIniFile = true; }
	if(settings.contains("hotkey_mod2")) { settings.remove("hotkey_mod2"); bUpdateIniFile = true; }
	if(settings.contains("hotkey_key")) { settings.remove("hotkey_key"); bUpdateIniFile = true; }
	if(settings.contains("hotkey_enabled")) { settings.remove("hotkey_enabled"); bUpdateIniFile = true; }
	if(settings.contains("minimize_to_tray")) { settings.remove("minimize_to_tray"); bUpdateIniFile = true; }
	if(settings.contains("hide_passwords_in_log")) { settings.remove("hide_passwords_in_log"); bUpdateIniFile = true; }
	if(settings.contains("auto_connect_enabled")) { settings.remove("auto_connect_enabled"); bUpdateIniFile = true; }
	if(settings.contains("auto_reconnect_enabled")) { settings.remove("auto_reconnect_enabled"); bUpdateIniFile = true; }
	if(settings.contains("auto_clearlog_enabled")) { settings.remove("auto_clearlog_enabled"); bUpdateIniFile = true; }

	//Upgrade: renamed in v0.9.16  NumberOfTunnels -> NumberOfHosts
	if(settings.contains("NumberOfTunnels")) { 
		int iCount = settings.value( "NumberOfTunnels", 0 ).toInt();
		settings.remove("NumberOfTunnels"); 
		settings.setValue("NumberOfHosts",iCount);
		bUpdateIniFile = true; 
	}

	//Upgrade: [Host_nnn] child data
	QStringList keys = settings.childGroups();
	for(int i=0; i<keys.count(); i++) {
		QString key = keys.at(i);
		if(key.indexOf("Tunnel") != -1 || key.indexOf("Host") != -1) { 
			if(key.indexOf("Tunnel") != -1) {
				bUpdateIniFile = true; //renamed from Tunnel_ to Host_ in v0.9.16
			}
		}

		settings.beginGroup( key );

		//very old
		if(settings.contains("SSHTermCmd")) {
			settings.setValue( "CustomButton1Label", CUSTOM_BUTTON_1_LABEL ) ;
			settings.setValue( "CustomButton1Cmd", settings.value( "SSHTermCmd" ).toString() ) ;
			settings.remove("SSHTermCmd");
			bUpdateIniFile = true; 
		}

		//very old
		if(settings.contains("SSHFTPCmd")) {
			settings.setValue( "CustomButton2Label", CUSTOM_BUTTON_2_LABEL ) ;
			settings.setValue( "CustomButton2Cmd", settings.value( "SSHFTPCmd" ).toString() ) ;
			settings.remove("SSHFTPCmd");
			bUpdateIniFile = true; 
		}

		//changed to array in v0.9.17
		if(settings.contains("CustomButton1Cmd")) {
			qDebug() << "Found CustomButton1Cmd, change to CustomActions array";
			bUpdateIniFile = true;
			QString lbl1 = settings.value("CustomButton1Label","").toString();
			QString cmd1 = settings.value("CustomButton1Cmd","").toString();
			QString lbl2 = settings.value("CustomButton2Label","").toString();
			QString cmd2 = settings.value("CustomButton2Cmd","").toString();
			settings.remove("CustomButton1Label");
			settings.remove("CustomButton1Cmd");
			settings.remove("CustomButton2Label");
			settings.remove("CustomButton2Cmd");
			settings.beginWriteArray("CustomActions");
			settings.setArrayIndex(0);
			settings.setValue("label",lbl1);
			settings.setValue("cmd",cmd1);
			settings.setArrayIndex(1);
			settings.setValue("label",lbl2);
			settings.setValue("cmd",cmd2);
			settings.endArray();
		}

		settings.endGroup();
	}

	//////////////////////
	// READ DoffenSSHTunnelApp.ini
	readAppSettings();

	//+++ VarUserDefined

	m_listUserDefinedVariables.clear();
	m_listUserDefinedVariables.append(readSettingsUserDefinedVariables(settings));

    if(m_listUserDefinedVariables.size() == 0) {

        #ifdef _WIN32

        #elif __APPLE__

        #elif __linux

        #elif __unix // all unices not caught above

        #elif __posix

        #else


        #endif
    }

	//--- VarUserDefined

	//+++ Hosts

	bool blocked = ui.treeTunnels->blockSignals(true);

	ui.treeTunnels->clear();
	QTreeWidgetItem* lastTwi = NULL;
	int lastLevel = 0;
	keys = settings.childGroups();
	for(int i=0; i<keys.count(); i++) {
		QString key = keys.at(i);
		if(key.indexOf("Tunnel") != -1 || key.indexOf("Host") != -1) { 

			settings.beginGroup( key );

			Tunnel_c* tunnel = readSettingsHost(settings);

			settings.endGroup();

			if(lastTwi != NULL && tunnel->iLevel > 0) {
				if(tunnel->iLevel > lastLevel) {
					tunnel->twi = new QTreeWidgetItem( lastTwi );
				} else if(tunnel->iLevel == lastLevel) {
					QTreeWidgetItem* parentTwi = lastTwi->parent();
					if(parentTwi != NULL) {
						tunnel->twi = new QTreeWidgetItem( parentTwi );
					} else {
						tunnel->twi = new QTreeWidgetItem( ui.treeTunnels );
					}
				} else {
					QTreeWidgetItem* parentTwi = lastTwi->parent();
					while(parentTwi != NULL && (tunnel->iLevel <= getTunnel(parentTwi)->iLevel)) {
						parentTwi = parentTwi->parent();
					}
					if(parentTwi != NULL) {
						tunnel->twi = new QTreeWidgetItem( parentTwi );
					} else {
						tunnel->twi = new QTreeWidgetItem( ui.treeTunnels );
                    }
				}
			} else {
				tunnel->twi = new QTreeWidgetItem( ui.treeTunnels );
			}

            if(!tunnel->strFgColor.isEmpty()) {
                QColor color = QColor(tunnel->strFgColor);
                if(color.isValid()) {
                    QBrush brush(color);
                    tunnel->twi->setForeground(0,brush);
                }
            }

            if(!tunnel->strBgColor.isEmpty()) {
                QColor color = QColor(tunnel->strBgColor);
                if(color.isValid()) {
                    QBrush brush(color);
                    tunnel->twi->setBackground(0,brush);
                }
            }

			tunnel->twi->setFlags(tunnel->twi->flags() | Qt::ItemIsEditable);
			tunnel->twi->setExpanded(tunnel->bIsExpanded);
			tunnel->twi->setText( 0, tunnel->strName );
			setTunnel(tunnel->twi, tunnel);
			treeTunnelUpdateFromParent(tunnel->twi);
			lastTwi = tunnel->twi;
			lastLevel = tunnel->iLevel;
			
			if(tunnel->iType == TUNNEL_TYPE_TUNNEL)	{
				treeTunnelSetIcon(tunnel->twi, Images::icon("ht_120_host_disconnected"));
			} else if(tunnel->iType == TUNNEL_TYPE_FOLDER) {
				if(tunnel->twi->isExpanded()) {
					treeTunnelSetIcon(tunnel->twi, Images::icon("ht_150_folder_open_disconnected"));
				} else {
					treeTunnelSetIcon(tunnel->twi, Images::icon("ht_170_folder_closed_disconnected"));
				}
			}

		} // for each -- Tunnel
	}

	//--- Hosts

	//+++ Executable variables

	//In case user opens an export file, then this ini file (DoffenSSHTunnel.ini) might
	//contain Executable variables. We merge those into the App settings (DoffenSSHTunnelApp.ini)

	QList<VariableStruct> execVarList = readSettingsExecutableVariables(settings);
	if(execVarList.size() > 0) {
		//Found [VarExecutables] inside our ini file
		//Figure out which ones are actually in use
		QList<VariableStruct> execVarListInUse = getVariablesUsedInTunnels(ui.treeTunnels, execVarList);
		importVariables(execVarListInUse);
	}

	//--- Executable variables

	ui.treeTunnels->blockSignals(blocked);

	if ( ui.treeTunnels->topLevelItemCount() > 0 )
	{
		QTreeWidgetItem *twi = ui.treeTunnels->topLevelItem( 0 );
		ui.treeTunnels->setCurrentItem( twi );
	}

	if(bUpdateIniFile) {
		settings.sync();
		writeSettings();
		autoBackupSettings( BackupInfo( BACKUP_AFTER_UPGRADE,"Upgraded settings to new version [%1]", APP_VERSION ) );
	}

    m_pMainWindow->m_pSearchWidget->slotIndex();
}

Tunnel_c* ATSkeletonWindow::readSettingsHost(QSettings &settings)
{
	Tunnel_c* tunnel = new Tunnel_c();

	QString sUuid = settings.value("uuid","").toString().trimmed();
	if(!sUuid.isEmpty()) {
		//uuid found
		tunnel->uUid = QUuid(sUuid);
		if(tunnel->uUid.isNull()) { 
			//invalid uuid format in .ini file 
			tunnel->uUid = QUuid::createUuid(); 
		}
	} else {
		//uuid not found or empty
		tunnel->uUid = QUuid::createUuid();
	}

    tunnel->strExtID          = settings.value("ExtID").toString().trimmed();

    tunnel->strName           = settings.value( "Name" ).toString().trimmed();

	//default 0 = TUNNEL_TYPE_TUNNEL, 5 = TUNNEL_TYPE_FOLDER
	tunnel->iType	          = settings.value( "Type", "0" ).toInt();

	int customActionsCount = settings.beginReadArray("CustomActions");
	for(int j=0;j<customActionsCount;j++) {
		settings.setArrayIndex(j);
		CustomActionStruct customActionStruct;
		sUuid = settings.value("uuid","").toString().trimmed();
		if(!sUuid.isEmpty()) {
			//uuid found
			customActionStruct.uUid = QUuid(sUuid);
			if(customActionStruct.uUid.isNull()) { 
				//invalid uuid format in .ini file 
				customActionStruct.uUid = QUuid::createUuid(); 
			}
		} else {
			//uuid not found or empty
			customActionStruct.uUid = QUuid::createUuid();
		}
		customActionStruct.sLabel = settings.value("label","").toString();
		customActionStruct.sCmd = settings.value("cmd","").toString();
		customActionStruct.iType = settings.value("type",CUSTOM_ACTION_TYPE_COMMAND).toInt();
		customActionStruct.iLevel = settings.value("level",0).toInt();
		customActionStruct.bExpanded = settings.value("expanded",true).toBool();
		tunnel->customActionList.append(customActionStruct);
	}
	settings.endArray();

	if(tunnel->iType == TUNNEL_TYPE_TUNNEL)
	{
		tunnel->setSSHHostList(settings.value( "SSHHostList", settings.value( "SSHHost" ) ).toString());	
		tunnel->setSelectedSSHHost(settings.value( "SSHHost" ).toString());

		tunnel->setRemoteHostList(settings.value( "RemoteHostList", settings.value( "RemoteHost" ) ).toString());	
		tunnel->setSelectedRemoteHost(settings.value( "RemoteHost" ).toString());

		tunnel->strUsername       = settings.value( "Username" ).toString();	

		tunnel->strPassword       = settings.value( "Password" ).toString();
		tunnel->strSSHKeyFilePassword    = settings.value( "SSHKeyFilePassword" ).toString();
		tunnel->strSSHKeyFile     = settings.value( "SSHKeyFile" ).toString();
		tunnel->strExtraArguments = settings.value( "ExtraArguments" ).toString();
		tunnel->strLocalIP	      = settings.value( "LocalIP","localhost" ).toString();
		tunnel->iLocalPort        = settings.value( "LocalPort" ).toInt();
		tunnel->iRemotePort       = settings.value( "RemotePort" ).toInt();
		tunnel->iDirection        = settings.value( "Direction" ).toInt();
		tunnel->bAutoConnect      = settings.value( "AutoConnect" ).toBool();
		tunnel->bCompression      = settings.value( "Compression" ).toBool();
		tunnel->bDoKeepAlivePing  = settings.value( "DoKeepAlivePing" ).toBool();
		tunnel->bAutoReconnect    = settings.value( "AutoReconnect" ).toBool();
        tunnel->iSSH1or2          = settings.value( "SSH1or2" ).toInt();
        if(tunnel->iSSH1or2 != 1 && tunnel->iSSH1or2 != 2) {
            tunnel->iSSH1or2 = 2;
        }

		int portForwardCount = settings.beginReadArray("PortForward");
		for(int j=0;j<portForwardCount;j++) {
			settings.setArrayIndex(j);
			PortForwardStruct portForwardStruct;
			sUuid = settings.value("uuid","").toString().trimmed();
			if(!sUuid.isEmpty()) {
				//uuid found
				portForwardStruct.uUid = QUuid(sUuid);
				if(portForwardStruct.uUid.isNull()) { 
					//invalid uuid format in .ini file 
					portForwardStruct.uUid = QUuid::createUuid(); 
				}
			} else {
				//uuid not found or empty
				portForwardStruct.uUid = QUuid::createUuid();
			}
			portForwardStruct.bEnabled = settings.value("enabled",true).toBool();
			portForwardStruct.nType = settings.value("type",PortForwardStruct::LOCAL).toInt();
			portForwardStruct.strName = settings.value("name","").toString();
			portForwardStruct.strLocalIP = settings.value("lip","localhost").toString();
			portForwardStruct.nLocalPort = settings.value("lport",0).toInt();
			portForwardStruct.strDestinationHost = settings.value("dhost","").toString();
			portForwardStruct.nDestinationPort = settings.value("dport",0).toInt();
			portForwardStruct.strDescription = settings.value("descr","").toString();
			tunnel->portForwardList.append(portForwardStruct);
		}
		settings.endArray();
	}
    else if(tunnel->iType == TUNNEL_TYPE_FOLDER)
    {
        tunnel->strChildNodesCommand = settings.value("ChildNodesCommand","").toString();
        tunnel->bActivateDisconnects = settings.value( "ActivateDisconnects", true ).toBool();
    }

	tunnel->iLevel            = settings.value( "Level" ).toInt();	
	tunnel->bIsExpanded		  = settings.value( "Expanded", true ).toBool();
    tunnel->strFgColor = settings.value("FgColor","").toString().trimmed(); 
    tunnel->strBgColor = settings.value("BgColor","").toString().trimmed();
    tunnel->bChildNodesCommandType = settings.value("ChildNodesCommandType",false).toBool();

	return tunnel;
}

Tunnel_c* ATSkeletonWindow::readSettingsHost(QJsonObject &json)
{
    Tunnel_c* tunnel = new Tunnel_c();

    QString sUuid = json.value("uuid").toString().trimmed();
    if(!sUuid.isEmpty()) {
        //uuid found
        tunnel->uUid = QUuid(sUuid);
        if(tunnel->uUid.isNull()) {
            //invalid uuid format in .ini file
            tunnel->uUid = QUuid::createUuid();
        }
    } else {
        //uuid not found or empty
        tunnel->uUid = QUuid::createUuid();
    }

    tunnel->strExtID          = json.value("ExtID").toString().trimmed();

    tunnel->strName           = json.value( "Name" ).toString().trimmed();

    //default 0 = TUNNEL_TYPE_TUNNEL, 5 = TUNNEL_TYPE_FOLDER
    tunnel->iType	          = json.value( "Type" ).toInt(0);

    QJsonArray jsonActions = json["CustomActions"].toArray();
    for(int j=0;j<jsonActions.size();j++) {
        CustomActionStruct customActionStruct;
        QJsonObject jsonAction = jsonActions[j].toObject();
        sUuid = jsonAction.value("uuid").toString().trimmed();
        if(!sUuid.isEmpty()) {
            //uuid found
            customActionStruct.uUid = QUuid(sUuid);
            if(customActionStruct.uUid.isNull()) {
                //invalid uuid format in .ini file
                customActionStruct.uUid = QUuid::createUuid();
            }
        } else {
            //uuid not found or empty
            customActionStruct.uUid = QUuid::createUuid();
        }
        customActionStruct.sLabel = jsonAction.value("label").toString();
        customActionStruct.sCmd = jsonAction.value("cmd").toString();
        customActionStruct.iType = jsonAction.value("type").toInt(CUSTOM_ACTION_TYPE_COMMAND);
        customActionStruct.iLevel = jsonAction.value("level").toInt(0);
        customActionStruct.bExpanded = jsonAction.value("expanded").toBool(true);
        tunnel->customActionList.append(customActionStruct);
    }

    if(tunnel->iType == TUNNEL_TYPE_TUNNEL)
    {
        QString sshHost = json.value( "SSHHost" ).toString();
        tunnel->setSSHHostList(json.value( "SSHHostList" ).toString( sshHost ));
        tunnel->setSelectedSSHHost(sshHost);

        QString remoteHost = json.value( "RemoteHost" ).toString();
        tunnel->setRemoteHostList(json.value( "RemoteHostList").toString( remoteHost ));
        tunnel->setSelectedRemoteHost(remoteHost);

        tunnel->strUsername       = json.value( "Username" ).toString();

        tunnel->strPassword       = json.value( "Password" ).toString();
        tunnel->strSSHKeyFilePassword    = json.value( "SSHKeyFilePassword" ).toString();
        tunnel->strSSHKeyFile     = json.value( "SSHKeyFile" ).toString();
        tunnel->strExtraArguments = json.value( "ExtraArguments" ).toString("-N");
        tunnel->strLocalIP	      = json.value( "LocalIP" ).toString("localhost");
        tunnel->iLocalPort        = json.value( "LocalPort" ).toInt();
        tunnel->iRemotePort       = json.value( "RemotePort" ).toInt();
        tunnel->iDirection        = json.value( "Direction" ).toInt();
        tunnel->bAutoConnect      = json.value( "AutoConnect" ).toBool();
        tunnel->bCompression      = json.value( "Compression" ).toBool();
        tunnel->bDoKeepAlivePing  = json.value( "DoKeepAlivePing" ).toBool();
        tunnel->bAutoReconnect    = json.value( "AutoReconnect" ).toBool();
        tunnel->iSSH1or2          = json.value( "SSH1or2" ).toInt(2);
        if(tunnel->iSSH1or2 != 1 && tunnel->iSSH1or2 != 2) {
            tunnel->iSSH1or2 = 2;
        }

        QJsonArray jsonPortForwards = json["PortForward"].toArray();
        for(int j=0;j<jsonPortForwards.size();j++) {
            PortForwardStruct portForwardStruct;
            QJsonObject jsonForward = jsonPortForwards[j].toObject();
            sUuid = jsonForward.value("uuid").toString().trimmed();
            if(!sUuid.isEmpty()) {
                //uuid found
                portForwardStruct.uUid = QUuid(sUuid);
                if(portForwardStruct.uUid.isNull()) {
                    //invalid uuid format in .ini file
                    portForwardStruct.uUid = QUuid::createUuid();
                }
            } else {
                //uuid not found or empty
                portForwardStruct.uUid = QUuid::createUuid();
            }
            portForwardStruct.bEnabled = jsonForward.value("enabled").toBool(true);
            portForwardStruct.nType = jsonForward.value("type").toInt(PortForwardStruct::LOCAL);
            portForwardStruct.strName = jsonForward.value("name").toString();
            portForwardStruct.strLocalIP = jsonForward.value("lip").toString("localhost");
            portForwardStruct.nLocalPort = jsonForward.value("lport").toInt(0);
            portForwardStruct.strDestinationHost = jsonForward.value("dhost").toString();
            portForwardStruct.nDestinationPort = jsonForward.value("dport").toInt(0);
            portForwardStruct.strDescription = jsonForward.value("descr").toString();
            tunnel->portForwardList.append(portForwardStruct);
        }
    } else if(tunnel->iType == TUNNEL_TYPE_FOLDER) {
        tunnel->strChildNodesCommand = json.value("ChildNodesCommand").toString();
        tunnel->bActivateDisconnects  = json.value( "ActivateDisconnects" ).toBool(true);
    }

    tunnel->iLevel            = json.value( "Level" ).toInt();
    tunnel->bIsExpanded		  = json.value( "Expanded" ).toBool(true);
    tunnel->strFgColor = json.value("FgColor").toString("").trimmed();
    tunnel->strBgColor = json.value("BgColor").toString("").trimmed();
    tunnel->bChildNodesCommandType = json.value("ChildNodesCommandType").toBool(false);

    return tunnel;
}

void ATSkeletonWindow::readAppSettings()
{
	qDebug() << "readAppSettings()";

	QSettings settings( g_strAppIniFile, QSettings::IniFormat);

	//+++ VarExecutables

	m_listExecutableVariables.clear();
	m_listExecutableVariables.append(readSettingsExecutableVariables(settings));



        #ifdef Q_OS_WIN

            VariableStruct puttyVarStruct = VariableStruct(QUuid::createUuid(),"SSH Term","putty","${appdir}/putty.exe",
                "-loghost \"${name} - ${rhost}\"",VariableStruct::EXECUTABLE);
            VariableStruct plinkVarStruct = VariableStruct(QUuid::createUuid(),"Putty Plink","plink","${appdir}/plink.exe",
                "",VariableStruct::EXECUTABLE);
            VariableStruct winscpVarStruct = VariableStruct(QUuid::createUuid(),"WinSCP","winscp","${appdir}/winscp.exe",
                "",VariableStruct::EXECUTABLE);
            VariableStruct filezillaVarStruct = VariableStruct(QUuid::createUuid(),"Alias","filezilla","${winscp}",
                "",VariableStruct::EXECUTABLE);
            VariableStruct sshVarStruct = VariableStruct(QUuid::createUuid(),"Alias","ssh","${putty}",
                "",VariableStruct::EXECUTABLE);

            if(m_listExecutableVariables.size() == 0) {

                QString chromeExe = QString("C:/Program Files/Google/Chrome/Application/chrome.exe");
                QString firefoxExe = "C:/Program Files/Mozilla Firefox/firefox.exe";
                QString iexploreExe = "C:/Program Files/Internet Explorer/iexplore.exe";
                QString consoleExecVar = QString("${appdir}/exec2.bat");

                m_listExecutableVariables.append(
                    VariableStruct(QUuid::createUuid(),"Firefox","firefox",firefoxExe,"",VariableStruct::EXECUTABLE) );
                m_listExecutableVariables.append(
                    VariableStruct(QUuid::createUuid(),"Google Chrome","chrome",chromeExe,"",VariableStruct::EXECUTABLE) );
		
            }

            bool foundPlink = false;
            bool foundPutty = false;
            bool foundWinscp = false;
            bool foundFilezilla = false;
            bool foundSSH = false;
            for(int i = 0; i < m_listExecutableVariables.size(); i++) {
                VariableStruct var = m_listExecutableVariables.at(i);
                QString name = var.strName.trimmed().toLower();
                if(name == "plink") {
                    foundPlink = true;
                } else if(name == "putty") {
                    foundPutty = true;
                } else if(name == "winscp") {
                    foundWinscp = true;
                } else if(name == "filezilla") {
                    foundFilezilla = true;
                } else if(name == "ssh") {
                    foundSSH = true;
                }
            }

            if(!foundPutty) {
                m_listExecutableVariables.append(puttyVarStruct);
            }
            if(!foundSSH) {
                m_listExecutableVariables.append(sshVarStruct);
            }
            if(!foundWinscp) {
                m_listExecutableVariables.append(winscpVarStruct);
            }
            if(!foundFilezilla) {
                m_listExecutableVariables.append(filezillaVarStruct);
            }
            if(!foundPlink) {
                m_listExecutableVariables.append(plinkVarStruct);
            }


        #endif
        #ifdef Q_OS_MACOS

            VariableStruct exec1VarStruct = VariableStruct(QUuid::createUuid(),
               "Executes a command. If the command fails then it will wait for you to press a key so you can see the error message.",
               "exec1", "${appdir}/../Resources/exec1.sh", "",
                VariableStruct::EXECUTABLE);

            VariableStruct exec2VarStruct = VariableStruct(QUuid::createUuid(),
                "Executes a command and then it waits for you to press a key so you can see the output.",
                "exec2", "${appdir}/../Resources/exec2.sh", "",
                VariableStruct::EXECUTABLE);

            VariableStruct exec3VarStruct = VariableStruct(QUuid::createUuid(),
                "Executes a command and lets you navigate/browse its output.",
                "exec3", "${appdir}/../Resources/exec3.sh", "",
                VariableStruct::EXECUTABLE);

            VariableStruct consoleExecVarStruct = VariableStruct(QUuid::createUuid(),
                 "Executes a command in the console/terminal and lets you see its output.",
                 "console_exec", "${appdir}/../Resources/iterm2win.sh", "\"${name} - ${rhost}\" ${exec2}",
                 VariableStruct::EXECUTABLE);

            VariableStruct consoleExecViewVarStruct = VariableStruct(QUuid::createUuid(),
                "Executes a command in the console/terminal and lets you navigate/browse its output.",
                "console_exec_view", "${appdir}/../Resources/iterm2win.sh", "\"${name} - ${rhost}\" ${exec3}",
                VariableStruct::EXECUTABLE);

            VariableStruct safariVarStruct = VariableStruct(QUuid::createUuid(),
                "Safari",
                "safari","/usr/bin/open","-a Safari",
                VariableStruct::EXECUTABLE);

            VariableStruct chromeVarStruct = VariableStruct(QUuid::createUuid(),
                "Google Chrome",
                "chrome","/usr/bin/open","-a \"Google Chrome\"",
                VariableStruct::EXECUTABLE);

            VariableStruct firefoxVarStruct = VariableStruct(QUuid::createUuid(),
                "Firefox",
                "firefox","/usr/bin/open","-a Firefox",
                VariableStruct::EXECUTABLE);

            VariableStruct iexploreVarStruct = VariableStruct(QUuid::createUuid(),
                "Alias",
                "iexplore","${safari}","",
                VariableStruct::EXECUTABLE);

            VariableStruct filezillaVarStruct = VariableStruct(QUuid::createUuid(),
                "FileZilla",
                "filezilla","/usr/bin/open","-na FileZilla --args -l interactive",
                VariableStruct::EXECUTABLE);

            VariableStruct winscpVarStruct = VariableStruct(QUuid::createUuid(),
                "Alias",
                "winscp","${filezilla}","",
                VariableStruct::EXECUTABLE);

            VariableStruct sshTermVarStruct = VariableStruct(QUuid::createUuid(),
                "SSH Term",
                "ssh","${appdir}/../Resources/iterm2win.sh","\"${name} - ${rhost}\" ssh -oHostKeyAlgorithms=+ssh-dss",
                VariableStruct::EXECUTABLE);

            VariableStruct puttVarStruct = VariableStruct(QUuid::createUuid(),
                "Alias",
                "putty","${ssh}","",
                VariableStruct::EXECUTABLE);


            //VarStruct, exists
            QList<QPair<VariableStruct,bool>> ensureTheseExecutableVariablesExists;
            ensureTheseExecutableVariablesExists.append(QPair<VariableStruct,bool>(exec1VarStruct,false));
            ensureTheseExecutableVariablesExists.append(QPair<VariableStruct,bool>(exec2VarStruct,false));
            ensureTheseExecutableVariablesExists.append(QPair<VariableStruct,bool>(exec3VarStruct,false));
            ensureTheseExecutableVariablesExists.append(QPair<VariableStruct,bool>(consoleExecVarStruct,false));
            ensureTheseExecutableVariablesExists.append(QPair<VariableStruct,bool>(consoleExecViewVarStruct,false));
            ensureTheseExecutableVariablesExists.append(QPair<VariableStruct,bool>(safariVarStruct,false));
            ensureTheseExecutableVariablesExists.append(QPair<VariableStruct,bool>(chromeVarStruct,false));
            ensureTheseExecutableVariablesExists.append(QPair<VariableStruct,bool>(firefoxVarStruct,false));
            ensureTheseExecutableVariablesExists.append(QPair<VariableStruct,bool>(iexploreVarStruct,false));
            ensureTheseExecutableVariablesExists.append(QPair<VariableStruct,bool>(filezillaVarStruct,false));
            ensureTheseExecutableVariablesExists.append(QPair<VariableStruct,bool>(winscpVarStruct,false));
            ensureTheseExecutableVariablesExists.append(QPair<VariableStruct,bool>(sshTermVarStruct,false));
            ensureTheseExecutableVariablesExists.append(QPair<VariableStruct,bool>(puttVarStruct,false));

            //set exists flag
            for(int j = 0; j < ensureTheseExecutableVariablesExists.size(); j++) {
                VariableStruct varThatShallExist = ensureTheseExecutableVariablesExists.at(j).first;
                QString nameThatShallExist = varThatShallExist.strName.trimmed().toLower();
                for(int i = 0; i < m_listExecutableVariables.size(); i++) {
                    VariableStruct varThatExists = m_listExecutableVariables.at(i);
                    QString nameThatExists = varThatExists.strName.trimmed().toLower();
                    if(nameThatExists == nameThatShallExist) {
                        ensureTheseExecutableVariablesExists.replace(j,QPair<VariableStruct,bool>(exec1VarStruct,true));
                    }
                }
            }

            //check exists flag
            for(int j = 0; j < ensureTheseExecutableVariablesExists.size(); j++) {
                if(!ensureTheseExecutableVariablesExists.at(j).second) {
                    //the bool flag is false (it does not exist) so add the VariableStruct
                    m_listExecutableVariables.append(ensureTheseExecutableVariablesExists.at(j).first);
                }
            }


        #endif
        #ifdef Q_OS_LINUX

            if(m_listExecutableVariables.size() == 0) {

                m_listExecutableVariables.append(
                    VariableStruct(QUuid::createUuid(),
                                   "Executes a command. If the command fails then it will wait for you to press a key so you can see the error message.",
                                   "exec1",
                                   "${appdir}/exec1.sh", "",
                                   VariableStruct::EXECUTABLE) );
                m_listExecutableVariables.append(
                    VariableStruct(QUuid::createUuid(),
                                   "Firefox",
                                   "firefox",
                                   "/usr/bin/firefox", "",
                                   VariableStruct::EXECUTABLE) );
                m_listExecutableVariables.append(
                    VariableStruct(QUuid::createUuid(),"Alias","safari","${firefox}","",VariableStruct::EXECUTABLE) );
                m_listExecutableVariables.append(
                    VariableStruct(QUuid::createUuid(),"Alias","chrome","${firefox}","",VariableStruct::EXECUTABLE) );
                m_listExecutableVariables.append(
                    VariableStruct(QUuid::createUuid(),"Alias","iexplore","${firefox}","",VariableStruct::EXECUTABLE) );
                m_listExecutableVariables.append(
                    VariableStruct(QUuid::createUuid(),
                                   "SSH Term",
                                   "ssh",
                                   "/usr/bin/terminator", "--title \"${name} - ${rhost}\" --execute ${exec1} /usr/bin/ssh",
                                   VariableStruct::EXECUTABLE) );
                m_listExecutableVariables.append(
                    VariableStruct(QUuid::createUuid(),"Alias","putty","${ssh}","",VariableStruct::EXECUTABLE) );
                m_listExecutableVariables.append(
                    VariableStruct(QUuid::createUuid(),
                                   "FileZilla",
                                   "filezilla",
                                   "/usr/bin/filezilla", "-l interactive",
                                   VariableStruct::EXECUTABLE) );
                m_listExecutableVariables.append(
                    VariableStruct(QUuid::createUuid(),"Alias","winscp","${filezilla}","",VariableStruct::EXECUTABLE) );

            }

		#endif
	
	//--- VarExecutables
}

void ATSkeletonWindow::delayWriteSettings()
{
    qDebug( "%s", Q_FUNC_INFO );
	m_timerDelayWriteSettings.stop();
	m_timerDelayWriteSettings.start();
}

void ATSkeletonWindow::slotDelayWriteSettings()
{
    qDebug( "%s", Q_FUNC_INFO );
	m_timerDelayWriteSettings.stop();
	writeSettings();
}

void ATSkeletonWindow::writeSettings()
{
    qDebug( "writeSettings() g_strTunnelIniFile: %s", qPrintable( g_strTunnelIniFile ) );

	QSettings settings( g_strTunnelIniFile, QSettings::IniFormat);

	settings.setValue( "Version", APP_VERSION );

	//User Defined Variables
	writeSettingsUserDefinedVariables(settings, m_listUserDefinedVariables);

	//Tunnels
	writeSettingsTreeWidget(settings, ui.treeTunnels);

	settings.sync();

    m_pMainWindow->m_pSearchWidget->slotIndex();
}

void ATSkeletonWindow::exportSettingsTunnelList(QString filePath, QList<Tunnel_c*> &tunnelList)
{
	QSettings settings( filePath, QSettings::IniFormat);

	settings.setValue( "Version", APP_VERSION );

	//Find variables used in the tunnels
	QList<VariableStruct> varList;
	varList.append( getVariablesUsedInTunnels(tunnelList,m_listExecutableVariables) );
	varList.append( getVariablesUsedInTunnels(tunnelList,m_listUserDefinedVariables) );

    replaceExecutableVariables(varList);

	writeSettingsExecutableVariables(settings, varList);
	writeSettingsUserDefinedVariables(settings, varList);

	//Tunnels
	writeSettingsTunnelList(settings, tunnelList);

	settings.sync();
}

QList<VariableStruct> ATSkeletonWindow::getVariablesUsedInTunnels(QTreeWidget *tunnelTreeWidget, QList<VariableStruct> &varCheckList)
{
    QList<QTreeWidgetItem*> treeTunnelItems = tunnelTreeWidget->findItems(".*", Qt::MatchFlags(Qt::MatchRegularExpression | Qt::MatchRecursive), 0);
	QList<Tunnel_c*> tunnelList;
	for(int i=0;i<treeTunnelItems.size();i++)
	{
		QTreeWidgetItem *treeTunnelItem = treeTunnelItems[i];
		Tunnel_c *pt = getTunnel(treeTunnelItem);
		ATASSERT(pt);
		if(pt->iType == TUNNEL_TYPE_TUNNEL) {
			tunnelList.append(pt);
		}
	}
	return getVariablesUsedInTunnels(tunnelList, varCheckList);
}

QList<VariableStruct> ATSkeletonWindow::getVariablesUsedInTunnels(QList<Tunnel_c*> &tunnelList, QList<VariableStruct> &varCheckList)
{
	QList<VariableStruct> varList;

	//Variable usage in tunnels
	for(int i=0;i<tunnelList.size();i++) {
		Tunnel_c *pt = tunnelList.at(i);
		ATASSERT(pt);
		
        for(int j=0;j<varCheckList.size();j++) {
            VariableStruct var = varCheckList.at(j);
			QString name = var.strName.trimmed();
			if(name.isEmpty()) {
				continue;
			}
			VariableStatTunnelStruct varStatTunnel = getVariableUsageInTunnel(pt,name);
			if(varStatTunnel.totalUsageCount() > 0) {
				if(!variableNameExists(var, varList)) {
					varList.append(var);
				}
			}
		}

	} //end - for(int i=0;i<tunnelList.size();i++)

	return varList;
}

void ATSkeletonWindow::importSettingsTunnel(QTreeWidgetItem *twiImportAt, int importMode)
{
	enableTreeTunnelsPaste(false);

	QString filePath = m_pImportHostsDialog->fileDialog();
	if(filePath.isEmpty()) {
		return;
	}
	if(!QFile::exists(filePath)) {
			QMessageBox::warning(this, "Oops!", "The file does not exist !\n"
				+filePath);
		return;
	}

	m_pImportHostsDialog->show(filePath, twiImportAt, importMode);
}

void ATSkeletonWindow::importTunnels(QList<Tunnel_c*> &tunnelList, QTreeWidgetItem *twiImportAt, int importMode)
{
	bool blocked = ui.treeTunnels->blockSignals(true);

	int levelOffset = 0;
	int lastLevel = 0;
	QTreeWidgetItem* lastTwi = NULL;
	if(importMode == IMPORT_AFTER) {
		lastTwi = twiImportAt;
		lastLevel = TreeWidget::getItemLevel(lastTwi);
		levelOffset = lastLevel;
	} else if(importMode == IMPORT_CHILD) {
		lastTwi = twiImportAt;
		lastLevel = TreeWidget::getItemLevel(lastTwi);
		levelOffset = lastLevel + 1;
	}

	QList<Tunnel_c*> tunnelListCreated;

	//Find current ports in use
	QMap<int,bool> portMap;
    QList<QTreeWidgetItem*> treeTunnelItems = ui.treeTunnels->findItems(".*", Qt::MatchFlags(Qt::MatchRegularExpression | Qt::MatchRecursive), 0);
	for(int i=0;i<treeTunnelItems.size();i++) {
		Tunnel_c* pt = getTunnel(treeTunnelItems[i]);
		ATASSERT(pt);
		portMap.insert(pt->iLocalPort, true);
	}

	QMap<QTreeWidgetItem*,int> mapLastChildIndex;
	int lastTopLevelIndex = -1;

	for(int i=0;i<tunnelList.size();i++) {
		Tunnel_c *tunnelImport = tunnelList.at(i);
		Tunnel_c *tunnel = new Tunnel_c();
		tunnel->copyFrom(tunnelImport);
		tunnel->iLevel += levelOffset;
		tunnelListCreated.append(tunnel);
		if(lastTwi != NULL && tunnel->iLevel > 0) {
			if(tunnel->iLevel > lastLevel) {
				tunnel->twi = new QTreeWidgetItem( lastTwi );
				mapLastChildIndex.insert(lastTwi, lastTwi->indexOfChild(tunnel->twi));
			} else if(tunnel->iLevel == lastLevel) {
				tunnel->twi = new QTreeWidgetItem();
				QTreeWidgetItem* parentTwi = lastTwi->parent();
				if(parentTwi != NULL) {
					int lastIndex = -1;
					if(mapLastChildIndex.contains(parentTwi)) lastIndex = mapLastChildIndex[parentTwi];
					else lastIndex = parentTwi->indexOfChild(lastTwi);
					parentTwi->insertChild(lastIndex+1, tunnel->twi);
					mapLastChildIndex.insert(parentTwi,lastIndex+1);
				} else {
					int lastIndex = -1;
					if(lastTopLevelIndex != -1) lastIndex = lastTopLevelIndex;
					else lastIndex = ui.treeTunnels->indexOfTopLevelItem(lastTwi);
					ui.treeTunnels->insertTopLevelItem(lastIndex+1, tunnel->twi);
					lastTopLevelIndex = lastIndex + 1;
				}
			} else {
				QTreeWidgetItem* parentTwi = lastTwi->parent();
				while(parentTwi != NULL && (tunnel->iLevel <= getTunnel(parentTwi)->iLevel)) {
					parentTwi = parentTwi->parent();
				}
				tunnel->twi = new QTreeWidgetItem();
				if(parentTwi != NULL) {
					int lastIndex = -1;
					if(mapLastChildIndex.contains(parentTwi)) lastIndex = mapLastChildIndex[parentTwi];
					else lastIndex = parentTwi->indexOfChild(lastTwi);
					parentTwi->insertChild(lastIndex+1, tunnel->twi);
					mapLastChildIndex.insert(parentTwi,lastIndex+1);
				} else {
					int lastIndex = -1;
					if(lastTopLevelIndex != -1) lastIndex = lastTopLevelIndex;
					else lastIndex = ui.treeTunnels->indexOfTopLevelItem(lastTwi);
					ui.treeTunnels->insertTopLevelItem(lastIndex+1, tunnel->twi);
					lastTopLevelIndex = lastIndex + 1;
				}
			}
		} else {
			tunnel->twi = new QTreeWidgetItem();
			int lastIndex = -1;
			if(lastTopLevelIndex != -1) lastIndex = lastTopLevelIndex;
			else lastIndex = ui.treeTunnels->indexOfTopLevelItem(lastTwi);
			ui.treeTunnels->insertTopLevelItem(lastIndex+1, tunnel->twi);
			lastTopLevelIndex = lastIndex + 1;
		}

		tunnel->twi->setExpanded(false);

		if(tunnel->iType == TUNNEL_TYPE_TUNNEL)	{
			treeTunnelSetIcon(tunnel->twi, Images::icon("ht_120_host_disconnected"));
		} else if(tunnel->iType == TUNNEL_TYPE_FOLDER) {
			if(tunnel->twi->isExpanded()) {
				treeTunnelSetIcon(tunnel->twi, Images::icon("ht_150_folder_open_disconnected"));
			} else {
				treeTunnelSetIcon(tunnel->twi, Images::icon("ht_170_folder_closed_disconnected"));
			}
		}

        if(!tunnel->strFgColor.isEmpty()) {
            QColor color = QColor(tunnel->strFgColor);
            if(color.isValid()) {
                QBrush brush = tunnel->twi->foreground(0);
                brush.setColor(color);
                tunnel->twi->setForeground(0,brush);
            }
        }

		tunnel->twi->setFlags(tunnel->twi->flags() | Qt::ItemIsEditable);
		tunnel->twi->setText( 0, tunnel->strName );
		setTunnel(tunnel->twi, tunnel);
		treeTunnelUpdateFromParent(tunnel->twi);
		lastTwi = tunnel->twi;
		lastLevel = tunnel->iLevel;

		if(portMap.contains(tunnel->iLocalPort)) {
			int newPort = proposeNewLocalPort(tunnel->twi);
			tunnel->iLocalPort = newPort;
			portMap.insert(newPort,true);
		}

	} // end - for each Tunnel

	ui.treeTunnels->blockSignals(blocked);
}

void ATSkeletonWindow::slotLoadIcons()
{
	//tree
	int iConnectedCount = 0;
    QList<QTreeWidgetItem*> treeTunnelItems = ui.treeTunnels->findItems(".*", Qt::MatchFlags(Qt::MatchRegularExpression | Qt::MatchRecursive), 0);
	for(int i=0;i<treeTunnelItems.size();i++) {
		QTreeWidgetItem *twi = treeTunnelItems[i];
		Tunnel_c* tunnel = getTunnel(twi);
		ATASSERT(tunnel);
		if(tunnel == NULL) continue;
		if ( tunnel->iConnectStatus == CONNECTED ) iConnectedCount++;
		if(tunnel->iType == TUNNEL_TYPE_TUNNEL)	{
			if(tunnel->iConnectStatus == CONNECTING || tunnel->iConnectStatus == MARKCONNECT) {
				treeTunnelSetIcon(tunnel->twi, Images::icon("ht_130_host_connecting"));
			} else if(tunnel->iConnectStatus == CONNECTED) {
				treeTunnelSetIcon(tunnel->twi, Images::icon("ht_140_host_connected"));
			} else {
				treeTunnelSetIcon(tunnel->twi, Images::icon("ht_120_host_disconnected"));
			}
		} else if(tunnel->iType == TUNNEL_TYPE_FOLDER) {
			if(twi->isExpanded()) {
				if(tunnel->iConnectStatus == CONNECTED) {
					treeTunnelSetIcon(tunnel->twi, Images::icon("ht_160_folder_open_connected"));
				} else {
					treeTunnelSetIcon(tunnel->twi, Images::icon("ht_150_folder_open_disconnected"));
				}
			} else {
				if(tunnel->iConnectStatus == CONNECTED) {
					treeTunnelSetIcon(tunnel->twi, Images::icon("ht_180_folder_closed_connected"));
				} else {
					treeTunnelSetIcon(tunnel->twi, Images::icon("ht_170_folder_closed_disconnected"));
				}
			}
		}
	}
	//window and tray icon	
	if ( iConnectedCount == 0 ) {
		emit signalSetTrayIcon( 0 );
	} else {
		emit signalSetTrayIcon( 1 );
	}

	ui.btnAddFolder->setIcon(Images::icon("ht_010_btn_add_folder"));
	ui.btnAddTunnel->setIcon(Images::icon("ht_020_btn_add_host"));
	ui.btnDuplicate->setIcon(Images::icon("ht_030_btn_dup"));
	ui.btnDeleteTunnel->setIcon(Images::icon("ht_040_btn_del"));
	ui.btnLeftTunnel->setIcon(Images::icon("ht_080_btn_left"));
	ui.btnUpTunnel->setIcon(Images::icon("ht_090_btn_up"));
	ui.btnDownTunnel->setIcon(Images::icon("ht_100_btn_down"));
	ui.btnRightTunnel->setIcon(Images::icon("ht_110_btn_right"));

	//connect tab
	ui.btnConnect->setIcon(Images::icon("ht_050_btn_connect"));
	ui.btnDisconnect->setIcon(Images::icon("ht_060_btn_disconnect"));
    m_pLogMenuClearAction->setIcon(Images::icon("ht_070_btn_clearlog"));
	ui.widgetCustomActions->slotLoadIcons();

	//edit tab
	ui.btnMoreTunnelsAdd->setIcon(Images::icon("mt_010_btn_add"));
	ui.btnMoreTunnelsDelete->setIcon(Images::icon("mt_020_btn_del"));
	ui.btnMoreTunnelsMoveUp->setIcon(Images::icon("mt_030_btn_up"));
	ui.btnMoreTunnelsMoveDown->setIcon(Images::icon("mt_040_btn_down"));

    //search box
    ui.widgetTreeTunnelSearch_2->slotLoadIcons();

}

void ATSkeletonWindow::importVariables(QList<VariableStruct> &varList)
{
	for(int i=0;i<varList.size();i++) {
		VariableStruct var = varList.at(i);

		QString name = var.strName.trimmed();
		if(name.isEmpty()) continue;
		var.strValue = var.strValue.trimmed();
        var.strArgs = var.strArgs.trimmed();
		var.strDescription = var.strDescription.trimmed();
		if(var.strDescription.size() > 0) {
			var.strDescription = "Imported: " + var.strDescription;
		} else {
			var.strDescription = "Imported";
		}

        qDebug() << Q_FUNC_INFO << var.toString();

		if(var.nType == VariableStruct::EXECUTABLE) {
			if(!variableNameExists(var,m_listExecutableVariables)) {
				m_listExecutableVariables.append(var);
			}
		} else {
			if(!variableNameExists(var,m_listUserDefinedVariables)) {
				m_listUserDefinedVariables.append(var);
			}
		}
	}
}

void ATSkeletonWindow::replaceExecutableVariables(QList<VariableStruct> &varList)
{
	for(int i=0;i<varList.size();i++) {
		//find other variables using this variable
		VariableStruct var = varList.at(i);
		resolveVariable(var, varList, m_listExecutableVariables);	//var.strName used in m_listExecutableVariables ?
		resolveVariable(var, varList, m_listUserDefinedVariables);  //var.strName used in m_listUserDefinedVariables ?
	}
}

//var.strName used in checkList[i].strValue or checkList[i].strArgs ? IF yes then add var to addToList
void ATSkeletonWindow::resolveVariable(VariableStruct var, QList<VariableStruct> &addToList, QList<VariableStruct> &checkList)
{
	QString valueCheck = var.strValue.trimmed();
	if(valueCheck.isEmpty()) {
		return;
	}

    QString argsCheck = var.strArgs.trimmed();

	for(int i=0;i<checkList.size();i++) {
		VariableStruct varCheck = checkList.at(i);
		QString name = varCheck.strName.trimmed();
		if(name.isEmpty()) continue;
		QString varName1 = "${"+varCheck.strName+"}";
		QString varName2 = "$"+varCheck.strName;
        if(valueCheck.contains(varName1) || valueCheck.contains(varName2)
           || argsCheck.contains(varName1) || argsCheck.contains(varName2)) {
			if(!variableNameExists(varCheck, addToList)) {
				addToList.append(varCheck);
			}
        }
	}
}

bool ATSkeletonWindow::isDuplicateBuiltinVariableName(const QString& name)
{
	QString varName = name.trimmed().toLower();
    if(varName == "appdir") return true;
    if(varName == "name") return true;
	if(varName == "host") return true;
	if(varName == "shost") return true;
	if(varName == "sport") return true;
	if(varName == "uid") return true;
	if(varName == "pwd") return true;
	if(varName == "kfile") return true;
	if(varName == "kpwd") return true;
    if(varName == "ip") return true;
	if(varName == "port") return true;
	if(varName == "rhost") return true;
	if(varName == "rport") return true;
	return false;
}

//Passwords, Executables, Userdefined
bool ATSkeletonWindow::isDuplicateVariableName(const QString& name)
{
	QString varName = name.trimmed().toUpper();
	if(varName.isEmpty()) return false;

	if(m_pMainWindow->variableSettingsDialog->isVisible()) {
		if(m_pMainWindow->variableSettingsDialog->varNameExists(varName)) return true;
	} else {
		for(int i=0;i<m_listExecutableVariables.size();i++) {
			VariableStruct lvar = m_listExecutableVariables.at(i);
			if(lvar.strName.trimmed().toUpper() == varName) {
				return true;
			}
		}
		for(int i=0;i<m_listUserDefinedVariables.size();i++) {
			VariableStruct lvar = m_listUserDefinedVariables.at(i);
			if(lvar.strName.trimmed().toUpper() == varName) {
				return true;
			}
		}

        PasswordDb *pdb = PasswordDb::getInstance();
        for(unsigned int i=0; i<pdb->size(); i++) {
            Identifier ident = pdb->at(i);
			if(ident.getDescription().trimmed().toUpper() == varName) {
				return true;
			}
		}
	}

	return false;
}

bool ATSkeletonWindow::isDuplicatePortForwardVariableName(const QString& name, QTableWidgetItem *ignorethis)
{
	QString varName = name.trimmed().toUpper();
	if(varName.isEmpty()) return false;

    QList<QTreeWidgetItem*> treeTunnelItems = ui.treeTunnels->findItems(".*", Qt::MatchFlags(Qt::MatchRegularExpression | Qt::MatchRecursive), 0);
	for(int i=0;i<treeTunnelItems.size();i++) {
		QTreeWidgetItem *twi = treeTunnelItems.at(i);
		if(twi == m_pTreeTunnelsItemEdit && ui.widgetEditTunnel->isVisible()) continue; //we will check this below
		Tunnel_c *pt = getTunnel(twi);
		ATASSERT(pt);
		if(pt == NULL) continue;
		for(int j=0;j<pt->portForwardList.size();j++) {
			PortForwardStruct pfs = pt->portForwardList.at(j);
			if(pfs.strName.trimmed().toUpper() == varName) {
				return true;
			}
		}
	}
	
	if(ui.widgetEditTunnel->isVisible()) {
		for(int i=0;i<ui.tableMoreTunnels->rowCount();i++) {
			QTableWidgetItem *twi = ui.tableMoreTunnels->item(i,MoreTunnelsEditWidget::COL_NAME);
            if(ignorethis != NULL && ignorethis == twi) continue;
			if(twi->text().trimmed().toUpper() == varName) {
				return true;
			}
		}
	}

	return false;
}

//static
bool ATSkeletonWindow::variableNameExists(VariableStruct var, QList<VariableStruct> &varList)
{
	return variableNameExists(var.strName, varList);
}

//static
bool ATSkeletonWindow::variableNameExists(const QString& name, QList<VariableStruct> &varList)
{
	QString varName = name.trimmed();
	if(varName.isEmpty()) return false;
	for(int i=0;i<varList.size();i++) {
		VariableStruct lvar = varList.at(i);
		if(lvar.strName.trimmed() == varName) {
			return true;
		}
	}
	return false;
}

//static
void ATSkeletonWindow::writeSettingsUserDefinedVariables(QSettings &settings, QList<VariableStruct> &varList)
{
	QList<VariableStruct> userVarList;
	for(int j=0;j<varList.size();j++) {
		VariableStruct var = varList.at(j);
		if(var.nType == VariableStruct::USERDEFINED) {
			userVarList.append(var);
		}
	}

	//User Defined Variables
	settings.remove("VarUserDefined");
	settings.beginGroup("VarUserDefined");
	settings.beginWriteArray("userdefvars");
	for(int j=0;j<userVarList.size();j++) {
		settings.setArrayIndex(j);
		VariableStruct var = userVarList.at(j);
		if(var.nType == VariableStruct::USERDEFINED) {
			writeSettingsVariable(settings,var);
		}
	}
	settings.endArray();
	settings.endGroup();
}

//static
void ATSkeletonWindow::writeSettingsExecutableVariables(QSettings &settings, QList<VariableStruct> &varList)
{
	QList<VariableStruct> execVarList;
	for(int j=0;j<varList.size();j++) {
		VariableStruct var = varList.at(j);
		if(var.nType == VariableStruct::EXECUTABLE) {
			execVarList.append(var);
		}
	}

	//Executable Variables
	settings.remove("VarExecutables");
	settings.beginGroup("VarExecutables");
	settings.beginWriteArray("executables");
	for(int j=0;j<execVarList.size();j++) {
		settings.setArrayIndex(j);
		VariableStruct var = execVarList.at(j);
		if(var.nType == VariableStruct::EXECUTABLE) {
			writeSettingsVariable(settings,var);
		}
	}
	settings.endArray();
	settings.endGroup();
}

//static
QList<VariableStruct> ATSkeletonWindow::readSettingsUserDefinedVariables(QSettings &settings)
{
	QList<VariableStruct> varList;
	settings.beginGroup("VarUserDefined");
	int userdefvarsCount = settings.beginReadArray("userdefvars");
	for(int j=0;j<userdefvarsCount;j++) {
		settings.setArrayIndex(j);
		VariableStruct var = readSettingsVariable(settings);
		var.nType = VariableStruct::USERDEFINED;
		varList.append( var );
	}
	settings.endArray();
	settings.endGroup();
	return varList;
}

//static
QList<VariableStruct> ATSkeletonWindow::readSettingsExecutableVariables(QSettings &settings)
{
	QList<VariableStruct> varList;
	settings.beginGroup("VarExecutables");
	int varCount = settings.beginReadArray("executables");
	for(int j=0;j<varCount;j++) {
		settings.setArrayIndex(j);
		VariableStruct var = readSettingsVariable(settings);
        var.nType = VariableStruct::EXECUTABLE;

        //begin v0.9.42 on windows add prefix ${appdir} to plink, putty and winscp
        #ifdef Q_OS_WIN
            QString strValue = var.strValue.trimmed().toLower();
            if(strValue == "plink.exe") {
                var.strValue = "${appdir}/plink.exe";
            } else if(strValue == "putty.exe") {
                var.strValue = "${appdir}/putty.exe";
            } else if(strValue == "winscp.exe") {
                var.strValue = "${appdir}/winscp.exe";
            }
        #endif
        //end v.0.9.42

        //begin v0.9.42 macos: moved my .sh files from MacOS dir to Resources dir
        #ifdef Q_OS_MACOS
            QString strValue = var.strValue.trimmed().toLower();
            if(strValue == "${appdir}/exec1.sh") {
                var.strValue = "${appdir}/../Resources/exec1.sh";
            } else if(strValue == "${appdir}/exec2.sh") {
                var.strValue = "${appdir}/../Resources/exec2.sh";
            } else if(strValue == "${appdir}/exec3.sh") {
                var.strValue = "${appdir}/../Resources/exec3.sh";
            } else if(strValue == "${appdir}/iterm2win.sh") {
                var.strValue = "${appdir}/../Resources/iterm2win.sh";
            }
        #endif
        //end v.0.9.42

        varList.append( var );
	}
	settings.endArray();
	settings.endGroup();
	return varList;
}

//static
VariableStruct ATSkeletonWindow::readSettingsVariable(QSettings &settings)
{
	VariableStruct var;
	QString sUuid = settings.value("uuid","").toString().trimmed();
	if(!sUuid.isEmpty()) {
		//uuid found
		var.uUid = QUuid(sUuid);
		if(var.uUid.isNull()) { 
			//invalid uuid format in .ini file 
			var.uUid = QUuid::createUuid(); 
		}
	} else {
		//uuid not found or empty
		var.uUid = QUuid::createUuid();
	}
	
	var.nType = VariableStruct::USERDEFINED;
	var.strDescription = settings.value("description","").toString().trimmed();
	var.strName = settings.value("name","").toString().trimmed();
	var.strValue = settings.value("value","").toString().trimmed();
    var.strArgs = settings.value("args","").toString().trimmed();
	return var;
}

//static
void ATSkeletonWindow::writeSettingsVariable(QSettings &settings, VariableStruct &var)
{
	settings.setValue("uuid", var.uUid.toString());
	settings.setValue("description", var.strDescription);
	settings.setValue("name", var.strName);
    settings.setValue("value", var.strValue);
    settings.setValue("args", var.strArgs);
	settings.setValue("type", var.nType);
}

//static
void ATSkeletonWindow::writeSettingsTreeWidget(QSettings &settings, QTreeWidget *treeWidget)
{
	QList<Tunnel_c*> tunnelList = getTunnelList(treeWidget);
	writeSettingsTunnelList(settings, tunnelList);
}

//static
QList<Tunnel_c*> ATSkeletonWindow::getTunnelList(QTreeWidget *treeWidget)
{
    QList<QTreeWidgetItem*> treeTunnelItems = treeWidget->findItems(".*", Qt::MatchFlags(Qt::MatchRegularExpression | Qt::MatchRecursive), 0);

	//Tunnels
	QList<Tunnel_c*> tunnelList;
	for(int i=0;i<treeTunnelItems.size();i++)
	{
		QTreeWidgetItem *treeTunnelItem = treeTunnelItems[i];
		Tunnel_c *it = getTunnel(treeTunnelItem);
        ATASSERT(it);
        tunnelList.append(it);
	}
	
	return tunnelList;
}

//static
void ATSkeletonWindow::writeSettingsTunnelList(QSettings &settings, QList<Tunnel_c*> &tunnelList)
{
	//Delete all tunnels
	QStringList keys = settings.childGroups();
	for(int i=0; i<keys.count(); i++) {
		QString key = keys.at(i);
		if(key.indexOf("Tunnel") != -1 || key.indexOf("Host") != -1) { //renamed from Tunnel_ to Host_ in v0.9.16
			settings.remove(key);
		}
	}

	settings.setValue( "NumberOfHosts", tunnelList.size() );

	//Then save all tunnels
	for(int i=0;i<tunnelList.size();i++)
	{
        QString strGroup = QString( "Host_%1" ).arg( i+1, 6, 10, QChar('0') );
		settings.beginGroup( strGroup );
		Tunnel_c *it = tunnelList.at(i);
		writeSettingsTunnel(settings, it);
		settings.endGroup();
	}
}

//static
void ATSkeletonWindow::writeSettingsTunnel(QSettings &settings, Tunnel_c *it)
{
	settings.setValue( "uuid",			  it->uUid.toString());  
    settings.setValue( "ExtID",			  it->strExtID);
	settings.setValue( "Name",            it->strName );
	settings.setValue( "Type",            it->iType );
	settings.setValue( "Level",			  it->iLevel );
	settings.setValue( "Expanded",		  it->bIsExpanded );
    settings.setValue( "FgColor", it->strFgColor );
    settings.setValue( "BgColor", it->strBgColor );
    settings.setValue( "ChildNodesCommandType", it->bChildNodesCommandType );

	if(it->iType == TUNNEL_TYPE_TUNNEL)
	{

		settings.setValue( "SSHHost",		  it->getSelectedSSHHost() );
		settings.setValue( "SSHHostList",     it->getSSHHostList() );
		settings.setValue( "LocalIP",         it->strLocalIP );
		settings.setValue( "LocalPort",       it->iLocalPort );
		settings.setValue( "RemoteHost",	  it->getSelectedRemoteHost() );
		settings.setValue( "RemoteHostList",  it->getRemoteHostList() );
		settings.setValue( "RemotePort",      it->iRemotePort );
		settings.setValue( "Username",        it->strUsername );
		settings.setValue( "Password",        it->strPassword );
		settings.setValue( "SSHKeyFilePassword",     it->strSSHKeyFilePassword );
		settings.setValue( "SSHKeyFile",      it->strSSHKeyFile );
		settings.setValue( "Direction",       it->iDirection );
		settings.setValue( "AutoConnect",     it->bAutoConnect );
		settings.setValue( "Compression",     it->bCompression );
		settings.setValue( "DoKeepAlivePing", it->bDoKeepAlivePing );
		settings.setValue( "AutoReconnect",   it->bAutoReconnect );
		settings.setValue( "SSH1or2",         it->iSSH1or2 );
		settings.setValue( "ExtraArguments",  it->strExtraArguments );

		settings.beginWriteArray("PortForward");
		for(int j=0;j<it->portForwardList.size();j++) {
			PortForwardStruct pfs = it->portForwardList.at(j);
			settings.setArrayIndex(j);
			settings.setValue("uuid",pfs.uUid.toString());
			settings.setValue("enabled",pfs.bEnabled);
			settings.setValue("type",pfs.nType);
			settings.setValue("name",pfs.strName);
			settings.setValue("lip",pfs.strLocalIP);
			settings.setValue("lport",pfs.nLocalPort);
			settings.setValue("dhost",pfs.strDestinationHost);
			settings.setValue("dport",pfs.nDestinationPort);
			settings.setValue("descr",pfs.strDescription);
        }
		settings.endArray();

    }
    else if(it->iType == TUNNEL_TYPE_FOLDER)
    {
        settings.setValue( "ChildNodesCommand", it->strChildNodesCommand );
        settings.setValue( "ActivateDisconnects", it->bActivateDisconnects );
    }

	settings.beginWriteArray("CustomActions");
	for(int j=0;j<it->customActionList.size();j++) {
		CustomActionStruct cas = it->customActionList.at(j);
		settings.setArrayIndex(j);
		settings.setValue("uuid",cas.uUid.toString());
		settings.setValue("label",cas.sLabel);
		settings.setValue("cmd",cas.sCmd);
		settings.setValue("type",cas.iType);
		settings.setValue("level",cas.iLevel);
		settings.setValue("expanded",cas.bExpanded);
	}	
	settings.endArray();
}

void ATSkeletonWindow::updateParentFoldersIcon(QTreeWidgetItem *twi)
{
	qDebug() << "updateParentFoldersIcon() " << twi->text(0);
    if(twi->parent() != NULL) {
        updateCurrentAndParentFoldersIcon(twi->parent());
    }
}

void ATSkeletonWindow::updateCurrentAndParentFoldersIcon(QTreeWidgetItem *twi)
{
    qDebug() << "updateCurrentAndParentFoldersIcon() " << twi->text(0);
    QTreeWidgetItem *twiParent = twi;
    while(twiParent != NULL) {
        Tunnel_c *ptParent = getTunnel(twiParent);
        if(ptParent->iType == TUNNEL_TYPE_FOLDER) {
            int connectionCount = getConnectionCountChildren(twiParent, true); //true=count CONNECTED folders
            int delayRetryConnectionCount = getDelayRetryConnectCountChildren(twiParent);
            qDebug() << "Node [" << twiParent->text(0) << "] connection count = " << connectionCount;
            if(connectionCount > 0) {
                ptParent->iConnectStatus = CONNECTED;
                updateControlsTunnel(ptParent);
                if(twiParent->isExpanded()) {
                    treeTunnelSetIcon(twiParent, Images::icon("ht_160_folder_open_connected"));
                } else {
                    treeTunnelSetIcon(twiParent, Images::icon("ht_180_folder_closed_connected"));
                }
            } else if (delayRetryConnectionCount > 0) {
                ptParent->iConnectStatus = MARKCONNECT;
                updateControlsTunnel(ptParent);
                if(twiParent->isExpanded()) {
                    treeTunnelSetIcon(twiParent, Images::icon("ht_160_folder_open_connected"));
                } else {
                    treeTunnelSetIcon(twiParent, Images::icon("ht_180_folder_closed_connected"));
                }
            } else {
                ptParent->iConnectStatus = DISCONNECTED;
                updateControlsTunnel(ptParent);
                if(twiParent->isExpanded()) {
                    treeTunnelSetIcon(twiParent, Images::icon("ht_150_folder_open_disconnected"));
                } else {
                    treeTunnelSetIcon(twiParent, Images::icon("ht_170_folder_closed_disconnected"));
                }
            }
        }
        twiParent = twiParent->parent();
    }
}


void ATSkeletonWindow::slotManualBackupSelected()
{
	QString backupDescription = "Manual backup";

	bool ok;
	QString text = QInputDialog::getText(this, tr("Manual backup"),
			tr("Backup description. Cancel cancels backup."), QLineEdit::Normal,
			backupDescription, &ok);
	if (ok) {
		text = text.trimmed();
		if(!text.isEmpty()) {
			QString textLower = text.toLower();
			QString backupDescriptionLower = backupDescription.toLower();
			if(textLower.compare(backupDescriptionLower) != 0) {
				if(textLower.contains(backupDescriptionLower)) {
					backupDescription = text; //Manual backup: bla bla
				} else {
					backupDescription = backupDescription + ": " + text;
				}
			}
		}
		backupSettings( BackupInfo(BACKUP_MANUAL,backupDescription) );
	}

}

void ATSkeletonWindow::autoBackupSettings(const BackupInfo &backupInfoData)
{
	if(!m_bAutoBackupSettingsDisabled) {
		if(m_pMainWindow->preferences()->autoBackupEnabled()) {
			backupSettings(backupInfoData);
		}
	}
}

void ATSkeletonWindow::backupSettings(const BackupInfo &backupInfoData)
{
	Backup backup;

	BackupStatus status = backup.backup(backupInfoData);

	if(status.bIsOK == false && backupInfoData.getName() == BACKUP_MANUAL) {
		QMessageBox::warning(this, tr("Ooops! Backup Failed"),status.sMsg,QMessageBox::Close);
	}

	QString backupName = backupInfoData.getName();
	if(backupName != BACKUP_BEFORE_APPSTOP && backupName != BACKUP_AFTER_APPSTOP) {
		qDebug() << "backupSettings() emit signalCleanBackup";
		emit signalCleanBackup();
	}
}


void ATSkeletonWindow::restoreBackup(QString backupInfoFilePath)
{
	if(!QFile::exists(backupInfoFilePath)) {
		QMessageBox::warning(this, tr("File not found"),backupInfoFilePath,QMessageBox::Close);
		return;
	}

	Backup backup;
	RestoreStatus status = backup.canRestore(backupInfoFilePath);
	if(status.bIsOK == false) {
		QMessageBox::warning(this, "Ooops! Cannot Restore",status.sMsg,QMessageBox::Close);
		return;
	}
	
	backupSettings(BackupInfo(BACKUP_BEFORE_RESTORE,"Before restoring backup file [%1]",backupInfoFilePath) );

	m_pTreeTunnelsItemEdit = NULL;
	enableTreeTunnelsPaste(false);
	m_bAutoBackupSettingsDisabled = true; //stop readSettings() from backing up

    ATVERIFY( disconnect( ui.treeTunnels, &TunnelTreeWidget::itemSelectionChanged, nullptr, nullptr )  );
    ATVERIFY( disconnect( ui.treeTunnels, &TunnelTreeWidget::activated, nullptr, nullptr ) );
    ATVERIFY( disconnect( this, &ATSkeletonWindow::signalAutoConnect, nullptr, nullptr )  );

	while(ui.treeTunnels->topLevelItemCount() > 0) {
		QTreeWidgetItem* twi = ui.treeTunnels->topLevelItem(0);
		recursiveDisconnectTunnelSignals(twi);
		recursiveDisconnectTunnel(twi);
		recursiveDeleteTunnel(twi);
		delete twi;
	}

	ui.treeTunnels->clear();

	status = backup.restore(backupInfoFilePath);
	if(!status.bIsOK) {
		QMessageBox::warning(this, tr("Ooops! Restore Failed"),status.sMsg,QMessageBox::Close);
	}

    ATVERIFY( connect( ui.treeTunnels, &TunnelTreeWidget::itemSelectionChanged,	this, &ATSkeletonWindow::slotSelectTunnel ) );
    ATVERIFY( connect( ui.treeTunnels, &TunnelTreeWidget::activated, this, &ATSkeletonWindow::slotItemActivated ) );

	//////////////////////
	// READ DoffenSSHTunnel.ini & DoffenSSHTunnelApp.ini
	readSettings();

    ATVERIFY( connect( this, &ATSkeletonWindow::signalAutoConnect, this, &ATSkeletonWindow::slotAutoConnect, Qt::QueuedConnection ) );

	m_bAutoBackupSettingsDisabled = false;

	if(status.bIsOK) {
		autoBackupSettings(BackupInfo(BACKUP_AFTER_RESTORE,"Restored backup file [%1]",backupInfoFilePath) );
	}
}

void ATSkeletonWindow::slotCleanBackup()
{
	if (m_cleanBackupFutureWatcher.isRunning()) {
		qDebug() << "slotCleanBackup() abort clean backup already running";
		return;
	}

	//http://doc.qt.nokia.com/4.7-snapshot/qtconcurrent-imagescaling-imagescaling-cpp.html

    QFuture<void> future = QtConcurrent::run([this]() {
        cleanBackup(g_strTunnelIniFile);
    });

	m_cleanBackupFutureWatcher.setFuture(future);
}

void ATSkeletonWindow::slotCleanBackupFinished()
{
	qDebug() << "cleanBackup() finished";
}

void ATSkeletonWindow::cleanBackup(QString iniFilePath)
{
	qDebug() << "cleanBackup() started [" + iniFilePath + "]";

	QFileInfo iniFileInfo(iniFilePath);
	QString iniDirPath = iniFileInfo.absolutePath();			
	QString iniFileName = iniFileInfo.fileName();
	QDir iniDir(iniDirPath);
	QFile iniFile(iniFilePath);	

	if(!iniFile.exists()) {
		qDebug() << "cleanBackup() ini file not found [" + iniFilePath + "]";
		return;
	}

	QString backupDirName = "DoffenSSHTunnelBackup";
	QFileInfo backupDirInfo(iniDirPath,backupDirName);
	QString backupDirPath = backupDirInfo.absoluteFilePath();			
	QDir backupDir(backupDirPath);

	if(!backupDir.exists()) {
		qDebug() << "cleanBackup() backup dir not found [" + backupDirPath + "]";
		return;
	}

	QString fileNamePattern = "*#*#"+iniFileInfo.baseName()+".ini";
	QStringList files = backupDir.entryList(QStringList(fileNamePattern), QDir::Files | QDir::NoSymLinks, QDir::Name | QDir::Reversed);

	qDebug() << "cleanBackup() found " << files.size() << " ini files";

	QList<QString> autoFiles;
	QList<QString> manualFiles;

	for(int i=0;i<files.size();i++) {
		QString backupFileName = files[i];
		if(backupFileName.contains(BACKUP_MANUAL)) {
			manualFiles.append(backupFileName);
		} else {
			autoFiles.append(backupFileName);
		}
	}

	for(int i=400;i<autoFiles.size();i++) {
		deleteBackup(backupDir, autoFiles[i]);
	}

	for(int i=100;i<manualFiles.size();i++) {
		deleteBackup(backupDir, manualFiles[i]);
	}

	qDebug() << "cleanBackup() finished clean backup dir [" + backupDirPath + "]";
}

void ATSkeletonWindow::deleteBackup(const QDir& backupDir, const QString& backupFileName)
{
	QString backupFilePath = backupDir.absoluteFilePath(backupFileName);
	QFileInfo backupFileInfo(backupFilePath);

	//20120502_140119_104#appstop#DoffenSSHTunnelApp.ini
	//Find the last #
	int skiIndex = backupFileName.lastIndexOf("#");
	if(skiIndex == -1 || skiIndex >= (backupFileName.length()-2)) {
		return;
	}
	QString searchPattern = backupFileName.left(skiIndex+1) + "*";

    qDebug() << Q_FUNC_INFO << "SEARCH PATTERN: " << searchPattern;
	QStringList files = backupDir.entryList(QStringList(searchPattern), QDir::Files | QDir::NoSymLinks, QDir::Name);

	for(int i=0;i<files.size();i++) {
		QString fileName = files.at(i);
		QFileInfo fileInfo(backupDir, fileName);
		QString filePath = fileInfo.absoluteFilePath();
		if(QFile::exists(filePath)) {
			if(QFile::remove(filePath)) {
				qDebug() << "cleanBackup() deleted file [" + filePath + "]";
			} else {
				qDebug() << "cleanBackup() QFile::remove() returned false [" + filePath + "]";
			}
		}
	}
}


void ATSkeletonWindow::updateControls()
{
	Tunnel_c *pt = NULL;
	QTreeWidgetItem *twi = ui.treeTunnels->currentItem();
	if(twi != NULL) {
        //qDebug("%s() %s",Q_FUNC_INFO,twi->text(0));
		pt = getTunnel(twi);
		if(pt->iType == TUNNEL_TYPE_FOLDER) {
			updateControlsFolder();
		} else {
			updateControlsTunnel();
		}
	} else {
		updateControlsFolder();
	}

	ui.btnAddFolder->setEnabled( twi != NULL);
	ui.btnAddTunnel->setEnabled( twi != NULL);
	ui.btnDuplicate->setEnabled( twi != NULL);
	ui.btnDeleteTunnel->setEnabled( twi != NULL);

	m_pTreeTunnelsActionExport->setEnabled( twi != NULL );

	treeTunnelUpdateMoveControls();
}

void ATSkeletonWindow::treeTunnelUpdateMoveControls()
{
	ui.btnLeftTunnel->setEnabled(false);
	ui.btnRightTunnel->setEnabled(false);
	ui.btnUpTunnel->setEnabled(false);
	ui.btnDownTunnel->setEnabled(false);

	QTreeWidgetItem *twi = ui.treeTunnels->currentItem();
	if(twi != NULL) {
		if(ui.treeTunnels->canMoveLeft(twi)) {
			ui.btnLeftTunnel->setEnabled( twi != NULL );
		}
		if(ui.treeTunnels->canMoveRight(twi)) {
			ui.btnRightTunnel->setEnabled( twi != NULL );
		}
		if(ui.treeTunnels->canMoveUp(twi)) {
			ui.btnUpTunnel->setEnabled( twi != NULL );
		}
		if(ui.treeTunnels->canMoveDown(twi)) {
			ui.btnDownTunnel->setEnabled( twi != NULL );
		}
	}
}

void ATSkeletonWindow::updateControlsTunnel()
{
    ui.stackedWidgetEdit->setCurrentWidget(ui.widgetEditTunnel);

	QTreeWidgetItem *twi = ui.treeTunnels->currentItem();
	ui.btnDuplicate->setEnabled( twi != NULL );
	ui.btnDeleteTunnel->setEnabled( twi != NULL );
	ui.widgetEditTunnel->setEnabled( twi != NULL );
	ui.widgetCustomActions->setEnabled( twi != NULL );

	updateControlsTunnel(getTunnel(twi));
}

void ATSkeletonWindow::updateControlsTunnel(Tunnel_c *pt)
{
    if(pt != NULL && ui.treeTunnels->currentItem() != pt->twi) return;
    ui.btnConnect->setVisible(true);
    ui.btnDisconnect->setVisible(true);
    ui.btnConnect->setEnabled(false);
	ui.btnDisconnect->setEnabled(false);
	if(pt != NULL) {
        if( pt->isActiveTimerDelayRetryConnect() ) {
            //waiting for timer to auto reconnect
            ui.btnConnect->setEnabled(true);
            ui.btnDisconnect->setEnabled(true);
        } else if ( pt->iConnectStatus == CONNECTED || pt->iConnectStatus == CONNECTING || pt->iConnectStatus == MARKCONNECT) {
			//connected or connecting/retrying
			ui.btnConnect->setEnabled(false);
			ui.btnDisconnect->setEnabled(true);
		} else {
            if(!pt->getSelectedSSHHost().isEmpty()) {
				ui.btnConnect->setEnabled(true);
			}
            if(!pt->strChildNodesCommand.isEmpty()) {
                ui.btnConnect->setEnabled(true);
            }
            ui.btnDisconnect->setEnabled(false);
		}
	}
}

void ATSkeletonWindow::updateControlsFolder()
{
    ui.stackedWidgetEdit->setCurrentWidget(ui.widgetEditFolder);

    ui.btnConnect->setVisible(true);
    ui.btnDisconnect->setVisible(true);
    ui.btnConnect->setEnabled(false);
    ui.btnDisconnect->setEnabled(false);

	QTreeWidgetItem *twi = ui.treeTunnels->currentItem();
	ui.btnDuplicate->setEnabled( twi != NULL );
	ui.btnDeleteTunnel->setEnabled( twi != NULL );

    //m_pTreeTunnelsActionClearLog->setEnabled( true );
	ui.widgetEditTunnel->setEnabled(false);

    ui.widgetCustomActions->setEnabled(true);

    int connectionCount = getConnectionCountChildren(twi, true);
    int delayRetryConnectCount = getDelayRetryConnectCountChildren(twi);
    if(connectionCount > 0 || delayRetryConnectCount > 0) {
        ui.btnDisconnect->setEnabled(true);
    }
    Tunnel_c *pt = getTunnel(twi);
    if(!pt->strChildNodesCommand.isEmpty()) {
        ui.btnConnect->setEnabled(true);
    }
}

void ATSkeletonWindow::slotUpdateClearLogButtons()
{
	if(ui.textBrowser->toPlainText().isEmpty()) {
        m_pLogMenuClearAction->setEnabled(false);
		m_pTreeTunnelsActionClearLog->setEnabled(false);
	} else {
        m_pLogMenuClearAction->setEnabled(true);
		m_pTreeTunnelsActionClearLog->setEnabled(true);
	}
}

void ATSkeletonWindow::slotEnableEditingToggled(bool checked)
{
	qDebug() << Q_FUNC_INFO << checked;

	m_bEditingEnabled = checked;

	//Hosts tree: clear copy/cut status and disable paste in context menu
	enableTreeTunnelsPaste(false);

	//Custom actions edit: clear copy/cut status and disable paste in context menu
	ui.treeCustomActionsEdit->clearPasteItems();

	//splitter left top buttons
	ui.widgetTreeTunnelActionButtons->setEnabled(checked);
	ui.widgetTreeTunnelActionButtons->setVisible(checked);

	//splitter left tunnel tree
	if(checked) {
		enableTreeTunnelDragDrop(true);
		ui.treeTunnels->setContextMenuPolicy(Qt::ActionsContextMenu);
		ui.treeTunnels->setEditTriggers(QAbstractItemView::EditKeyPressed);
	} else {
		enableTreeTunnelDragDrop(false);
		ui.treeTunnels->setContextMenuPolicy(Qt::NoContextMenu);
		ui.treeTunnels->setEditTriggers(QAbstractItemView::NoEditTriggers);
	}

	//splitter left bottom buttons
	ui.widgetTreeTunnelMoveButtons->setEnabled(checked);
	ui.widgetTreeTunnelMoveButtons->setVisible(checked);
	
	//splitter right connect tab
	ui.widgetCustomActionsEditButtons->setEnabled(checked);
	ui.widgetCustomActionsEditButtons->setVisible(checked);
	ui.treeCustomActionsEdit->slotEnableEditing(checked);
	if(checked) {
		ui.treeCustomActionsEdit->setDragEnabled(true);
		ui.treeCustomActionsEdit->setEditTriggers(QAbstractItemView::EditKeyPressed|QAbstractItemView::SelectedClicked);
	} else {
		ui.treeCustomActionsEdit->setDragEnabled(false);
		ui.treeCustomActionsEdit->setEditTriggers(QAbstractItemView::NoEditTriggers);
	}

	//splitter right edit tab
	ui.tabEdit->setEnabled(checked);
}

void ATSkeletonWindow::slotTreeTunnelExpanded(QTreeWidgetItem *twi)
{
	Tunnel_c *pt = getTunnel(twi);
	ATASSERT(pt);
	if(pt == NULL) return;
	bool blocked = ui.treeTunnels->blockSignals(true);
	if(pt->iType == TUNNEL_TYPE_FOLDER) {
		if(pt->iConnectStatus == CONNECTED) {
			treeTunnelSetIcon(twi, Images::icon("ht_160_folder_open_connected"));
		} else {
			treeTunnelSetIcon(twi, Images::icon("ht_150_folder_open_disconnected"));
		}
	}
    m_pMainWindow->m_pSearchWidget->slotUpdateCompleterIcons();
	ui.treeTunnels->blockSignals(blocked);
}

void ATSkeletonWindow::slotTreeTunnelCollapsed(QTreeWidgetItem *twi)
{
	Tunnel_c *pt = getTunnel(twi);
	ATASSERT(pt);
	if(pt == NULL) return;
	bool blocked = ui.treeTunnels->blockSignals(true);
	if(pt->iType == TUNNEL_TYPE_FOLDER) {
		if(pt->iConnectStatus == CONNECTED) {
			treeTunnelSetIcon(twi, Images::icon("ht_180_folder_closed_connected"));
		} else {
			treeTunnelSetIcon(twi, Images::icon("ht_170_folder_closed_disconnected"));
		}
	}
    m_pMainWindow->m_pSearchWidget->slotUpdateCompleterIcons();
	ui.treeTunnels->blockSignals(blocked);
}

void ATSkeletonWindow::slotTreeTunnelMoveUp()
{
	if(!m_bEditingEnabled) return;

 	QTreeWidgetItem *item = ui.treeTunnels->currentItem();
	ATASSERT(item);
	if ( item == NULL ) return;

	Tunnel_c *pt = getTunnel(item);
	ATASSERT(pt);
	if ( pt == NULL ) return;

	QTreeWidgetItem *parentItem = item->parent();
	if(parentItem != NULL) {
		int itemIndex = parentItem->indexOfChild(item);
		if(itemIndex < 1) return; //At top
		enableTreeTunnelsPaste(false);
		if(pt->iType == TUNNEL_TYPE_FOLDER) {
			autoBackupSettings( BackupInfo( BACKUP_BEFORE_MOVE_FOLDER_UP,"Before moving folder up [%1]",item->text(0) ) );
		} else {
			autoBackupSettings( BackupInfo( BACKUP_BEFORE_MOVE_TUNNEL_UP,"Before moving tunnel up [%1]",item->text(0) ) );
		}
		QTreeWidgetItem *itemAbove = parentItem->takeChild(itemIndex - 1);
		parentItem->insertChild(itemIndex, itemAbove);
		delayWriteSettings();
	} else {
		int itemIndex = ui.treeTunnels->indexOfTopLevelItem(item);
		if(itemIndex < 1) return; //At top
		enableTreeTunnelsPaste(false);
		if(pt->iType == TUNNEL_TYPE_FOLDER) {
			autoBackupSettings( BackupInfo( BACKUP_BEFORE_MOVE_FOLDER_UP,"Before moving folder up [%1]",item->text(0) ) );
		} else {
			autoBackupSettings( BackupInfo( BACKUP_BEFORE_MOVE_TUNNEL_UP,"Before moving tunnel up [%1]",item->text(0) ) );
		}
		QTreeWidgetItem *itemAbove = ui.treeTunnels->takeTopLevelItem(itemIndex - 1);
		ui.treeTunnels->insertTopLevelItem(itemIndex, itemAbove);
		delayWriteSettings();
	}

	treeTunnelUpdateMoveControls();
}

void ATSkeletonWindow::slotTreeTunnelMoveDown()
{
	if(!m_bEditingEnabled) return;

 	QTreeWidgetItem *item = ui.treeTunnels->currentItem();
	ATASSERT(item);
	if ( item == NULL ) return;

	Tunnel_c *pt = getTunnel(item);
	ATASSERT(pt);
	if ( pt == NULL ) return;

	QTreeWidgetItem *parentItem = item->parent();
	if(parentItem != NULL) {
		int itemIndex = parentItem->indexOfChild(item);
		if(itemIndex == parentItem->childCount()-1) return; //At bottom
		enableTreeTunnelsPaste(false);
		if(pt->iType == TUNNEL_TYPE_FOLDER) {
			autoBackupSettings( BackupInfo( BACKUP_BEFORE_MOVE_FOLDER_DOWN,"Before moving folder down [%1]",item->text(0) ) );
		} else {
			autoBackupSettings( BackupInfo( BACKUP_BEFORE_MOVE_TUNNEL_DOWN,"Before moving tunnel down [%1]",item->text(0) ) );
		}
		QTreeWidgetItem *itemBelow = parentItem->takeChild(itemIndex + 1);
		parentItem->insertChild(itemIndex, itemBelow);
		delayWriteSettings();
	} else {
		int itemIndex = ui.treeTunnels->indexOfTopLevelItem(item);
		if(itemIndex == ui.treeTunnels->topLevelItemCount()-1) return; //At bottom
		enableTreeTunnelsPaste(false);
		if(pt->iType == TUNNEL_TYPE_FOLDER) {
			autoBackupSettings( BackupInfo( BACKUP_BEFORE_MOVE_FOLDER_DOWN,"Before moving folder down [%1]",item->text(0) ) );
		} else {
			autoBackupSettings( BackupInfo( BACKUP_BEFORE_MOVE_TUNNEL_DOWN,"Before moving tunnel down [%1]",item->text(0) ) );
		}
		QTreeWidgetItem *itemBelow = ui.treeTunnels->takeTopLevelItem(itemIndex + 1);
		ui.treeTunnels->insertTopLevelItem(itemIndex, itemBelow);
		delayWriteSettings();
	}

	treeTunnelUpdateMoveControls();
}

void ATSkeletonWindow::slotTreeTunnelMoveLeft()
{
	if(!m_bEditingEnabled) return;

 	QTreeWidgetItem *item = ui.treeTunnels->currentItem();
	ATASSERT(item);
	if ( item == NULL ) return;

	Tunnel_c *pt = getTunnel(item);
	ATASSERT(pt);
	if ( pt == NULL ) return;

	int level = getItemLevel(item);

	if(level > 1) 
	{
		enableTreeTunnelsPaste(false);

		if(pt->iType == TUNNEL_TYPE_FOLDER) {
			autoBackupSettings( BackupInfo( BACKUP_BEFORE_MOVE_FOLDER_LEFT,"Before moving folder left [%1]",item->text(0) ) );
		} else {
			autoBackupSettings( BackupInfo( BACKUP_BEFORE_MOVE_TUNNEL_LEFT,"Before moving tunnel left [%1]",item->text(0) ) );
		}

		// root
		// item1           grandParentItem level 0
		// |-- item2       parentItem      level 1
		//     |-- item3   currentItem     level 2
		QTreeWidgetItem *parentItem = item->parent();
		QTreeWidgetItem *grandParentItem = parentItem->parent();
		int itemIndex = parentItem->indexOfChild(item);
		int parentItemIndex = grandParentItem->indexOfChild(parentItem);
		item = parentItem->takeChild(itemIndex);
		grandParentItem->insertChild(parentItemIndex+1, item);
		treeTunnelUpdateFromParent(item);
		ui.treeTunnels->setCurrentItem( item );
		delayWriteSettings();
	}
	else if(level == 1)
	{
		enableTreeTunnelsPaste(false);

		if(pt->iType == TUNNEL_TYPE_FOLDER) {
			autoBackupSettings( BackupInfo( BACKUP_BEFORE_MOVE_FOLDER_LEFT,"Before moving folder left [%1]",item->text(0) ) );
		} else {
			autoBackupSettings( BackupInfo( BACKUP_BEFORE_MOVE_TUNNEL_LEFT,"Before moving tunnel left [%1]",item->text(0) ) );
		}

		// root
        // item1           parentItem      level 0
		// |-- item2       currentItem     level 1
		QTreeWidgetItem *parentItem = item->parent();
		int itemIndex = parentItem->indexOfChild(item);
		int parentItemIndex = ui.treeTunnels->indexOfTopLevelItem(parentItem);
		item = parentItem->takeChild(itemIndex);
		ui.treeTunnels->insertTopLevelItem(parentItemIndex+1, item);
		treeTunnelUpdateFromParent(item);
		ui.treeTunnels->setCurrentItem( item );
		delayWriteSettings();
	}

	treeTunnelUpdateMoveControls();
}

void ATSkeletonWindow::slotTreeTunnelMoveRight()
{
	if(!m_bEditingEnabled) return;

 	QTreeWidgetItem *item = ui.treeTunnels->currentItem();
	ATASSERT(item);
	if ( item == NULL ) return;

	Tunnel_c *pt = getTunnel(item);
	ATASSERT(pt);
	if ( pt == NULL ) return;

	QTreeWidgetItem *beforeItem = NULL;

	QTreeWidgetItem *parentItem = item->parent();
	if(parentItem != NULL) {
		int itemIndex = parentItem->indexOfChild(item);
		if(itemIndex > 0) {
			beforeItem = parentItem->child(itemIndex-1);
			item = parentItem->takeChild(itemIndex);
		}
	} else {
		int itemIndex = ui.treeTunnels->indexOfTopLevelItem(item);
		if(itemIndex > 0) {
			beforeItem = ui.treeTunnels->topLevelItem(itemIndex-1);
			item = ui.treeTunnels->takeTopLevelItem(itemIndex);
		}
	}

	if(beforeItem != NULL) {
		enableTreeTunnelsPaste(false);

		if(pt->iType == TUNNEL_TYPE_FOLDER) {
			autoBackupSettings( BackupInfo( BACKUP_BEFORE_MOVE_FOLDER_RIGHT,"Before moving folder right [%1]",item->text(0) ) );
		} else {
			autoBackupSettings( BackupInfo( BACKUP_BEFORE_MOVE_TUNNEL_RIGHT,"Before moving tunnel right [%1]",item->text(0) ) );
		}

		beforeItem->insertChild(beforeItem->childCount(), item);
		treeTunnelUpdateFromParent(item);
		ui.treeTunnels->setCurrentItem( item );
		delayWriteSettings();
	}

	treeTunnelUpdateMoveControls();
}

void ATSkeletonWindow::slotDragMoveStarted(QTreeWidgetItem *twi)
{
	enableTreeTunnelsPaste(false);
	ATASSERT(twi);
	qDebug() << Q_FUNC_INFO << twi->text(0);
	Tunnel_c *pt = getTunnel(twi);
	ATASSERT(pt);
	if(pt->iType == TUNNEL_TYPE_FOLDER) {
		autoBackupSettings( BackupInfo( BACKUP_BEFORE_DRAG_FOLDER,"Before drag&drop folder [%1]",twi->text(0) ) );
	} else {
		autoBackupSettings( BackupInfo( BACKUP_BEFORE_DRAG_TUNNEL,"Before drag&drop tunnel [%1]",twi->text(0) ) );
	}
}

void ATSkeletonWindow::slotDragMoveFinished(QTreeWidgetItem *twi)
{
	enableTreeTunnelsPaste(false);
	ATASSERT(twi);
	qDebug() << Q_FUNC_INFO << twi->text(0);
	Tunnel_c *pt = getTunnel(twi);
	ATASSERT(pt);

	treeTunnelUpdateFromParent(twi);
	writeSettings();
}

int ATSkeletonWindow::getItemLevel(QTreeWidgetItem* item)
{
	int level = 0;
	QTreeWidgetItem* parentItem = item->parent();
	while(parentItem != NULL) {
		level++;
		parentItem = parentItem->parent();
	}
	return level;
}


Tunnel_c* ATSkeletonWindow::getTunnel(QTreeWidgetItem* twi)
{
	if(twi == NULL) return NULL;
	QVariant v = twi->data(0, Qt::UserRole);
    if(!v.isNull() && v.isValid()) {
		Tunnel_c* pt = (Tunnel_c*)v.value<qulonglong>();
		pt->iLevel = TreeWidget::getItemLevel(twi);
		pt->bIsExpanded = twi->isExpanded();
		return pt;
    } else {
        return NULL;
    }
}

void ATSkeletonWindow::setTunnel(QTreeWidgetItem* twi, Tunnel_c* pt)
{
    if(pt != NULL) {
        twi->setData( 0, Qt::UserRole, QVariant::fromValue((qulonglong)pt) );
		pt->twi = twi;
    }
    else {
        twi->setData( 0, Qt::UserRole, QVariant() );
    }
}


void ATSkeletonWindow::slotCustomActionExec(const CustomActionStruct& cas)
{
 	QTreeWidgetItem *twi = ui.treeTunnels->currentItem();
	ATASSERT( twi );
	if ( twi == NULL ) return;

	Tunnel_c *pt = getTunnel(twi);
	ATASSERT( pt );
	if ( pt == NULL ) return;


    QString sCmd = QString(cas.sCmd);

    if(sCmd.contains("$putty") || sCmd.contains("${putty}") || sCmd.contains("$ssh") || sCmd.contains("${ssh}")) {
#ifdef Q_OS_WIN
        //On Windows $ssh is aliased with $putty which executes putty.exe -P <port> ...
        //port option on win/putty is -P so make sure it's -P here
        sCmd = sCmd.replace("-p ","-P ");
#else
        //On Mac/Linx $putty is aliased with $ssh which executes iterm2win.sh ssh -p <port> ...
        //port option on mac/ssh is -p, on win/putty it's -P so make sure it's -p here
        sCmd = sCmd.replace("-P ","-p ");
        //remove -pw <password>
        sCmd = sCmd.replace(QRegularExpression("-pw\\s+\\S+\\s+"),"");
#endif
    }

    QString sCmdLog = replaceVarsLog(*pt, sCmd);
    sCmd = replaceVars(*pt, sCmd);

    bool bOutputWindow = false;
    if(sCmd.startsWith("OutputWindow ")) {
        bOutputWindow = true;
        sCmd = sCmd.replace(QRegularExpression("^OutputWindow\\s*"),"");
    } else if(sCmd.startsWith("OW ")) {
        bOutputWindow = true;
        sCmd = sCmd.replace(QRegularExpression("^OW\\s*"),"");
    }

    AddToLog( *pt, QString("Launching %1: %2\n").arg(cas.sLabel).arg(sCmdLog) );

    if(bOutputWindow) {
        OutputWindow * pOutputWindow = new OutputWindow(qApp->activeWindow());
        pOutputWindow->setModal(true);
        pOutputWindow->setAutoClose(false);
        pOutputWindow->setAutoCloseVisible(false);
        pOutputWindow->show();
        QProcess * process = new QProcess( parent() );
        //My hack not using setProgram. Setting arguments only
        //causes pOutputWindow->addProcess to call QProcess::start(process->arguments())
        //If I execute through the shell with process->setProgram("/bin/sh") then Kill process
        //will kill the shell only. The actual program running in the shell will not be killed
        //e.g. on windows cmd /c <long running program>
        process->setArguments(QStringList() << sCmd);
        pOutputWindow->addProcess( process );
        pOutputWindow->noMoreProcesses();
    } else {
        QStringList parts = QProcess::splitCommand(sCmd);
        bool startResult = QProcess::startDetached( parts.takeFirst(), parts );
        if(!startResult) {
            AddToLog( *pt, QString("Failed to start") );
        }
    }
}

void ATSkeletonWindow::slotTabChanged()
{
	
}

void ATSkeletonWindow::slotConnect()
{
	QTreeWidgetItem *twi = ui.treeTunnels->currentItem();
	ATASSERT( twi );
	if ( twi == NULL ) return;

	Tunnel_c *pt = getTunnel(twi);
	ATASSERT( pt );
	if(pt == NULL) return;

    if(pt->iType == TUNNEL_TYPE_FOLDER) {
        if(!pt->strChildNodesCommand.isEmpty()) {
            populateChildNodesWithExternalCommand(twi);
        }
        return;
    } else {
        twi = markConnect(twi);
        if(twi != NULL) {
            crawlConnect(twi);
        }
    }
}

void ATSkeletonWindow::slotDisconnect()
{
	QTreeWidgetItem *twi = ui.treeTunnels->currentItem();
	ATASSERT( twi );
	if ( twi == NULL ) return;

    recursiveDisconnectTunnelSignals( twi );
    recursiveDisconnectTunnel( twi );

}

void ATSkeletonWindow::slotDisconnectAllTunnels()
{
	for(int i=0;i<ui.treeTunnels->topLevelItemCount();i++) {
		QTreeWidgetItem* twi = ui.treeTunnels->topLevelItem(i);
		recursiveDisconnectTunnelSignals(twi);
		recursiveDisconnectTunnel(twi);
	}
}

void ATSkeletonWindow::slotItemActivated()
{
	QTreeWidgetItem *twi = ui.treeTunnels->currentItem();
	if ( twi == NULL ) return;

	qDebug() << "slotItemActivated(): " << twi->text(0);

	Tunnel_c *pt = getTunnel(twi);
	ATASSERT( pt );
	if(pt == NULL) return;

	if(pt->iType == TUNNEL_TYPE_FOLDER) {
        if(pt->bActivateDisconnects) {
            slotDisconnect();
        }
        if(!pt->strChildNodesCommand.isEmpty()) {
            ui.tabWidget->setCurrentIndex( PAGE_CONNECT );
            populateChildNodesWithExternalCommand(twi);
        }
	} else {
		if (pt->pProcess != NULL || pt->iConnectStatus == MARKCONNECT) {
			slotDisconnect();
		} else {
			QString sshHost = pt->getSelectedSSHHost();
			if(!sshHost.isEmpty()) {
				ui.tabWidget->setCurrentIndex( PAGE_CONNECT );
				slotConnect();
			}
		}
	}
}

void ATSkeletonWindow::slotBrowseKeyFile()
{
    QString strCurrentFilename = ui.editSSHKeyFile->text();
	QString strCurrentDir;
	QFileInfo fi(strCurrentFilename);
	if(QFile::exists(fi.absolutePath())) {
		strCurrentDir = fi.absolutePath();
	}

    QString strFilename = QFileDialog::getOpenFileName( ui.widgetEditTunnel, "Select SSH key file", strCurrentDir );

	if ( !strFilename.isNull() )
	{
		ui.editSSHKeyFile->setText( strFilename );
	}
}

void ATSkeletonWindow::delayRetryConnect( Tunnel_c *pt, int autoReconnectIntervalSeconds )
{
    qDebug() << Q_FUNC_INFO;
    if( pt ) {
        ATVERIFY( connect( pt, &Tunnel_c::signalTimerDelayRetryConnect, this, &ATSkeletonWindow::slotDelayRetryConnect ) );
        pt->startTimerDelayRetryConnect(autoReconnectIntervalSeconds);
        updateControlsTunnel(pt);
        //tunnel icon
        QTreeWidgetItem *twi = pt->twi;
        if(twi) {
            treeTunnelSetIcon(twi, Images::icon("ht_130_host_connecting"));
            updateParentFoldersIcon( twi );
        }
    }
}

void ATSkeletonWindow::slotDelayRetryConnect( Tunnel_c *pt )
{
    qDebug() << Q_FUNC_INFO;
    if( pt ) {
        if ( pt->pProcess != NULL ) return; // already connected?
        emit signalAutoConnect( pt );
    }
}

void ATSkeletonWindow::slotConnectorFinished( Tunnel_c *pt )
{
    qDebug() << Q_FUNC_INFO;
	if ( pt ) {
		disconnectTunnel( *pt );
        bool autoReconnectEnabled = m_pMainWindow->preferences()->autoReconnectEnabled();
        int autoReconnectMaxAttempts = m_pMainWindow->preferences()->autoReconnectMaxAttempts();
        int autoReconnectIntervalSeconds = m_pMainWindow->preferences()->autoReconnectIntervalSeconds();
        if ( pt->bAutoReconnect
             && pt->bShouldReconnect
             && autoReconnectEnabled
             && autoReconnectIntervalSeconds > 0
             && pt->iReconnectRetries < autoReconnectMaxAttempts )
        {
            QDateTime dt = QDateTime::currentDateTime().addSecs(autoReconnectIntervalSeconds);
            QString dtStr = dt.toString();

            AddToLog( *pt, QString("Connection lost, reconnecting in %1 seconds at %2 (attempt %3 of %4)\n")
                      .arg(autoReconnectIntervalSeconds)
                      .arg(dtStr)
                      .arg(pt->iReconnectRetries+1)
                      .arg(autoReconnectMaxAttempts) );
            pt->iReconnectRetries++;
            delayRetryConnect(pt, autoReconnectIntervalSeconds);
		} else	{
			recursiveDisconnectTunnelSignals( pt->twi );
			recursiveDisconnectTunnel( pt->twi );
            AddToLog( *pt, "Connection lost, giving up.\n" );
		}
	}
}

QString ATSkeletonWindow::replaceBuiltinVars( QString str )
{
    if(!str.contains("$")) {
        return str;
    }
    QString replaced = str;
    replaced = replaced.replace("${appdir}", g_strAppDirectoryPath, Qt::CaseInsensitive);
    replaced = replaced.replace("$appdir", g_strAppDirectoryPath, Qt::CaseInsensitive);
    replaced = replaced.replace("${datadir}", g_strDataDirectoryPath, Qt::CaseInsensitive);
    replaced = replaced.replace("$datadir", g_strDataDirectoryPath, Qt::CaseInsensitive);
    return replaced;
}

QString ATSkeletonWindow::replaceBuiltinVars( Tunnel_c *pt, QString str )
{
    if(!str.contains("$")) {
        return str;
    }
    QString replaced = replaceBuiltinVars(str);
	replaced = replaced.replace("${host}", pt->getSelectedSSHHost(), Qt::CaseInsensitive);
	replaced = replaced.replace("$host", pt->getSelectedSSHHost(), Qt::CaseInsensitive);
	replaced = replaced.replace("${rhost}", pt->getSelectedRemoteHost(), Qt::CaseInsensitive);
	replaced = replaced.replace("$rhost", pt->getSelectedRemoteHost(), Qt::CaseInsensitive);
	replaced = replaced.replace("${uid}", pt->strUsername, Qt::CaseInsensitive);
	replaced = replaced.replace("$uid", pt->strUsername, Qt::CaseInsensitive);
	replaced = replaced.replace("${pwd}", pt->strPassword, Qt::CaseInsensitive);
	replaced = replaced.replace("$pwd", pt->strPassword, Qt::CaseInsensitive);
	replaced = replaced.replace("${kpwd}", pt->strSSHKeyFilePassword, Qt::CaseInsensitive);
	replaced = replaced.replace("$kpwd", pt->strSSHKeyFilePassword, Qt::CaseInsensitive);
	replaced = replaced.replace("${kfile}", pt->strSSHKeyFile, Qt::CaseInsensitive);
	replaced = replaced.replace("$kfile", pt->strSSHKeyFile, Qt::CaseInsensitive);
    replaced = replaced.replace("${name}", pt->strName.replace("\"",""), Qt::CaseInsensitive);
    replaced = replaced.replace("$name", pt->strName.replace("\"",""), Qt::CaseInsensitive);
	QString strIP = pt->strLocalIP;
	if(strIP.isEmpty()) {
		strIP = "localhost";
	}
	replaced = replaced.replace("${ip}", QString("%1").arg(strIP), Qt::CaseInsensitive);
	replaced = replaced.replace("$ip", QString("%1").arg(strIP), Qt::CaseInsensitive);
    replaced = replaced.replace("${rport}", QString("%1").arg(pt->iRemotePort), Qt::CaseInsensitive);
    replaced = replaced.replace("$rport", QString("%1").arg(pt->iRemotePort), Qt::CaseInsensitive);
	replaced = replaced.replace("${port}", QString("%1").arg(pt->iLocalPort), Qt::CaseInsensitive);
	replaced = replaced.replace("$port", QString("%1").arg(pt->iLocalPort), Qt::CaseInsensitive);
	return replaced;
}

QString ATSkeletonWindow::replacePasswordVars( QString str )
{
    if(!str.contains("$")) {
        return str;
    }

	QString replaced = str;
    PasswordDb *pdb = PasswordDb::getInstance();
    for(unsigned int i = 0; i < pdb->size(); ++i) {
        Identifier ident = pdb->at(i);
		QString name = ident.getDescription().trimmed();
		if(name.isEmpty()) {
			continue;
		}
		QString login = ident.getLogin();
		QString pass = ident.getPassword();
		QString loginVar1 = "${" + name + ".uid}";
		QString loginVar2 = "$" + name + ".uid";
		QString passVar1 = "${" + name + ".pwd}";
		QString passVar2 = "$" + name + ".pwd";
		replaced = replaced.replace(loginVar1, login, Qt::CaseInsensitive);
		replaced = replaced.replace(loginVar2, login, Qt::CaseInsensitive);
		replaced = replaced.replace(passVar1, pass, Qt::CaseInsensitive);
		replaced = replaced.replace(passVar2, pass, Qt::CaseInsensitive);
	}	
	return replaced;
}

QString ATSkeletonWindow::replaceExecutableVariables( QString str )
{
    if(!str.contains("$")) {
        return str;
    }

    //sort reverse by name
    QList<VariableStruct> executableVariables = m_listExecutableVariables;
    std::sort(executableVariables.begin(), executableVariables.end(), VariableStruct::stdComparatorNameReverse);


    //my variable values might themselves have other executable
    //variables in them. Start by resolving these.
    struct ResolvedVariableStruct {
        int nType;
        QString strName;
        QString strExecutable;
        QString strArgs;
        ResolvedVariableStruct(int type, QString name, QString executable, QString args)
        {
            nType = type;
            strName = name;
            strExecutable = executable;
            strArgs = args;
         }
    };

    QList<ResolvedVariableStruct> variablesResolved;
    for(int i = 0; i < executableVariables.size(); i++) {
        VariableStruct var = executableVariables.at(i);
        QString name = var.strName.trimmed();
        if(name.isEmpty()) {
            continue;
        }
        QString strExecutable = replaceBuiltinVars(var.strValue).trimmed();
        variablesResolved.append(ResolvedVariableStruct(var.nType,name,strExecutable,var.strArgs.trimmed()));
    }



    for(int i = 0; i < variablesResolved.size(); i++) {
        ResolvedVariableStruct var = variablesResolved.at(i);
        for(int j = 0; j < 3 && var.strExecutable.contains("$"); j++) {
            for(int k = 0; k < variablesResolved.size() && var.strExecutable.contains("$"); k++) {
                ResolvedVariableStruct var2 = variablesResolved.at(k);
                if(i == k) continue; //it's me
                QString name1 = "${" + var2.strName + "}";
                QString name2 = "$" + var2.strName;
                var.strExecutable = var.strExecutable.replace(name1, var2.strExecutable, Qt::CaseInsensitive).trimmed();
                var.strExecutable = var.strExecutable.replace(name2, var2.strExecutable, Qt::CaseInsensitive).trimmed();
            }
        }
        if(var.strExecutable.contains(" ") && !var.strExecutable.contains("$")
                && !var.strExecutable.contains("\"") && !str.contains("\"")) {
            if(!var.strExecutable.startsWith("\"")) {
                var.strExecutable = "\"" + var.strExecutable;
            }
            if(!var.strExecutable.endsWith("\"")) {
                var.strExecutable = var.strExecutable + "\"";
            }
        }
        variablesResolved.replace(i,var);
    }

    QString replaced = str;
    for(int i = 0; i < variablesResolved.size(); i++) {
        ResolvedVariableStruct var = variablesResolved.at(i);
        if(replaced.contains("$")) {
            QString name1 = "${" + var.strName + "}";
            QString name2 = "$" + var.strName;

            QString replaceWith = var.strExecutable + " " + var.strArgs;
            replaceWith = replaceWith.trimmed();

            replaced = replaced.replace(name1, replaceWith, Qt::CaseInsensitive).trimmed();
            replaced = replaced.replace(name2, replaceWith, Qt::CaseInsensitive).trimmed();
        }
    }

    replaced = replaced.replace("\"\"", "\"", Qt::CaseInsensitive).trimmed();
	return replaced;
}

QString ATSkeletonWindow::replaceUserDefVars( QString str )
{
    if(!str.contains("$")) {
        return str;
    }

    //sort reverse by name
    QList<VariableStruct> userDefinedVariables = m_listUserDefinedVariables;
    std::sort(userDefinedVariables.begin(), userDefinedVariables.end(), VariableStruct::stdComparatorNameReverse);

    qDebug() << "--SORTED--";
    for(int i = 0; i < userDefinedVariables.size(); i++) {
        qDebug() << userDefinedVariables.at(i).strName;
    }

    QList<QPair<QString,QString> > varList;
    //QList<VariableStruct> varListDesc = m_listUserDefinedVariables.so
    for(int i = 0; i < userDefinedVariables.size(); i++) {
        VariableStruct var = userDefinedVariables.at(i);
		QString name = var.strName.trimmed();
		if(name.isEmpty()) {
			continue;
		}
		QString value = var.strValue.trimmed();
		QString name1 = "${" + name + "}";
		QString name2 = "$" + name;
		varList.append(QPair<QString,QString>(name1,value));
		varList.append(QPair<QString,QString>(name2,value));
	}

	QString replaced = str;
    for(int i=0;i<3;i++) {
        if(replaced.contains("$")) {
            for(int j = 0; j < varList.size(); j++) {
                replaced = replaced.replace( varList.at(j).first, varList.at(j).second, Qt::CaseInsensitive );
            }
        }
    }

    return replaced;
}

QString ATSkeletonWindow::replaceVars( Tunnel_c &tunnel, QString str )
{
	Tunnel_c *pt = &tunnel;

	QString replaced = str;
    for(int i=0;i<3;i++) {
        if(replaced.contains("$")) {
            replaced = replaceBuiltinVars(pt, replaced);
            replaced = replacePasswordVars(replaced);
            replaced = replaceExecutableVariables(replaced);
            replaced = replaceUserDefVars(replaced);
            replaced = replaceBuiltinVars(replaced);
        }
    }
	
	return replaced;
}

QString ATSkeletonWindow::replaceVarsLog( Tunnel_c &tunnel, QString str )
{
	QString replaced = str;

	replaced = replaceVars(tunnel, replaced);

	if(m_pMainWindow->preferences()->hidePasswordsInLogEnabled())
	{
		//Replace passwords
		if(!tunnel.strPassword.trimmed().isEmpty()) {
			replaced = replaced.replace(tunnel.strPassword, "*****");
		}
		if(!tunnel.strSSHKeyFilePassword.trimmed().isEmpty()) {
			replaced = replaced.replace(tunnel.strSSHKeyFilePassword, "*****");
		}
        PasswordDb *pdb = PasswordDb::getInstance();
        for(unsigned int i = 0; i < pdb->size(); ++i) {
            Identifier ident = pdb->at(i);
			QString password = ident.getPassword();
			if(!password.trimmed().isEmpty()) {
				replaced = replaced.replace(password, "*****");
			}
		}
	}

	return replaced;
}

QStringList ATSkeletonWindow::replaceVars( Tunnel_c &tunnel, QStringList strList ) {
    // Create a copy
    QStringList replaced = strList;

    // Apply replaceVarsLog to each element
    for (QString &arg : replaced) {
        arg = replaceVars(tunnel, arg);
    }

    return replaced;
}

QStringList ATSkeletonWindow::replaceVarsLog( Tunnel_c &tunnel, QStringList strList ) {
    // Create a copy
    QStringList replaced = strList;

    // Apply replaceVarsLog to each element
    for (QString &arg : replaced) {
        arg = replaceVarsLog(tunnel, arg);
    }

    return replaced;
}

void ATSkeletonWindow::slotConnected( QTreeWidgetItem *twi )
{
	qDebug() << "setIcon 4";
	treeTunnelSetIcon(twi, Images::icon("ht_140_host_connected"));
 	emit signalSetTrayIcon( 1 );
	Tunnel_c *pt = getTunnel(twi);
	pt->iConnectStatus = CONNECTED;
	updateControlsTunnel(pt);
    pt->iReconnectRetries = 0;
    pt->bShouldReconnect = true;
	pt->iPasswordRetries = 0;
	pt->iKeyPasswordRetries = 0;
	pt->iChallengeResponseRetries = 0;
    AddToLog( *pt, "Connected.\n" );
	updateParentFoldersIcon( twi );   
	crawlConnect(twi);
    m_pMainWindow->m_pSearchWidget->slotUpdateCompleterIcons();
}

void ATSkeletonWindow::slotKillConnection( QTreeWidgetItem *twi )
{
    recursiveDisconnectTunnelSignals( twi );
    recursiveDisconnectTunnel( twi );
}

/**
* Find real tunnel parent. Skips parent folders.
* twi										=> returns NULL
* rootfolder -> tunnel1 -> folder -> twi	=> returns tunnel1
* roottunnel -> folder -> twi				=> returns roottunnel
* roottunnel -> twi							=> returns roottunnel
* rotttunnel -> tunnel1 -> twi				=> returns tunnel1
* rootfolder -> folder -> twi				=> returns NULL
* rootfolder -> twi							=> returns NULL
* 
*/
QTreeWidgetItem* ATSkeletonWindow::findParentTunnelNode(QTreeWidgetItem *twi)
{
	QTreeWidgetItem *twiParent = twi->parent();
	
	//crawl up 
	while(twiParent != NULL) {
		Tunnel_c *ptParent = getTunnel(twiParent);
		ATASSERT(ptParent);
		if(ptParent->iType == TUNNEL_TYPE_FOLDER) {
			qDebug() << "findParentTunnelNode() parent is folder, skip " << ptParent->strName;
			twiParent = twiParent->parent();
		} else {
			break; //found parent tunnel
		}
	}

	if(twiParent != NULL) {
		Tunnel_c *pt = getTunnel(twiParent);
		if(pt->iType == TUNNEL_TYPE_FOLDER) {
			twiParent = NULL;
		}
	}

	return twiParent;
}

/**
* Find first parent folder. Skips parent tunnels.
* twi										=> returns NULL
* rootfolder								=> returns NULL
* rootfolder -> folder						=> returns rootfolder
* rootfolder -> tunnel1 -> folder -> twi	=> returns folder
* roottunnel -> folder -> twi				=> returns folder
* roottunnel -> twi							=> returns NULL
* rotttunnel -> tunnel1 -> twi				=> returns NULL
* rootfolder -> folder -> twi				=> returns folder
* rootfolder -> twi							=> returns rootfolder
* 
*/
QTreeWidgetItem* ATSkeletonWindow::findParentFolderNode(QTreeWidgetItem *twi)
{
	QTreeWidgetItem *twiParent = twi->parent();
	
	//crawl up 
	while(twiParent != NULL) {
		Tunnel_c *ptParent = getTunnel(twiParent);
		ATASSERT(ptParent);
		if(ptParent->iType == TUNNEL_TYPE_FOLDER) {
			break; //found parent folder
		} else {
			qDebug() << Q_FUNC_INFO << "parent is tunnel, skip " << ptParent->strName;
			twiParent = twiParent->parent();
		}
	}

	if(twiParent != NULL) {
		Tunnel_c *pt = getTunnel(twiParent);
		if(pt->iType != TUNNEL_TYPE_FOLDER) {
			twiParent = NULL;
		}
	}

	return twiParent;
}

/**
* Find real tunnel childs. Traverse child folders.
* twi	(tunnel)						=> returns 0
* twi	(folder)						=> returns 0
* twi (folder)	-> folder1 -> folder2	=> returns 0
* twi (folder/tunnel)	-> tunnel1		=> returns tunnel1
* twi (folder/tunnel)	-> folder1 		=> returns tunnel1
							-> tunnel1
* twi (folder/tunnel)	-> tunnel1		=> returns tunnel1,tunnel2,tunnel3,tunnel4
						-> tunnel2
							->tunnel22
						-> folder1
						-> folder2
							-> tunnel3
							-> folder3
								-> tunnel4
*/
QList<QTreeWidgetItem*> ATSkeletonWindow::findChildTunnelNodes(QTreeWidgetItem *twi)
{
	qDebug() << Q_FUNC_INFO << twi->text(0);

	QList<QTreeWidgetItem*> twiList;

	for(int i=0;i<twi->childCount();i++) {
		QTreeWidgetItem* child = twi->child(i);
		Tunnel_c *pt = getTunnel(child);
		ATASSERT(pt);
		if(pt->iType == TUNNEL_TYPE_FOLDER) {
			qDebug() << Q_FUNC_INFO << "enter folder " << child->text(0);
			twiList.append( findChildTunnelNodes(child) );
		} else {
			qDebug() << Q_FUNC_INFO << "append tunnel " << child->text(0);
			twiList.append( child );
		}
	}

	return twiList;
}

QTreeWidgetItem* ATSkeletonWindow::markConnect( QTreeWidgetItem* twi )
{
    Tunnel_c *pt = getTunnel(twi);
	if(pt == NULL) return NULL;

	if ( pt->iConnectStatus == CONNECTED || pt->iConnectStatus == CONNECTING ) return NULL; // already connected or connecting/retrying

	if ( pt->iConnectStatus == DISCONNECTED ) {
		qDebug() << "setIcon 5";
		treeTunnelSetIcon(twi, Images::icon("ht_130_host_connecting"));
		pt->iConnectStatus = MARKCONNECT;
		updateControlsTunnel(pt);
		Tunnel_c *ptParent = getTunnel(findParentTunnelNode(twi)); //returns NULL of there is no parent
		if(ptParent != NULL && ptParent->iConnectStatus == DISCONNECTED) {
            AddToLog( *pt, "Waiting for parent to get connected...\n" );
		}	
	}

	while(findParentTunnelNode(twi) != NULL) {
		twi = findParentTunnelNode(twi);
		pt = getTunnel(twi);
		if( pt->iConnectStatus == CONNECTED || pt->iConnectStatus == CONNECTING ) break; // parent already connected or connecting/retrying
		if ( pt->iConnectStatus == DISCONNECTED ) {
			qDebug() << "setIcon 6";
			treeTunnelSetIcon(twi, Images::icon("ht_130_host_connecting"));
			pt->iConnectStatus = MARKCONNECT;
			updateControlsTunnel(pt);
			Tunnel_c *ptParent = getTunnel(findParentTunnelNode(twi)); //returns NULL of there is no parent
			if(ptParent != NULL && ptParent->iConnectStatus == DISCONNECTED) {
                AddToLog( *pt, "Waiting for parent to get connected...\n" );
			}
		}
	}

	if(findParentTunnelNode(twi) != NULL) {
		pt = getTunnel(findParentTunnelNode(twi));
		if(pt->iConnectStatus == MARKCONNECT || pt->iConnectStatus == CONNECTING) {
			twi = NULL;
		}
	}

	return twi;
}

void ATSkeletonWindow::crawlConnect( QTreeWidgetItem* twi )
{
	if(twi != NULL) 
	{
		Tunnel_c *pt = getTunnel(twi);
		if(pt == NULL) return;
		if(pt->iConnectStatus == MARKCONNECT) {
			qDebug() << "crawlConnect() one item " << pt->strName;
			connectTunnel(*pt);
		} else {
			qDebug() << "crawlConnect() children " << pt->strName;
			for(int i=0;i<twi->childCount();i++) {
				QTreeWidgetItem* child = twi->child(i);
				pt = getTunnel(child);
				if(pt == NULL) continue;
				if(pt->iType == TUNNEL_TYPE_FOLDER) {
					qDebug() << "crawlConnect() recursive crawlConnect child folder " << pt->strName;
					crawlConnect(child);
					continue;
				}
				if(pt->iConnectStatus != MARKCONNECT) continue;
				connectTunnel(*pt);
			}
		}
	}
	else
	{
		for(int i=0;i<ui.treeTunnels->topLevelItemCount();i++) {
			twi = ui.treeTunnels->topLevelItem(i);
			Tunnel_c *pt = getTunnel(twi);
			if(pt == NULL) continue;
			if(pt->iType == TUNNEL_TYPE_FOLDER) {
				qDebug() << "crawlConnect() recursive crawlConnect child folder " << pt->strName;
				crawlConnect(twi);
				continue;
			}
			if(pt->iConnectStatus != MARKCONNECT) continue;
			qDebug() << "crawlConnect() root item " << pt->strName;
			connectTunnel(*pt);
		}
	}
}


void ATSkeletonWindow::connectTunnel( Tunnel_c &tunnel )
{
    tunnel.stopTimerDelayRetryConnect();

	ATASSERT(tunnel.twi);
	if ( !tunnel.twi ) return;

    Tunnel_c *pt = &tunnel;
	QTreeWidgetItem *twi = pt->twi;
	
	if(m_pMainWindow->preferences()->autoClearLogEnabled()) {
		clearTunnelLog(pt);
	}

	pt->iPasswordRetries = 0; //counts the number of times we have sent the password to stdin
	pt->iKeyPasswordRetries = 0; //counts the number of times we have sent the private key password to stdin
	pt->iChallengeResponseRetries = 0; //counts the number of times we have sent the challenge response password to stdin

	//+++ setup plink command string

	qDebug() << "connectTunnel()" << pt->strName;

	qDebug( "Connecting %s...", qPrintable( pt->strName ) );
    AddToLog( tunnel, QString("Connecting %1...\n").arg( pt->strName ) );
	qDebug( "Host: %s", qPrintable( pt->getSelectedSSHHost() ) );
    AddToLog( tunnel, QString("Host: %1\n").arg( pt->getSelectedSSHHost() ) );

	if ( !pt->getSelectedRemoteHost().isEmpty() )
	{
		if(pt->strLocalIP.isEmpty()) {
			qDebug( "Tunnel: %d:%s:%d", pt->iLocalPort, qPrintable( pt->getSelectedRemoteHost() ), pt->iRemotePort );
            AddToLog( tunnel, QString("Tunnel: %1:%2:%3\n").arg(pt->iLocalPort).arg( pt->getSelectedRemoteHost() ).arg(pt->iRemotePort) );
		} else {
			qDebug( "Tunnel: %s:%d:%s:%d", qPrintable( pt->strLocalIP ), pt->iLocalPort, qPrintable( pt->getSelectedRemoteHost() ), pt->iRemotePort );
            AddToLog( tunnel, QString("Tunnel: %1:%2:%3:%4\n").arg( pt->strLocalIP ).arg(pt->iLocalPort).arg( pt->getSelectedRemoteHost() ).arg(pt->iRemotePort) );
		}
	}

	if ( pt->getSelectedSSHHost().isEmpty() )
 	{
		qDebug( "Error: Tunnel %s has no host, please check the settings.", qPrintable( pt->strName ) );
        AddToLog( tunnel, QString("Error: Tunnel %1 has no host, please check the settings.\n").arg( pt->strName ) );
 		return;
 	}
 
 #ifdef WIN32
    QString strPlink = "";
    for(int i = 0; i < m_listExecutableVariables.size(); i++) {
        VariableStruct var = m_listExecutableVariables.at(i);
        QString name = var.strName.trimmed();
        if(name == "plink") {
            strPlink = var.strValue.trimmed();
        }
    }
    // Check that the executable is found if set in var executables
    if(!strPlink.isEmpty()) {
        QString strPlinkLog = replaceVarsLog(*pt, strPlink);
        strPlink = replaceVars(*pt, strPlink);
        QFileInfo qfi = QFileInfo(strPlink);
        if(!qfi.exists()) {
            qDebug( "Error: Could not find plink executable %s. Verfiy the executable variable plink in settings.", qPrintable( strPlinkLog ) );
            AddToLog( tunnel, QString("Error: Could not find %1. Verfiy the executable variable plink in settings.\n").arg( strPlinkLog ) );
            return;
        }
    }
    // plink executable var NOT set, use default plink.exe in DoffenSSHTunnel app directory
    if(strPlink.isEmpty()) {
        strPlink = "plink.exe";
        QDir dir( g_strAppDirectoryPath );
 		if ( !dir.exists( strPlink ) )
 		{
            qDebug( "Error: Could not find %s, please check that it is in the directory %s OR add executable variable plink in settings.", qPrintable( strPlink ), qPrintable( dir.absolutePath() ) );
            AddToLog( tunnel, QString("Error: Could not find %1, please check that it is in the directory %2 OR add executable variable plink in settings.\n").arg( strPlink ).arg( dir.absolutePath() ) );
            return;
 		}
        strPlink = QFileInfo(dir, strPlink).absoluteFilePath();
 	}
    if(strPlink.contains(" ")) {
        if(!strPlink.startsWith("\"")) {
            strPlink = "\"" + strPlink;
        }
        if(!strPlink.endsWith("\"")) {
            strPlink = strPlink + "\"";
        }
    }
 #else
    QString strPlink = "";
    for(int i = 0; i < m_listExecutableVariables.size(); i++) {
        VariableStruct var = m_listExecutableVariables.at(i);
        QString name = var.strName.trimmed();
        if(name == "sshpass") {
            //I use this when developing sshpass
            //Setting it in executable vars to point to sshpass in my deps dir
            strPlink = var.strValue.trimmed();
        }
    }
    if(strPlink.isEmpty()) {
        strPlink = "sshpass";
    }
    // Check that the executable is found
    if(strPlink == "sshpass") {
        QDir dir( g_strAppDirectoryPath );
        if ( !dir.exists( strPlink ) )
        {
            qDebug( "Error: Could not find %s, please check that it is in the directory %s", qPrintable( strPlink ), qPrintable( dir.absolutePath() ) );
            AddToLog( tunnel, QString("Error: Could not find %1, please check that it is in the directory %2").arg(strPlink , dir.absolutePath() ) );
             return;
        }

        strPlink = QFileInfo(dir, strPlink).absoluteFilePath();
        strPlink = strPlink + " ssh";
    }

 #endif

    bool usingPlink = strPlink.toLower().contains("plink");

 	if ( pt->pProcess != NULL ) return; // already connected?
 
    if(usingPlink) {
        QVersionNumber pLinkVersion = getPlinkVersion(strPlink, tunnel);
        if(pLinkVersion.isNull()) {
            AddToLog( tunnel, "Failed to detect plink version!" );
        } else {
            AddToLog( tunnel, "Detected plink version: " + pLinkVersion.toString());
        }
    }

    QStringList arguments;
 
    arguments << "-v"; // increase verbosity
 
    QStringList strListSSHHost = pt->getSelectedSSHHost().split( ':', Qt::SkipEmptyParts );
    if ( strListSSHHost.count() == 1 ) strListSSHHost << "22";

    if(usingPlink) {
        //windows with plink.exe or linux with plink
        QVersionNumber pLinkVersion = getPlinkVersion(strPlink, tunnel);
        if(pLinkVersion.isNull()) {
            AddToLog( tunnel, "Failed to detect plink version!" );
        } else {
            AddToLog( tunnel, "Detected plink version: " + pLinkVersion.toString());
        }
        //windows with plink.exe or linux with plink
        if(pLinkVersion >= QVersionNumber(0, 82)) {
            // https://the.earth.li/~sgtatham/putty/0.82/htmldoc/Chapter3.html#using-cmdline-legacy-stdio-prompts
            arguments << "-legacy-stdio-prompts" << "-legacy-charset-handling";
        }
    }

    if(usingPlink) {
        //windows with plink.exe or linux with plink
        arguments << strListSSHHost.at(0) << "-P" << strListSSHHost.at(1);
    } else {
        //sshpass or ssh (openssh) on mac/linux
        arguments << strListSSHHost.at(0) << "-p" << strListSSHHost.at(1);
    }

    if ( !pt->strUsername.isEmpty() ) {
        arguments << "-l" << pt->strUsername;
    }
 
    if ( pt->bCompression ) {
        arguments <<  "-C";
    }
 
    if(pt->iSSH1or2 == 1) {
        arguments << "-1";
    } else {
        arguments << "-2";
    }
	
	if ( !pt->getSelectedRemoteHost().isEmpty() && pt->iLocalPort > 0 && pt->iRemotePort > 0 )
	{
        if(!usingPlink) {
            arguments << "-g";
        }
		if(pt->strLocalIP.isEmpty()) {
            arguments << "-L" << QString( "%1:%2:%3" ).arg( pt->iLocalPort ).arg( pt->getSelectedRemoteHost()).arg( pt->iRemotePort );
		} else {
            arguments << "-L" << QString( "%1:%2:%3:%4" ).arg( pt->strLocalIP ).arg( pt->iLocalPort ).arg( pt->getSelectedRemoteHost()).arg( pt->iRemotePort );
		}
	}

	for(int i=0;i<pt->portForwardList.size();i++) {
		PortForwardStruct pfs = pt->portForwardList.at(i);
		if(!pfs.bEnabled) continue;
		if(pfs.nType == PortForwardStruct::LOCAL) {
			if(pfs.nLocalPort > 0 && !pfs.strDestinationHost.isEmpty() && pfs.nDestinationPort > 0) {
				if(pfs.strLocalIP.isEmpty()) {
                    arguments << "-L" << QString( "%1:%2:%3" ).arg( pfs.nLocalPort ).arg( pfs.strDestinationHost ).arg( pfs.nDestinationPort );
				} else {
                    arguments << "-L" << QString( "%1:%2:%3:%4" ).arg( pfs.strLocalIP ).arg( pfs.nLocalPort ).arg( pfs.strDestinationHost ).arg( pfs.nDestinationPort );
				}
			}
		} else if(pfs.nType == PortForwardStruct::REMOTE) {
			if(pfs.nLocalPort > 0 && !pfs.strDestinationHost.isEmpty() && pfs.nDestinationPort > 0) {
				if(pfs.strLocalIP.isEmpty()) {
                    arguments << "-R" << QString( "%1:%2:%3" ).arg( pfs.nLocalPort ).arg( pfs.strDestinationHost ).arg( pfs.nDestinationPort );
				} else {
                    arguments << "-R" << QString( "%1:%2:%3:%4" ).arg( pfs.strLocalIP ).arg( pfs.nLocalPort ).arg( pfs.strDestinationHost ).arg( pfs.nDestinationPort );
				}
			}
		} else if(pfs.nType == PortForwardStruct::DYNAMIC) {
			if(pfs.nLocalPort > 0) {
				if(pfs.strLocalIP.isEmpty()) {
                    arguments << "-D" << QString( "%1" ).arg(pfs.nLocalPort);
				} else {
                    arguments << "-D" << QString( "%1:%2" ).arg( pfs.strLocalIP ).arg( pfs.nLocalPort );
				}
			}
		}
	}

	if ( !pt->strSSHKeyFile.trimmed().isEmpty() ) {
        QString keyFile = replaceVars(*pt, pt->strSSHKeyFile.trimmed());
        keyFile = keyFile.replace("\"","");
        keyFile = keyFile.replace("''","");
        QFileInfo keyFileInfo = QFileInfo(keyFile);
        //When moving the .ini file between OS'es there are certain things that
        //that must be converted on the fly. For instance the private key file
        //on windows is named <keyfile>.ppk but on mac and linux it's <keyfile>.pem or just <keyfile> (no extension)
        if(keyFileInfo.dir().exists()) {
            if(usingPlink) {
                //On windows
                if(keyFileInfo.suffix().isEmpty()) {
                    //This host has most probably been defined on mac or linux since there are no suffix (extension) in the filename
                    keyFileInfo = QFileInfo(keyFileInfo.filePath()+".ppk");
                } else if(keyFileInfo.suffix() == "pem") {
                    //This host has most probably been defined on mac or linux since the suffix (extension) is .pem
                    keyFileInfo = QFileInfo(keyFileInfo.filePath().replace(".pem",".ppk"));
                }
            } else {
                //On Mac or Linux
                if(keyFileInfo.suffix() == "ppk") {
                    //This host has most probably been defined on windows the suffix is .ppk (the putty format)
                    keyFileInfo = QFileInfo(keyFileInfo.filePath().replace(".ppk",".pem"));
                    if(!keyFileInfo.exists()) {
                        //The .pem file does not exist. It might also
                        keyFileInfo = QFileInfo(keyFileInfo.filePath().replace(".pem",""));
                    }
                }
            }
        } else {
            qWarning( "Could not find SSH Key File directory %s", qPrintable( keyFileInfo.dir().path() ) );
            AddToLog( tunnel, QString("Error: Could not find SSH Key File directory %1").arg( keyFileInfo.dir().path() ) );
            return;
        }
        if(!keyFileInfo.exists()) {
            //Just fall back to the original key file they specified so that we log exactly what they specified
            //and not one of the converted names we might have tried above.
            keyFileInfo = QFileInfo(keyFile);
        }
        keyFile = keyFileInfo.filePath();
        if(keyFile.indexOf(" ") != -1) {
            if(!keyFile.startsWith("\"")) {
                keyFile = "\"" + keyFile;
            }
            if(!keyFile.endsWith("\"")) {
                keyFile = keyFile + "\"";
            }
        }
        arguments << "-i" << keyFile;
	}
 
    arguments << pt->strExtraArguments;

    QString strCommandLog = replaceVarsLog(*pt, QString("%1 %2")
        .arg(replaceVarsLog(*pt, strPlink), replaceVarsLog(*pt, arguments).join(' ')) );
    AddToLog( tunnel, QString("%1\n").arg( strCommandLog ) );

	//--- setup plink command string

	//+++ check that tunnel local port is free

	if ( true )
	{
		//User configured the default Tunnel
		//Key: port number, Value: connect status (CONNECTED, CONNECTING)
		QList<int> portList = getLocalTunnelPortsInUse(pt);
		if(portList.size() > 0) {
			QStringList sPortList;
			for(int i=0;i<portList.size()&&i<5;i++) {
				sPortList.append(QString("%1").arg(portList.at(i)));
			}
			if(portList.size()>5) {
				sPortList.append("...");
			}
            QString errMsg = QString("Cannot connect host %1.\nLocal tunnel port(s) %2 already in use.")
                    .arg(pt->strName, sPortList.join(","));
			qDebug( "Error: %s", qPrintable( errMsg ) );
            AddToLog( tunnel, QString("Error: %1\n").arg( errMsg ) );
			recursiveDisconnectTunnelSignals(twi);
			recursiveDisconnectTunnel(twi);
			QMessageBox::warning(this, "Oops!", errMsg);
			return;
		}
	}

	//--- check that tunnel local port is free

	//+++ setup the process

	treeTunnelSetIcon(twi, Images::icon("ht_130_host_connecting"));

 	ATASSERT( pt->pConnector == NULL );
 	pt->pConnector = new ATTunnelConnector_c( this, tunnel.twi );
 
	pt->iConnectStatus = CONNECTING;
	updateControlsTunnel(pt);

 	pt->pProcess = new QProcess;
 	pt->pProcess->setProcessChannelMode( QProcess::MergedChannels );

    ATVERIFY( connect( pt->pProcess, &QProcess::readyReadStandardOutput, pt->pConnector, &ATTunnelConnector_c::slotProcessReadStandardOutput ) );
    ATVERIFY( connect( pt->pProcess, &QProcess::readyReadStandardError, pt->pConnector, &ATTunnelConnector_c::slotProcessReadStandardError ) );
    ATVERIFY( connect( pt->pProcess, &QProcess::errorOccurred, pt->pConnector, &ATTunnelConnector_c::slotProcessError ) );
    ATVERIFY( connect( pt->pProcess, &QProcess::finished, pt->pConnector, &ATTunnelConnector_c::slotProcessFinished ) );
    ATVERIFY( connect( pt->pConnector, &ATTunnelConnector_c::finished, this, &ATSkeletonWindow::slotConnectorFinished, Qt::QueuedConnection ) );
    ATVERIFY( connect( pt->pConnector, &ATTunnelConnector_c::signalConnected, this, &ATSkeletonWindow::slotConnected, Qt::QueuedConnection ) );
    ATVERIFY( connect( pt->pConnector, &ATTunnelConnector_c::signalKillConnection, this, &ATSkeletonWindow::slotKillConnection, Qt::QueuedConnection ) );

	//--- setup the process

    //QString strCommand = replaceVars(*pt, QString("%1 %2").arg(strPlink).arg(arguments.join(' ')) );
    pt->pProcess->start( replaceVars(*pt, strPlink), replaceVars(*pt, arguments) );
}

QVersionNumber ATSkeletonWindow::getPlinkVersion(const QString& plinkPath, Tunnel_c &tunnel)
{
    QStringList arguments;
    arguments << "--version";

    AddToLog(tunnel, QString("\"%1\" %2").arg(plinkPath, arguments.join(' ')));

    QProcess versionProcess;
    versionProcess.start(plinkPath, arguments);
    if (!versionProcess.waitForFinished(1000)) {
        AddToLog( tunnel, "Timeout waiting for plink version process to finish" );
        return QVersionNumber(); // Invalid version
    }

    QString output = QString::fromLocal8Bit(versionProcess.readAllStandardOutput());
    QRegularExpression re(R"(Release\s+(\d+)\.(\d+))");
    QRegularExpressionMatch match = re.match(output);

    if (match.hasMatch()) {
        int major = match.captured(1).toInt();
        int minor = match.captured(2).toInt();
        return QVersionNumber(major, minor);
    }

    AddToLog( tunnel, QString("Could not parse plink version output '%1'").arg(output) );
    return QVersionNumber(); // Invalid version
}



void ATSkeletonWindow::populateChildNodesWithExternalCommand(QTreeWidgetItem* twi)
{
    qDebug() << Q_FUNC_INFO << twi->text(0);

    ATASSERT(twi);
    if(twi == NULL) return;
    Tunnel_c *pt = getTunnel(twi);
    ATASSERT(pt);
    if(pt == NULL) return;

    if(pt->pPopulateChildNodesProcess != NULL) {
        disconnect( pt->pPopulateChildNodesProcess, 0, 0, 0 );
        pt->pPopulateChildNodesProcess->kill();
        bool bOk = pt->pPopulateChildNodesProcess->waitForFinished( WAIT_FOR_FINISHED_TIMEOUT );
        Q_UNUSED(bOk)
        pt->pPopulateChildNodesProcess->deleteLater();
        pt->pPopulateChildNodesProcess = NULL;
    }
    if(pt->pPopulateChildNodesConnector != NULL) {
        disconnect( pt->pPopulateChildNodesConnector, 0, 0, 0 );
        pt->pPopulateChildNodesConnector->deleteLater();
        pt->pPopulateChildNodesConnector = NULL;
    }

    if(pt->strChildNodesCommand.trimmed().isEmpty()) {
        return;
    }

    if(pt->iType != TUNNEL_TYPE_FOLDER) {
        return;
    }

    if(m_pMainWindow->preferences()->autoClearLogEnabled()) {
        clearTunnelLog(pt);
    }

    QString strCommand = pt->strChildNodesCommand;

    AddToLog( *pt, QString("> %1\n").arg( replaceVarsLog(*pt, strCommand) ) );

    pt->pPopulateChildNodesConnector = new ATPopulateChildNodesConnector_c( this, twi );

    pt->pPopulateChildNodesProcess = new QProcess;
    pt->pPopulateChildNodesProcess->setProcessChannelMode( QProcess::MergedChannels );

    ATVERIFY( connect( pt->pPopulateChildNodesProcess,      &QProcess::readyReadStandardOutput,
                       pt->pPopulateChildNodesConnector,    &ATPopulateChildNodesConnector_c::slotProcessReadStandardOutput ) );
    ATVERIFY( connect( pt->pPopulateChildNodesProcess,      &QProcess::readyReadStandardError,
                       pt->pPopulateChildNodesConnector,    &ATPopulateChildNodesConnector_c::slotProcessReadStandardError ) );
    ATVERIFY( connect( pt->pPopulateChildNodesProcess,      &QProcess::errorOccurred,
                       pt->pPopulateChildNodesConnector,    &ATPopulateChildNodesConnector_c::slotProcessError ) );
    ATVERIFY( connect( pt->pPopulateChildNodesProcess,      &QProcess::finished,
                       pt->pPopulateChildNodesConnector,    &ATPopulateChildNodesConnector_c::slotProcessFinished ) );
    ATVERIFY( connect( pt->pPopulateChildNodesConnector,    &ATPopulateChildNodesConnector_c::finished,
                       this, &ATSkeletonWindow::slotConnectorPopulateChildNodesWithExternalCommandFinished, Qt::QueuedConnection ) );

    pt->pPopulateChildNodesProcess->start( replaceVars(*pt, strCommand) );
    if(pt->pPopulateChildNodesProcess->waitForStarted() ) {
        QProgressDialog *pd = new QProgressDialog("Fetching data...", "Cancel", 0, 0, this, Qt::CustomizeWindowHint);
        ATVERIFY( connect( pt->pPopulateChildNodesProcess, &QProcess::finished, pd, &QProgressDialog::cancel ) );
        ATVERIFY( connect( pd, &QProgressDialog::canceled, pt->pPopulateChildNodesConnector, &ATPopulateChildNodesConnector_c::slotCancel ) );
        pd->setModal(true);
        pd->show();
    } else {
        pt->pPopulateChildNodesProcess->deleteLater();
        pt->pPopulateChildNodesProcess = NULL;
        pt->pPopulateChildNodesConnector->deleteLater();
        pt->pPopulateChildNodesConnector = NULL;
    }
}

void ATSkeletonWindow::slotConnectorPopulateChildNodesWithExternalCommandFinished( Tunnel_c *pt )
{
    qDebug() << Q_FUNC_INFO;

    ATASSERT(pt);
    if(pt == NULL) return;

    QTreeWidgetItem* twi = pt->twi;
    ATASSERT(twi);
    if(twi == NULL) return;

    ATASSERT(pt->pPopulateChildNodesConnector);

    bool bOk = false; //set to true when OK, show dialog at the end if false.


    enableTreeTunnelsPaste(false);

    bool blocked = ui.treeTunnels->blockSignals(true);

    int exitCode = pt->pPopulateChildNodesConnector->getExitCode();
    QString rawOutput = pt->pPopulateChildNodesConnector->getOutput().simplified();
    bool bCanceled = pt->pPopulateChildNodesConnector->isCanceled();

    int jsonDataBeginIndex = rawOutput.indexOf("BEGIN_DST_JSON_DATA")+19;
    int jsonDataEndIndex = rawOutput.lastIndexOf("END_DST_JSON_DATA");

    bool bHasJsonOutput = false;
    if(exitCode == 0 && !bCanceled) {
        bHasJsonOutput = true;
        if(jsonDataBeginIndex == -1) {
            bHasJsonOutput = false;
            AddToLog( *pt, QString("> String not found BEGIN_DST_JSON_DATA\n") );
        }
        else if(jsonDataEndIndex == -1) {
            bHasJsonOutput = false;
            AddToLog( *pt, QString("> String not found END_DST_JSON_DATA\n") );
        }
        else if((jsonDataEndIndex-jsonDataBeginIndex) <= 0) {
            bHasJsonOutput = false;
            AddToLog( *pt, QString("> Found nothing between BEGIN_DST_JSON_DATA and END_DST_JSON_DATA\n") );
        }
    }

    if(bHasJsonOutput) {

        QString jsonOutput = rawOutput.mid(jsonDataBeginIndex,jsonDataEndIndex-jsonDataBeginIndex);

        qDebug() << "--->" << jsonOutput << "<---";

        QJsonParseError err;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonOutput.toUtf8(), &err);

        if(err.error == QJsonParseError::NoError) {

            QJsonObject json = jsonDoc.object();

            if(json.contains("Hosts")) {

                QJsonArray jsonHosts = json["Hosts"].toArray();

                twi->setExpanded(true);

                //If we remove a connected tunnel, then remembers its
                //local port. This is to ensure we do not auto-propose
                //this local port.
                QList<int> excludePorts;

                //Current hosts to be removed
                QList<QTreeWidgetItem*> toRemove;
                for (int i = 0; i < twi->childCount(); i++) {
                    QTreeWidgetItem *child = twi->child(i);
                    Tunnel_c *pt2 = getTunnel(child);
                    if(pt2 == NULL || pt2->bChildNodesCommandType) {
                        toRemove.append(child);
                        if(pt2->iConnectStatus != DISCONNECTED) {
                            excludePorts.append(pt2->iLocalPort);
                        }
                    }
                }
                for (int i = 0; i < toRemove.size(); i++) {
                    twi->removeChild(toRemove[i]);
                }

                //Add the new hosts
                int levelOffset = TreeWidget::getItemLevel(twi) + 1;

                for(int jsonHostsIndex = 0; jsonHostsIndex < jsonHosts.size(); jsonHostsIndex++) {

                    //insert new host in the tree

                    QJsonObject jsonHost = jsonHosts[jsonHostsIndex].toObject();
                    Tunnel_c *newTunnel = readSettingsHost(jsonHost);
                    newTunnel->bChildNodesCommandType = true; //Allows it to be deleted whenever we refresh this list
                    newTunnel->iLevel += levelOffset;

                    QTreeWidgetItem *newTwi = new QTreeWidgetItem();
                    newTunnel->twi = newTwi;

                    twi->insertChild(twi->childCount(), newTunnel->twi);

                    newTwi->setExpanded(newTunnel->bIsExpanded);

                    if(newTunnel->iType == TUNNEL_TYPE_TUNNEL)	{
                        treeTunnelSetIcon(newTwi, Images::icon("ht_120_host_disconnected"));
                    } else if(newTunnel->iType == TUNNEL_TYPE_FOLDER) {
                        if(newTunnel->twi->isExpanded()) {
                            treeTunnelSetIcon(newTwi, Images::icon("ht_150_folder_open_disconnected"));
                        } else {
                            treeTunnelSetIcon(newTwi, Images::icon("ht_170_folder_closed_disconnected"));
                        }
                    }

                    if(!newTunnel->strFgColor.isEmpty()) {
                        QColor color = QColor(newTunnel->strFgColor);
                        if(color.isValid()) {
                            QBrush brush = newTunnel->twi->foreground(0);
                            brush.setColor(color);
                            newTwi->setForeground(0,brush);
                        }
                    }

                    if(!newTunnel->strBgColor.isEmpty()) {
                        QColor color = QColor(newTunnel->strBgColor);
                        if(color.isValid()) {
                            QBrush brush = QBrush(color);
                            newTwi->setBackground(0,brush);
                        }
                    }

                    newTwi->setFlags(newTunnel->twi->flags() | Qt::ItemIsEditable);
                    newTwi->setText( 0, newTunnel->strName );

                    QString tooltip = jsonHost.value("Tooltip").toString().trimmed();
                    newTwi->setToolTip(0,tooltip);

                    setTunnel(newTwi, newTunnel);
                    treeTunnelUpdateFromParent(newTwi);

                    if(newTunnel->iLocalPort == 0) {
                        setNewLocalPort(newTwi, false, excludePorts);
                    }

                    //keep connected ?

                    if(!newTunnel->strExtID.isEmpty() && jsonHost.value("KeepConn").toBool(false)) {

                        Tunnel_c *ptToKeepConn = NULL;
                        for(int k=0;k<toRemove.size();k++) {
                            QTreeWidgetItem *twiToRemove = toRemove.at(k);
                            Tunnel_c *ptToRemove = getTunnel(twiToRemove);
                            if(ptToRemove == NULL) {
                                continue;
                            }
                            if(ptToRemove->iConnectStatus != CONNECTED) {
                                continue;
                            }
                            if(ptToRemove->pProcess == NULL) {
                                continue;
                            }
                            if(ptToRemove->pConnector == NULL) {
                                continue;
                            }
                            if(ptToRemove->strExtID != newTunnel->strExtID) {
                                continue;
                            }
                            if(twiToRemove->childCount() != 0) {
                                continue;
                            }
                            if(!ptToRemove->isConnectionDetailsEqual(newTunnel)) {
                                continue;
                            }
                            ptToKeepConn = ptToRemove;
                            break;
                        }

                        if(ptToKeepConn != NULL) {
                            newTwi->setIcon(0, ptToKeepConn->twi->icon(0));
                            newTunnel->iLocalPort = ptToKeepConn->iLocalPort;
                            newTunnel->iConnectStatus = ptToKeepConn->iConnectStatus;
                            newTunnel->pProcess = ptToKeepConn->pProcess;
                            newTunnel->pConnector = ptToKeepConn->pConnector;
                            newTunnel->pConnector->m_pTreeTunnelsItem = newTwi;
                            newTunnel->log = ptToKeepConn->log;
                            ptToKeepConn->iConnectStatus = DISCONNECTED;
                            ptToKeepConn->pProcess = NULL;
                            ptToKeepConn->pConnector = NULL;
                        }

                    }//end - if keep conn

                } // end - for each Tunnel


                //Remove old
                for (int i = 0; i < toRemove.size(); i++) {
                    QTreeWidgetItem *child = toRemove[i];
                    recursiveDisconnectTunnelSignals(child);
                    recursiveDisconnectTunnelSilent(child);
                    recursiveDeleteTunnel(child);
                    delete child;
                }

                updateCurrentAndParentFoldersIcon(twi);

                bOk = true;

            }//end - if(json.contains("Hosts"))
            else {
                AddToLog( *pt, QString("> JSON Parser: %1\n").arg( "Hosts key not found" ) );
            }

        } //end - if(err.error == QJsonParseError::NoError)
        else {
            AddToLog( *pt, QString("> JSON Parser: %1\n").arg( err.errorString() ) );
        }

    } //end - if(bHasJsonOutput)


    if(pt->pPopulateChildNodesProcess != NULL) {
        ATVERIFY( disconnect( pt->pPopulateChildNodesProcess, 0, 0, 0 ) );
        pt->pPopulateChildNodesProcess->deleteLater();
        pt->pPopulateChildNodesProcess = NULL;
    }

    if(pt->pPopulateChildNodesConnector != NULL) {
        ATVERIFY( disconnect( pt->pPopulateChildNodesConnector, 0, 0, 0 ) );
        pt->pPopulateChildNodesConnector->deleteLater();
        pt->pPopulateChildNodesConnector = NULL;
    }

    slotUpdateTrayIcon();

    m_pMainWindow->m_pSearchWidget->slotIndex(); //update search box at top

    ui.treeTunnels->blockSignals(blocked);

    if(!bOk && !bCanceled) {
        QMessageBox::warning(this, "Oops!", "It went bad !");
    }
}

void ATSkeletonWindow::slotUpdateTrayIcon()
{
    int iConnectedCount = 0;

    QList<QTreeWidgetItem*> treeTunnelItems = ui.treeTunnels->findItems(".*", Qt::MatchFlags(Qt::MatchRegularExpression | Qt::MatchRecursive), 0);
    for(int i=0;i<treeTunnelItems.size();i++) {
        Tunnel_c* pt = getTunnel(treeTunnelItems[i]);
        if(pt != NULL) {
            if ( pt->iConnectStatus == CONNECTED ) iConnectedCount++;
        }
    }

    if ( iConnectedCount > 0 ) {
        emit signalSetTrayIcon( 1 );
    } else {
        emit signalSetTrayIcon( 0 );
    }
}

void ATSkeletonWindow::slotTunnelNameFgColorDialog()
{
    ui.editTunnelName->setFocus();
    QPalette palette = ui.editTunnelName->palette();
    QColor currentColor = palette.color(QPalette::Normal, QPalette::Text);
    QColor newColor = QColorDialog::getColor(currentColor, ui.btnTunnelNameMenu, "Set foreground color", QColorDialog::DontUseNativeDialog);
    if(newColor.isValid() && newColor != currentColor) {
        palette.setColor(QPalette::Text,newColor);
        ui.editTunnelName->setPalette(palette);
        slotHostModified();
    }
}

void ATSkeletonWindow::slotTunnelNameBgColorDialog()
{
    ui.editTunnelName->setFocus();
    QPalette palette = ui.editTunnelName->palette();
    QColor currentColor = palette.color(QPalette::Normal, QPalette::Base);
    QColor newColor = QColorDialog::getColor(currentColor, ui.btnTunnelNameMenu, "Set background color", QColorDialog::DontUseNativeDialog);
    if(newColor.isValid() && newColor != currentColor) {
        palette.setColor(QPalette::Base,newColor);
        ui.editTunnelName->setPalette(palette);
        slotHostModified();
    }
}

void ATSkeletonWindow::slotFolderNameFgColorDialog()
{
    ui.editFolderName->setFocus();
    QPalette palette = ui.editFolderName->palette();
    QColor currentColor = palette.color(QPalette::Normal, QPalette::Text);
    QColor newColor = QColorDialog::getColor(currentColor, ui.btnTunnelNameMenu, "Set foreground color", QColorDialog::DontUseNativeDialog);
    if(newColor.isValid() && newColor != currentColor) {
        palette.setColor(QPalette::Text,newColor);
        ui.editFolderName->setPalette(palette);
        slotHostModified();
    }
}

void ATSkeletonWindow::slotFolderNameBgColorDialog()
{
    ui.editFolderName->setFocus();
    QPalette palette = ui.editFolderName->palette();
    QColor currentColor = palette.color(QPalette::Normal, QPalette::Base);
    QColor newColor = QColorDialog::getColor(currentColor, ui.btnTunnelNameMenu, "Set background color", QColorDialog::DontUseNativeDialog);
    if(newColor.isValid() && newColor != currentColor) {
        palette.setColor(QPalette::Base,newColor);
        ui.editFolderName->setPalette(palette);
        slotHostModified();
    }
}

void ATSkeletonWindow::slotTunnelNameFgColorReset()
{
    ui.editTunnelName->setFocus();
    QPalette palette = ui.editTunnelName->palette();
    QBrush brush = QApplication::palette().brush(QPalette::Normal, QPalette::Text);
    brush.setStyle(Qt::NoBrush);
    palette.setBrush(QPalette::Text, brush);
    ui.editTunnelName->setPalette(palette);
    slotHostModified();
}

void ATSkeletonWindow::slotFolderNameFgColorReset()
{
    ui.editFolderName->setFocus();
    QPalette palette = ui.editFolderName->palette();
    QBrush brush = QApplication::palette().brush(QPalette::Normal, QPalette::Text);
    brush.setStyle(Qt::NoBrush);
    palette.setBrush(QPalette::Text, brush);
    ui.editFolderName->setPalette(palette);
    slotHostModified();
}

void ATSkeletonWindow::slotTunnelNameBgColorReset()
{
    ui.editTunnelName->setFocus();
    QPalette palette = ui.editTunnelName->palette();
    palette.setBrush(QPalette::Normal, QPalette::Base, QBrush());
    ui.editTunnelName->setPalette(palette);
    slotHostModified();
}

void ATSkeletonWindow::slotFolderNameBgColorReset()
{
    ui.editFolderName->setFocus();
    QPalette palette = ui.editFolderName->palette();
    palette.setBrush(QPalette::Normal, QPalette::Base, QBrush());
    ui.editFolderName->setPalette(palette);
    slotHostModified();
}

QList<int> ATSkeletonWindow::getLocalTunnelPortsInUse(Tunnel_c *pt)
{
	QList<int> ret;
	QMap<int,int> allPortsInUse = getAllLocalTunnelPortsInUse(pt); //do not count pt itself
	if(!pt->getSelectedRemoteHost().isEmpty() && pt->iLocalPort > 0 && pt->iRemotePort > 0) {
		if(allPortsInUse.contains(pt->iLocalPort)) {
			ret.append(pt->iLocalPort);
		}
	}
	for(int j=0; j<pt->portForwardList.size(); j++) {
		PortForwardStruct pfs = pt->portForwardList.at(j);
		if(!pfs.bEnabled) continue;
        if(pfs.nType == PortForwardStruct::LOCAL) {
			if(pfs.bEnabled && pfs.nLocalPort > 0 && !pfs.strDestinationHost.isEmpty() && pfs.nDestinationPort > 0) {
				if(allPortsInUse.contains(pfs.nLocalPort)) {
					ret.append(pfs.nLocalPort);
				}
			}
		}
		else if(pfs.nType == PortForwardStruct::DYNAMIC) {
			if(pfs.bEnabled && pfs.nLocalPort > 0) {
				if(allPortsInUse.contains(pfs.nLocalPort)) {
					ret.append(pfs.nLocalPort);
				}
			}
		}
	}
	return ret;
}

//Key: port number, Value: connect status (CONNECTED, CONNECTING)
QMap<int,int> ATSkeletonWindow::getAllLocalTunnelPortsInUse(Tunnel_c *ptIgnore)
{
	//Find local ports already in use
	QMap<int,int> portMap;
    QList<QTreeWidgetItem*> treeTunnelItems = ui.treeTunnels->findItems(".*", Qt::MatchFlags(Qt::MatchRegularExpression | Qt::MatchRecursive), 0);
	for(int i=0;i<treeTunnelItems.size();i++) {
		Tunnel_c* pt = getTunnel(treeTunnelItems[i]); 
		ATASSERT(pt);
		if(ptIgnore != NULL && pt->uUid == ptIgnore->uUid) {
			continue;
		}
		if(pt->iType == TUNNEL_TYPE_TUNNEL && pt->iConnectStatus != DISCONNECTED) {
			if(!portMap.contains(pt->iLocalPort)) {
				portMap.insert(pt->iLocalPort, pt->iConnectStatus);
			}
			for(int j=0; j<pt->portForwardList.size(); j++) {
				PortForwardStruct pfs = pt->portForwardList.at(j);
				if(pfs.bEnabled == true && !portMap.contains(pfs.nLocalPort)) {
					portMap.insert(pfs.nLocalPort, pt->iConnectStatus);
				}
			}
		}
	}
	return portMap;
}

void ATSkeletonWindow::recursiveDisconnectTunnelSignals( QTreeWidgetItem* twi )
{
	if(twi == NULL) return;

	Tunnel_c *pt = getTunnel(twi);
	if(pt != NULL) 
	{
		qDebug() << Q_FUNC_INFO << pt->strName;
		if(pt->pProcess != NULL) {
            ATVERIFY( disconnect( pt->pProcess, &QProcess::readyReadStandardOutput, nullptr, nullptr ) );
            ATVERIFY( disconnect( pt->pProcess, &QProcess::readyReadStandardError, nullptr, nullptr ) );
            ATVERIFY( disconnect( pt->pProcess, &QProcess::errorOccurred, nullptr, nullptr ) );
            ATVERIFY( disconnect( pt->pProcess, &QProcess::finished, nullptr, nullptr ) );
		}
		if(pt->pConnector != NULL) {
            ATVERIFY( disconnect( pt->pConnector, &ATTunnelConnector_c::finished, nullptr, nullptr ) );
            ATVERIFY( disconnect( pt->pConnector, &ATTunnelConnector_c::signalConnected, nullptr, nullptr ) );
            ATVERIFY( disconnect( pt->pConnector, &ATTunnelConnector_c::signalKillConnection, nullptr, nullptr ) );
        }
	}

	for (int i = 0; i < twi->childCount(); ++i) {
		QTreeWidgetItem *child = twi->child(i);
		recursiveDisconnectTunnelSignals(child);
	}
}

void ATSkeletonWindow::recursiveDeleteTunnel( QTreeWidgetItem* twi )
{
	if(twi == NULL) return;

	Tunnel_c *pt = getTunnel(twi);	
    if(pt != NULL) {
        qDebug() << "recursiveDeleteTunnel()" << pt->strName;
        delete pt;
    }

	for (int i = 0; i < twi->childCount(); ++i) {
		QTreeWidgetItem *child = twi->child(i);
		recursiveDeleteTunnel(child);
	}
}

void ATSkeletonWindow::recursiveDisconnectTunnel( QTreeWidgetItem* twi )
{
	if(twi == NULL) return;
	Tunnel_c *pt = getTunnel(twi);
	if(pt == NULL) return;
	
	qDebug() << "recursiveDisconnectTunnel()" << pt->strName;

    pt->iReconnectRetries = 0;
    pt->bShouldReconnect = false;
	disconnectTunnel(*pt);

	for (int i = 0; i < twi->childCount(); ++i) {
		QTreeWidgetItem *child = twi->child(i);
		recursiveDisconnectTunnel(child);
	}
}

void ATSkeletonWindow::recursiveDisconnectTunnelSilent( QTreeWidgetItem* twi )
{
    if(twi == NULL) return;

    Tunnel_c *pt = getTunnel(twi);
    if(pt != NULL) {
        qDebug() << "recursiveDisconnectTunnelSilent()" << pt->strName;
        pt->iReconnectRetries = 0;
        pt->bShouldReconnect = false;
    }

    disconnectTunnelSilent(twi);

    for (int i = 0; i < twi->childCount(); ++i) {
        QTreeWidgetItem *child = twi->child(i);
        recursiveDisconnectTunnelSilent(child);
    }
}

void ATSkeletonWindow::disconnectTunnel( Tunnel_c &tunnel )
{
	Tunnel_c *pt = &tunnel;

	qDebug() << "ATSkeletonWindow::disconnectTunnel() " << pt->strName;

	if(pt->iType == TUNNEL_TYPE_FOLDER) return;

	qDebug() << "setIcon 8";
	if ( pt->twi ) treeTunnelSetIcon(pt->twi, Images::icon("ht_120_host_disconnected"));
	pt->iConnectStatus = DISCONNECTED;

    if(pt->isActiveTimerDelayRetryConnect()) {
        AddToLog( *pt, QString("Aborted reconnect timer\n"));
    }
    pt->stopTimerDelayRetryConnect();

	updateControlsTunnel(pt);

	QTreeWidgetItem* twi = pt->twi;
	if( twi ) {
		updateParentFoldersIcon( twi );
	}

	if ( pt->pProcess == NULL ) return; 
	
	qDebug() << "disconnectTunnel()" << pt->strName;
    AddToLog( tunnel, QString("Disconnecting %1...\n").arg( pt->strName ) );
	//TODO qApp->processEvents();

#ifdef _WIN32
    pt->pProcess->kill();
#else
    pt->pProcess->terminate(); //use SIGTERM with sspass on MAC so it can kill it's child tty and take down the ssh process
#endif
	bool bOk = pt->pProcess->waitForFinished( WAIT_FOR_FINISHED_TIMEOUT );
    Q_UNUSED( bOk )
	//TODO delete pt->pProcess;
	//pt->pProcess = NULL;
	pt->pProcess->deleteLater();
	pt->pProcess = NULL;

	if(pt->pConnector != NULL) {
		//TODO delete pt->pConnector;
		//pt->pConnector = NULL;
		pt->pConnector->deleteLater();
		pt->pConnector = NULL;
	}

    AddToLog( tunnel, "Disconnected.\n" );

    slotUpdateTrayIcon();

    m_pMainWindow->m_pSearchWidget->slotUpdateCompleterIcons();
}

void ATSkeletonWindow::disconnectTunnelSilent( QTreeWidgetItem* twi )
{
	Tunnel_c *pt = getTunnel(twi);

    pt->stopTimerDelayRetryConnect();

	if ( pt->pProcess == NULL ) return; // not connected?

#ifdef _WIN32
    pt->pProcess->kill();
#else
    pt->pProcess->terminate(); //use SIGTERM with sspass on MAC so it can kill it's child tty and take down the ssh process
#endif
	bool bOk = pt->pProcess->waitForFinished( WAIT_FOR_FINISHED_TIMEOUT );
    Q_UNUSED( bOk )
	delete pt->pProcess;
	pt->pProcess = NULL;

	if(pt->pConnector != NULL) {
		delete pt->pConnector;
		pt->pConnector = NULL;
	}
}

void ATSkeletonWindow::disconnectAllTunnelsSilent()
{
	qDebug() << "disconnectAllTunnelsSilent()";

	// Make sure we won't try to reconnect
    ATVERIFY( disconnect( this, SIGNAL( signalAutoConnect(Tunnel_c*) ), 0, 0 ) );

    QList<QTreeWidgetItem*> treeTunnelItems = ui.treeTunnels->findItems(".*", Qt::MatchFlags(Qt::MatchRegularExpression | Qt::MatchRecursive), 0);
	for(int i=0;i<treeTunnelItems.size();i++)
	{
		QTreeWidgetItem *twi = treeTunnelItems[i];
		Tunnel_c *it = getTunnel(twi);

		// Make sure we don't process any stdout,stderr from ssh process
		if(it->pProcess != NULL) {
 			ATVERIFY( disconnect( it->pProcess, SIGNAL( readyReadStandardOutput() ), 0, 0 ) );
 			ATVERIFY( disconnect( it->pProcess, SIGNAL( readyReadStandardError() ), 0, 0 ) );
 			ATVERIFY( disconnect( it->pProcess, SIGNAL( error(QProcess::ProcessError) ), 0, 0 ) );
 			ATVERIFY( disconnect( it->pProcess, SIGNAL( finished(int, QProcess::ExitStatus) ), 0, 0 ) );
		}
		if(it->pConnector != NULL) {
 			ATVERIFY( disconnect( it->pConnector, SIGNAL( finished(Tunnel_c*) ), 0, 0 ) );
			ATVERIFY( disconnect( it->pConnector, SIGNAL( signalConnected(QTreeWidgetItem*) ), 0, 0 ) );
            ATVERIFY( disconnect( it->pConnector, SIGNAL( signalKillConnection(QTreeWidgetItem*) ), 0, 0 ) );
        }
		disconnectTunnelSilent( twi );
		ATASSERT( it->pProcess == NULL );
	}	
}


void ATSkeletonWindow::slotSave()
{
    qDebug("%s()",Q_FUNC_INFO);
	m_pMainWindow->setWindowModified(false);
	enableTreeTunnelsPaste(false);
    saveEditPane();
	ui.tabWidget->setCurrentIndex( PAGE_CONNECT );
    delayUpdateUIFromTWI();
}


void ATSkeletonWindow::slotCancel()
{
    qDebug("%s()",Q_FUNC_INFO);
	m_pMainWindow->setWindowModified(false);
	enableTreeTunnelsPaste(false);
	populateEditUIFromTwi( m_pTreeTunnelsItemEdit );
	ui.tabWidget->setCurrentIndex( PAGE_CONNECT );
	ui.treeCustomActionsEdit->clearPasteItems();
}

void ATSkeletonWindow::setTunnelDataFromEditPane(Tunnel_c *pt)
{
    //qDebug("%s() %s",Q_FUNC_INFO,pt->strName);

    if(pt->iType == TUNNEL_TYPE_TUNNEL) {

        pt->strName = ui.editTunnelName->text().trimmed();
        pt->setSSHHostList(editGetSSHHost());
        pt->setSelectedSSHHost(editGetSelectedSSHHost());
        pt->strLocalIP = ui.editLocalIP->text().trimmed();

        pt->iLocalPort = ui.editLocalPort->text().trimmed().toInt();
        pt->setRemoteHostList(editGetRemoteHost());
        pt->setSelectedRemoteHost(editGetSelectedRemoteHost());
        pt->iRemotePort = ui.editRemotePort->text().toInt();
        pt->strExtraArguments = ui.editExtraArguments->text();
        pt->strUsername = ui.editUsername->text();
        pt->strPassword = ui.editPassword->text();
        pt->strSSHKeyFile = ui.editSSHKeyFile->text();
        pt->strSSHKeyFilePassword = ui.editSSHKeyFilePassword->text();
        //TODO removed from UI
        //pt->iDirection = ui.comboDirection->currentIndex();
        pt->bAutoConnect = ui.checkAutoConnect->isChecked();
        pt->bCompression = ui.checkCompression->isChecked();
        //TODO removed from UI
        //pt->bDoKeepAlivePing = ui.checkDoKeepAlivePing->isChecked();
        pt->bAutoReconnect = ui.checkAutoReconnect->isChecked();
        pt->iSSH1or2 = ui.radioSSH1->isChecked() ? 1 : 2;

        pt->portForwardList.clear();
        pt->portForwardList.append(ui.widgetMoreTunnels->getData());

        pt->strFgColor = "";
        QBrush fgBrush = ui.editTunnelName->palette().brush(QPalette::Normal, QPalette::Text);
        if(fgBrush.style() != Qt::NoBrush) {
            if(fgBrush.color() != QApplication::palette().brush(QPalette::Normal, QPalette::Text).color()) {
                pt->strFgColor = fgBrush.color().name();
            }
        }

        pt->strBgColor = "";
        QBrush bgBrush = ui.editTunnelName->palette().brush(QPalette::Normal, QPalette::Base);
        if(bgBrush.style() != Qt::NoBrush) {
            pt->strBgColor = bgBrush.color().name();
        }

    } else {

        pt->strName = ui.editFolderName->text().trimmed();
        pt->strChildNodesCommand = ui.editFolderChildNodesCommand->text().trimmed();

        pt->strFgColor = "";
        QBrush fgBrush = ui.editFolderName->palette().brush(QPalette::Normal, QPalette::Text);
        if(fgBrush.style() != Qt::NoBrush) {
            if(fgBrush.color() != QApplication::palette().brush(QPalette::Normal, QPalette::Text).color()) {
                pt->strFgColor = fgBrush.color().name();
            }
        }

        pt->strBgColor = "";
        QBrush bgBrush = ui.editFolderName->palette().brush(QPalette::Normal, QPalette::Base);
        if(bgBrush.style() != Qt::NoBrush) {
            pt->strBgColor = bgBrush.color().name();
        }

        pt->bActivateDisconnects = ui.checkFolderActivateDisconnectsAllChildren->isChecked();
    }

    pt->customActionList.clear();
    pt->customActionList.append(ui.widgetCustomActions->getData());
}

void ATSkeletonWindow::saveEditPane()
{
    qDebug("%s()",Q_FUNC_INFO);
    ATASSERT(m_pTreeTunnelsItemEdit);
	if(m_pTreeTunnelsItemEdit == NULL) {
		return;
	}

 	Tunnel_c *pt = getTunnel(m_pTreeTunnelsItemEdit);
	ATASSERT(pt);
    if(pt == NULL) return;
    ATASSERT(pt->iType == TUNNEL_TYPE_TUNNEL || pt->iType == TUNNEL_TYPE_FOLDER);
    if(pt->iType != TUNNEL_TYPE_TUNNEL && pt->iType != TUNNEL_TYPE_FOLDER) return;

    if(pt->iType == TUNNEL_TYPE_TUNNEL) {
        autoBackupSettings( BackupInfo(BACKUP_BEFORE_EDITTUNNEL,"Before saving edited tunnel [%1]",m_pTreeTunnelsItemEdit->text(0) ) );   
    } else {
        autoBackupSettings( BackupInfo(BACKUP_BEFORE_EDITFOLDER,"Before saving edited folder [%1]",m_pTreeTunnelsItemEdit->text(0) ) );
    }

 	setTunnelDataFromEditPane(pt);

	bool blocked = ui.treeTunnels->blockSignals(true);

    //+++ fg/bg color

    QLineEdit *editName = NULL;
    if(pt->iType == TUNNEL_TYPE_TUNNEL) {
        editName = ui.editTunnelName;
    } else if(pt->iType == TUNNEL_TYPE_FOLDER) {
        editName = ui.editFolderName;
    }
    if(editName != NULL) {
        QBrush fgBrush = editName->palette().brush(QPalette::Normal, QPalette::Text);
        m_pTreeTunnelsItemEdit->setForeground(0,fgBrush);
        QBrush bgBrush = editName->palette().brush(QPalette::Normal, QPalette::Base);
        m_pTreeTunnelsItemEdit->setBackground(0,bgBrush);
    }

    //--- fg/bg color

    for(int i=0;i<m_pTreeTunnelsItemEdit->childCount();i++) {
		QTreeWidgetItem *child = m_pTreeTunnelsItemEdit->child(i);
		treeTunnelUpdateFromParent(child);
	}

	m_pTreeTunnelsItemEdit->setText( 0, pt->strName );

    ui.treeTunnels->blockSignals(blocked);

	writeSettings();
}

void ATSkeletonWindow::slotComboPasswordSelectSelectionChanged(int index)
{
    qDebug( "%s", Q_FUNC_INFO );
	if(index > 0) {
		ui.editUsername->setText( "${" + ui.comboPasswordSelect->itemText(index) + ".uid}" );
		ui.editPassword->setText( "${" + ui.comboPasswordSelect->itemText(index) + ".pwd}" );
	}
}

void ATSkeletonWindow::slotComboKeyPasswordSelectSelectionChanged(int index)
{
    qDebug( "%s", Q_FUNC_INFO );
	if(index > 0) {
		ui.editSSHKeyFilePassword->setText( "${" + ui.comboKeyPasswordSelect->itemText(index) + ".pwd}" );
	}
}

void ATSkeletonWindow::slotEditSSHHost()
{
	m_pMainWindow->setDetectSaveTunnel(false);
	SSHHostDialog dlg(this, editGetSSHHost());
	int dlgStatus = dlg.exec();
	if(dlgStatus == QDialog::Accepted) {	
		editSetSSHHost(dlg.sshHost);
	}	
	if(ui.editSSHHost->isVisible()) ui.editSSHHost->setFocus(Qt::OtherFocusReason);
	if(ui.comboSSHHost->isVisible()) ui.comboSSHHost->setFocus(Qt::OtherFocusReason);
	m_pMainWindow->setDetectSaveTunnel(true);
}

void ATSkeletonWindow::editSetSSHHost(const QString& host)
{
	QString selectedHost = ui.comboSSHHost->currentText();
	ui.comboSSHHost->clear();
    QStringList strListSSHHost = host.split( '\n', Qt::SkipEmptyParts );
	QStringList strListSSHHostFiltered;

	for(int i=0; i<strListSSHHost.size(); i++) {
		QString theHost = strListSSHHost.at(i).trimmed();
		if(!theHost.isEmpty()) {
			strListSSHHostFiltered << theHost;
			ui.comboSSHHost->addItem(theHost);
		}
	}
	editSetSelectedSSHHost(selectedHost);

	ui.editSSHHost->setText(strListSSHHostFiltered.join("\n"));

    if(strListSSHHost.size() > 1) {
		ui.stackedWidgetSSHHost->setCurrentIndex(1); //combo box
    } else {
        ui.stackedWidgetSSHHost->setCurrentIndex(0); //text box
    }

}

void ATSkeletonWindow::editSetSelectedSSHHost(const QString& host)
{
	qDebug() << "editSetSelectedSSHHost(): " << host;
	int index = ui.comboSSHHost->findText(host);
	if(index != -1) {
		ui.comboSSHHost->setCurrentIndex(index);
	}
}


QString ATSkeletonWindow::editGetSSHHost()
{
	return ui.editSSHHost->text().trimmed();
}

QString ATSkeletonWindow::editGetSelectedSSHHost()
{
	QString host = "";
	if(ui.stackedWidgetSSHHost->currentIndex() == 1) {
		//combo box
		host = ui.comboSSHHost->currentText();
	} else {
		//text box
	}
	return host;
}



void ATSkeletonWindow::slotEditRemoteHost()
{
	m_pMainWindow->setDetectSaveTunnel(false);
	SSHHostDialog dlg(this, editGetRemoteHost());
	int dlgStatus = dlg.exec();
	if(dlgStatus == QDialog::Accepted) {	
		editSetRemoteHost(dlg.sshHost);
	}	
	if(ui.editRemoteHost->isVisible()) ui.editRemoteHost->setFocus(Qt::OtherFocusReason);
	if(ui.comboRemoteHost->isVisible()) ui.comboRemoteHost->setFocus(Qt::OtherFocusReason);
	m_pMainWindow->setDetectSaveTunnel(true);
}

void ATSkeletonWindow::editSetRemoteHost(const QString& host)
{
	QString selectedHost = ui.comboRemoteHost->currentText();
	ui.comboRemoteHost->clear();
    QStringList strListRemoteHost = host.split( '\n', Qt::SkipEmptyParts );
	QStringList strListRemoteHostFiltered;

	for(int i=0; i<strListRemoteHost.size(); i++) {
		QString theHost = strListRemoteHost.at(i).trimmed();
		if(!theHost.isEmpty()) {
			strListRemoteHostFiltered << theHost;
			ui.comboRemoteHost->addItem(theHost);
		}
	}
	editSetSelectedRemoteHost(selectedHost);

	ui.editRemoteHost->setText(strListRemoteHostFiltered.join("\n"));

	if(strListRemoteHost.size() > 1) {
		ui.stackedWidgetRemoteHost->setCurrentIndex(1); //combo box
	} else {
		ui.stackedWidgetRemoteHost->setCurrentIndex(0); //text box
	}
}

void ATSkeletonWindow::editSetSelectedRemoteHost(const QString& host)
{
	qDebug() << "editSetSelectedRemoteHost(): " << host;
	int index = ui.comboRemoteHost->findText(host);
	if(index != -1) {
		ui.comboRemoteHost->setCurrentIndex(index);
	}
}


QString ATSkeletonWindow::editGetRemoteHost()
{
	return ui.editRemoteHost->text();
}

QString ATSkeletonWindow::editGetSelectedRemoteHost()
{
	QString host = "";
	if(ui.stackedWidgetRemoteHost->currentIndex() == 1) {
		//combo box
		host = ui.comboRemoteHost->currentText();
	} else {
		//text box
	}
	return host;
}

void ATSkeletonWindow::enableTreeTunnelsPaste(bool enable)
{
	m_pTreeTunnelsActionPasteNodeChild->setEnabled(enable);
	m_pTreeTunnelsActionPasteNodeAfter->setEnabled(enable);
	if(!enable) {
		if(m_pTreeTunnelsPaste != NULL) {
			bool blocked = ui.treeTunnels->blockSignals(true);
			TreeWidget::setItemForegroundCut(m_pTreeTunnelsPaste,false);
			ui.treeTunnels->blockSignals(blocked);
		}
		m_pTreeTunnelsPaste = NULL;
		m_pTreeTunnelsPasteMode = 0;
	}
}

void ATSkeletonWindow::enableTreeTunnelDragDrop(bool enable) 
{
	if(enable) {
		ui.treeTunnels->setDragEnabled(true);
		ui.treeTunnels->setAcceptDrops(true);
		ui.treeTunnels->setDragDropMode(QAbstractItemView::InternalMove);
		ui.treeTunnels->setDefaultDropAction(Qt::MoveAction);
	} else {
		ui.treeTunnels->setDragEnabled(false);
		ui.treeTunnels->setAcceptDrops(false);
		ui.treeTunnels->setDragDropMode(QAbstractItemView::NoDragDrop);
		ui.treeTunnels->setDefaultDropAction(Qt::IgnoreAction);
	}
}

void ATSkeletonWindow::slotTreeTunnelExportAll()
{
	QList<QTreeWidgetItem*> twiExportList;
	for(int i=0;i<ui.treeTunnels->topLevelItemCount();i++) {
		twiExportList.append(ui.treeTunnels->topLevelItem(i));
	}

	m_pExportHostsDialog->show(twiExportList);
}

void ATSkeletonWindow::slotTreeTunnelExportCurrent()
{
	enableTreeTunnelsPaste(false);

	QTreeWidgetItem *twiCurrent = ui.treeTunnels->currentItem();
	ATASSERT( twiCurrent );
	if ( twiCurrent == NULL ) return;

	Tunnel_c *ptCurrent = getTunnel(twiCurrent);
	ATASSERT( ptCurrent );
	if ( ptCurrent == NULL ) return;

	QList<QTreeWidgetItem*> twiExportList;
	twiExportList.append(twiCurrent);

	m_pExportHostsDialog->show(twiExportList);
}

void ATSkeletonWindow::slotTreeTunnelImportAfter()
{
	QTreeWidgetItem *twiCurrent = ui.treeTunnels->currentItem();
	ATASSERT( twiCurrent );
	if ( twiCurrent == NULL ) return;

	importSettingsTunnel(twiCurrent, IMPORT_AFTER);
}

void ATSkeletonWindow::slotTreeTunnelImportChild()
{
	QTreeWidgetItem *twiCurrent = ui.treeTunnels->currentItem();
	ATASSERT( twiCurrent );
	if ( twiCurrent == NULL ) return;

	importSettingsTunnel(twiCurrent, IMPORT_CHILD);
}

/*
void ATSkeletonWindow::slotTreeTunnelEdit()
{
	enableTreeTunnelsPaste(false);
	
	QTreeWidgetItem *twi = ui.treeTunnels->currentItem();
	ATASSERT( twi );
	if ( twi == NULL ) return;

	Tunnel_c *pt = getTunnel(twi);
	ATASSERT( pt );
	if ( pt == NULL ) return;

	if(pt->iType == TUNNEL_TYPE_FOLDER) {
		if(!m_bEditingEnabled) return;
		bool ok;
		QString text = QInputDialog::getText(this, tr("Edit folder name"),
			tr("Folder name:"), QLineEdit::Normal,
			pt->strName, &ok);
		if (ok) {
			autoBackupSettings( BackupInfo(BACKUP_BEFORE_EDITFOLDER,"Before changing folder name from [%1] to [%2]",pt->strName,text ) );
			pt->strName = text;
			twi->setText(0, text);
			writeSettings();
		}
	} else {
		ui.tabWidget->setCurrentIndex( PAGE_EDIT );
	}
}*/

void ATSkeletonWindow::slotTreeTunnelRename()
{
	enableTreeTunnelsPaste(false);
	QTreeWidgetItem *twi = ui.treeTunnels->currentItem();
	if(twi != NULL) {
		ui.treeTunnels->editItem(twi,0);
	}
}

void ATSkeletonWindow::slotTreeTunnelItemChanged(QTreeWidgetItem* twi,int col)
{
    qDebug("%s() %s",Q_FUNC_INFO,qPrintable(twi->text(0)));
    Q_UNUSED(col)
	enableTreeTunnelsPaste(false);
	Tunnel_c* pt = getTunnel(twi);
	ATASSERT(pt);
	if(pt == NULL) return;
	if(twi->text(0) != pt->strName) {
		QString oldName = pt->strName;
		QString newName = twi->text(0).trimmed();
		pt->strName = newName;
		bool blocked = ui.editTunnelName->blockSignals(true);
		ui.editTunnelName->setText(newName);
		ui.editTunnelName->blockSignals(blocked);
		if(newName != oldName) {
			if(pt->iType == TUNNEL_TYPE_FOLDER) {
				autoBackupSettings( BackupInfo( BACKUP_BEFORE_RENAMEFOLDER,"Before rename folder from [%1] to [%2]",oldName, newName ) );
			} else {
				autoBackupSettings( BackupInfo( BACKUP_BEFORE_RENAMETUNNEL,"Before rename host from [%1] to [%2]",oldName, newName ) );
			}
			delayWriteSettings();
		}
	}
}

void ATSkeletonWindow::slotTreeTunnelDuplicate()
{
	if(!m_bEditingEnabled) return;

	enableTreeTunnelsPaste(false);

	QTreeWidgetItem *twi = ui.treeTunnels->currentItem();
	ATASSERT( twi );
	if ( twi == NULL ) return;

	autoBackupSettings( BackupInfo(BACKUP_BEFORE_DUPLICATE,"Before duplicating [%1]",twi->text(0) ) );

	QTreeWidgetItem* newTwi = treeTunnelPaste(twi, twi, PASTE_MODE_COPY, INSERT_MODE_AFTER);

	ui.treeTunnels->setCurrentItem( newTwi );
	//ui.tabWidget->setCurrentIndex( PAGE_EDIT );
	//ui.editTunnelName->setFocus();
 	//qApp->processEvents();
 	//ui.editTunnelName->selectAll();
	writeSettings();
}

void ATSkeletonWindow::slotTreeTunnelInsertChildFolder()
{
	if(!m_bEditingEnabled) return;

	enableTreeTunnelsPaste(false);

	QTreeWidgetItem *twi = ui.treeTunnels->currentItem();
	ATASSERT( twi );
	if ( twi == NULL ) return;

	autoBackupSettings( BackupInfo(BACKUP_BEFORE_INSERTCHILDFOLDER,"Before inserting child folder at [%1]",twi->text(0) ) );

	treeTunnelCreateNewFolder(twi, INSERT_MODE_CHILD);
	writeSettings();
}

void ATSkeletonWindow::slotTreeTunnelInsertFolderAfter()
{
	if(!m_bEditingEnabled) return;

	enableTreeTunnelsPaste(false);

	QTreeWidgetItem *twi = ui.treeTunnels->currentItem();
	ATASSERT( twi );
	if ( twi == NULL ) return;

	autoBackupSettings( BackupInfo(BACKUP_BEFORE_INSERTFOLDERAFTER,"Before inserting folder after [%1]",twi->text(0) ) );

	treeTunnelCreateNewFolder(twi, INSERT_MODE_AFTER);
	writeSettings();
}

void ATSkeletonWindow::slotTreeTunnelInsertFolderBefore()
{
	if(!m_bEditingEnabled) return;

	enableTreeTunnelsPaste(false);

	QTreeWidgetItem *twi = ui.treeTunnels->currentItem();
	ATASSERT( twi );
	if ( twi == NULL ) return;

	autoBackupSettings( BackupInfo(BACKUP_BEFORE_INSERTFOLDERBEFORE,"Before inserting folder before [%1]",twi->text(0) ) );

	treeTunnelCreateNewFolder(twi, INSERT_MODE_BEFORE);
	writeSettings();
}

void ATSkeletonWindow::slotTreeTunnelInsertChild()
{
	if(!m_bEditingEnabled) return;

	enableTreeTunnelsPaste(false);

	QTreeWidgetItem *twi = ui.treeTunnels->currentItem();
	ATASSERT( twi );
	if ( twi == NULL ) return;

	autoBackupSettings( BackupInfo(BACKUP_BEFORE_INSERTCHILDTUNNEL,"Before inserting tunnel child at [%1]",twi->text(0) ) );

	treeTunnelCreateNew(twi, INSERT_MODE_CHILD);
	writeSettings();
}

void ATSkeletonWindow::slotTreeTunnelInsertNodeAfter()
{
	if(!m_bEditingEnabled) return;

	enableTreeTunnelsPaste(false);

	QTreeWidgetItem *twi = ui.treeTunnels->currentItem();
	ATASSERT( twi );
	if ( twi == NULL ) return;

	autoBackupSettings( BackupInfo(BACKUP_BEFORE_INSERTTUNNELAFTER,"Before inserting tunnel after [%1]",twi->text(0) ) );

	treeTunnelCreateNew(twi, INSERT_MODE_AFTER);
	writeSettings();
}

void ATSkeletonWindow::slotTreeTunnelInsertNodeBefore()
{
	if(!m_bEditingEnabled) return;

	enableTreeTunnelsPaste(false);

	QTreeWidgetItem *twi = ui.treeTunnels->currentItem();
	ATASSERT( twi );
	if ( twi == NULL ) return;

	autoBackupSettings( BackupInfo(BACKUP_BEFORE_INSERTTUNNELBEFORE,"Before inserting tunnel before [%1]",twi->text(0) ) );

	treeTunnelCreateNew(twi, INSERT_MODE_BEFORE);
	writeSettings();
}

void ATSkeletonWindow::slotTreeTunnelCutNode()
{
	if(!m_bEditingEnabled) return;

	enableTreeTunnelsPaste(false);

	QTreeWidgetItem *twi = ui.treeTunnels->currentItem();
	ATASSERT( twi );
	if ( twi == NULL ) return;

	enableTreeTunnelsPaste(true);

	m_pTreeTunnelsPaste = twi;
	bool blocked = ui.treeTunnels->blockSignals(true);
	TreeWidget::setItemForegroundCut(m_pTreeTunnelsPaste,true);
	ui.treeTunnels->blockSignals(blocked);
	m_pTreeTunnelsPasteMode = PASTE_MODE_CUT;
}

void ATSkeletonWindow::slotTreeTunnelCopyNode()
{
	if(!m_bEditingEnabled) return;

	enableTreeTunnelsPaste(false);

	QTreeWidgetItem *twi = ui.treeTunnels->currentItem();
	ATASSERT( twi );
	if ( twi == NULL ) return;

	enableTreeTunnelsPaste(true);

	m_pTreeTunnelsPaste = twi;
	m_pTreeTunnelsPasteMode = PASTE_MODE_COPY;
}

void ATSkeletonWindow::slotTreeTunnelPasteNodeChild()
{
	if(!m_bEditingEnabled) return;

	QTreeWidgetItem *twi = ui.treeTunnels->currentItem();
	ATASSERT( twi );
	if ( twi == NULL ) return;

	autoBackupSettings( BackupInfo(BACKUP_BEFORE_PASTECHILD,"Before pasting node child at [%1]",twi->text(0) ) );
		
	QTreeWidgetItem* twiInserted = treeTunnelPaste(m_pTreeTunnelsPaste, twi, m_pTreeTunnelsPasteMode, INSERT_MODE_CHILD);
	if(twiInserted != NULL) {
		ui.treeTunnels->setCurrentItem( twiInserted );
	}
	qApp->processEvents();
	enableTreeTunnelsPaste(false);
	writeSettings();
}

void ATSkeletonWindow::slotTreeTunnelPasteNodeAfter()
{
	if(!m_bEditingEnabled) return;

	if(m_pTreeTunnelsPaste != NULL)
	{
		QTreeWidgetItem *twi = ui.treeTunnels->currentItem();
		ATASSERT( twi );
		if ( twi == NULL ) return;

		autoBackupSettings( BackupInfo(BACKUP_BEFORE_PASTEAFTER,"Before pasting node after [%1]",twi->text(0) ) );
			
		QTreeWidgetItem* twiInserted = treeTunnelPaste(m_pTreeTunnelsPaste, twi, m_pTreeTunnelsPasteMode, INSERT_MODE_AFTER);
		if(twiInserted != NULL) {
			ui.treeTunnels->setCurrentItem( twiInserted );
		}
		qApp->processEvents();

		writeSettings();

		if(m_pTreeTunnelsPasteMode != PASTE_MODE_COPY) {
			enableTreeTunnelsPaste(false);
		}
	}
}

void ATSkeletonWindow::slotTreeTunnelDeleteNode()
{
	if(!m_bEditingEnabled) return;

	enableTreeTunnelsPaste(false);

	QTreeWidgetItem *twi = ui.treeTunnels->currentItem();
	ATASSERT( twi );
 	if ( twi == NULL ) return;

	Tunnel_c *pt = getTunnel(twi);
	ATASSERT( pt );
	if ( pt == NULL ) return;

 	QString strQuestion = QString( "Do you want to delete \"%1\"?" ).arg( twi->text( 0 ) );

 	QMessageBox::StandardButton iRet = QMessageBox::question( this, APP_NICE_NAME, strQuestion, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes );
 
 	if ( iRet == QMessageBox::Yes )
 	{
		backupSettings( BackupInfo(BACKUP_BEFORE_DELETE,"Before deleting node [%1]",twi->text(0) ) );

		recursiveDisconnectTunnelSignals(twi);
		recursiveDisconnectTunnel(twi);
		recursiveDeleteTunnel(twi);
		delete twi;

		if(ui.treeTunnels->topLevelItemCount() == 0) {
			treeTunnelCreateNew(NULL, 0);
		}

		writeSettings();
 	}
}

void ATSkeletonWindow::slotTreeTunnelClearNodeLog()
{
	QTreeWidgetItem *twi = ui.treeTunnels->currentItem();
	ATASSERT( twi );
 	clearNodeLog( twi );
}

void ATSkeletonWindow::clearNodeLog(QTreeWidgetItem *twi)
{
	if ( twi == NULL ) return;

	Tunnel_c *pt = getTunnel(twi);
	clearTunnelLog(pt);
}

void ATSkeletonWindow::clearTunnelLog(Tunnel_c *pt)
{
	if(pt == NULL) return;
    pt->log.clear();
    ui.textBrowser->setHtml( pt->log.toHtml() );
}


QTreeWidgetItem* ATSkeletonWindow::treeTunnelPaste(QTreeWidgetItem *twiPasteFrom, QTreeWidgetItem *twiPasteTo, int pasteMode, int insertMode)
{
	if(twiPasteFrom == NULL)
		return NULL;
	if(twiPasteTo == NULL) 
		return NULL;

	qDebug() << "treeTunnelPaste()";
	qDebug() << "  twiPasteFrom: " << twiPasteFrom->text(0);
	qDebug() << "    twiPasteTo: " << twiPasteTo->text(0);
	qDebug() << "     pasteMode: " << pasteMode;
	qDebug() << "    insertMode: " << insertMode;

	if(pasteMode != PASTE_MODE_COPY && pasteMode != PASTE_MODE_CUT) return NULL;
	if(insertMode != INSERT_MODE_AFTER && insertMode != INSERT_MODE_BEFORE && insertMode != INSERT_MODE_CHILD) return NULL;

	QTreeWidgetItem* twiInserted = NULL;

	bool blocked = ui.treeTunnels->blockSignals(true);

	if(pasteMode == PASTE_MODE_CUT) {
		QTreeWidgetItem *twiCutParent = twiPasteFrom->parent();
		if(twiCutParent != NULL) {
			int index = twiCutParent->indexOfChild(m_pTreeTunnelsPaste);
			twiInserted = twiCutParent->takeChild(index);
		} else {
			int index = ui.treeTunnels->indexOfTopLevelItem(m_pTreeTunnelsPaste);
			twiInserted = ui.treeTunnels->takeTopLevelItem(index);
		}
		treeTunnelInsert(twiPasteTo, twiInserted, insertMode);
	}
	else if(pasteMode == PASTE_MODE_COPY) {
		twiInserted = twiPasteFrom->clone(); 
		recursiveReplaceTunnelWithCopy(twiInserted);
		treeTunnelInsert(twiPasteTo, twiInserted, insertMode);
	}

	if(pasteMode == PASTE_MODE_COPY) {
		if(treeTunnelIsDuplicateByName(twiInserted)) {
			twiInserted->setText(0, twiInserted->text(0) + " Copy");
			getTunnel(twiInserted)->strName = twiInserted->text(0);
		}
		setNewLocalPort(twiInserted, true); //replace tunnel local ports with new unique ports
	}

	ui.treeTunnels->blockSignals(blocked);

	return twiInserted;
}

void ATSkeletonWindow::recursiveReplaceTunnelWithCopy( QTreeWidgetItem* twi )
{
	if(twi == NULL) return;
	Tunnel_c *pt = getTunnel(twi);
	if(pt == NULL) return;
	qDebug() << "dublicateTunnels()" << pt->strName;

	if(pt->iType == TUNNEL_TYPE_TUNNEL) {
		qDebug() << "setIcon 9";
		treeTunnelSetIcon(twi, Images::icon("ht_120_host_disconnected"));
	} else if(pt->iType == TUNNEL_TYPE_FOLDER) {
		qDebug() << "setIcon 10";
		if(twi->isExpanded()) {
			treeTunnelSetIcon(twi, Images::icon("ht_150_folder_open_disconnected"));
		} else {
			treeTunnelSetIcon(twi, Images::icon("ht_170_folder_closed_disconnected"));
		}
	}
	updateControlsTunnel(pt);

	Tunnel_c *ptCopy = new Tunnel_c();
	ptCopy->copyFrom(pt);
	setTunnel(twi, ptCopy);

	for (int i = 0; i < twi->childCount(); ++i) {
		QTreeWidgetItem *child = twi->child(i);
		recursiveReplaceTunnelWithCopy(child);
	}
}

QTreeWidgetItem* ATSkeletonWindow::treeTunnelCreateNew()
{
	return treeTunnelCreateNew(NULL,0);
}

QTreeWidgetItem* ATSkeletonWindow::treeTunnelCreateNew(QTreeWidgetItem *twiInsertAt, int insertMode)
{
	bool blocked = ui.treeTunnels->blockSignals(true);

	Tunnel_c *tunnel = new Tunnel_c();
	tunnel->iType = TUNNEL_TYPE_TUNNEL;
	tunnel->strName = "Untitled";

	tunnel->customActionList.append(CustomActionStruct(QUuid(),CUSTOM_BUTTON_1_LABEL,CUSTOM_BUTTON_1_CMD));
	tunnel->customActionList.append(CustomActionStruct(QUuid(),CUSTOM_BUTTON_2_LABEL,CUSTOM_BUTTON_2_CMD));

	QTreeWidgetItem *twiInserted = new QTreeWidgetItem( );
	twiInserted->setFlags(twiInserted->flags() | Qt::ItemIsEditable);
	twiInserted->setText( 0, tunnel->strName );
	qDebug() << "setIcon 11";
	treeTunnelSetIcon(twiInserted, Images::icon("ht_120_host_disconnected"));
	setTunnel(twiInserted, tunnel);

	ui.treeTunnels->blockSignals(blocked);

	treeTunnelInsert(twiInsertAt, twiInserted, insertMode);
	ui.treeTunnels->setCurrentItem( twiInserted );

	m_pMainWindow->setDetectSaveTunnel(false);

	//propose tunnel local port
	setNewLocalPort(twiInserted, false);
	//propose tunnel remove port
	tunnel->iRemotePort = 22;
	ui.editRemotePort->setText( QString("%1").arg(tunnel->iRemotePort) );

	m_pMainWindow->setWindowModified(false);
	m_pMainWindow->setDetectSaveTunnel(true);

	//ui.tabWidget->setCurrentIndex( PAGE_EDIT );
	//ui.editTunnelName->setFocus();
	//ui.editTunnelName->selectAll();

	return twiInserted;
}

void ATSkeletonWindow::setNewLocalPort(QTreeWidgetItem *twi, bool alsoUpdateChildren)
{
    QList<int> empty;
    setNewLocalPort(twi, alsoUpdateChildren, empty);
}

void ATSkeletonWindow::setNewLocalPort(QTreeWidgetItem *twi, bool alsoUpdateChildren, QList<int> &excludePorts)
{
	ATASSERT(twi);
	Tunnel_c *pt = getTunnel(twi);

	ATASSERT(pt);
	if(pt == NULL) return;

	int tunnelLocalPort = 0;
	
	if(pt->iType == TUNNEL_TYPE_TUNNEL) {
        tunnelLocalPort = proposeNewLocalPort(twi, excludePorts);
	}

	if(tunnelLocalPort != 0) {
		pt->iLocalPort = tunnelLocalPort;
		if(twi == ui.treeTunnels->currentItem()) {
			bool blocked = ui.editLocalPort->blockSignals(true);
			ui.editLocalPort->setText( QString("%1").arg(pt->iLocalPort) );
			ui.editLocalPort->blockSignals(blocked);
		}
	}

	if(alsoUpdateChildren) {
		for(int i=0;i<twi->childCount();i++) {
			QTreeWidgetItem *twiChild = twi->child(i);
            setNewLocalPort(twiChild, true, excludePorts);
		}
    }
}

int ATSkeletonWindow::proposeNewLocalPort(QTreeWidgetItem *twi)
{
    QList<int> empty;
    return proposeNewLocalPort(twi,empty);
}

int ATSkeletonWindow::proposeNewLocalPort(QTreeWidgetItem *twi, QList<int> &excludePorts)
{
	//The well-known ports cover the range of possible port numbers from 0 through 1023. 
	//The registered ports are numbered from 1024 through 49151. 
	//The remaining ports, referred to as dynamic ports or private ports, are numbered from 49152 through 65535.

	//Find current ports
	int hostCount = 0;
	int maxPort = 50001; //figure out if we have any higher
	QMap<int,bool> portMap;

    for(int i=0;i<excludePorts.size();i++) {
        portMap.insert(excludePorts[i],true);
    }

    QList<QTreeWidgetItem*> treeTunnelItems = ui.treeTunnels->findItems(".*", Qt::MatchFlags(Qt::MatchRegularExpression | Qt::MatchRecursive), 0);
	for(int i=0;i<treeTunnelItems.size();i++) {
		Tunnel_c* pt = getTunnel(treeTunnelItems[i]);
        if(pt != NULL) {
            //save local port
            portMap.insert(pt->iLocalPort, true);
            //currently editing host local port (might not be saved yet)
            QString strEditLocalPort = ui.editLocalPort->text().trimmed();
            if(strEditLocalPort.isEmpty()) strEditLocalPort = "0";
            bool bEditLocalPortOk;
            int nEditLocalPort = strEditLocalPort.toInt(&bEditLocalPortOk);
            if(bEditLocalPortOk) portMap.insert(nEditLocalPort, true);
            //saved portforward list
            for(int j=0;j<pt->portForwardList.size();j++) {
                PortForwardStruct pfs = pt->portForwardList.at(j);
                portMap.insert(pfs.nLocalPort, true);
            }
            //currently editing host portforward list (might not be saved yet)
            for(int j=0;j<ui.tableMoreTunnels->rowCount();j++) {
                QTableWidgetItem *itemLocalPort = ui.tableMoreTunnels->item(j, MoreTunnelsEditWidget::COL_LOCALPORT);
                QString strLocalPort = itemLocalPort->text().trimmed();
                if(strLocalPort.isEmpty()) strLocalPort = "0";
                bool bLocalPortOk;
                int nLocalPort = strLocalPort.toInt(&bLocalPortOk);
                if(bLocalPortOk) portMap.insert(nLocalPort, true);
            }
            if(pt->iType == TUNNEL_TYPE_TUNNEL) {
                hostCount = hostCount + 1;
            }
            if(pt->iType == TUNNEL_TYPE_TUNNEL) {
                if(pt->iRemotePort == 22 && pt->iLocalPort > maxPort) {
                    maxPort = pt->iLocalPort;
                }
            }
        }
	}

	int proposePort = 0;

	//Just created a new tree ?
	if(hostCount <= 1) {
		proposePort = 50001;
	}

	//Find max port of other tunnels at same level
	if(proposePort == 0) {
		QTreeWidgetItem *twiParentNode = twi->parent();
		int maxChildPort = 0;
		if(twiParentNode != NULL) {
			for(int i=0;i<twiParentNode->childCount();i++) {
                QTreeWidgetItem *twiChild = twiParentNode->child(i);
                Tunnel_c* pt = getTunnel(twiChild);
                if(pt != NULL) {
                    if(pt->iType == TUNNEL_TYPE_TUNNEL) {
                        if(pt->iRemotePort == 22 && pt->iLocalPort > maxChildPort) {
                            maxChildPort = pt->iLocalPort;
                        }
                    }
                }
			}
		}
		if(maxChildPort > 0) {
			proposePort = maxChildPort + 1;
		}
	}

	if(proposePort == 0) {
		//Find local port of parent tunnel
		QTreeWidgetItem *twiParent = findParentTunnelNode(twi);
		if(twiParent != NULL) {
			Tunnel_c* pt = getTunnel(twiParent);
            if(pt != NULL) {
                if(pt->iType == TUNNEL_TYPE_TUNNEL) {
                    if(pt->iLocalPort > 0) {
                        proposePort = pt->iLocalPort + 1;
                    }
                }
            }
		}
	}

	if(proposePort == 0) {
		proposePort = maxPort + 1;
	}

	if(proposePort < 1025) {
		//0-1024 are reserved
		proposePort = 1025;
    }
	
	//Make sure it is unique
	while(portMap.contains(proposePort)) {
		proposePort = proposePort + 1;
	}
	
	if(proposePort > 65534) {
		proposePort = 0;
	}
	
    return proposePort;
}



QTreeWidgetItem* ATSkeletonWindow::treeTunnelCreateNewFolder(QTreeWidgetItem *twiInsertAt, int insertMode)
{
	bool blocked = ui.treeTunnels->blockSignals(true);

	Tunnel_c *tunnel = new Tunnel_c();
	tunnel->iType = TUNNEL_TYPE_FOLDER;
	tunnel->strName = "Untitled";

	QTreeWidgetItem *twiInserted = new QTreeWidgetItem( );
	twiInserted->setFlags(twiInserted->flags() | Qt::ItemIsEditable);
	twiInserted->setText( 0, tunnel->strName );
	qDebug() << "setIcon 12";
	treeTunnelSetIcon(twiInserted, Images::icon("ht_170_folder_closed_disconnected"));
	setTunnel(twiInserted, tunnel);

	treeTunnelInsert(twiInsertAt, twiInserted, insertMode);
	
	ui.treeTunnels->blockSignals(blocked);

	ui.treeTunnels->setCurrentItem( twiInserted );
 	qApp->processEvents();

	return twiInserted;
}

bool ATSkeletonWindow::treeTunnelIsDuplicateByName(QTreeWidgetItem* twi)
{
	bool foundSameName = false;
	QTreeWidgetItem *parent = twi->parent();
	if(parent != NULL) {
		for(int i=0;i<parent->childCount();i++) {
			QTreeWidgetItem *child = parent->child(i);
			if(child != twi && child->text(0) == twi->text(0)) {
				foundSameName = true;
				break;
			}
		}
	} else {
		for(int i=0;i<ui.treeTunnels->topLevelItemCount();i++) {
			QTreeWidgetItem *child = ui.treeTunnels->topLevelItem(i);
			if(child != twi && child->text(0) == twi->text(0)) {
				foundSameName = true;
				break;
			}
		}
	}
	return foundSameName;
}


void ATSkeletonWindow::treeTunnelInsert(QTreeWidgetItem *twiInsertAt, QTreeWidgetItem *twiInsert, int insertMode)
{
	if(twiInsert == NULL) {
		return;
	}

	if(twiInsertAt != NULL) 
	{
		if(insertMode == INSERT_MODE_AFTER) {
			QTreeWidgetItem *twiInsertAtParent = twiInsertAt->parent();
			if(twiInsertAtParent != NULL) {
				int index = twiInsertAtParent->indexOfChild(twiInsertAt);
				twiInsertAtParent->insertChild(index+1, twiInsert);
			} else {
				int index = ui.treeTunnels->indexOfTopLevelItem(twiInsertAt);
				ui.treeTunnels->insertTopLevelItem(index+1, twiInsert);
			}
			treeTunnelUpdateFromParent(twiInsert);
		}
		else if(insertMode == INSERT_MODE_BEFORE) {
			QTreeWidgetItem *twiInsertAtParent = twiInsertAt->parent();
			if(twiInsertAtParent != NULL) {
				int index = twiInsertAtParent->indexOfChild(twiInsertAt);
				twiInsertAtParent->insertChild(index, twiInsert);
			} else {
				int index = ui.treeTunnels->indexOfTopLevelItem(twiInsertAt);
				ui.treeTunnels->insertTopLevelItem(index, twiInsert);
			}
			treeTunnelUpdateFromParent(twiInsert);
		}
		else if(insertMode == INSERT_MODE_CHILD) {
			twiInsertAt->insertChild(twiInsertAt->childCount(), twiInsert);
			treeTunnelUpdateFromParent(twiInsert);
		}
	}
	else
	{
		//insert at bottom of root
		ui.treeTunnels->insertTopLevelItem(ui.treeTunnels->topLevelItemCount(), twiInsert);
		treeTunnelUpdateFromParent(twiInsert);
	}
}

void ATSkeletonWindow::treeTunnelUpdateFromParent(QTreeWidgetItem* twi)
{
	qDebug() << Q_FUNC_INFO;

	ATASSERT(twi);
	Tunnel_c *pt = getTunnel(twi);
	ATASSERT(pt);
	
	QTreeWidgetItem *twiParent = findParentTunnelNode(twi);
	if(twiParent != NULL) {
		Tunnel_c *ptParent = getTunnel(twiParent);
		ATASSERT(ptParent);
		QList<QTreeWidgetItem*> twiUpdateList;
		if(pt->iType == TUNNEL_TYPE_FOLDER) {
			twiUpdateList.append( findChildTunnelNodes(twi) );
		} else {
			twiUpdateList.append( twi );
		}

        //build strListSSHHostParent (list of parents main tunnel + more tunnels)
        QStringList strListSSHHostParent;

        if(ptParent->strLocalIP.isEmpty()) {
            strListSSHHostParent.append( QString("%1:%2").arg("localhost").arg(ptParent->iLocalPort) );
        } else {
            strListSSHHostParent.append( QString("%1:%2").arg(ptParent->strLocalIP).arg(ptParent->iLocalPort) );
        }

        for(int i=0;i<ptParent->portForwardList.size();i++) {
            PortForwardStruct pfs = ptParent->portForwardList.at(i);
            if(!pfs.bEnabled) continue;
            if(pfs.nType == PortForwardStruct::LOCAL) {
                if(pfs.nLocalPort > 0 && !pfs.strDestinationHost.isEmpty() && pfs.nDestinationPort > 0) {
                    if(pfs.strLocalIP.isEmpty()) {
                        strListSSHHostParent.append( QString( "%1:%2" ).arg("localhost").arg( pfs.nLocalPort ) );
                    } else {
                        strListSSHHostParent.append( QString( "%1:%2" ).arg( pfs.strLocalIP ).arg( pfs.nLocalPort ) );
                    }
                }
            }
        }

        QString strListSSHHostParentString = strListSSHHostParent.join("\n");

        //update child tunnels
		for(int i=0;i<twiUpdateList.size();i++) {
			QTreeWidgetItem* twiUpdate = twiUpdateList.at(i);
			Tunnel_c *ptUpdate = getTunnel(twiUpdate);
			qDebug() << Q_FUNC_INFO << "setSSHHostList " << ptParent->iLocalPort;
            ptUpdate->setSSHHostList( strListSSHHostParentString );
		}
	}
}



void ATSkeletonWindow::slotSelectTunnel()
{
	qDebug() << "ATSkeletonWindow::slotSelectTunnel()";
	delayUpdateUIFromTWI();
}

void ATSkeletonWindow::delayUpdateUIFromTWI()
{
	qDebug() << "ATSkeletonWindow::delayUpdateUIFromTWI()";
	m_timerDelayUpdateUIFromTWI.start();
}

void ATSkeletonWindow::slotUpdateUIFromTWI()
{
	qDebug() << "ATSkeletonWindow::slotUpdateUIFromTWI()";
	QTreeWidgetItem *twi = ui.treeTunnels->currentItem();
	m_pTreeTunnelsItemEdit = twi;
	populateEditUIFromTwi(twi);
	populateConnectUIFromTwi(twi);
	updateControls();					//if folder, then disable connect & edit tab
}

void ATSkeletonWindow::populateEditUIFromTwi( QTreeWidgetItem *twi )
{
	if(twi == NULL) {
		return;
	}

	qDebug() << "ATSkeletonWindow::populateEditUIFromTwi() " << twi->text(0);

    Tunnel_c *pt = getTunnel(twi);
    ATASSERT(pt);
	Tunnel_c *ptParent = getTunnel(findParentTunnelNode(twi));
	
    if(pt->iType == TUNNEL_TYPE_TUNNEL) {

        bool wasBlocked = ui.widgetEditTunnel->blockSignals(true);

        if(ptParent != NULL && ptParent->iType == TUNNEL_TYPE_TUNNEL) {
            ui.btnEditSSHHost->setEnabled(false);
            ui.editSSHHost->setReadOnly(true);
            QPalette pal;
            pal.setColor(QPalette::Text, Qt::gray);
            ui.editSSHHost->setPalette(pal);
        } else {
            QPalette pal;
            pal.setColor(QPalette::Text, Qt::black);
            ui.editSSHHost->setPalette(pal);
            ui.editSSHHost->setReadOnly(false);
            ui.btnEditSSHHost->setEnabled(true);
        }

        ui.editTunnelName->setText( pt->strName );

        //fg/bg color
        QPalette palette = ui.editTunnelName->palette();
        if(twi->foreground(0).style() != Qt::NoBrush) {
            palette.setColor(QPalette::Text, twi->foreground(0).color());
        } else {
            QBrush brush = QApplication::palette().brush(QPalette::Normal, QPalette::Text);
            brush.setStyle(Qt::NoBrush);
            palette.setBrush(QPalette::Text, brush);
        }
        if(twi->background(0).style() != Qt::NoBrush) {
            palette.setColor(QPalette::Base, twi->background(0).color());
        } else {
            palette.setBrush(QPalette::Base, QBrush());
        }

        ui.editTunnelName->setPalette(palette);

        editSetSSHHost( pt->getSSHHostList() );
        editSetSelectedSSHHost( pt->getSelectedSSHHost() );
        editSetRemoteHost( pt->getRemoteHostList() );
        editSetSelectedRemoteHost( pt->getSelectedRemoteHost() );
        ui.editExtraArguments->setText( pt->strExtraArguments );
        ui.editUsername->setText( pt->strUsername );
        ui.editPassword->setText( pt->strPassword );
        ui.editSSHKeyFile->setText( pt->strSSHKeyFile );
        ui.editSSHKeyFilePassword->setText( pt->strSSHKeyFilePassword );
        ui.checkAutoConnect->setChecked( pt->bAutoConnect );
        ui.checkCompression->setChecked( pt->bCompression );
        ui.checkAutoReconnect->setChecked( pt->bAutoReconnect );
        ui.radioSSH1->setChecked( pt->iSSH1or2 == 1 );
        ui.radioSSH2->setChecked( pt->iSSH1or2 != 1 );

        ui.editLocalIP->setText( pt->strLocalIP );

        if ( pt->iLocalPort )
            ui.editLocalPort->setText( QString::number( pt->iLocalPort ) );
        else ui.editLocalPort->clear();

        if ( pt->iRemotePort )
            ui.editRemotePort->setText( QString::number( pt->iRemotePort ) );
        else ui.editRemotePort->clear();

        ui.comboPasswordSelect->clear();
        ui.comboPasswordSelect->addItem("None");
        ui.comboKeyPasswordSelect->clear();
        ui.comboKeyPasswordSelect->addItem("None");

        PasswordDb *pdb = PasswordDb::getInstance();
        for(unsigned int i = 0; i < pdb->size(); ++i) {
            Identifier ident = pdb->at(i);
            QString pwdName = ident.getDescription();
            ui.comboPasswordSelect->addItem(pwdName);
            ui.comboKeyPasswordSelect->addItem(pwdName);
        }

        ui.widgetMoreTunnels->setData(pt->portForwardList);
        ui.widgetMoreTunnels->slotUpdateControls();

        ui.widgetEditTunnel->blockSignals(wasBlocked);


    } //end - if(pt->iType == TUNNEL_TYPE_TUNNEL)
    else if(pt->iType == TUNNEL_TYPE_FOLDER) {

        bool wasBlocked = ui.widgetEditFolder->blockSignals(true);

        ui.editFolderName->setText( pt->strName );

        //fg/bg color
        QPalette palette = ui.editFolderName->palette();
        if(twi->foreground(0).style() != Qt::NoBrush) {
            palette.setColor(QPalette::Text, twi->foreground(0).color());
        } else {
            QBrush brush = QApplication::palette().brush(QPalette::Normal, QPalette::Text);
            brush.setStyle(Qt::NoBrush);
            palette.setBrush(QPalette::Text, brush);
        }
        if(twi->background(0).style() != Qt::NoBrush) {
            palette.setColor(QPalette::Base, twi->background(0).color());
        } else {
            palette.setBrush(QPalette::Base, QBrush());
        }

        ui.editFolderName->setPalette(palette);

        ui.editFolderChildNodesCommand->setText( pt->strChildNodesCommand );    

        ui.checkFolderActivateDisconnectsAllChildren->setChecked( pt->bActivateDisconnects );

        ui.widgetEditFolder->blockSignals(wasBlocked);

    }

    ui.widgetCustomActions->setData(pt);







}


void ATSkeletonWindow::populateConnectUIFromTwi( QTreeWidgetItem *twi )
{
	if(twi == NULL) {
		return;
	}

	qDebug() << "populateConnectUIFromTwi() " << twi->text(0);

	Tunnel_c *pt = getTunnel(twi);
	ATASSERT(pt);
	if(pt != NULL) {
        ui.textBrowser->setHtml( pt->log.toHtml() );
	}
	ui.textBrowser->verticalScrollBar()->setValue( ui.textBrowser->verticalScrollBar()->maximum() );
}


void ATSkeletonWindow::slotFocusChanged(QWidget* oldFocus, QWidget* newFocus)
{
    qDebug("ATSkeletonWindow::slotFocusChanged()");
	qDebug("oldFocus: %s", oldFocus != NULL ? qPrintable(oldFocus->objectName()) : "NULL" );
	qDebug("newFocus: %s", newFocus != NULL ? qPrintable(newFocus->objectName()) : "NULL" );

    ATVERIFY( disconnect( qApp, &QApplication::focusChanged, nullptr, nullptr ) );
	
	if(oldFocus == ui.treeTunnels) {
		//Hosts tree: clear paste status and disable paste in context menu
		if(m_pTreeTunnelsPasteMode != PASTE_MODE_COPY) {
			enableTreeTunnelsPaste(false);
		}
		ui.widgetTreeTunnelMoveButtons->setEnabled(false);
		ui.widgetTreeTunnelActionButtons->setEnabled(false);
	} else if(oldFocus == ui.treeCustomActionsEdit) {
		//ui.widgetCustomActionsEditButtons->setEnabled(false);
	}

	if(newFocus == ui.treeTunnels && m_bEditingEnabled) {
		ui.widgetTreeTunnelMoveButtons->setEnabled(true);
		ui.widgetTreeTunnelActionButtons->setEnabled(true);
	} else if(newFocus == ui.treeCustomActionsEdit && m_bEditingEnabled) {
		//ui.widgetCustomActionsEditButtons->setEnabled(true);
	}

    ATVERIFY( connect(qApp, &QApplication::focusChanged, this, &ATSkeletonWindow::slotFocusChanged) );
}

bool ATSkeletonWindow::confirmSaveEditPane()
{
    qDebug("ATSkeletonWindow::confirmSaveTunnel()");
	if(m_pMainWindow->isWindowModified()) 
	{
		m_pMainWindow->setWindowModified(false);

		if(!m_pMainWindow->preferences()->autoSaveEditPaneEnabled()) {
 			QMessageBox::StandardButton iRet = QMessageBox::question( this, APP_NICE_NAME, "Do you want to save the changes?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes );
 			if ( iRet == QMessageBox::Yes )	{
                saveEditPane();
				return true;
            }
		} else {
            saveEditPane();
			return true;
		}

		//populateEditUIFromTwi(m_pTreeTunnelsItemEdit);
		//populateConnectUIFromTwi(m_pTreeTunnelsItemEdit);
	}
	
	return false;
}

bool ATSkeletonWindow::askforPassword( Tunnel_c &tunnel )
{
	Tunnel_c *pt = &tunnel;

	// Early out if the user has already provided a password.
	if ( pt->strPassword != INVALID_PASSWORD )
		return true;

	QDialog dlg;

	dlg.setWindowTitle( APP_NICE_NAME );

	QLabel *pLabel = new QLabel( &dlg );
	QString msg = "Password for user '$uid' on host '$host':";
	pLabel->setText( replaceVars(tunnel, msg) );

	QLineEdit *pEdit = new QLineEdit( &dlg );
	pEdit->setEchoMode( QLineEdit::Password );

	QPushButton *pButton = new QPushButton( &dlg );
	pButton->setText( "Ok" );
	pButton->setDefault( true );
	ATVERIFY( connect( pButton, SIGNAL( clicked() ), &dlg, SLOT( accept() ) ) );

	QVBoxLayout *vbox = new QVBoxLayout;
	vbox->addWidget( pLabel );
	vbox->addWidget( pEdit );
	vbox->addWidget( pButton );
	dlg.setLayout( vbox );

	if ( dlg.exec() )
	{
		pt->strPassword = pEdit->text();
		return true;
	}

	return false;
}

QString ATSkeletonWindow::askforPassword( QString label )
{
	QDialog dlg;

	dlg.setWindowTitle( APP_NICE_NAME );

	QLabel *pLabel = new QLabel( &dlg );
	pLabel->setText( label );

	QLineEdit *pEdit = new QLineEdit( &dlg );
	pEdit->setEchoMode( QLineEdit::Password );

	QPushButton *pButton = new QPushButton( &dlg );
	pButton->setText( "Ok" );
	pButton->setDefault( true );
	ATVERIFY( connect( pButton, SIGNAL( clicked() ), &dlg, SLOT( accept() ) ) );

	QVBoxLayout *vbox = new QVBoxLayout;
	vbox->addWidget( pLabel );
	vbox->addWidget( pEdit );
	vbox->addWidget( pButton );
	dlg.setLayout( vbox );

	if ( dlg.exec() )
	{
		return pEdit->text();
	}

	return "";
}

int ATSkeletonWindow::getConnectionCountChildren(QTreeWidgetItem* twi, bool countFolders)
{
	int iConnectionCount = 0;

	for(int i=0;i<twi->childCount();i++) {
		QTreeWidgetItem* child = twi->child(i);
		Tunnel_c *pt = getTunnel(child);
		if(pt != NULL) {
			if(pt->iConnectStatus == CONNECTED) {
				if(pt->iType == TUNNEL_TYPE_TUNNEL) {
					iConnectionCount++;
				} else if(pt->iType == TUNNEL_TYPE_FOLDER) {
					if(countFolders) {
						iConnectionCount++;
					}
				}
			}
		}
	}

	return iConnectionCount;
}

int ATSkeletonWindow::getDelayRetryConnectCountChildren(QTreeWidgetItem* twi)
{
    int iConnectionCount = 0;

    for(int i=0;i<twi->childCount();i++) {
        QTreeWidgetItem* child = twi->child(i);
        Tunnel_c *pt = getTunnel(child);
        if(pt != NULL) {
            if(pt->iType == TUNNEL_TYPE_TUNNEL) {
                if(pt->isActiveTimerDelayRetryConnect()) {
                    iConnectionCount++;
                }
            } else if(pt->iType == TUNNEL_TYPE_FOLDER) {
                iConnectionCount += getDelayRetryConnectCountChildren(child);
            }
        }
    }

    return iConnectionCount;
}

int ATSkeletonWindow::getConnectionCount()
{
    QList<QTreeWidgetItem*> treeTunnelItems = ui.treeTunnels->findItems(".*", Qt::MatchFlags(Qt::MatchRegularExpression | Qt::MatchRecursive), 0);
	int iConnectionCount = 0;
	for(int i=0;i<treeTunnelItems.size();i++)
	{
		Tunnel_c *it = getTunnel(treeTunnelItems[i]);
		if ( it->pProcess )
		{
			iConnectionCount++;
		}
	}
	return iConnectionCount;
}

bool ATSkeletonWindow::onClose()
{
	qDebug() << "ATSkeletonWindow::onClose()";

    confirmSaveEditPane();

	if ( m_pMainWindow->preferences()->confirmOnQuitEnabled() )
	{
		int iConnectionCount = getConnectionCount();
		if ( iConnectionCount )
		{
			QString strQuestion;

			if ( iConnectionCount == 1 )
				strQuestion = "There are 1 active connection.";
			else
				strQuestion = QString( "There are %1 active connections." ).arg( iConnectionCount );

			strQuestion += "\nAre you sure you want to disconnect and quit ?";


			QMessageBox::StandardButton iRet = QMessageBox::question( this, APP_NICE_NAME, strQuestion, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes );

			if ( iRet == QMessageBox::No )
				return false;
		}
	}

    m_pMainWindow->m_pSearchWidget->blockSignals(true);

    autoBackupSettings( BackupInfo(BACKUP_BEFORE_APPSTOP,"Before writing settings at application shutdown") );

    writeSettings();
    m_pMainWindow->writeWindowSettings();

	autoBackupSettings( BackupInfo(BACKUP_AFTER_APPSTOP,"Application shutdown") );

    disconnectAllTunnelsSilent();

	return true;
}

void ATSkeletonWindow::AddToLog( Tunnel_c &tunnel, const QString &strLog )
{
    QTreeWidgetItem *twi = ui.treeTunnels->currentItem();
    if ( ( twi != NULL ) && ( twi != tunnel.twi ) ) {
        twi = NULL;
    }

    if ( tunnel.log.size() > (LOG_MAX_BUFFER_SIZE*2) )
    {
        tunnel.log.trimToSize(LOG_MAX_BUFFER_SIZE);
        if ( twi != NULL )
        {
            ui.textBrowser->setHtml( tunnel.log.toHtml() );
            ui.textBrowser->verticalScrollBar()->setValue( ui.textBrowser->verticalScrollBar()->maximum() );
        }
    }

    tunnel.log.append( strLog );
    if ( twi != NULL ) {
        ui.textBrowser->append( TunnelLog::toHtml(strLog) );
    }
}


void ATSkeletonWindow::treeTunnelSetIcon(QTreeWidgetItem *twi, QPixmap pm)
{
	bool blocked = ui.treeTunnels->blockSignals(true);
	twi->setIcon( 0, pm );
	ui.treeTunnels->blockSignals(blocked);
}

void ATSkeletonWindow::slotAutoConnect( Tunnel_c *pt )
{
	// If it's a valid index, auto connect it, otherwise auto connect all that were ticked in the settings.
	if ( pt != NULL )
	{
		connectTunnel( *pt );
	}
	else
	{
		if(m_pMainWindow->preferences()->autoConnectEnabled())
		{
            QList<QTreeWidgetItem*> treeTunnelItems = ui.treeTunnels->findItems(".*", Qt::MatchFlags(Qt::MatchRegularExpression | Qt::MatchRecursive), 0);
			for(int i=0;i<treeTunnelItems.size();i++)
			{
				Tunnel_c* it = getTunnel(treeTunnelItems[i]);
				if ( it->bAutoConnect )
				{
					markConnect(treeTunnelItems[i]);
				}
			}

			crawlConnect(NULL);
		}
	}
}

QList<VariableStruct> ATSkeletonWindow::getPasswordVariables()
{
	QList<VariableStruct> varList;

    PasswordDb* pdb = PasswordDb::getInstance();
    for(unsigned int i=0; i<pdb->size(); i++) {
        VariableStruct var;
        Identifier ident = pdb->at(i);
        var.nType = VariableStruct::PASSWORDS;
        var.uUid = ident.getUuid();
        var.strDescription = ident.getDescription();
        var.strName = ident.getLogin();
        var.strValue = ident.getPassword();
        varList.append(var);
    }

	return varList;
}

QMap<QUuid,VariableStatStruct> ATSkeletonWindow::getVariableStatistics()
{
	QMap<QUuid,VariableStatStruct> statMap;

	//Password vars used in m_listExecutableVariables OR m_listUserDefinedVariables ?
	QList<VariableStruct> passwordVariables = getPasswordVariables();
	for(int pwdVarIndex=0;pwdVarIndex<passwordVariables.size();pwdVarIndex++) {
		VariableStruct var = passwordVariables.at(pwdVarIndex);
		QString name = var.strDescription.trimmed();
		if(name.isEmpty()) {
			continue;
		}

		statMap.insert(var.uUid,VariableStatStruct(var.uUid)); //Empty stats

		QString varNameUid1 = "${"+name+".uid}";
		QString varNameUid2 = "$"+name+".uid";
		QString varNamePwd1 = "${"+name+".pwd}";
		QString varNamePwd2 = "$"+name+".pwd";

        //Password var used in m_listExecutableVariables filename ?
		for(int execVarCheckIndex=0;execVarCheckIndex<m_listExecutableVariables.size();execVarCheckIndex++) {
			VariableStruct varCheck = m_listExecutableVariables.at(execVarCheckIndex);
			QString valueCheck = varCheck.strValue;
            QString argsCheck = varCheck.strArgs;
            bool varUsed = false;
			if(valueCheck.contains(varNameUid1) || valueCheck.contains(varNameUid2)
				|| valueCheck.contains(varNamePwd1) || valueCheck.contains(varNamePwd2)) {
                varUsed = true;
            } else if(argsCheck.contains(varNameUid1) || argsCheck.contains(varNameUid2)
                  || argsCheck.contains(varNamePwd1) || argsCheck.contains(varNamePwd2)) {
                varUsed = true;
              }
            if(varUsed) {
                VariableStatStruct varStat = statMap.value(var.uUid);
                varStat.listExecVarables.append(varCheck.uUid); //the exec var that reference this password var
                statMap.insert(var.uUid,varStat);
            }
		}

		//Password var used in m_listUserDefinedVariables ?
		for(int userVarCheckIndex=0;userVarCheckIndex<m_listUserDefinedVariables.size();userVarCheckIndex++) {
			VariableStruct varCheck = m_listUserDefinedVariables.at(userVarCheckIndex);
			QString valueCheck = varCheck.strValue;
			if(valueCheck.contains(varNameUid1) || valueCheck.contains(varNameUid2)
				|| valueCheck.contains(varNamePwd1) || valueCheck.contains(varNamePwd2)) {
				VariableStatStruct varStat = statMap.value(var.uUid);
				varStat.listUserDefVarables.append(varCheck.uUid); //the userdef var that reference this exec var
				statMap.insert(var.uUid,varStat);
			}
		}
	}

	//Executable vars used in m_listExecutableVariables OR m_listUserDefinedVariables ?

	for(int execVarIndex=0;execVarIndex<m_listExecutableVariables.size();execVarIndex++) {
		VariableStruct var = m_listExecutableVariables.at(execVarIndex);
		QString name = var.strName.trimmed();
		if(name.isEmpty()) {
			continue;
		}

		statMap.insert(var.uUid,VariableStatStruct(var.uUid)); //Empty stats

		QString varName1 = "${"+name+"}";
		QString varName2 = "$"+name;

		//Executable var used in m_listExecutableVariables ?
		for(int execVarCheckIndex=0;execVarCheckIndex<m_listExecutableVariables.size();execVarCheckIndex++) {
			VariableStruct varCheck = m_listExecutableVariables.at(execVarCheckIndex);
			QString valueCheck = varCheck.strValue;
            QString argsCheck = varCheck.strArgs;
            bool varUsed = false;
			if(valueCheck.contains(varName1) || valueCheck.contains(varName2)) {
                varUsed = true;
            } else if(argsCheck.contains(varName1) || argsCheck.contains(varName2)) {
                varUsed = true;
            }
            if(varUsed) {
                VariableStatStruct varStat = statMap.value(var.uUid);
                varStat.listExecVarables.append(varCheck.uUid); //the exec var that reference this exec var
                statMap.insert(var.uUid,varStat);
            }
		}

		//Executable var used in m_listUserDefinedVariables ?
		for(int userVarCheckIndex=0;userVarCheckIndex<m_listUserDefinedVariables.size();userVarCheckIndex++) {
			VariableStruct varCheck = m_listUserDefinedVariables.at(userVarCheckIndex);
			QString valueCheck = varCheck.strValue;
			if(valueCheck.contains(varName1) || valueCheck.contains(varName2)) {
				VariableStatStruct varStat = statMap.value(var.uUid);
				varStat.listUserDefVarables.append(varCheck.uUid); //the userdef var that reference this exec var
				statMap.insert(var.uUid,varStat);
			}
		}
	}

	//Userdefined vars used in m_listExecutableVariables OR m_listUserDefinedVariables ?

	for(int userVarIndex=0;userVarIndex<m_listUserDefinedVariables.size();userVarIndex++) {
		VariableStruct var = m_listUserDefinedVariables.at(userVarIndex);
		QString name = var.strName.trimmed();
		if(name.isEmpty()) {
			continue;
		}

		statMap.insert(var.uUid,VariableStatStruct(var.uUid)); //Empty stats

		QString varName1 = "${"+name+"}";
		QString varName2 = "$"+name;

		//Userdefined var used in m_listUserDefinedVariables ?
		for(int userVarCheckIndex=0;userVarCheckIndex<m_listUserDefinedVariables.size();userVarCheckIndex++) {
			VariableStruct varCheck = m_listUserDefinedVariables.at(userVarCheckIndex);
			QString valueCheck = varCheck.strValue;
			if(valueCheck.contains(varName1) || valueCheck.contains(varName2)) {
				VariableStatStruct varStat = statMap.value(var.uUid);
				varStat.listUserDefVarables.append(varCheck.uUid); //the userdef var that reference this userdefined var
				statMap.insert(var.uUid,varStat);
			}
		}

		//Userdefined var used in m_listExecutableVariables ?
		for(int execVarCheckIndex=0;execVarCheckIndex<m_listExecutableVariables.size();execVarCheckIndex++) {
			VariableStruct varCheck = m_listExecutableVariables.at(execVarCheckIndex);
			QString valueCheck = varCheck.strValue;
            QString argsCheck = varCheck.strArgs;
            bool varUsed = false;
			if(valueCheck.contains(varName1) || valueCheck.contains(varName2)) {
                varUsed = true;
            } else if(argsCheck.contains(varName1) || argsCheck.contains(varName2)) {
                varUsed = true;
            }
            if(varUsed) {
                VariableStatStruct varStat = statMap.value(var.uUid);
                varStat.listExecVarables.append(varCheck.uUid); //the executable var that reference this userdefined var
                statMap.insert(var.uUid,varStat);
            }
		}

	}

    // Usage in tunnels and folders

    QList<QTreeWidgetItem*> treeTunnelItems = ui.treeTunnels->findItems(".*", Qt::MatchFlags(Qt::MatchRegularExpression | Qt::MatchRecursive), 0);
	for(int twiIndex=0;twiIndex<treeTunnelItems.size();twiIndex++) {
		Tunnel_c *pt = getTunnel(treeTunnelItems[twiIndex]);
		if(pt == NULL) {
			continue;
		}

		// Password vars used in this tunnel
		for(int pwdVarIndex=0;pwdVarIndex<passwordVariables.size();pwdVarIndex++) {
			VariableStruct var = passwordVariables.at(pwdVarIndex);
			QString name = var.strDescription.trimmed();
			if(name.isEmpty()) {
				continue;
			}
			QString nameUid = name+".uid";
			QString namePwd = name+".pwd";
			VariableStatTunnelStruct varStatTunnelUid = getVariableUsageInTunnel(pt,nameUid);
			VariableStatTunnelStruct varStatTunnelPwd = getVariableUsageInTunnel(pt,namePwd);
			if(varStatTunnelUid.totalUsageCount() > 0 || varStatTunnelPwd.totalUsageCount() > 0) {
				VariableStatStruct varStat = statMap.value(var.uUid,VariableStatStruct(var.uUid));
                if(varStatTunnelUid.totalUsageCount() > 0) {
                    varStat.listTunnelStats.append(varStatTunnelUid);
                } else {
                    varStat.listTunnelStats.append(varStatTunnelPwd);
                }
				statMap.insert(var.uUid,varStat);
			}
		}


		// Executable vars used in this tunnel
		for(int execVarIndex=0;execVarIndex<m_listExecutableVariables.size();execVarIndex++) {
			VariableStruct var = m_listExecutableVariables.at(execVarIndex);
			QString name = var.strName.trimmed();
			if(name.isEmpty()) {
				continue;
			}
			VariableStatTunnelStruct varStatTunnel = getVariableUsageInTunnel(pt,name);
			if(varStatTunnel.totalUsageCount() > 0) {
				VariableStatStruct varStat = statMap.value(var.uUid,VariableStatStruct(var.uUid));
				varStat.listTunnelStats.append(varStatTunnel);
				statMap.insert(var.uUid,varStat);
			}
		}

		// Userdefined vars used in this tunnel
		for(int userVarIndex=0;userVarIndex<m_listUserDefinedVariables.size();userVarIndex++) {
			VariableStruct var = m_listUserDefinedVariables.at(userVarIndex);
			QString name = var.strName.trimmed();
			if(name.isEmpty()) {
				continue;
			}
			VariableStatTunnelStruct varStatTunnel = getVariableUsageInTunnel(pt,name);
			if(varStatTunnel.totalUsageCount() > 0) {
				VariableStatStruct varStat = statMap.value(var.uUid,VariableStatStruct(var.uUid));
				varStat.listTunnelStats.append(varStatTunnel);
				statMap.insert(var.uUid,varStat);
			}
		}
	}

	return statMap;
}

//static
VariableStatTunnelStruct ATSkeletonWindow::getVariableUsageInTunnel(Tunnel_c *it, QString name)
{
	VariableStatTunnelStruct varTS;

	if(name.trimmed().isEmpty()) {
		return varTS;
	}

	QString varName1 = "${"+name.trimmed()+"}";
	QString varName2 = "$"+name.trimmed();

	for(int hostIndex=0;hostIndex<it->strSSHHostList.size();hostIndex++) {
		QString host = it->strSSHHostList.at(hostIndex);
		if(host.contains(varName1) || host.contains(varName2)) {
			varTS.listFields.append(VariableStatTunnelStruct::FIELD_SSH_HOST);
		}
	}

	if(it->strPassword.contains(varName1) || it->strPassword.contains(varName2)) {
		varTS.listFields.append(VariableStatTunnelStruct::FIELD_PASSWORD);
	}

	if(it->strUsername.contains(varName1) || it->strUsername.contains(varName2)) {
		varTS.listFields.append(VariableStatTunnelStruct::FIELD_USER_NAME);
	}

	if(it->strSSHKeyFile.contains(varName1) || it->strSSHKeyFile.contains(varName2)) {
		varTS.listFields.append(VariableStatTunnelStruct::FIELD_SSH_KEY_FILE);
	}

	if(it->strSSHKeyFilePassword.contains(varName1) || it->strSSHKeyFilePassword.contains(varName2)) {
		varTS.listFields.append(VariableStatTunnelStruct::FIELD_KEY_PASSWORD);
	}

	if(it->strExtraArguments.contains(varName1) || it->strExtraArguments.contains(varName2)) {
		varTS.listFields.append(VariableStatTunnelStruct::FIELD_EXTRA_ARGUMENTS);
	}

    if(it->strChildNodesCommand.contains(varName1) || it->strChildNodesCommand.contains(varName2)) {
        varTS.listFields.append(VariableStatTunnelStruct::FIELD_CHILDNODES_COMMAND);
    }

	for(int hostIndex=0;hostIndex<it->strRemoteHostList.size();hostIndex++) {
		QString host = it->strRemoteHostList.at(hostIndex);
		if(host.contains(varName1) || host.contains(varName2)) {
			varTS.listFields.append(VariableStatTunnelStruct::FIELD_DESTINATION_HOST);
		}
	}

	for(int actionIndex=0;actionIndex<it->customActionList.size();actionIndex++) {
		CustomActionStruct cas = it->customActionList.at(actionIndex);
		if(cas.sCmd.contains(varName1) || cas.sCmd.contains(varName2)) {
			varTS.listCustomActions.append(cas);
		}
	}

	return varTS;
}



//QPair.first = original varname
//QPair.second = new varname
//static
void ATSkeletonWindow::changeTunnelVarNames(QList<QPair<QString,QString> > & varList)
{
    qDebug() << Q_FUNC_INFO  << " varList.size(): " << varList.size();
	if(varList.size() == 0) return;

    QList<QPair<QString,QString> > varNameList;
	for(int varIndex=0;varIndex<varList.size();varIndex++) {
		//QPair.first = new name
		//QPair.second = original name
		QString origName = varList.at(varIndex).first.trimmed();
		QString newName = varList.at(varIndex).second.trimmed();
		if(origName.isEmpty() || newName.isEmpty()) {
			continue;
		}
		//varname can be referenced by $name or ${name}. Replace ${name} first, then $name
		QString origVarName1 = "${" + origName + "}";
		QString origVarName2 = "$" + origName;
		QString newVarName1 = "${" + newName + "}";
		QString newVarName2 = "$" + newName;
		if(newName.contains(" ")) {
			newVarName2 = "${" + newName + "}";
		}
		varNameList.append( QPair<QString,QString>(origVarName1,newVarName1) );
		varNameList.append( QPair<QString,QString>(origVarName2,newVarName2) );
	}

	QStringList varListForLog;
	for(int varIndex=0;varIndex<varList.size();varIndex++) {
		varListForLog.append(QString("[%1]->[%2]").arg(varList.at(varIndex).first).arg(varList.at(varIndex).second));
	}

	// +++ UPDATE TUNNELS

    QList<QTreeWidgetItem*> treeTunnelItems = ui.treeTunnels->findItems(".*", Qt::MatchFlags(Qt::MatchRegularExpression | Qt::MatchRecursive), 0);
	for(int i=0;i<treeTunnelItems.size();i++)
	{
		Tunnel_c *it = getTunnel(treeTunnelItems[i]);

		for(int varIndex=0;varIndex<varNameList.size();varIndex++) {
			QString origVarName = varNameList.at(varIndex).first;
			QString newVarName = varNameList.at(varIndex).second;

			for(int hostIndex=0;hostIndex<it->strSSHHostList.size();hostIndex++) {
				QString host = it->strSSHHostList.at(hostIndex);
				host = host.replace(origVarName, newVarName);
				it->strSSHHostList.replace(hostIndex,host);
			}

			for(int hostIndex=0;hostIndex<it->strRemoteHostList.size();hostIndex++) {
				QString host = it->strRemoteHostList.at(hostIndex);
				host = host.replace(origVarName, newVarName);
				it->strRemoteHostList.replace(hostIndex,host);
			}

			it->strUsername = it->strUsername.replace(origVarName, newVarName);
			it->strPassword = it->strPassword.replace(origVarName, newVarName);

			for(int actionIndex=0;actionIndex<it->customActionList.size();actionIndex++) {
				CustomActionStruct cas = it->customActionList.at(actionIndex);
				cas.sCmd = cas.sCmd.replace(origVarName, newVarName);
				it->customActionList.replace(actionIndex,cas);
			}

			it->strSSHKeyFile = it->strSSHKeyFile.replace(origVarName, newVarName);
			it->strSSHKeyFilePassword = it->strSSHKeyFilePassword.replace(origVarName, newVarName);
			it->strExtraArguments = it->strExtraArguments.replace(origVarName, newVarName);	

            it->strChildNodesCommand = it->strChildNodesCommand.replace(origVarName, newVarName);

		} //end for - each variable

	} //end for - each tunnel

	// --- UPDATE TUNNELS


	// +++ UPDATE USERDEFINED VARIABLE VALUES

	//Update the variable value

	for(int j=0;j<varNameList.size();j++) {
		QString origVarName = varNameList.at(j).first;
		QString newVarName = varNameList.at(j).second;
		for(int i=0;i<m_listUserDefinedVariables.size();i++) {
			VariableStruct var = m_listUserDefinedVariables.at(i);
			var.strValue = var.strValue.replace(origVarName, newVarName);
			m_listUserDefinedVariables.replace(i,var);
		}
	}

	// --- UPDATE USERDEFINED VARIABLE VALUES


	//UPDATE variable names i GUI
	delayUpdateUIFromTWI();

	qDebug() << QString( "Changed [%1] variable names: [%2]").arg(varList.size()).arg(varListForLog.join(" ") );
}

void ATSkeletonWindow::slotPasswordDialog()
{
	m_pMainWindow->menuBar()->setEnabled(false);
	setEnabled(false);

    QFileInfo pwdFileInfo(g_strPwdFile);
    if(pwdFileInfo.exists())
    {
        //Ask for password
        PasswordDialog dlg(this);
        int dlgStatus = dlg.exec();
        if(dlgStatus != QDialog::Accepted) {
            qApp->exit(0);
            return;
        }

        try
        {
            PasswordDb* pdb = PasswordDb::getInstance();
            pdb->openFile(g_strPwdFile, dlg.password);
        }
        catch(std::exception &ex) {
            QMessageBox::critical(this, "Oops!", QString::fromStdString(ex.what()));
            qApp->exit(0);
            return;
        }
    }
    else //end if -- password db file exists
    {
        //Ask user to choose a password to create password db file
        CreatePasswordDialog dlg(this);
        bool rejected = (dlg.exec()!=QDialog::Accepted);
        if(rejected) {
            qApp->exit(0);
            return;
        }

        try
        {
            PasswordDb* pdb = PasswordDb::getInstance();
            pdb->createEmptyFile(g_strPwdFile, dlg.password);
        }
        catch(ExBadFileFormat &ex) {     
            QMessageBox::critical(this, "Wrong password", QString::fromStdString(ex.what()));
            qApp->exit(0);
            return;
        }
        catch(std::exception &ex) {
            QMessageBox::critical(this, "Oops!", QString::fromStdString(ex.what()));
            qApp->exit(0);
            return;
        }
    }

	m_pMainWindow->menuBar()->setEnabled(true);
	setEnabled(true);
	wireSignals();
	autoBackupSettings( BackupInfo(BACKUP_BEFORE_APPSTART,"Before reading settings at application start" ) );
	readSettings();

	if ( ui.treeTunnels->topLevelItemCount() == 0 )
		treeTunnelCreateNew();

	updateControls();

	ui.treeTunnels->setFocus();
	
	emit signalAutoConnect(NULL); // auto connect all	
}

// TunnelLog

TunnelLog::TunnelLog()
{
    m_strLog = QString();
}

void TunnelLog::append(const QString &str)
{
    if(!m_strLog.endsWith('\n') && !str.startsWith('\n') && !str.isEmpty() && !m_strLog.isEmpty()) {
        m_strLog.append('\n');
    }
    m_strLog = QString("%1%2").arg(m_strLog).arg(str);
}

void TunnelLog::trimToSize(int size)
{
    m_strLog = m_strLog.right(size);
}

int TunnelLog::size()
{
    return m_strLog.size();
}

void TunnelLog::clear()
{
    m_strLog.clear();
}

QString TunnelLog::toHtml()
{
    return toHtml(m_strLog);
}

//static
QString TunnelLog::toHtml(const QString &strLog)
{
    QString ret = "";

    QString str = strLog.trimmed();
    str = str.replace('\r',"");

    QStringList strList = QStringList();
    QStringList strSplit = str.split("\n", Qt::SkipEmptyParts);
    for(int i=0;i<strSplit.size();i++) {
        QString strPart = strSplit.at(i).trimmed();
        //if(strPart.length() > 0) {
            strList.append(strPart);
        //}
    }

    ret.append("<p style=\"margin-top:0px; margin-bottom:0px;\">");
    ret.append(strList.join("<br />"));
    ret.append("</p>");

    return ret;
}

// Tunnel_c

Tunnel_c::Tunnel_c()
{
	pProcess = NULL;
	pConnector = NULL;
    pPopulateChildNodesProcess = NULL;
    pPopulateChildNodesConnector = NULL;
	strPassword = INVALID_PASSWORD;
	strSSHKeyFilePassword = INVALID_PASSWORD;
	iPasswordRetries = 0;
	iKeyPasswordRetries = 0;
	iChallengeResponseRetries = 0;
    iReconnectRetries = 0;
    bShouldReconnect = false;
	iLevel = 0;
	iConnectStatus = DISCONNECTED;
	twi = NULL;

	init();
}

Tunnel_c::Tunnel_c(const Tunnel_c* other)
{
	copyFrom(other); //creates new UUID
	uUid = other->uUid; //reset it
}

Tunnel_c::~Tunnel_c()
{
	// The process should be terminated before a tunnel is destroyed
	ATASSERT( pProcess == NULL );
	ATASSERT( pConnector == NULL );
    ATASSERT( pPopulateChildNodesProcess == NULL );
    ATASSERT( pPopulateChildNodesConnector == NULL );
}

void Tunnel_c::init()
{
	uUid = QUuid::createUuid();
    strExtID = "";
	strName = "Untitled";
	iType = TUNNEL_TYPE_TUNNEL;
	bCompression = false;
	bDoKeepAlivePing = false;
	bAutoReconnect = false;
	iSSH1or2 = 2;
	strLocalIP = "localhost";
	iLocalPort = 0;
	iRemotePort = 0;
	iDirection = 0;
	bAutoConnect = false;
	strExtraArguments = "-N";
    strChildNodesCommand = "";
    bChildNodesCommandType = false;
    strFgColor = "";
    strBgColor = "";
    bActivateDisconnects = true;
    m_timerDelayRetryConnect.setSingleShot(true);
    ATVERIFY( connect( &m_timerDelayRetryConnect, SIGNAL( timeout() ), this, SLOT( slotTimerDelayRetryConnectTimeout() ), Qt::UniqueConnection ) );
}

void Tunnel_c::copyFrom( const Tunnel_c *orig )
{
	uUid = QUuid::createUuid();
    strExtID = orig->strExtID; //External ID (I.e Amazon EC2 instance cannot be copied)

	strName = orig->strName;
	iType = orig->iType;
	iLevel = orig->iLevel;

	pProcess = NULL;
	pConnector = NULL;
    pPopulateChildNodesProcess = NULL;
    pPopulateChildNodesConnector = NULL;
    iReconnectRetries = 0;
    bShouldReconnect = false;

	strSSHHostList = QStringList(orig->strSSHHostList);
	iSSHHost = orig->iSSHHost;
	strLocalIP = orig->strLocalIP;
	iLocalPort = orig->iLocalPort;
	strRemoteHostList = QStringList(orig->strRemoteHostList);
	iRemoteHost = orig->iRemoteHost;
	iRemotePort = orig->iRemotePort;
	strUsername = orig->strUsername;
	strPassword = orig->strPassword;
	customActionList = QList<CustomActionStruct>(orig->customActionList);
	portForwardList = QList<PortForwardStruct>(orig->portForwardList);
	strSSHKeyFile = orig->strSSHKeyFile;
	strSSHKeyFilePassword = orig->strSSHKeyFilePassword;
	iDirection = orig->iDirection;
	bAutoConnect = orig->bAutoConnect;
	bCompression = orig->bCompression;
	bDoKeepAlivePing = orig->bDoKeepAlivePing;
	bAutoReconnect = orig->bAutoReconnect;
	iSSH1or2 = orig->iSSH1or2;
	strExtraArguments = orig->strExtraArguments;	
	strPassword = orig->strPassword;
    strChildNodesCommand = orig->strChildNodesCommand;
    bChildNodesCommandType = orig->bChildNodesCommandType;
    strFgColor = orig->strFgColor;
    strBgColor = orig->strBgColor;
    bActivateDisconnects = orig->bActivateDisconnects;
}

bool Tunnel_c::isConnectionDetailsEqual(const Tunnel_c *other)
{
    if(iType != other->iType) {
        return false;
    }
    if(strSSHHostList.size() != other->strSSHHostList.size()) {
        return false;
    }
    for(int i=0;i<strSSHHostList.size();i++) {
        if(strSSHHostList.at(i) != other->strSSHHostList.at(i)) {
            return false;
        }
    }
    if(iSSHHost != other->iSSHHost) {
        return false;
    }
    if(strLocalIP != other->strLocalIP) {
        return false;
    }
    /*
     * DO NOT CHECK THIS. IT IS DYNAMIC.
    if(iLocalPort != other->iLocalPort) {
        return false;
    }
    */
    if(strRemoteHostList.size() != other->strRemoteHostList.size()) {
        return false;
    }
    for(int i=0;i<strRemoteHostList.size();i++) {
        if(strRemoteHostList.at(i) != other->strRemoteHostList.at(i)) {
            return false;
        }
    }
    if(iRemoteHost != other->iRemoteHost) {
        return false;
    }
    if(iRemotePort != other->iRemotePort) {
        return false;
    }
    if(strUsername != other->strUsername) {
        return false;
    }
    if(strPassword != other->strPassword) {
        return false;
    }
    if(portForwardList.size() != other->portForwardList.size()) {
        return false;
    }
    for(int i=0;i<portForwardList.size();i++) {
        if(portForwardList.at(i) != other->portForwardList.at(i)) {
            return false;
        }

    }
    if(strSSHKeyFile != other->strSSHKeyFile) {
        return false;
    }
    if(strSSHKeyFilePassword != other->strSSHKeyFilePassword) {
        return false;
    }
    if(iDirection != other->iDirection) {
        return false;
    }
    if(iSSH1or2 != other->iSSH1or2) {
        return false;
    }
    if(strExtraArguments != other->strExtraArguments) {
        return false;
    }
    if(strPassword != other->strPassword) {
        return false;
    }
    return true;
}


void Tunnel_c::setSSHHostList( const QString& sshHostList )
{
	QString selectedHost = getSelectedSSHHost();
    strSSHHostList = sshHostList.split("\n", Qt::SkipEmptyParts);
	setSelectedSSHHost(selectedHost);
}

QString Tunnel_c::getSSHHostList( )
{
	return strSSHHostList.join("\n");
}

QString Tunnel_c::getSelectedSSHHost()
{
	QString host = "";
	if(iSSHHost >= 0 && iSSHHost < strSSHHostList.size()) {
		host = strSSHHostList.at(iSSHHost);
	} else if(strSSHHostList.size() > 0) {
		host = strSSHHostList.at(0);
	}
	return host;
}

	
void Tunnel_c::setSelectedSSHHost( const QString& sshHost )
{
	iSSHHost = strSSHHostList.indexOf(sshHost);
	if(iSSHHost == -1) {
		iSSHHost = 0;
	}
}


void Tunnel_c::setSelectedSSHHost( int sshHostListIndex )
{
	if(sshHostListIndex >= 0 && sshHostListIndex < strSSHHostList.size()) {
		iSSHHost = sshHostListIndex;
	} else {
		iSSHHost = 0;
	}
}



void Tunnel_c::setRemoteHostList( const QString& sshHostList )
{
	QString selectedHost = getSelectedRemoteHost();
    strRemoteHostList = sshHostList.split("\n", Qt::SkipEmptyParts);
	setSelectedRemoteHost(selectedHost);
}

QString Tunnel_c::getRemoteHostList( )
{
	return strRemoteHostList.join("\n");
}

QString Tunnel_c::getSelectedRemoteHost()
{
	QString host = "";
	if(iRemoteHost >= 0 && iRemoteHost < strRemoteHostList.size()) {
		host = strRemoteHostList.at(iRemoteHost);
	} else if(strRemoteHostList.size() > 0) {
		host = strRemoteHostList.at(0);
	}
	return host;
}

	
void Tunnel_c::setSelectedRemoteHost( const QString& sshHost )
{
	iRemoteHost = strRemoteHostList.indexOf(sshHost);
	if(iRemoteHost == -1) {
		iRemoteHost = 0;
	}
}


void Tunnel_c::setSelectedRemoteHost( int sshHostListIndex )
{
	if(sshHostListIndex >= 0 && sshHostListIndex < strRemoteHostList.size()) {
		iRemoteHost = sshHostListIndex;
	} else {
		iRemoteHost = 0;
	}
}

// ATTunnelConnector_c

ATTunnelConnector_c::ATTunnelConnector_c( ATSkeletonWindow *pParent, QTreeWidgetItem *pTreeTunnelsItem ) : QObject(),
m_pParent( pParent ),
m_pTreeTunnelsItem( pTreeTunnelsItem )
{
	ATASSERT( m_pParent );
	ATASSERT( m_pTreeTunnelsItem );
}

void ATTunnelConnector_c::slotProcessReadStandardOutput()
{
    qDebug() << Q_FUNC_INFO;
	ATASSERT( m_pParent );
	ATASSERT( m_pTreeTunnelsItem );

	Tunnel_c *pt = ATSkeletonWindow::getTunnel(m_pTreeTunnelsItem);
	ATASSERT( pt );

	QByteArray b = pt->pProcess->readAllStandardOutput();

    QString s = QString(b);
    QStringList lines = s.remove('\r').split('\n', Qt::SkipEmptyParts);
    for(int i=0;i<lines.size();i++) {
        QString line = lines.at(i);
        m_pParent->AddToLog( *pt, QString("1> %1").arg(line) );
        processPlinkOutput( line );
    }
}

void ATTunnelConnector_c::slotProcessReadStandardError()
{
    qDebug() << Q_FUNC_INFO;
	ATASSERT( m_pParent );
	ATASSERT( m_pTreeTunnelsItem );

	Tunnel_c *pt = ATSkeletonWindow::getTunnel(m_pTreeTunnelsItem);
	ATASSERT( pt );

	QByteArray b = pt->pProcess->readAllStandardError();

    QString s = QString(b);
    QStringList lines = s.remove('\r').split('\n', Qt::SkipEmptyParts);
    for(int i=0;i<lines.size();i++) {
        QString line = lines.at(i);
        m_pParent->AddToLog( *pt, QString("2> %1").arg(line) );
        processPlinkOutput( line );
    }
}

void ATTunnelConnector_c::slotProcessError(QProcess::ProcessError error)
{
    qDebug() << Q_FUNC_INFO << " processError: " << error;
	ATASSERT( m_pParent );
	ATASSERT( m_pTreeTunnelsItem );

	Tunnel_c *pt = ATSkeletonWindow::getTunnel(m_pTreeTunnelsItem);
	ATASSERT( pt );

    m_pParent->AddToLog( *pt, QString("Process error: %1\n").arg(error) );
	
	emit finished( pt );
}

void ATTunnelConnector_c::slotProcessFinished(int exitCode, QProcess::ExitStatus /*exitStatus*/)
{
    qDebug() << Q_FUNC_INFO << " exitCode: " << exitCode;
	ATASSERT( m_pParent );
	ATASSERT( m_pTreeTunnelsItem );

	Tunnel_c *pt = ATSkeletonWindow::getTunnel(m_pTreeTunnelsItem);
	ATASSERT( pt );

    m_pParent->AddToLog( *pt, QString("Process exit: %1\n").arg(exitCode) );

	emit finished( pt );
}

void ATTunnelConnector_c::processPlinkOutput( QString &str )
{
    qDebug() << Q_FUNC_INFO;

	if ( !m_pParent ) return;
	if ( !m_pTreeTunnelsItem ) return;

    QString strTest = QString(str).replace("\r","").replace("\n","").trimmed();

    //v0.9.50 workaround. This was added after the Password: prompt for some reason. In a normal plink session, it is printed on the line before the password prompt.
    //plink in the console:
    //
    //-- Keyboard-interactive authentication prompts from server: ------------------
    //| t505327@TRA-SSH's password:
    //
    //this string:
    //
    //| t505327@TRA-SSH's password: -- Keyboard-interactive authentication prompts from server: ------------------
    //
    strTest = strTest.replace("-- Keyboard-interactive authentication prompts from server: ------------------","").trimmed();

    if ( strTest.contains( "Login as:", Qt::CaseInsensitive) )
	{
		//Since we send the arg: -l username to plink.exe we know
		//that we need to ask for a username here
		Tunnel_c *pt = m_pParent->getTunnel( m_pTreeTunnelsItem );
		if( pt && pt->pProcess )
		{
			bool ok;
			QString strInput = QInputDialog::getText(m_pParent, pt->strName,
                                                  str, QLineEdit::Normal,
												  "", &ok);
			if(pt->pProcess != NULL) {
				if(ok) {
                    m_pParent->AddToLog( *pt, QString("Sent username: %1\n").arg( strInput ) );
					pt->pProcess->write( qPrintable( strInput + "\r\n" ) );
				} else {
					emit finished( pt );
				}
			}
		}
	}
    else if ( strTest.contains( "Password", Qt::CaseInsensitive ) && strTest.endsWith(":"))
	{
		Tunnel_c *pt = m_pParent->getTunnel( m_pTreeTunnelsItem );
		if( pt && pt->pProcess)
		{
			if(!pt->strPassword.trimmed().isEmpty())
			{
				//User stored password
				if( pt->iPasswordRetries <= PASSWORD_RETRIES )
				{
					QString strPassword = m_pParent->replaceVars( *pt, pt->strPassword );
					QString strStarPassword(strPassword);
					if( m_pParent->m_pMainWindow->preferences()->hidePasswordsInLogEnabled() ) {
						if(strStarPassword.length() > 0) strStarPassword = "*****";
					}
					if(pt->pProcess) {
                        m_pParent->AddToLog( *pt, QString("Sent password: %1\n").arg( strStarPassword ) );
						pt->pProcess->write( qPrintable( strPassword + "\r\n" ) );
						pt->iPasswordRetries++;
					}
				}
				else
				{
					if(pt->pProcess != NULL) {
						emit finished( pt );
					}
				}
			}
			else 
			{
				//User did not store password
				bool ok;
				QString strPassword = QInputDialog::getText(m_pParent, pt->strName,
                                                      str, QLineEdit::Password,
													  "", &ok);
				if(pt->pProcess != NULL) {
					if(ok) {
						QString strStarPassword(strPassword);
						if( m_pParent->m_pMainWindow->preferences()->hidePasswordsInLogEnabled() ) {
							if(strStarPassword.length() > 0) strStarPassword = "*****";
						}
                        m_pParent->AddToLog( *pt, QString("Sent password: %1\n").arg( strStarPassword ) );
						pt->pProcess->write( qPrintable( strPassword + "\r\n" ) );
					} else {
						emit finished( pt );
					}
				}
			}
		}
	}
    else if ( strTest.contains( "Passphrase for key", Qt::CaseInsensitive ) )
	{
		//Passphrase for key "rsa-key-20100514-password-protected": 
		Tunnel_c *pt = m_pParent->getTunnel( m_pTreeTunnelsItem );
		if( pt && pt->pProcess )
		{
			if(!pt->strSSHKeyFilePassword.trimmed().isEmpty())
			{
				//User stored keyfile password
				if( pt->iKeyPasswordRetries <= KEY_PASSWORD_RETRIES )
				{
					QString strPassword = m_pParent->replaceVars( *pt, pt->strSSHKeyFilePassword );
					QString strStarPassword(strPassword);
					if( m_pParent->m_pMainWindow->preferences()->hidePasswordsInLogEnabled() ) {
						if(strStarPassword.length() > 0) strStarPassword = "*****";
					}
					if(pt->pProcess) {
                        m_pParent->AddToLog( *pt, QString("Sent password: %1\n").arg( strStarPassword ) );
						pt->pProcess->write( qPrintable(strPassword + "\r\n") );
						pt->iKeyPasswordRetries++;
					}
				}
				else
				{
					if(pt->pProcess != NULL) {
						emit finished( pt );
					}
				}
			}
			else
			{
				//User did not store keyfile password
				bool ok;
				QString strPassword = QInputDialog::getText(m_pParent, pt->strName,
                                                      str, QLineEdit::Password,
													  "", &ok);
				if(pt->pProcess != NULL) {
					if(ok) {
						QString strStarPassword(strPassword);
						if(m_pParent->m_pMainWindow->preferences()->hidePasswordsInLogEnabled() ) {
							if(strStarPassword.length() > 0) strStarPassword = "*****";
						}
                        m_pParent->AddToLog( *pt, QString("Sent password: %1\n").arg( strStarPassword ) );
						pt->pProcess->write( qPrintable(strPassword + "\r\n") );
					} else {
						emit finished( pt );
					}
				}
			}
		}
	}
    else if ( strTest.contains( "Store key in cache", Qt::CaseInsensitive ) )
    {
        //Store key in cache? (y/n)
        if ( m_pTreeTunnelsItem )
        {
            Tunnel_c *pt = ATSkeletonWindow::getTunnel(m_pTreeTunnelsItem);
            if(pt && pt->pProcess) {
                m_pParent->AddToLog( *pt, "Sent: n\n" );
                pt->pProcess->write( "n\n" );
            }
        }
    }
    else if ( strTest.contains( "Update cached key", Qt::CaseInsensitive ) )
	{
		//Update cached key? (y/n, Return cancels connection)
		if ( m_pTreeTunnelsItem )
		{
			Tunnel_c *pt = ATSkeletonWindow::getTunnel(m_pTreeTunnelsItem);
			if(pt && pt->pProcess) {
                m_pParent->AddToLog( *pt, "Sent: y\n" );
				pt->pProcess->write( "y\n" );
			}
		}
	}
    else if ( strTest.contains( "Continue connecting (yes/no)", Qt::CaseInsensitive ) )
    {
        //The authenticity of host 'ssh-z2.nsc.no (148.123.15.231)' can't be established.
        //DSA key fingerprint is 68:f1:ab:e0:75:23:d2:30:3f:7e:25:25:5f:ad:c7:c7.
        //Are you sure you want to continue connecting (yes/no)?
        if ( m_pTreeTunnelsItem )
        {
            Tunnel_c *pt = ATSkeletonWindow::getTunnel(m_pTreeTunnelsItem);
            if(pt && pt->pProcess) {
                m_pParent->AddToLog( *pt, "Sent: yes\n" );
                pt->pProcess->write( "yes\n" );
            }
        }
    }
    else if ( strTest.contains( "challenge", Qt::CaseInsensitive ) && strTest.contains( "response", Qt::CaseInsensitive ) && strTest.endsWith(":"))
	{
		//enter challenge response
		if ( m_pTreeTunnelsItem )
		{
			Tunnel_c *pt = ATSkeletonWindow::getTunnel(m_pTreeTunnelsItem);
			if(pt && pt->pProcess) 
			{
				bool ok;
				QString strPassword = QInputDialog::getText(m_pParent, pt->strName,
                                                      str, QLineEdit::Password,
													  "", &ok);
				if(pt->pProcess != NULL) {
					if(ok)
					{
						QString strStarPassword(strPassword);
						if( m_pParent->m_pMainWindow->preferences()->hidePasswordsInLogEnabled() ) {
							if(strStarPassword.length() > 0) strStarPassword = "*****";
						}
                        m_pParent->AddToLog( *pt, QString("Sent challenge password: %1\n").arg( strStarPassword ) );
						pt->pProcess->write( qPrintable(strPassword + "\r\n") );
					}
					else
					{
						emit finished( pt );
					}
				}
			}
		}
	}
    else if ( strTest.contains( "Verification code:", Qt::CaseInsensitive ) )
    {
        //enter challenge response
        if ( m_pTreeTunnelsItem )
        {
            Tunnel_c *pt = ATSkeletonWindow::getTunnel(m_pTreeTunnelsItem);
            if(pt && pt->pProcess)
            {
                bool ok;
                QString strPassword = QInputDialog::getText(m_pParent, pt->strName,
                                                      str, QLineEdit::Password,
                                                      "", &ok);
                if(pt->pProcess != NULL) {
                    if(ok)
                    {
                        QString strStarPassword(strPassword);
                        if( m_pParent->m_pMainWindow->preferences()->hidePasswordsInLogEnabled() ) {
                            if(strStarPassword.length() > 0) strStarPassword = "*****";
                        }
                        m_pParent->AddToLog( *pt, QString("Sent verification code: %1\n").arg( strStarPassword ) );
                        pt->pProcess->write( qPrintable(strPassword + "\r\n") );
                    }
                    else
                    {
                        emit finished( pt );
                    }
                }
            }
        }
    }
    else if ( strTest.contains( "Enter personal code" , Qt::CaseInsensitive ))
	{
		//enter personal code
		if ( m_pTreeTunnelsItem )
		{
			Tunnel_c *pt = ATSkeletonWindow::getTunnel(m_pTreeTunnelsItem);
			if(pt && pt->pProcess)
			{
				bool ok;
				QString strPassword = QInputDialog::getText(m_pParent, pt->strName,
                                                      str, QLineEdit::Password,
													  "", &ok);
				if(pt->pProcess != NULL) {
					if(ok)
					{
						QString strStarPassword(strPassword);
						if( m_pParent->m_pMainWindow->preferences()->hidePasswordsInLogEnabled() ) {
							if(strStarPassword.length() > 0) strStarPassword = "*****";
						}
                        m_pParent->AddToLog( *pt, QString("Sent personal code: %1\n").arg( strStarPassword ) );
						pt->pProcess->write( qPrintable(strPassword + "\r\n") );
					}
					else
					{
						emit finished( pt );
					}
				}
			}
		}
	}
    else if ( strTest.contains( "Access granted", Qt::CaseInsensitive ) )
	{
		//Access granted
		if ( m_pTreeTunnelsItem )
		{
			emit signalConnected( m_pTreeTunnelsItem );
		}
	}
    else if ( strTest.contains( "Authentication succeeded", Qt::CaseInsensitive ) )
	{
		//Authentication succeeded
		if ( m_pTreeTunnelsItem )
		{
			emit signalConnected( m_pTreeTunnelsItem );
		}
	}
    else if ( strTest.contains( "Access denied", Qt::CaseInsensitive ) )
	{
		//Access denied
		if ( m_pTreeTunnelsItem )
		{
			
			
		}
	}
    else if( strTest.contains( "Address already in use", Qt::CaseInsensitive )) {
        //Forwarding port already in use
        if ( m_pTreeTunnelsItem )
        {
            emit signalKillConnection( m_pTreeTunnelsItem );
        }
    }
    else if( strTest.contains( "PrivateKey failed", Qt::CaseInsensitive )) {
        //Could not read private key
        if ( m_pTreeTunnelsItem )
        {
            emit signalKillConnection( m_pTreeTunnelsItem );
        }
    }
}



// ATPopulateChildNodesConnector_c

ATPopulateChildNodesConnector_c::ATPopulateChildNodesConnector_c( ATSkeletonWindow *pParent, QTreeWidgetItem *pTreeTunnelsItem ) : QObject(),
m_pParent( pParent ),
m_pTreeTunnelsItem( pTreeTunnelsItem )
{
    qDebug() << Q_FUNC_INFO;
    ATASSERT( m_pParent );
    ATASSERT( m_pTreeTunnelsItem );
    m_strOutput = "";
    m_nExitCode = -1;
    m_bCancelled = false;
}

ATPopulateChildNodesConnector_c::~ATPopulateChildNodesConnector_c()
{
    qDebug() << Q_FUNC_INFO;
}



void ATPopulateChildNodesConnector_c::slotProcessReadStandardOutput()
{
    qDebug() << Q_FUNC_INFO;
    ATASSERT( m_pParent );
    ATASSERT( m_pTreeTunnelsItem );

    Tunnel_c *pt = ATSkeletonWindow::getTunnel(m_pTreeTunnelsItem);
    ATASSERT( pt );

    ATASSERT(pt->pPopulateChildNodesProcess);

    QByteArray b = pt->pPopulateChildNodesProcess->readAllStandardOutput();

    QString s = QString(b);

    m_pParent->AddToLog( *pt, s );
    processCommandOutput( b );
}

void ATPopulateChildNodesConnector_c::slotProcessReadStandardError()
{
    qDebug() << Q_FUNC_INFO;
    ATASSERT( m_pParent );
    ATASSERT( m_pTreeTunnelsItem );

    Tunnel_c *pt = ATSkeletonWindow::getTunnel(m_pTreeTunnelsItem);
    ATASSERT( pt );

    ATASSERT(pt->pPopulateChildNodesProcess);

    QByteArray b = pt->pPopulateChildNodesProcess->readAllStandardError();

    QString s = QString(b);

    m_pParent->AddToLog( *pt, s );
    processCommandOutput( b );
}

void ATPopulateChildNodesConnector_c::slotProcessError(QProcess::ProcessError error)
{
    qDebug() << Q_FUNC_INFO << " processError: " << error;
    ATASSERT( m_pParent );
    ATASSERT( m_pTreeTunnelsItem );

    Tunnel_c *pt = ATSkeletonWindow::getTunnel(m_pTreeTunnelsItem);
    ATASSERT( pt );

    m_pParent->AddToLog( *pt, QString("> Process error: %1\n").arg(error) );
}

void ATPopulateChildNodesConnector_c::slotProcessFinished(int exitCode, QProcess::ExitStatus /*exitStatus*/)
{
    qDebug() << Q_FUNC_INFO << " exitCode: " << exitCode;
    ATASSERT( m_pParent );
    ATASSERT( m_pTreeTunnelsItem );

    Tunnel_c *pt = ATSkeletonWindow::getTunnel(m_pTreeTunnelsItem);
    ATASSERT( pt );

    m_pParent->AddToLog( *pt, QString("> Process exit: %1\n").arg(exitCode) );

    m_nExitCode = exitCode;

    emit finished( pt );
}

void ATPopulateChildNodesConnector_c::slotCancel()
{
    qDebug() << Q_FUNC_INFO;

    Tunnel_c *pt = ATSkeletonWindow::getTunnel(m_pTreeTunnelsItem);
    ATASSERT( pt );

    pt->pPopulateChildNodesProcess->kill();
    m_bCancelled = true;

}



void ATPopulateChildNodesConnector_c::processCommandOutput( const QByteArray &b )
{
    qDebug() << Q_FUNC_INFO;

    if ( !m_pParent ) return;
    if ( !m_pTreeTunnelsItem ) return;

    QString str = QString::fromUtf8(b.data());
    m_strOutput += str;
}




