#include "ImportHostsDialog.h"
#include "ATSkeleton.h"
#include "Backup.h"
#include "Images.h"

ImportHostsDialog::ImportHostsDialog(QWidget *parent, ATSkeletonWindow *pMainWindow)
: QDialog(parent)
{
	setupUi(this);
	m_pMainWindow = pMainWindow;

	m_pTwiImportAt = NULL;
	m_nImportMode = IMPORT_AFTER;
	m_sFilePath = "";
	setDefaultDirectoryPath();	//..../My Documents
	setDefaultFileName();		//DOffenSSHTunnel_Export.ini

	treeWidget->setupCheckControls();
	treeWidget->setCheckStateInheritanceEnabled(true);

	QPushButton *buttonImport = buttonBox->button(QDialogButtonBox::Ok);
	ATASSERT(buttonImport); //Detect when someone remove the button using the Designer (then this code must change)
	if(buttonImport != NULL) {
		buttonImport->setText("Import");
	}

    ATVERIFY( connect(chkAutoCheck, &QAbstractButton::toggled, this, &ImportHostsDialog::slotAutoCheckToggled ) );
    ATVERIFY( connect(buttonBox, &QDialogButtonBox::accepted,  this, &ImportHostsDialog::slotImport ) );
    ATVERIFY( connect(buttonBox, &QDialogButtonBox::rejected,  this, &ImportHostsDialog::reject ) );
}

ImportHostsDialog::~ImportHostsDialog()
{
	m_pMainWindow = NULL;
	reset();
}

void ImportHostsDialog::reset()
{
    QList<QTreeWidgetItem*> items = treeWidget->findItems(".*", Qt::MatchFlags(Qt::MatchRegularExpression | Qt::MatchRecursive), 0);
	for(int i=0;i<items.size();i++) {
		Tunnel_c* pt = ATSkeletonWindow::getTunnel(items[i]);
		delete pt;
	}
	treeWidget->clear();
}


void ImportHostsDialog::showEvent(QShowEvent * event)
{
	qDebug("ImportHostsDialog::showEvent()");
	QDialog::showEvent(event);
	if(m_pos.x() != 0 && m_pos.y() != 0) {
		move(m_pos);
	}
}

void ImportHostsDialog::hideEvent(QHideEvent * event) 
{
	qDebug("ImportHostsDialog::hideEvent()");
	m_pos = pos();
	QDialog::hideEvent(event);
}

void ImportHostsDialog::setDefaultDirectoryPath(QString directoryPath/*=QString()*/)
{
	if(directoryPath.trimmed().isEmpty()) {
        QStringList locations = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
        if(locations.size() > 0) {
            m_sDefaultDirectoryPath = locations.at(0);
        }
	} else {
		m_sDefaultDirectoryPath = directoryPath;
		if(!QFile::exists(m_sDefaultDirectoryPath)) {
            QStringList locations = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
            if(locations.size() > 0) {
                m_sDefaultDirectoryPath = locations.at(0);
            }
		}
	}
}

void ImportHostsDialog::setDefaultFileName(QString fileName/*=QString()*/)
{
	if(fileName.trimmed().isEmpty()) {
		m_sDefaultFileName = "DoffenSSHTunnel_Export.ini";
	} else {
		m_sDefaultFileName = fileName;
	}
}

QString ImportHostsDialog::fileDialog()
{
	QFileInfo defaultFileInfo(QDir(m_sDefaultDirectoryPath),m_sDefaultFileName);

	QSettings settings(g_strAppIniFile, QSettings::IniFormat);
	QString sLastSavedFilePath = settings.value("last_host_import_path",
								defaultFileInfo.absoluteFilePath()).toString();

	// .../his dir/DOffenSSHTunnel_Export.ini
	QFileInfo fileInfo(sLastSavedFilePath);
	fileInfo = QFileInfo(fileInfo.absolutePath(),m_sDefaultFileName); 

	if(!QFile::exists(fileInfo.absolutePath())) {
		//The dir does not exist
		fileInfo = defaultFileInfo; 
	}

	QString fileNamePathSelected = QFileDialog::getOpenFileName(this, tr("Select file"),
           fileInfo.absoluteFilePath(), QString("%1 (*.ini)").arg(APP_NICE_NAME));

	return fileNamePathSelected;
}

