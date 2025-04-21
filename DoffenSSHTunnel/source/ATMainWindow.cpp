#include "Images.h"
#include "Paths.h"
#include "pch.h"
#include "ATMainWindow.h"
#include "ATNamedAction.h"
#include "ATSkeleton.h"
#include "Backup.h"
#include "ChangePasswordDialog.h"
#include "RestoreDialog.h"
#include "VariableSettingsDialog.h"
#include "PreferencesDialog.h"
#include "PasswordDb.h"
#include "exceptions.h"
#include "searchwidget.h"

#include <QtWidgets/QWidget>
#include <QtGui/QCloseEvent>
#include <QtCore/QString>
#include <QtCore/QObject>
#include <QtWidgets/QMenu>
#include <QtCore/QEvent>
#include <QtWidgets/QToolBar>

#ifdef Q_OS_MACOS
#include <QtMac>
#endif

#define MIN_TRIGGER_TIMER (200)
#define EZ_HOTKEY (10)

ATMainWindow_c::ATMainWindow_c( QWidget *vpParent ):
QMainWindow( vpParent )
{
	Paths::setAppPath( qApp->applicationDirPath() );

	m_bDoDetectSaveTunnel = true;
    m_pIconsMenu = NULL;

	m_pSkeletonWindow = new ATSkeletonWindow(this);
	setCentralWidget( m_pSkeletonWindow );

	restoreDialog = new RestoreDialog(this);
    ATVERIFY( connect( restoreDialog, &RestoreDialog::accepted, this, &ATMainWindow_c::slotRestoreDialogAccepted ) );

	variableSettingsDialog = new VariableSettingsDialog(m_pSkeletonWindow);
    ATVERIFY( connect( variableSettingsDialog, &VariableSettingsDialog::accepted, this, &ATMainWindow_c::slotVariableSettingsDialogAccepted ) );

	preferencesDialog = new PreferencesDialog(this, m_pSkeletonWindow);
    ATVERIFY( connect( preferencesDialog, &PreferencesDialog::accepted, this, &ATMainWindow_c::slotPreferencesDialogAccepted ) );

    setWindowTitle( QString( "%1[*] - %2 %3 " ).arg( g_strWindowTitle ).arg( APP_NICE_NAME ).arg( APP_VERSION ) );

	InitMenusAndActions();

    readWindowSettings();

	slotChangeStyle( m_strStyle );

	slotChangeIcons( m_strIconTheme );

	setWindowIcon( Images::icon("app_020_disconnected") );

	//File Menu->Enable editing: save state to my m_bEnableEditing
    ATVERIFY( connect( m_pActionEnableEditing, &QAction::toggled, this, &ATMainWindow_c::slotEnableEditingToggled ) );
    ATVERIFY( connect( m_pSkeletonWindow->ui.btnTabEditMaximize, &QToolButton::clicked, this, &ATMainWindow_c::slotTabEditHideButton ) );

    ATVERIFY( connect( m_pSkeletonWindow, &ATSkeletonWindow::signalSetTrayIcon, this, &ATMainWindow_c::slotSetTrayIcon ) );

    m_pSkeletonWindow->ui.widgetTreeTunnelSearch_2->setVisible( preferencesDialog->showTreeSearchEnabled() );

    qApp->installEventFilter(this);
}

ATMainWindow_c::~ATMainWindow_c()
{
    //preferences()->writeSettings(); //call in
    //writeWindowSettings();
}

void ATMainWindow_c::slotChangeIcons( QString theme )
{
	m_strIconTheme = theme;

	if(m_pIconsMenu != NULL) {
		QList<QAction*> actionList = m_pIconsMenu->actions();
		for(int i=0;i<actionList.size();i++) {
			ATIconThemeAction *ta = qobject_cast<ATIconThemeAction*>( actionList.at(i) );
			if(ta!=NULL) {
				ta->setChecked(false);
				if(ta->theme() == theme) {
					ta->setChecked(true);
				}
			}
		}
	}

	Images::setIconTheme(theme);
	slotLoadIcons();
}

void ATMainWindow_c::slotLoadIcons()
{
	QApplication::setOverrideCursor(Qt::WaitCursor);
	QPixmapCache::clear();
	m_pSkeletonWindow->slotLoadIcons();
	variableSettingsDialog->slotLoadIcons();
    m_pSearchWidget->slotLoadIcons();
    m_pSearchWidget->slotUpdateCompleterIcons();
	QApplication::restoreOverrideCursor();
}

void ATMainWindow_c::keyPressEvent(QKeyEvent *e)
{
    qDebug() << "ATMainWindow_c::keyPressEvent() key: " << e->key();
    switch (e->key()) {
        case Qt::Key_F:
            if(e->modifiers() == Qt::ControlModifier) {
                e->ignore();
                m_pSearchWidget->focus();
                m_pSearchWidget->showCompleter();
            }
            break;
        case Qt::Key_K:
            if(e->modifiers() == Qt::ControlModifier) {
                e->ignore();
                m_pSkeletonWindow->ui.widgetTreeTunnelSearch_2->slotShow();
            }
            break;
        default:
            break;
    }
}

