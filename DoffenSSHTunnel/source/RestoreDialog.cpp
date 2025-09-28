#include "RestoreDialog.h"
#include "Backup.h"

TableItem::TableItem(int nBackupType, const QString& backupFileTimestamp, const QString& backupFilePath, const QString& infoFilePath)
{
	m_strBackupFileTimestamp = backupFileTimestamp;
	m_nBackupType = nBackupType;
	m_isInfoLoaded = false;
	m_backupFilePath = backupFilePath;
	m_infoFilePath = infoFilePath;
	m_strDateTime = "";
	m_strDescription = "";
}

TableItem::~TableItem()
{

}

int TableItem::getBackupType()
{
	return m_nBackupType;
}

const QString& TableItem::getBackupFileTimestamp()
{
	return m_strBackupFileTimestamp;

}

const QString& TableItem::getBackupFilePath()
{
	return m_backupFilePath;
}

const QString& TableItem::getInfoFilePath()
{
	return m_infoFilePath;
}

const QString& TableItem::getDateTime() 
{
	if(!m_isInfoLoaded) {
		loadInfo();
	}
	return m_strDateTime;
}

const QString& TableItem::getDescription()
{
	if(!m_isInfoLoaded) {
		loadInfo();
	}
	return m_strDescription;
}

void TableItem::loadInfo()
{
	if(!QFile::exists(m_infoFilePath)) {
        qDebug() << "loadInfo file not found: " << m_infoFilePath;
		return;
	}

	qDebug() << "loadInfo " << m_infoFilePath;

	QSettings ini(m_infoFilePath, QSettings::IniFormat);
	m_strDateTime = BackupInfo::formatDateTime(ini);
	m_strDescription = BackupInfo::formatInfo(ini);
	m_isInfoLoaded = true;
}

TableModel::TableModel(QObject *parent)
     :QAbstractTableModel(parent)
{
	
}

TableModel::~TableModel()
{
	deleteTableData();
}

int TableModel::rowCount(const QModelIndex & parent) const
{
	Q_UNUSED(parent);
	return m_tableData.size();
}

int TableModel::columnCount(const QModelIndex & parent) const
{
	Q_UNUSED(parent);
	return 2;
}

QVariant TableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole || role == Qt::ToolTipRole)
	{
		int rowIndex = index.row();
		int colIndex = index.column();

		if(rowIndex < m_tableData.size()) {
			TableItem *item = m_tableData.at(rowIndex);
			switch(colIndex) {
				case 0: return item->getDateTime();
				case 1: return item->getDescription();
				default: return QVariant();
			}
		} else {
			return QVariant();
		}
	}
	return QVariant();
}

 QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
 {
     if (role == Qt::DisplayRole)
     {
         if (orientation == Qt::Horizontal) {
             switch (section)
             {
             case 0:
                 return QString("Date/Time");
             case 1:
                 return QString("Description");
             default:
                 return QVariant();
             }
         }
     }
     return QVariant();
 }

void TableModel::deleteTableData()
{
    beginResetModel();
	for(int i=0;i<m_tableData.size();i++) {
		delete m_tableData.at(i);
	}
	m_tableData.clear();
    endResetModel();
}

void TableModel::setTableData(const QList<TableItem*> &tableData)
{
    beginResetModel();
    for(int i=0;i<m_tableData.size();i++) {
        delete m_tableData.at(i);
    }
    m_tableData.clear();
	m_tableData.append(tableData);
    endResetModel();
}

TableItem* TableModel::getTableItem(const QModelIndex &modelIndex)
{
	int rowIndex = modelIndex.row();
	if(rowIndex >= 0 && rowIndex < m_tableData.size()) {
		return m_tableData.at(rowIndex);
	}
	return NULL;
}


RestoreDialogSortFilterProxyModel::RestoreDialogSortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
	m_nBackupType = 0;
}


void RestoreDialogSortFilterProxyModel::setBackupType(int nBackupType)
{
	m_nBackupType = nBackupType;
	invalidateFilter();
}

int RestoreDialogSortFilterProxyModel::getBackupType()
{
	return m_nBackupType;
}


bool RestoreDialogSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
	bool accept = true;

	TableModel *tableModel = static_cast<TableModel*>(sourceModel());
	TableItem *tableItem = tableModel->getTableItem(sourceRow);
	if( (tableItem->getBackupType() & m_nBackupType) == 0) {
		accept = false;
	}
	if(accept) {
		accept = QSortFilterProxyModel::filterAcceptsRow(sourceRow, sourceParent);
	}

	return accept;
}

bool RestoreDialogSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
	TableModel *tableModel = static_cast<TableModel*>(sourceModel());
	TableItem *tableItemLeft = tableModel->getTableItem(left);
	TableItem *tableItemRight = tableModel->getTableItem(right);
	bool ret = false;
	if(left.column() == 0) {
		int nCompare = QString::localeAwareCompare(tableItemLeft->getBackupFileTimestamp(), tableItemRight->getBackupFileTimestamp());
		if(nCompare < 0) {
			ret = true;
		}
	} else if(left.column() == 1) {
		int nCompare = QString::localeAwareCompare(tableItemLeft->getDescription(), tableItemRight->getDescription());
		if(nCompare < 0) {
			ret = true;
		}
	}
	return ret;
}