void ImportHostsDialog::show(QString filePath, QTreeWidgetItem *twiImportAt, int importMode)
{
	reset();

	m_pTwiImportAt = twiImportAt;
	m_sFilePath = QFileInfo(filePath).absoluteFilePath();
	m_nImportMode = importMode;

	if(importMode == IMPORT_CHILD) {
		setWindowTitle("Import and insert as children of selected host");
	} else if(importMode == IMPORT_AFTER) {
		setWindowTitle("Import and insert after selected host");
	}

	QSettings settings( filePath, QSettings::IniFormat);
	
	m_listUserDefinedVariables.clear();
	m_listUserDefinedVariables.append( ATSkeletonWindow::readSettingsUserDefinedVariables(settings) );
	
	m_listExecutableVariables.clear();
	m_listExecutableVariables.append( ATSkeletonWindow::readSettingsExecutableVariables(settings) );
	
	

	QTreeWidgetItem* lastTwi = NULL;
	int lastLevel = 0;
	QStringList keys = settings.childGroups();
	for(int i=0; i<keys.count(); i++) {
		QString key = keys.at(i);
		if(key.indexOf("Tunnel") != -1 || key.indexOf("Host") != -1) { 

			settings.beginGroup( key );

			Tunnel_c* tunnel = ATSkeletonWindow::readSettingsHost(settings);

			settings.endGroup();

			if(lastTwi != NULL && tunnel->iLevel > 0) {
				if(tunnel->iLevel > lastLevel) {
					tunnel->twi = new QTreeWidgetItem( lastTwi );
				} else if(tunnel->iLevel == lastLevel) {
					QTreeWidgetItem* parentTwi = lastTwi->parent();
					if(parentTwi != NULL) {
						tunnel->twi = new QTreeWidgetItem( parentTwi );
					} else {
						tunnel->twi = new QTreeWidgetItem( treeWidget );
					}
				} else {
					QTreeWidgetItem* parentTwi = lastTwi->parent();
					while(parentTwi != NULL && (tunnel->iLevel <= ATSkeletonWindow::getTunnel(parentTwi)->iLevel)) {
						parentTwi = parentTwi->parent();
					}
					if(parentTwi != NULL) {
						tunnel->twi = new QTreeWidgetItem( parentTwi );
					} else {
						tunnel->twi = new QTreeWidgetItem( treeWidget );
					}
				}
			} else {
				tunnel->twi = new QTreeWidgetItem( treeWidget );
			}

			tunnel->twi->setExpanded(true);

			if(tunnel->iType == TUNNEL_TYPE_TUNNEL)	{
				tunnel->twi->setIcon( 0, Images::icon("ht_120_host_disconnected") );
			} else if(tunnel->iType == TUNNEL_TYPE_FOLDER) {
				if(tunnel->twi->isExpanded()) {
					tunnel->twi->setIcon( 0, Images::icon("ht_150_folder_open_disconnected") );
				} else {
					tunnel->twi->setIcon( 0, Images::icon("ht_170_folder_closed_disconnected") );
				}
			}

			tunnel->twi->setFlags(tunnel->twi->flags() | Qt::ItemIsUserCheckable);
			tunnel->twi->setCheckState(0,Qt::Checked);
			tunnel->twi->setText( 0, tunnel->strName );
			ATSkeletonWindow::setTunnel(tunnel->twi, tunnel);
			lastTwi = tunnel->twi;
			lastLevel = tunnel->iLevel;
				
		} // for each -- Tunnel
	}	

	if(treeWidget->topLevelItemCount() > 0) {
		QTreeWidgetItem *twi = treeWidget->topLevelItem( 0 );
		treeWidget->setCurrentItem( twi );
	}

	chkAutoCheck->setChecked(true);

	QDialog::show();
}


//private
QList<Tunnel_c*> ImportHostsDialog::getCheckedTunnels()
{
    QList<QTreeWidgetItem*> treeTunnelItems = treeWidget->findItems(".*", Qt::MatchFlags(Qt::MatchRegularExpression | Qt::MatchRecursive), 0);
	QList<Tunnel_c*> tunnelList;
	for(int i=0;i<treeTunnelItems.size();i++)
	{
		QTreeWidgetItem *treeTunnelItem = treeTunnelItems[i];
		Qt::CheckState checkState = treeTunnelItem->checkState(0);
		if(checkState == Qt::Checked || checkState == Qt::PartiallyChecked) {
			Tunnel_c *it = ATSkeletonWindow::getTunnel(treeTunnelItem);
			ATASSERT(it);
			//adjust level in case user de-selected automatically check parents and children
			QTreeWidgetItem *twiParent = treeTunnelItem->parent();
			while(twiParent != NULL) {
				if(twiParent->checkState(0) == Qt::Unchecked) {
					it->iLevel = it->iLevel - 1;
				}
				twiParent = twiParent->parent();
			}
			tunnelList.append(it);
		}
	}
	return tunnelList;
}

//private
void ImportHostsDialog::slotAutoCheckToggled(bool checked)
{
	treeWidget->setCheckStateInheritanceEnabled(checked);
}

//private
void ImportHostsDialog::slotImport()
{
	//Tunnels
	QList<Tunnel_c*> tunnelList = getCheckedTunnels();

	if(tunnelList.size() == 0) {
		QMessageBox::warning(this, "Oops!", "Nothing selected !\n");
		return;
	}

	//Find variables used in the tunnels
	QList<VariableStruct> varList;
	varList.append( ATSkeletonWindow::getVariablesUsedInTunnels(tunnelList,m_listExecutableVariables) );
	varList.append( ATSkeletonWindow::getVariablesUsedInTunnels(tunnelList,m_listUserDefinedVariables) );

	//...and find other variables that use them
	for(int i=0;i<varList.size();i++) {
		VariableStruct var = varList.at(i);
		ATSkeletonWindow::resolveVariable(var, varList, m_listExecutableVariables);	//var.strName used in m_listExecutableVariables ?
		ATSkeletonWindow::resolveVariable(var, varList, m_listUserDefinedVariables);  //var.strName used in m_listUserDefinedVariables ?
	}

	m_pMainWindow->backupSettings(BackupInfo(BACKUP_BEFORE_IMPORT_HOSTS,"Before import file [%1]",m_sFilePath) );

	//Import tunnels
	m_pMainWindow->importTunnels(tunnelList, m_pTwiImportAt, m_nImportMode);

	//Import variables
	m_pMainWindow->importVariables(varList);

	QSettings settings(g_strAppIniFile, QSettings::IniFormat);
	settings.setValue("last_host_import_path",m_sFilePath);
	settings.sync();
	accept();

	m_pMainWindow->writeSettings();
}