bool ATMainWindow_c::eventFilter( QObject *o, QEvent *e )   
{
	if (e->type() == QEvent::KeyPress || e->type() == QEvent::MouseButtonPress
		|| e->type() == QEvent::FocusOut || e->type() == QEvent::FocusIn) {
		if(o == style()
			|| o == this
			|| o == m_pSkeletonWindow
            || o->objectName() == "ATMainWindow_cWindow"
			|| o == m_pSkeletonWindow->ui.splitter
			|| o->objectName() == "qt_splithandle_"
			|| o == m_pSkeletonWindow->ui.tabWidget
			|| o == m_pSkeletonWindow->ui.tabEdit
			|| o->objectName() == "qt_tabwidget_stackedwidget"
            || o->objectName() == "QComboBoxPrivateContainerClassWindow"
			|| o == m_pSkeletonWindow->ui.widgetEditTunnel 
            || m_pSkeletonWindow->ui.widgetEditTunnel->isChildWidget(o)
            || o == m_pSkeletonWindow->ui.widgetEditFolder
            || m_pSkeletonWindow->ui.widgetEditFolder->isChildWidget(o)
            || o == m_pSkeletonWindow->ui.stackedWidgetEdit
            || o->objectName() == "QMenuClassWindow" //Click in the My Name menu
            || o->objectName() == "QColorDialogClassWindow" //The My Name color picker dialog
            || o->objectName() == "QMessageBoxClassWindow" //The confirm delete tunnel dialog in more tunnels table
        )
		{
			//noop
//            qDebug() << "***************************** SKIP: " << e->type() << " : " << o->objectName();
		} else {
//            qDebug() << "*****************************: " << e->type() << " : " << o->objectName();
//            while(o->parent() != NULL) {
//                qDebug() << "           parent: " << o->parent()->objectName();
//                o = o->parent();
//            }
//            for(int i=0;i<o->children().size();i++) {
//                qDebug() << "            child: " << o->children().at(i)->objectName();
//            }
			if(m_bDoDetectSaveTunnel) {
				if(isWindowModified()) {
                    m_pSkeletonWindow->confirmSaveEditPane();
                    m_pSkeletonWindow->delayUpdateUIFromTWI();
				}
            }
		}
	}

	return false;
}