RestoreDialog::RestoreDialog(QWidget *parent)
: QDialog(parent)
{
	setupUi(this);

	m_selectedBackupDateTime = QString();
	m_selectedBackupDescription = QString();
	m_selectedBackupFilePath = QString();
	m_selectedBackupInfoFilePath = QString();

	model = new TableModel(this);
	proxyModel = new RestoreDialogSortFilterProxyModel(this);
	proxyModel->setSourceModel(model);
	proxyModel->setFilterKeyColumn(-1);
	tableView->setModel(proxyModel);

	tableView->horizontalHeader()->resizeSection(0, 150);
    #if QT_VERSION >= 0x050000
        tableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    #else
        tableView->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
    #endif
	//tableView->setAlternatingRowColors(true);
	tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
	tableView->setSortingEnabled(true);
    tableView->verticalHeader()->hide();
    tableView->horizontalHeader()->setMinimumSectionSize(1);
    tableView->verticalHeader()->setMinimumSectionSize(1);

	buttonRestore->setEnabled(false);

	//Buttons
    ATVERIFY( connect( buttonRestore,	&QAbstractButton::clicked,	this, &RestoreDialog::OnOK ) );
    ATVERIFY( connect( buttonCancel,	&QAbstractButton::clicked,	this, &RestoreDialog::OnCancel ) );

	//Dialog
    ATVERIFY( connect( this,			&RestoreDialog::finished,       this, &RestoreDialog::slotFinished ) );
    ATVERIFY( connect( this,			&RestoreDialog::signalLoadData, this, &RestoreDialog::slotLoadData, Qt::QueuedConnection ) );

	//TableView
    connect( tableView->selectionModel(), &QItemSelectionModel::currentChanged, this, &RestoreDialog::slotCurrentChanged );

	//Backup type filter
    ATVERIFY( connect(chkManual,		&QAbstractButton::toggled,	this, &RestoreDialog::slotBackupTypeFilterChanged ) );
    ATVERIFY( connect(chkAuto,			&QAbstractButton::toggled,	this, &RestoreDialog::slotBackupTypeFilterChanged ) );
    ATVERIFY( connect(chkAutoSpes,		&QAbstractButton::toggled,	this, &RestoreDialog::slotBackupTypeFilterChanged ) );
	
	//Text filter
    ATVERIFY( connect(txtFilter,		&QLineEdit::textChanged,	this, &RestoreDialog::slotTextFilterChanged ) );

	//Counter
    ATVERIFY( connect(tableView->verticalHeader(), &QHeaderView::sectionCountChanged, this, &RestoreDialog::slotRowCountChanged ) );

	setWindowTitle("Restore backup");
}

RestoreDialog::~RestoreDialog()
{
}

void RestoreDialog::slotCurrentChanged(QModelIndex current, QModelIndex previous)
{
    Q_UNUSED(previous)
	TableItem *selectedItem = NULL;
	if(current.isValid()) {
		current = proxyModel->mapToSource(current);
		selectedItem = model->getTableItem(current);
		ATASSERT(selectedItem);
	}

	if(selectedItem != NULL) {
		buttonRestore->setEnabled(true);
		m_selectedBackupDateTime = selectedItem->getDateTime();
		m_selectedBackupDescription = selectedItem->getDescription();
		m_selectedBackupFilePath = selectedItem->getBackupFilePath();
		m_selectedBackupInfoFilePath = selectedItem->getInfoFilePath();
	} else {
		buttonRestore->setEnabled(false);
		m_selectedBackupDateTime = QString();
		m_selectedBackupDescription = QString();
		m_selectedBackupFilePath = QString();
		m_selectedBackupInfoFilePath = QString();
	}
}

QString RestoreDialog::getSelectedBackupFilePath()
{
	return m_selectedBackupFilePath;
}

QString RestoreDialog::getSelectedBackupInfoFilePath()
{
	return m_selectedBackupInfoFilePath;
}

void RestoreDialog::OnCancel()
{
	qDebug() << "RestoreDialog::OnCancel()";
	reject();
}


void RestoreDialog::OnOK()
{
	qDebug() << "RestoreDialog::OnOK()";
	QMessageBox::StandardButton iRet = QMessageBox::question( this, "Restore ?", 
		"Confirm restore of:\n"+m_selectedBackupDateTime+"\n"+m_selectedBackupDescription, 
		QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes );
 	if(iRet == QMessageBox::Yes) {
		accept();
	}
}

void RestoreDialog::slotFinished()
{
	qDebug() << "RestoreDialog::slotFinished() " << result();
	model->deleteTableData();
}

