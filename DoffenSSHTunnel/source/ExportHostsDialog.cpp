#include "ExportHostsDialog.h"

#include "ATSkeleton.h"

ExportHostsDialog::ExportHostsDialog(QWidget *parent, ATSkeletonWindow *pMainWindow)
: QDialog(parent)
{
	setupUi(this);
	m_pMainWindow = pMainWindow;

	setDefaultDirectoryPath();	//..../My Documents
	setDefaultFileName();		//DOffenSSHTunnel_Export.ini

	
	treeWidget->setupCheckControls();
	treeWidget->setCheckStateInheritanceEnabled(true);

	QPushButton *buttonSave = buttonBox->button(QDialogButtonBox::Save);
	ATASSERT(buttonSave); //Detect when someone remove the button using the Designer (then this code must change)
	if(buttonSave != NULL) {
		buttonSave->setText("&Save As...");
	}

	ATVERIFY( connect(chkAutoCheck, SIGNAL(toggled(bool)), this, SLOT(slotAutoCheckToggled(bool) ) ) );
	ATVERIFY( connect(buttonBox, SIGNAL(accepted()), this, SLOT(slotExport() ) ) );
    ATVERIFY( connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject() ) ) );
}

ExportHostsDialog::~ExportHostsDialog()
{
	m_pMainWindow = NULL;
	reset();
}

void ExportHostsDialog::reset()
{
	treeWidget->clear();
}

void ExportHostsDialog::showEvent(QShowEvent * event)
{
	qDebug("ExportHostsDialog::showEvent()");
	QDialog::showEvent(event);
	if(m_pos.x() != 0 && m_pos.y() != 0) {
		move(m_pos);
	}
}

void ExportHostsDialog::hideEvent(QHideEvent * event) 
{
	qDebug("ExportHostsDialog::hideEvent()");
	m_pos = pos();
	QDialog::hideEvent(event);
}

void ExportHostsDialog::setDefaultDirectoryPath(QString directoryPath/*=QString()*/)
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

void ExportHostsDialog::setDefaultFileName(QString fileName/*=QString()*/)
{
	if(fileName.trimmed().isEmpty()) {
		m_sDefaultFileName = "DoffenSSHTunnel_Export.ini";
	} else {
		m_sDefaultFileName = fileName;
	}
}


void ExportHostsDialog::show(QList<QTreeWidgetItem*> twiExportList)
{
	reset();

	//Fill up tree
	for(int i=0;i<twiExportList.size();i++) {
		QTreeWidgetItem *twiExport = twiExportList.at(i);
		Tunnel_c *pt = ATSkeletonWindow::getTunnel(twiExport);
		ATASSERT(pt);
		QTreeWidgetItem *twiClone = twiExport->clone();
		treeWidget->insertTopLevelItem(i,twiClone);
        QList<QTreeWidgetItem*> twiList = treeWidget->findItems(".*", Qt::MatchFlags(Qt::MatchRegularExpression | Qt::MatchRecursive), 0);
		for(int i=0;i<twiList.size();i++) {
			QTreeWidgetItem *twi = twiList.at(i);
			twi->setFlags(twi->flags() | Qt::ItemIsUserCheckable);
			twi->setCheckState(0,Qt::Checked);
			twi->setExpanded(true);
		}
	}	

	if(treeWidget->topLevelItemCount() > 0) {
		QTreeWidgetItem *twi = treeWidget->topLevelItem( 0 );
		treeWidget->setCurrentItem( twi );
	}

	chkAutoCheck->setChecked(true);

	QDialog::show();
}


//private
QString ExportHostsDialog::fileDialog()
{
	QFileInfo defaultFileInfo(QDir(m_sDefaultDirectoryPath),m_sDefaultFileName);

	QSettings settings(g_strAppIniFile, QSettings::IniFormat);
	m_sLastSavedFilePath = settings.value("last_host_export_path",
								defaultFileInfo.absoluteFilePath()).toString();

	// .../his dir/DOffenSSHTunnel_Export.ini
	QFileInfo fileInfo(m_sLastSavedFilePath);
	fileInfo = QFileInfo(fileInfo.absolutePath(),m_sDefaultFileName); 

	if(!QFile::exists(fileInfo.absolutePath())) {
		//The dir does not exist
		fileInfo = defaultFileInfo; 
	}

	QString fileNamePathSelected = QFileDialog::getSaveFileName(this, tr("Select file"),
           fileInfo.absoluteFilePath(), QString("%1 (*.ini)").arg(APP_NICE_NAME));

	return fileNamePathSelected;
}

//private
void ExportHostsDialog::slotAutoCheckToggled(bool checked)
{
	treeWidget->setCheckStateInheritanceEnabled(checked);
}

//private
void ExportHostsDialog::slotExport()
{
	QString filePath = fileDialog();
	if(filePath.isEmpty()) {
		return;
	}
	QFileInfo fileInfo(filePath);
	if(!QFile::exists(fileInfo.absolutePath())) {
		QMessageBox::warning(this, "Oops!", "The directory does not exist !\n"
			+fileInfo.absolutePath());
		return;
	}

	filePath = fileInfo.absoluteFilePath();

    QList<QTreeWidgetItem*> treeTunnelItems = treeWidget->findItems(".*", Qt::MatchFlags(Qt::MatchRegularExpression | Qt::MatchRecursive), 0);

	//Tunnels
	QList<Tunnel_c*> tunnelList;
	for(int i=0;i<treeTunnelItems.size();i++)
	{
		QTreeWidgetItem *treeTunnelItem = treeTunnelItems[i];
		Qt::CheckState checkState = treeTunnelItem->checkState(0);
		if(checkState == Qt::Checked || checkState == Qt::PartiallyChecked) {
			Tunnel_c *it = ATSkeletonWindow::getTunnel(treeTunnelItem);
			ATASSERT(it);
			Tunnel_c *itCopy = new Tunnel_c(it);  //create a copy so we can change iLevel
			//adjust level in case user de-selected automatically check parents and children
			QTreeWidgetItem *twiParent = treeTunnelItem->parent();
			while(twiParent != NULL) {
				if(twiParent->checkState(0) == Qt::Unchecked) {
					itCopy->iLevel = itCopy->iLevel - 1;
				}
				twiParent = twiParent->parent();
			}
			tunnelList.append(itCopy);
		}
	}

	if(tunnelList.size() == 0) {
		QMessageBox::warning(this, "Oops!", "Nothing selected !\n");
		return;
	}

	m_pMainWindow->exportSettingsTunnelList(filePath, tunnelList);

	for(int i=0;i<tunnelList.size();i++) {
		delete tunnelList.at(i); //we created a copy above so we could change iLevel
	}

	m_sLastSavedFilePath = filePath;
	QSettings settings(g_strAppIniFile, QSettings::IniFormat);
	settings.setValue("last_host_export_path",filePath);
	settings.sync();

	accept();
}