bool ATMainWindow_c::InitMenusAndActions()
{
	//-------------
	// FILE MENU

	QMenu *pFileMenu	= new QMenu(this);

	//File -> &Enable Editing
	m_pActionEnableEditing = new QAction(QObject::tr("&Enable Editing"), this);
	m_pActionEnableEditing->setCheckable(true);
	m_pActionEnableEditing->setChecked(false);
	m_pActionEnableEditing->setShortcut(QObject::tr("CTRL+E"));

	pFileMenu->addAction( m_pActionEnableEditing );

	pFileMenu->addSeparator();

	//File -> Change Master &Password...
	m_pActionChangeMasterPassword = new QAction(QObject::tr("Change Master &Password..."), this);
	bool bRet = QObject::connect(m_pActionChangeMasterPassword, SIGNAL(triggered()), this, SLOT(slotChangeMasterPassword()));
	ATASSERT( bRet );

	pFileMenu->addAction( m_pActionChangeMasterPassword );

	pFileMenu->addSeparator();

	//File -> &Backup now...
	m_pActionBackupTunnelSettings = new QAction(QObject::tr("&Backup now..."), this);
	m_pActionBackupTunnelSettings->setShortcut(QObject::tr("CTRL+B"));
	bRet = QObject::connect(m_pActionBackupTunnelSettings, SIGNAL(triggered()), m_pSkeletonWindow, SLOT(slotManualBackupSelected()));
	ATASSERT( bRet );

	pFileMenu->addAction( m_pActionBackupTunnelSettings );

	//File -> &Restore a backup...
	m_pActionRestoreTunnelSettings = new QAction(QObject::tr("&Restore a backup..."), this);
	m_pActionRestoreTunnelSettings->setShortcut(QObject::tr("CTRL+R"));
	bRet = QObject::connect(m_pActionRestoreTunnelSettings, SIGNAL(triggered()), this, SLOT(slotRestore()));
	ATASSERT( bRet );

	pFileMenu->addAction( m_pActionRestoreTunnelSettings );

	pFileMenu->addSeparator();

	//File -> E&xport
	m_pMenuExportTunnels = new QMenu(QObject::tr("E&xport"), this);
	pFileMenu->addAction( m_pMenuExportTunnels->menuAction() );

		//File -> E&xport -> Export &Selected...
		m_pActionExportSelectedTunnels = new QAction(QObject::tr("Export &Selected..."), this);
		bRet = QObject::connect(m_pActionExportSelectedTunnels, SIGNAL(triggered()), m_pSkeletonWindow, SLOT(slotTreeTunnelExportCurrent()));
		ATASSERT( bRet );
		m_pMenuExportTunnels->addAction( m_pActionExportSelectedTunnels );

		//File -> E&xport -> Export &All...
		m_pActionExportAllTunnels = new QAction(QObject::tr("Export &All..."), this);
		bRet = QObject::connect(m_pActionExportAllTunnels, SIGNAL(triggered()), m_pSkeletonWindow, SLOT(slotTreeTunnelExportAll()));
		ATASSERT( bRet );
		m_pMenuExportTunnels->addAction( m_pActionExportAllTunnels );

	//File -> I&mport
	m_pMenuImportTunnels = new QMenu(QObject::tr("I&mport"), this);
	pFileMenu->addAction( m_pMenuImportTunnels->menuAction() );

		//File -> I&mport -> Import After Selected Folder/Host...
		m_pActionImportTunnelsAfter = new QAction(QObject::tr("Import After Selected Folder/Host..."), this);
		bRet = QObject::connect(m_pActionImportTunnelsAfter, SIGNAL(triggered()), m_pSkeletonWindow, SLOT(slotTreeTunnelImportAfter()));
		ATASSERT( bRet );
		m_pMenuImportTunnels->addAction( m_pActionImportTunnelsAfter );

		//File -> I&mport -> Import as Children of Selected Folder/Host...
		m_pActionImportTunnelsChild = new QAction(QObject::tr("Import as Children of Selected Folder/Host..."), this);
		bRet = QObject::connect(m_pActionImportTunnelsChild, SIGNAL(triggered()), m_pSkeletonWindow, SLOT(slotTreeTunnelImportChild()));
		ATASSERT( bRet );
		m_pMenuImportTunnels->addAction( m_pActionImportTunnelsChild );

	pFileMenu->addSeparator();

	QAction *pAction;

	//File -> &Quit
	pAction				= new QAction(QObject::tr("&Quit"), this);
	pAction->setShortcut(QObject::tr("CTRL+Q"));
	bRet			= QObject::connect(pAction, SIGNAL(triggered()), this, SLOT(slotTentativeQuit()));
	ATASSERT( bRet );

	pFileMenu->addAction( pAction );

	//-------------
	// TOOLS MENU

	QMenu *pToolsMenu	= new QMenu(this);

	//Tools -> &Disconnect all
	pAction				= new QAction(QObject::tr("&Disconnect all"), this);
	pAction->setShortcut(QObject::tr("CTRL+D"));
	bRet				= QObject::connect(pAction, SIGNAL(triggered()), m_pSkeletonWindow, SLOT(slotDisconnectAllTunnels()), Qt::QueuedConnection);
	ATASSERT( bRet );

	pToolsMenu->addAction( pAction );

	//Tools -> &Clear all logs
	pAction				= new QAction(QObject::tr("&Clear all logs"), this);
	bRet				= QObject::connect(pAction, SIGNAL(triggered()), this, SLOT(slotClearAllTunnelLogs()),Qt::QueuedConnection);
	ATASSERT( bRet );

	pToolsMenu->addAction( pAction );

	pToolsMenu->addSeparator();

	//Tools -> &View configured local ports...
	pAction				= new QAction(QObject::tr("&View configured local ports..."), this);
	bRet				= QObject::connect(pAction, SIGNAL(triggered()), this, SLOT(slotShowConfiguredLocalPorts()),Qt::QueuedConnection);
	ATASSERT( bRet );

	pToolsMenu->addAction( pAction );

	//-------------
	// SETTINGS MENU

	QMenu *pSettingsMenu	= new QMenu(this);

	//Settings -> &Preferences...
	pAction				= new QAction(QObject::tr("&Preferences..."), this);
	bRet				= QObject::connect(pAction, SIGNAL(triggered()), this, SLOT(slotPreferencesSetup()), Qt::QueuedConnection);
	ATASSERT( bRet );

	pSettingsMenu->addAction( pAction );

	pSettingsMenu->addSeparator();

	//Settings -> Setup &Variables...
	m_pActionSetupVariables	= new QAction(QObject::tr("Passwords and &Variables..."), this);
	bRet				= QObject::connect(m_pActionSetupVariables, SIGNAL(triggered()), this, SLOT(slotVariablesSetup()),Qt::QueuedConnection );
	ATASSERT( bRet );

	pSettingsMenu->addAction( m_pActionSetupVariables );

	pSettingsMenu->addSeparator();

	//Settings -> Style -> ...

	QMenu *pStyleMenu		= new QMenu("&Style", this);
	QStringList styleList	= QStyleFactory::keys();

	for( int i=0; i<styleList.count(); i++ )
	{
		ATNamedAction *pNamedAction = new ATNamedAction( styleList.at(i), this );
		ATASSERT( connect( pNamedAction, SIGNAL( signalTriggerNamed(QString) ), this, SLOT(slotChangeStyle(QString)),Qt::QueuedConnection ));
		pStyleMenu->addAction(pNamedAction);
	}

	pSettingsMenu->addAction( pStyleMenu->menuAction() );

	//Settings -> Icons -> ...
	QStringList iconThemes = Images::iconThemes();
	if( iconThemes.count() > 0 ) {
		m_pIconsMenu		= new QMenu("&Icons", this);

		//Reload icons
		pAction	= new QAction(QObject::tr("&Reload"), this);
		bRet				= QObject::connect(pAction, SIGNAL(triggered()), this, SLOT(slotLoadIcons()),Qt::QueuedConnection );
		ATASSERT( bRet );
		m_pIconsMenu->addAction( pAction );
		m_pIconsMenu->addSeparator();

		ATIconThemeAction *pIconThemeAction = new ATIconThemeAction( "Default (built-in)", ICON_THEME_DEFAULT, this );
		pIconThemeAction->setCheckable(true);
		ATASSERT( connect( pIconThemeAction, SIGNAL( signalTriggerTheme(QString) ), this, SLOT(slotChangeIcons(QString)),Qt::QueuedConnection ));
		m_pIconsMenu->addAction(pIconThemeAction);

		for( int i=0; i<iconThemes.count(); i++ )
		{
			QString theme = iconThemes.at(i);
			QString themeName = theme.trimmed();
			if(theme == ICON_THEME_CUSTOM) themeName = "Custom";
            themeName = themeName.replace("_"," ");
			pIconThemeAction = new ATIconThemeAction( themeName, theme, this ); 
			pIconThemeAction->setCheckable(true);
			ATASSERT( connect( pIconThemeAction, SIGNAL( signalTriggerTheme(QString) ), this, SLOT(slotChangeIcons(QString)),Qt::QueuedConnection ));
			m_pIconsMenu->addAction(pIconThemeAction);
		}

		pSettingsMenu->addAction( m_pIconsMenu->menuAction() );
	}

	//-------------
	// HELP MENU

	QMenu *pHelpMenu	= new QMenu(this);
	pAction				= new QAction(QObject::tr("&About..."), this);
	ATASSERT( QObject::connect(pAction, SIGNAL(triggered()), this, SLOT(slotShowAbout()),Qt::QueuedConnection) );
	pHelpMenu->addAction(pAction);

	menuBar()->addMenu(pFileMenu)->setText(QObject::tr("&File"));
	menuBar()->addMenu(pToolsMenu)->setText(QObject::tr("&Tools"));
	menuBar()->addMenu(pSettingsMenu)->setText(QObject::tr("&Settings"));
	menuBar()->addMenu(pHelpMenu)->setText(QObject::tr("&Help"));

    //-------------
    // TOOLBAR

    QToolBar *pToolBar  = new QToolBar(this);

    m_pSearchWidget = new SearchWidget(pToolBar);
    m_pSearchWidget->setup(m_pSkeletonWindow);

    pToolBar->addWidget(m_pSearchWidget);
    addToolBar(pToolBar);

	return true;
}