void RestoreDialog::showEvent(QShowEvent *e)
{
	qDebug("CustomButtonDialog::showEvent()");
	Q_UNUSED(e);
	m_selectedBackupDateTime = QString();
	m_selectedBackupDescription = QString();
	m_selectedBackupFilePath = QString();
	m_selectedBackupInfoFilePath = QString();
	buttonRestore->setEnabled(false);
	tableView->setFocus();
	if(m_pos.x() != 0 && m_pos.y() != 0) {
		move(m_pos);
	}
}

void RestoreDialog::hideEvent(QHideEvent * event) 
{
	Q_UNUSED(event);
	qDebug("RestoreDialog::hideEvent()");
	m_pos = pos();
}

void RestoreDialog::loadData()
{
	emit signalLoadData();
}

void RestoreDialog::slotLoadData()
{
	qDebug() << "***************** slotLoadData()";

	proxyModel->setBackupType(BACKUP_TYPE_MANUAL | BACKUP_TYPE_AUTOMATIC);
	chkManual->setChecked(true);
	chkAuto->setChecked(true);
	chkAutoSpes->setChecked(false);
	txtFilter->setText("");

	QStringList files;

	QFileInfo iniFileInfo(g_strTunnelIniFile);
	QString iniDirPath = iniFileInfo.absolutePath();			
	QString iniFilePath = iniFileInfo.absoluteFilePath();
	QString iniFileName = iniFileInfo.fileName();
	QDir iniDir(iniDirPath);
	QFile iniFile(iniFilePath);	

	QString backupDirName = "DoffenSSHTunnelBackup";
	QFileInfo backupDirInfo(iniDirPath,backupDirName);
	QString backupDirPath = backupDirInfo.absoluteFilePath();			
	QDir backupDir(backupDirPath);

	////20120403_182551_321#manual#DoffenSSHTunnel.ini
	QString fileNamePattern = "*#*#"+iniFileInfo.baseName()+".ini";
	files = backupDir.entryList(QStringList(fileNamePattern), QDir::Files | QDir::NoSymLinks, QDir::Name | QDir::Reversed);

	QList<TableItem*> tableData;

	for (int i = 0; i < files.size(); ++i) {

		//20120403_182551_321#manual#DoffenSSHTunnel.ini
		QString backupFileName = files[i];

		//[ "20120403_182551_321", "manual", "DoffenSSHTunnel.ini" ]
        QStringList backupFileNameSplit = backupFileName.split("#", Qt::SkipEmptyParts);
		if(backupFileNameSplit.size() != 3) {
			qDebug() << "IGNORE backupFileNameSplit.size() != 3[" << backupFileNameSplit.size() << " != 3]" << backupFileName;
			continue;
		}

		//manual
		QString backupFileTimeStamp = backupFileNameSplit.at(0);
		QString backupTypeName = backupFileNameSplit.at(1);
		int backupType = BackupInfo::getBackupType(backupTypeName);

		QString backupFilePath = backupDir.absoluteFilePath(backupFileName);
		QFileInfo backupFileInfo(backupFilePath);

		QString backupInfoFileName = backupFileInfo.baseName() + ".txt";
		QString backupInfoFilePath = backupDir.absoluteFilePath(backupInfoFileName);

		if(QFile::exists(backupInfoFilePath)) {
			tableData.append(new TableItem(backupType, backupFileTimeStamp, backupFilePath, backupInfoFilePath));
		}

		qApp->processEvents();
	}

	model->setTableData(tableData);

	if(tableData.size() > 0) {
		tableView->selectRow(0);
	}

	lblTotal->setText(QString("%1").arg(tableData.size()));
}


void RestoreDialog::slotBackupTypeFilterChanged()
{
	int nFilter = 0;
	if(chkManual->isChecked()) nFilter = nFilter | BACKUP_TYPE_MANUAL;
	if(chkAuto->isChecked()) nFilter = nFilter | BACKUP_TYPE_AUTOMATIC;
	if(chkAutoSpes->isChecked()) nFilter = nFilter | BACKUP_TYPE_AUTOMATIC_SPECIAL;
	proxyModel->setBackupType(nFilter);
}

void RestoreDialog::slotTextFilterChanged(const QString & text)
{
    if (text.isEmpty()) {
        // Clear filter if input is empty
        proxyModel->setFilterRegularExpression(QRegularExpression());
        return;
    }

    QString searchFor = text;
    searchFor.replace(' ', "*"); // Replace spaces with wildcard '*'
    if(!searchFor.startsWith("*")) {
        searchFor = "*" + searchFor;
    }
    if(!searchFor.endsWith("*")) {
        searchFor = searchFor + "*";
    }

    // Convert wildcards (*) to a valid regular expression
    QString regexPattern = QRegularExpression::wildcardToRegularExpression(searchFor);

    // Set the filter using QRegularExpression (modern Qt 6)
    proxyModel->setFilterRegularExpression(QRegularExpression(regexPattern, QRegularExpression::CaseInsensitiveOption));
}

void RestoreDialog::slotRowCountChanged(int oldRowCount,int newRowCount)
{
	Q_UNUSED(oldRowCount);
	lblCount->setText(QString("%1").arg(newRowCount));
}