/**
 * Read from DoffenSSHTunnelApp.ini. Called by constructor.
 */
void ATMainWindow_c::readWindowSettings()
{
    qDebug( "%s", Q_FUNC_INFO );

	QSettings settings( g_strAppIniFile, QSettings::IniFormat);

	//+++ General

	QPoint pos = settings.value( "pos", QPoint(0, 0) ).toPoint();
    QSize size = settings.value( "size", QSize(964, 648) ).toSize();
	int bMax = settings.value( "maximized", 0 ).toInt();
	m_strStyle = settings.value( "qtstyle", "" ).toString();
	m_strIconTheme = settings.value( "icons", ICON_THEME_CUSTOM ).toString();

	bool bEnableEditing = settings.value( "enabled_editing", true ).toBool();
	m_pActionEnableEditing->setChecked(bEnableEditing);
	slotEnableEditingToggled(bEnableEditing);

	//--- General
	
	if ( pos.x() && pos.y() ) move(pos);
	if ( !size.isEmpty() ) resize(size);
	if ( bMax ) showMaximized();

	QByteArray splitterPos = settings.value( "splitterPos", QByteArray() ).toByteArray();
	m_pSkeletonWindow->ui.splitter->restoreState(splitterPos);

	QByteArray splitterPosConnect = settings.value( "splitterPosConnect", QByteArray() ).toByteArray();
	m_pSkeletonWindow->ui.splitterConnect->restoreState(splitterPosConnect);

	QByteArray actionsEditHeaderState = settings.value( "actions_edit_header_state", QByteArray() ).toByteArray();
	m_pSkeletonWindow->ui.treeCustomActionsEdit->header()->restoreState(actionsEditHeaderState);
}

void ATMainWindow_c::writeWindowSettings()
{
    qDebug( "%s", Q_FUNC_INFO );

	int bMax = isMaximized();

	QSettings settings( g_strAppIniFile, QSettings::IniFormat);
	if ( !bMax )
	{
		settings.setValue( "pos", pos() );
		settings.setValue( "size", size() );
	}
	
	settings.setValue( "Version", APP_VERSION );
	settings.setValue( "splitterPos", m_pSkeletonWindow->ui.splitter->saveState() );
	settings.setValue( "splitterPosConnect", m_pSkeletonWindow->ui.splitterConnect->saveState() );
	settings.setValue( "maximized", bMax );
	settings.setValue( "qtstyle", m_strStyle );
	settings.setValue( "icons", m_strIconTheme );
	settings.setValue( "enabled_editing", m_pActionEnableEditing->isChecked() );
	settings.setValue( "actions_edit_header_state", m_pSkeletonWindow->ui.treeCustomActionsEdit->header()->saveState() );

	settings.sync();
}

void ATMainWindow_c::writeSettingsExecutableVariables()
{
    QSettings settings( g_strAppIniFile, QSettings::IniFormat);
    //Executable variables
    m_pSkeletonWindow->writeSettingsExecutableVariables(settings, m_pSkeletonWindow->m_listExecutableVariables);
    settings.sync();
}

void ATMainWindow_c::slotSetTrayIcon( int iIndex )
{
    //Cannot change the mac dock icon. The icon is set in the Info.plist file
#ifdef __APPLE__
    QtMac::setBadgeLabelText(iIndex ? "ssh": "");
#else
    setWindowIcon( iIndex ? Images::icon("app_010_connected") : Images::icon("app_020_disconnected") );
#endif
}


void ATMainWindow_c::slotChangeStyle( QString strStyle )
{
    qDebug( Q_FUNC_INFO );

	m_strStyle = strStyle;

	if ( !m_strStyle.isEmpty() )
	{
		QApplication::setPalette( QPalette() );
		QApplication::setStyle( strStyle );
	}
}


void ATMainWindow_c::closeEvent( QCloseEvent * event )
{
	if ( m_pSkeletonWindow->onClose() )
		event->accept();
	else
		event->ignore();
}

void ATMainWindow_c::slotTentativeQuit()
{
	if ( m_pSkeletonWindow->onClose() )
	{
		qApp->quit();
	}
}

void ATMainWindow_c::slotShowAbout()
{
	QString strTitle = QString( "About %1" ).arg( APP_NICE_NAME );

	QString strAbout;
	
	strAbout += QString( "%1 %2 - Freeware\n" ).arg( APP_NICE_NAME ).arg( APP_VERSION );
    strAbout += "http://sourceforge.net/p/doffensshtunnel\n";
    strAbout += "2019 Lars-Flemming Clausen <lars.flemming.clausen@gmail.com>\n";
	strAbout += "Qt " QT_VERSION_STR;
    strAbout += "\n";
    strAbout += QString("Data location: %1").arg( g_strDataDirectoryPath );

	showNormal();
	QMessageBox::about( this, strTitle, strAbout );
}

void ATMainWindow_c::slotChangeMasterPassword()
{
	ChangePasswordDialog dlg(this);
	bool rejected = (dlg.exec()!=QDialog::Accepted);
	if(rejected) {
		return;
	}

    try
    {
        PasswordDb *pdb = PasswordDb::getInstance();
        pdb->saveFile(dlg.password);
    }
    catch(std::exception &ex) {
        QMessageBox::critical(this, "Oops!", QString::fromStdString(ex.what()));
    }
}

void ATMainWindow_c::slotShowConfiguredLocalPorts()
{
    QMap<int,QMap<QString,QVariant> > portMap;
    QList<QTreeWidgetItem*> treeTunnelItems = m_pSkeletonWindow->ui.treeTunnels->findItems(".*", Qt::MatchFlags(Qt::MatchRegularExpression | Qt::MatchRecursive), 0);
	for(int i=0;i<treeTunnelItems.size();i++) {
		Tunnel_c* pt = m_pSkeletonWindow->getTunnel(treeTunnelItems[i]);
		if( pt && pt->iType == 0) {
			if(pt->iLocalPort != 0) {
				if(portMap.contains(pt->iLocalPort)) {
					QMap<QString,QVariant> portMapValue = portMap.value(pt->iLocalPort);
					portMapValue["COUNT"] = QVariant(portMapValue["COUNT"].toInt() + 1);
					portMap[pt->iLocalPort] = portMapValue;
				} else {
					QMap<QString,QVariant> portMapValue;
					portMapValue["COUNT"] = QVariant(1);
					portMap.insert(pt->iLocalPort,portMapValue);
				}
			}
			for(int j=0;j<pt->portForwardList.size();j++) {
				PortForwardStruct pfs = pt->portForwardList.at(j);
				if(pfs.nLocalPort != 0) {
					if(portMap.contains(pfs.nLocalPort)) {
						QMap<QString,QVariant> portMapValue = portMap.value(pfs.nLocalPort);
						portMapValue["COUNT"] = QVariant(portMapValue["COUNT"].toInt() + 1);
						portMap[pfs.nLocalPort] = portMapValue;
					} else {
						QMap<QString,QVariant> portMapValue;
						portMapValue["COUNT"] = QVariant(1);
						portMap.insert(pfs.nLocalPort,portMapValue);
					}
				}
			}
		}
	}

	QList<int> portList = portMap.keys();
    std::sort(portList.begin(), portList.end());

	QDialog dlg;

	dlg.setWindowTitle( "Configured local ports" );
	dlg.setWindowIcon(windowIcon());

	QTableWidget *pTable = new QTableWidget( &dlg );
	pTable->setColumnCount(2);
	pTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	QStringList labels;
    labels << tr("Local Port") << tr("Count");
    pTable->setHorizontalHeaderLabels(labels);
    //pTable->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
    pTable->verticalHeader()->hide();
    pTable->setShowGrid(false);

	for(int i=0;i<portList.size();i++) {
		int iLocalPort = portList.at(i);
		QMap<QString,QVariant> portMapValue = portMap.value(iLocalPort);
		QVariant qCount = portMapValue.value("COUNT");
		
		QTableWidgetItem *localPortItem = new QTableWidgetItem();
		localPortItem->setData(Qt::DisplayRole, QVariant(iLocalPort));
		localPortItem->setFlags(localPortItem->flags() ^ Qt::ItemIsEditable);
		localPortItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		
		QTableWidgetItem *countItem = new QTableWidgetItem();
		countItem->setData(Qt::DisplayRole, qCount);
		countItem->setFlags(countItem->flags() ^ Qt::ItemIsEditable);
		countItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
		
		int row = pTable->rowCount();
        pTable->insertRow(row);
        pTable->setItem(row, 0, localPortItem);
        pTable->setItem(row, 1, countItem);
	}

	pTable->setSortingEnabled(true);
	pTable->sortByColumn(0,Qt::AscendingOrder);
	pTable->horizontalHeader()->setStretchLastSection(true);
	pTable->resizeColumnToContents(0);
	pTable->resizeRowsToContents();

	QPushButton *pButton = new QPushButton( &dlg );
	pButton->setText( "Close" );
	pButton->setDefault( true );
	ATVERIFY( connect( pButton, SIGNAL( clicked() ), &dlg, SLOT( accept() ) ) );

	QVBoxLayout *vbox = new QVBoxLayout;
	vbox->addWidget( pTable );
	vbox->addWidget( pButton );
	dlg.setLayout( vbox );
	dlg.exec();
}


void ATMainWindow_c::slotClearAllTunnelLogs()
{
    QList<QTreeWidgetItem*> treeTunnelItems = m_pSkeletonWindow->ui.treeTunnels->findItems(".*", Qt::MatchFlags(Qt::MatchRegularExpression | Qt::MatchRecursive), 0);
	for(int i=0;i<treeTunnelItems.size();i++) {
		QTreeWidgetItem* twi = treeTunnelItems[i];
		m_pSkeletonWindow->clearNodeLog(twi);
	}
}

void ATMainWindow_c::slotEnableEditingToggled(bool checked)
{
	m_pActionBackupTunnelSettings->setEnabled(checked);
	m_pActionRestoreTunnelSettings->setEnabled(checked);
	m_pMenuExportTunnels->setEnabled(checked);
	m_pMenuImportTunnels->setEnabled(checked);
	m_pActionChangeMasterPassword->setEnabled(checked);
	preferencesDialog->enableEditingToggled(checked);
	variableSettingsDialog->enableEditingToggled(checked);
	m_pSkeletonWindow->slotEnableEditingToggled(checked);
}

void ATMainWindow_c::slotPreferencesSetup()
{
	preferencesDialog->setModal(true);
	preferencesDialog->show();
}

void ATMainWindow_c::slotPreferencesDialogAccepted()
{
    if(preferencesDialog->showTreeSearchEnabled()) {
        if(!m_pSkeletonWindow->ui.widgetTreeTunnelSearch_2->isVisible()) {
            m_pSkeletonWindow->ui.widgetTreeTunnelSearch_2->show();
        }
    } else {
        if(m_pSkeletonWindow->ui.widgetTreeTunnelSearch_2->isVisible()) {
            m_pSkeletonWindow->ui.widgetTreeTunnelSearch_2->hide();
        }
    }
}

void ATMainWindow_c::slotVariablesSetup()
{
	//create dialog
	variableSettingsDialog->setModal(true);
	variableSettingsDialog->setVariableStatistics(m_pSkeletonWindow->getVariableStatistics());
	variableSettingsDialog->setPasswordVariables(m_pSkeletonWindow->getPasswordVariables());
	variableSettingsDialog->setExecutableVariables(m_pSkeletonWindow->m_listExecutableVariables);
	variableSettingsDialog->setUserDefinedVariables(m_pSkeletonWindow->m_listUserDefinedVariables);
	variableSettingsDialog->show();
}

void ATMainWindow_c::slotVariableSettingsDialogAccepted()
{
    qDebug("%s",Q_FUNC_INFO);


	QList<VariableStruct> varListAdded;							//Detect that are added -> backup
	QList<VariableStruct> varListDeleted;						//Detect that variables are deleted -> backup
	QList<Identifier>	  varListPasswordAdded;					//Passwords that are addded
	QList<Identifier>	  varListPasswordDeleted;				//Passwords that are deleted
	QList<Identifier>	  varListPasswordModified;				//Passwords that are modified
    QList<QPair<VariableStruct,VariableStruct> > varListModifed;	//QPair.first = original, QPair.second = modified

	//////////////////////////////////
	/// Password Variables. New, Modified, Deleted.

    PasswordDb *pdb = PasswordDb::getInstance();

    //If user edited passwords he had to proved the master password
    //We need this to be able to save password changes below
    QString masterPassword = variableSettingsDialog->getMasterPassword();
    variableSettingsDialog->clearMasterPassword(); //Don't let it lay around in mem


	//Add executable variable names that changed to this list
	//QPair.first = original name
	//QPair.second = new name
    QList<QPair<QString,QString> > varListPasswordsThatChangedName;

	//QPair.first = The original VariableStruct passed to setPasswordVariables(..)
	// IF NEW var then QPair.first.uUid.isNull() == true
	//QPair.second = VariableStruct with edited values (from the dialog)
    QList<QPair<VariableStruct,VariableStruct> > varListPasswords = variableSettingsDialog->getPasswordVariables();

	try {
		//Find deleted Identifier's
        for(unsigned int i = 0; i < pdb->size(); ++i) {
			bool found = false;
            Identifier ident = pdb->at(i);
			for(int j=0;j<varListPasswords.size();j++) {
				VariableStruct varFind = varListPasswords.at(j).first;
				if(ident.getUuid() == varFind.uUid) {
					found = true;
					break;
				}
			}
			if(found == false) {
				varListPasswordDeleted.append(ident);
			}
		}

		//.... and remove them
		for(int i=0;i<varListPasswordDeleted.size();i++) {
			Identifier ident = varListPasswordDeleted.at(i);
            pdb->removeIdentifier(ident.getUuid());
		}

		//UPDATE OUR PASSWORDS DB _AND_ FIND the ones that changed name
		for(int i=0;i<varListPasswords.size();i++) {
			VariableStruct origVarStruct = varListPasswords.at(i).first;
			VariableStruct newVarStruct = varListPasswords.at(i).second;
			if(origVarStruct.uUid.isNull() == false) {
				//existing identifier
				if(origVarStruct != newVarStruct) {
					//modified something
					Identifier ident;
					ident.setUuid(newVarStruct.uUid);
					ident.setDescription(newVarStruct.strDescription);
					ident.setLogin(newVarStruct.strName);
					ident.setPassword(newVarStruct.strValue);
					varListPasswordModified.append(ident);
                    pdb->removeIdentifier(ident.getUuid());
                    pdb->addIdentifier(ident);
				}
				if(origVarStruct.strDescription != newVarStruct.strDescription) {
					//modified name of the existing identifier
					if(!origVarStruct.strDescription.isEmpty() && !newVarStruct.strDescription.isEmpty()) {
						varListPasswordsThatChangedName.append( QPair<QString,QString>(origVarStruct.strDescription+".uid",newVarStruct.strDescription+".uid") );
						varListPasswordsThatChangedName.append( QPair<QString,QString>(origVarStruct.strDescription+".pwd",newVarStruct.strDescription+".pwd") );
					}
				}

			} //end if - existing identifier
			else {
				//a new identifier
				Identifier ident;
				ident.setUuid(newVarStruct.uUid);
				ident.setDescription(newVarStruct.strDescription);
				ident.setLogin(newVarStruct.strName);
				ident.setPassword(newVarStruct.strValue);
				varListPasswordAdded.append(ident);
                pdb->addIdentifier(ident);
			}
		}
		
		qDebug("%d identifiers changed name",varListPasswordsThatChangedName.size() / 2);
	}
	catch(std::exception &ex) {
		QMessageBox::critical(this, "Oops!", QString::fromStdString(ex.what()));
    }

	//////////////////////////////////
	/// Executable Variables. New, Modified, Deleted.

	//Add executable variable names that changed to this list
	//QPair.first = original name
	//QPair.second = new name
    QList<QPair<QString,QString> > varListExecutablesThatChangedName;

	//QPair.first = The original VariableStruct passed to setExecutableVariables(..)
	// IF NEW var then QPair.first.uUid.isNull() == true
	//QPair.second = VariableStruct with edited values (from the dialog)
    QList<QPair<VariableStruct,VariableStruct> > varListExecutables = variableSettingsDialog->getExecutableVariables();

	//Find delete variables
	for(int i=0;i<m_pSkeletonWindow->m_listExecutableVariables.size();i++) {
		bool found = false;
		VariableStruct var = m_pSkeletonWindow->m_listExecutableVariables.at(i);
		for(int j=0;j<varListExecutables.size();j++) {
			VariableStruct varFind = varListExecutables.at(j).first;
			if(var.uUid == varFind.uUid) {
				found = true;
				break;
			}
		}
		if(found == false) {
			varListDeleted.append(var);
		}
	}

	//Variables are taken from this list when we execute a custom command
	m_pSkeletonWindow->m_listExecutableVariables.clear();

	//UPDATE OUR m_pSkeletonWindow->m_listExecutableVariables _AND_ FIND the EXECUTABLE variables that changed name
	for(int i=0;i<varListExecutables.size();i++) {
		VariableStruct origVarStruct = varListExecutables.at(i).first;
		//existing variable
		VariableStruct newVarStruct = varListExecutables.at(i).second;
		m_pSkeletonWindow->m_listExecutableVariables.append(newVarStruct); //Variables are taken from this list when we execute a custom command
		if(origVarStruct.uUid.isNull() == false) {
			if(origVarStruct != newVarStruct) {
				varListModifed.append( QPair<VariableStruct,VariableStruct>(origVarStruct,newVarStruct) );
			}
			if(origVarStruct.strName != newVarStruct.strName) {
				//modified name of the existing variable
				varListExecutablesThatChangedName.append( QPair<QString,QString>(origVarStruct.strName,newVarStruct.strName) );
			}
		} //end if - existing variable
		else {
			varListAdded.append(newVarStruct);
		}
	}
	
	qDebug("%d executable variables changed name",varListExecutablesThatChangedName.size());


	//////////////////////////////////
	/// User Defined Variables. New, Modified, Deleted.

	//Add other variable names that changed to this list
	//QPair.first = original name
	//QPair.second = new name
    QList<QPair<QString,QString> > varListUserThatChangedName;

    QList<QPair<VariableStruct,VariableStruct> > varListUser = variableSettingsDialog->getUserDefinedVariables();

	//Find delete variables
	for(int i=0;i<m_pSkeletonWindow->m_listUserDefinedVariables.size();i++) {
		bool found = false;
		VariableStruct var = m_pSkeletonWindow->m_listUserDefinedVariables.at(i);
		for(int j=0;j<varListUser.size();j++) {
			VariableStruct varFind = varListUser.at(j).first;
			if(var.uUid == varFind.uUid) {
				found = true;
				break;
			}
		}
		if(found == false) {
			varListDeleted.append(var);
		}
	}

	//Variables are taken from this list when we execute a custom command
	m_pSkeletonWindow->m_listUserDefinedVariables.clear();

	//QPair.first = The original VariableStruct passed to setUserVariables(..)
	// IF NEW var then QPair.first.uUid.isNull() == true
	//QPair.second = VariableStruct with edited values (from the dialog)
	for(int i=0;i<varListUser.size();i++) {
		VariableStruct origVarStruct = varListUser.at(i).first;
		VariableStruct newVarStruct = varListUser.at(i).second;
		m_pSkeletonWindow->m_listUserDefinedVariables.append(newVarStruct); //Variables are taken from this list when we run plink and execute a custom command
		if(origVarStruct.uUid.isNull() == false) {
			//existing variable
			if(origVarStruct != newVarStruct) {
				varListModifed.append( QPair<VariableStruct,VariableStruct>(origVarStruct,newVarStruct) );
			}
			if(origVarStruct.strName != newVarStruct.strName) {
				//modified name of the existing variable
				varListUserThatChangedName.append( QPair<QString,QString>(origVarStruct.strName,newVarStruct.strName) );
			}
		} //end if - existing variable
		else {
			varListAdded.append(newVarStruct);
		}
	}

	qDebug("%d user defined variables changed name",varListUserThatChangedName.size());

	//////////////////////////////////
	// Handle variable name change

	// This list contains all the variables
    QList<QPair<QString,QString> > varListAllThatChangedName;
	varListAllThatChangedName.append(varListExecutablesThatChangedName);
	varListAllThatChangedName.append(varListUserThatChangedName);
	qDebug("a total of %d variables/identifiers changed name",varListAllThatChangedName.size() + varListPasswordsThatChangedName.size() / 2);
	varListAllThatChangedName.append(varListPasswordsThatChangedName); //LEAVE IT HERE
	
	if(varListAllThatChangedName.size() > 0) {
		// Update tunnel variables, DoffenSSHTunnel.ini AND GUI
		m_pSkeletonWindow->changeTunnelVarNames(varListAllThatChangedName); //Also UPDATES the GUI
	} 

    //////////////////////////////////
    // Reset the dialog

    variableSettingsDialog->clearAllData();

	//////////////////////////////////
	// Save changes

	qDebug("SUMMARY");
	qDebug("%d identifiers changed",varListPasswordModified.size());
	qDebug("%d identifiers added",varListPasswordAdded.size());
	qDebug("%d identifiers deleted",varListPasswordDeleted.size());
	qDebug("%d variables changed",varListModifed.size());
	qDebug("%d variables added",varListAdded.size());
	qDebug("%d variables deleted",varListDeleted.size());

	if(varListPasswordModified.size() > 0 || varListPasswordAdded.size() > 0 || varListPasswordDeleted.size() > 0) {
		
		// SAVE to DoffenSSHTunnel.pwd
		try {
            if(masterPassword.isEmpty()) {
                //something wrong, this should not be possible
                //since user must provide master password before
                //being allowed to edit
                throw ExBad("You edited passwords but did not get promptet for the master password. This should not happen. Please make sure to click the button 'click me to enable editing' below the password list.");
            }

            pdb->saveFile(masterPassword);

			// UPDATE USER/PASS COMBO BOX's IN EDIT TAB
			bool blocked1 = m_pSkeletonWindow->ui.comboPasswordSelect->blockSignals(true);
			bool blocked2 = m_pSkeletonWindow->ui.comboKeyPasswordSelect->blockSignals(true);
			m_pSkeletonWindow->ui.comboPasswordSelect->clear();
			m_pSkeletonWindow->ui.comboPasswordSelect->addItem("None");
			m_pSkeletonWindow->ui.comboKeyPasswordSelect->clear();
			m_pSkeletonWindow->ui.comboKeyPasswordSelect->addItem("None");
            for(unsigned int i = 0; i < pdb->size(); ++i) {
                Identifier ident = pdb->at(i);
				QString pwdName = ident.getDescription();
				m_pSkeletonWindow->ui.comboPasswordSelect->addItem(pwdName);
				m_pSkeletonWindow->ui.comboKeyPasswordSelect->addItem(pwdName);
			}	
			m_pSkeletonWindow->ui.comboPasswordSelect->blockSignals(blocked1);
			m_pSkeletonWindow->ui.comboKeyPasswordSelect->blockSignals(blocked2);
		}
		catch(std::exception &ex) {
			QMessageBox::critical(this, "Oops! Could not save passwords", QString::fromStdString(ex.what()));
		}
	}

	if(varListAdded.size() > 0 || varListDeleted.size() > 0 || varListModifed.size() > 0 || varListPasswordsThatChangedName.size() > 0) {

		//autoBackup sees BACKUP_BEFORE_CHANGE_VAR _and_ and then also backs up DoffenSSHTunnelApp.ini
		m_pSkeletonWindow->backupSettings( BackupInfo(BACKUP_BEFORE_CHANGE_VAR,"Before edit variables") );

		//execuable variables are stored in the app settings file DoffenSSHTunnelApp.ini
        writeSettingsExecutableVariables(); //Save m_pSkeletonWindow->m_listExecutableVariables

		//Save UserDefined variables to DoffenSSHTunnel.ini
		//Save m_pSkeletonWindow->m_listExecutableVariables and Tunnels
		//that are affected because of a variable name change
		m_pSkeletonWindow->delayWriteSettings();

	}

} 

void ATMainWindow_c::slotRestore()
{
	if(m_pSkeletonWindow->m_cleanBackupFutureWatcher.isRunning()) {
		m_pSkeletonWindow->m_cleanBackupFutureWatcher.cancel();
		m_pSkeletonWindow->m_cleanBackupFutureWatcher.waitForFinished();
	}
	
	//create dialog
	restoreDialog->setWindowIcon(windowIcon());
	restoreDialog->setModal(true);
	restoreDialog->show();
	restoreDialog->loadData();
}

void ATMainWindow_c::slotRestoreDialogAccepted()
{
	qDebug() << "ATMainWindow_c::slotRestoreDialogAccepted()";
	QString selectedBackupInfoFilePath = restoreDialog->getSelectedBackupInfoFilePath();
	qDebug() << QString("Selected file [%1]").arg(selectedBackupInfoFilePath);
	if(selectedBackupInfoFilePath.isEmpty()) {
		qDebug() << "selectedBackupInfoFilePath.isEmpty()";
		return;
	}

	if(!QFile::exists(selectedBackupInfoFilePath)) {
		qDebug() << QString("File not found: %1").arg(selectedBackupInfoFilePath);
		QMessageBox::warning(this, tr("File not found"),selectedBackupInfoFilePath,QMessageBox::Close);
		return;
	}

	bool doRestore = true;

	int iConnectedCount = m_pSkeletonWindow->getConnectionCount();
	if(iConnectedCount > 0) {
		QString strQuestion = QString("Found %1 active connections. They will terminate if you restore. Continue ?").arg(iConnectedCount);
 		QMessageBox::StandardButton iRet = QMessageBox::question( this, APP_NICE_NAME, strQuestion, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes );
		if ( iRet != QMessageBox::Yes ) {
			doRestore = false;
		}
	}

	if(doRestore) {
		m_pSkeletonWindow->restoreBackup(selectedBackupInfoFilePath);
	}
}

void ATMainWindow_c::slotTabEditHideButton()
{
	if(m_pSkeletonWindow->ui.btnTabEditMaximize->isChecked()) {
		m_pSkeletonWindow->ui.groupBoxMainTunnel->hide();
		m_pSkeletonWindow->ui.groupBoxSSHSettings->hide();
		m_pSkeletonWindow->ui.btnTabEditMaximize->setArrowType(Qt::DownArrow);
	} else {
		m_pSkeletonWindow->ui.groupBoxMainTunnel->show();
		m_pSkeletonWindow->ui.groupBoxSSHSettings->show();
		m_pSkeletonWindow->ui.btnTabEditMaximize->setArrowType(Qt::UpArrow);
	}
}






