
#include "MoreTunnelsEditWidget.h"
#include "ATSkeleton.h"

MoreTunnelsEditWidget::MoreTunnelsEditWidget(QWidget *parent)
    : Widget(parent)
{
	m_pSkeletonWindow = NULL;
	m_pTable = NULL;
	m_pBtnAdd = NULL;
	m_pBtnDelete = NULL;
	m_pBtnMoveUp = NULL;
	m_pBtnMoveDown = NULL;
	m_pBtnMoreTunnelsMore = NULL;
}

void MoreTunnelsEditWidget::setup(ATSkeletonWindow *pSkeletonWindow)
{
	m_pSkeletonWindow = pSkeletonWindow;
	m_pTable = m_pSkeletonWindow->ui.tableMoreTunnels;
	m_pBtnAdd = m_pSkeletonWindow->ui.btnMoreTunnelsAdd;
	m_pBtnDelete = m_pSkeletonWindow->ui.btnMoreTunnelsDelete;
	m_pBtnMoveUp = m_pSkeletonWindow->ui.btnMoreTunnelsMoveUp;
	m_pBtnMoveDown = m_pSkeletonWindow->ui.btnMoreTunnelsMoveDown;
	m_pBtnMoreTunnelsMore = m_pSkeletonWindow->ui.btnMoreTunnelsMore;

#ifdef Q_OS_MACOS
    m_pBtnDelete->setToolTip("Delete (Fn+Backspace)");
    m_pBtnAdd->setToolTip("Add (Cmd+N)");
    m_pBtnMoveUp->setToolTip("Move Up");
    m_pBtnMoveDown->setToolTip("Move Down");
#endif

	bool blocked = m_pTable->blockSignals(true);
	
	m_pSkeletonWindow->ui.tableMoreTunnels->setColumnCount(8);
	m_pSkeletonWindow->ui.tableMoreTunnels->horizontalHeader()->show();
	m_pSkeletonWindow->ui.tableMoreTunnels->verticalHeader()->hide();

	QStringList identHeaders;
	identHeaders << "Enabled" << "Variable Name" << "Type" << "Local IP   " << "Local Port" << "Remote Host" << "Rem.. Port" << "Description";
	m_pTable->setHorizontalHeaderLabels(identHeaders);
/*
	m_pTable->setColumnWidth(COL_NAME,70);
	m_pTable->setColumnWidth(COL_TYPE,70);
	m_pTable->setColumnWidth(COL_LOCALPORT,70);
	m_pTable->setColumnWidth(COL_DESTHOST,90);
	m_pTable->setColumnWidth(COL_DESTPORT,70);
	m_pTable->setColumnWidth(COL_DESCRIPTION,90);*/

    ATVERIFY( connect( m_pTable,        &TableWidget::itemChanged,              this, &MoreTunnelsEditWidget::slotItemChanged ) );
    ATVERIFY( connect( m_pTable,        &TableWidget::itemSelectionChanged,     this, &MoreTunnelsEditWidget::slotUpdateControls ) );
    ATVERIFY( connect( m_pBtnAdd,       &QAbstractButton::clicked,              this, &MoreTunnelsEditWidget::slotAdd ) );
    ATVERIFY( connect( m_pTable,        &TableWidget::signalKeyCtrlNPressed,    this, &MoreTunnelsEditWidget::slotAdd ) );
    ATVERIFY( connect( m_pBtnDelete,    &QAbstractButton::clicked,              this, &MoreTunnelsEditWidget:: slotDelete ) );
    ATVERIFY( connect( m_pTable,        &TableWidget::signalKeyDeletePressed,   this, &MoreTunnelsEditWidget::slotDelete ) );
    ATVERIFY( connect( m_pBtnMoveUp,    &QAbstractButton::clicked,              this, &MoreTunnelsEditWidget::slotMoveUp ) );
    ATVERIFY( connect( m_pTable,        &TableWidget::signalKeyCtrlUpPressed,   this, &MoreTunnelsEditWidget::slotMoveUp ) );
    ATVERIFY( connect( m_pBtnMoveDown,  &QAbstractButton::clicked,              this, &MoreTunnelsEditWidget::slotMoveDown ) );
    ATVERIFY( connect( m_pTable,        &TableWidget::signalKeyCtrlDownPressed,	this, &MoreTunnelsEditWidget::slotMoveDown ) );
    ATVERIFY( connect( m_pTable,        &TableWidget::signalModified,           this, &MoreTunnelsEditWidget::slotModified ) );

	m_pBtnMoreTunnelsMore->setVisible(false);
    //m_pTable->hideColumn(COL_NAME);
	//slotMoreToggled(false);
	slotUpdateControls();

	m_pTable->blockSignals(blocked);
}
 
void MoreTunnelsEditWidget::setData(QList<PortForwardStruct> &portForwardStructList)
{
    qDebug() << Q_FUNC_INFO;

	bool blocked = m_pTable->blockSignals(true);
	m_pTable->setModifiedAndSaveSignalEnabled(false);

	m_pTable->clearContents();
	m_pTable->setRowCount(0);

	for(int i=0;i<portForwardStructList.size();i++) {
		PortForwardStruct pfs = portForwardStructList.at(i);
		int row = m_pTable->rowCount();
		m_pTable->insertRow(row);	
		setRowData(row,pfs);
	}

	m_pTable->resizeColumnsToContents();
	m_pTable->setColumnWidth(COL_TYPE,70);
	m_pTable->resizeRowsToContents();

	m_pTable->blockSignals(blocked);
	m_pTable->setModifiedAndSaveSignalEnabled(true);
}

void MoreTunnelsEditWidget::setRowData(int row, PortForwardStruct& pfs)
{
	QTableWidgetItem *itemEnabled = new QTableWidgetItem();
	itemEnabled->setFlags(Qt::ItemIsEnabled|Qt::ItemIsUserCheckable);
	if(pfs.bEnabled) {
		itemEnabled->setCheckState(Qt::Checked);
	} else {
		itemEnabled->setCheckState(Qt::Unchecked);
		//slotMoreToggled(true);
		//m_pBtnMoreTunnelsMore->setChecked(true);
	}
	m_pTable->setItem(row, COL_ENABLED, itemEnabled);

	QTableWidgetItem *itemName = new QTableWidgetItem();
	itemName->setText(pfs.strName.trimmed());
	itemName->setFlags(itemName->flags() | Qt::ItemIsEditable);
	m_pTable->setItem(row, COL_NAME, itemName);
	if(!itemName->text().isEmpty()) {
		//slotMoreToggled(true);
		//m_pBtnMoreTunnelsMore->setChecked(true);
	}

	//store the original value
	setOrigStruct(row, pfs);

	QTableWidgetItem *itemType = new QTableWidgetItem();
	if(pfs.nType == PortForwardStruct::LOCAL) itemType->setText(PORTFORWARD_TYPE_LOCAL);
	else if(pfs.nType == PortForwardStruct::REMOTE) itemType->setText(PORTFORWARD_TYPE_REMOTE);
	else if(pfs.nType == PortForwardStruct::DYNAMIC) itemType->setText(PORTFORWARD_TYPE_DYNAMIC);
	else itemType->setText(PORTFORWARD_TYPE_LOCAL);
	itemType->setFlags(itemName->flags() | Qt::ItemIsEditable);
	m_pTable->setItem(row, COL_TYPE, itemType);

	QTableWidgetItem *itemLocalIP = new QTableWidgetItem();
	itemLocalIP->setText(pfs.strLocalIP.trimmed());
	itemLocalIP->setFlags(itemLocalIP->flags() | Qt::ItemIsEditable);
	m_pTable->setItem(row, COL_LOCALIP, itemLocalIP);

	QTableWidgetItem *itemLocalPort = new QTableWidgetItem();
	itemLocalPort->setFlags(itemLocalPort->flags() | Qt::ItemIsEditable);
	if(pfs.nLocalPort>0) {
		itemLocalPort->setText(QString("%1").arg(pfs.nLocalPort));
	} else {
		itemLocalPort->setText("");
	}
	m_pTable->setItem(row, COL_LOCALPORT, itemLocalPort);

	QTableWidgetItem *itemDestHost = new QTableWidgetItem();
	itemDestHost->setText(pfs.strDestinationHost.trimmed());
	itemDestHost->setFlags(itemDestHost->flags() | Qt::ItemIsEditable);
	m_pTable->setItem(row, COL_DESTHOST, itemDestHost);

	QTableWidgetItem *itemDestPort = new QTableWidgetItem();
	itemDestPort->setFlags(itemDestPort->flags() | Qt::ItemIsEditable);
	if(pfs.nDestinationPort>0) {
		itemDestPort->setText(QString("%1").arg(pfs.nDestinationPort));
	} else {
		itemDestPort->setText("");
	}
	m_pTable->setItem(row, COL_DESTPORT, itemDestPort);

	QTableWidgetItem *itemDescription = new QTableWidgetItem();
	itemDescription->setText(pfs.strDescription.trimmed());
	itemDescription->setFlags(itemDescription->flags() | Qt::ItemIsEditable);
	m_pTable->setItem(row, COL_DESCRIPTION, itemDescription);

	updateRowStyle(row);
}

QList<PortForwardStruct> MoreTunnelsEditWidget::getData()
{
    qDebug() << Q_FUNC_INFO;

	QList<PortForwardStruct> data;

	for(int row=0;row<m_pTable->rowCount();row++) {
		PortForwardStruct pfs = getOrigStruct(row);
		
		if(pfs.uUid.isNull()) {
			//new row
			pfs.uUid = QUuid::createUuid();
		}

		QTableWidgetItem *itemEnabled = m_pTable->item(row, COL_ENABLED);
		if(itemEnabled->checkState() == Qt::Checked) {
			pfs.bEnabled = true;
		} else {
			pfs.bEnabled = false;
		}

		QTableWidgetItem *itemName = m_pTable->item(row, COL_NAME);
		pfs.strName = itemName->text().trimmed();

		QTableWidgetItem *itemType = m_pTable->item(row, COL_TYPE);
		QString strType = itemType->text().trimmed();
		if(strType == PORTFORWARD_TYPE_LOCAL) pfs.nType = PortForwardStruct::LOCAL;
		if(strType == PORTFORWARD_TYPE_REMOTE) pfs.nType = PortForwardStruct::REMOTE;
		if(strType == PORTFORWARD_TYPE_DYNAMIC) pfs.nType = PortForwardStruct::DYNAMIC;

		QTableWidgetItem *itemLocalIP = m_pTable->item(row, COL_LOCALIP);
		pfs.strLocalIP = itemLocalIP->text().trimmed();

		QTableWidgetItem *itemLocalPort = m_pTable->item(row, COL_LOCALPORT);
		QString strLocalPort = itemLocalPort->text().trimmed();
		if(strLocalPort.isEmpty()) strLocalPort = "0";
		bool bLocalPortOk;
		pfs.nLocalPort = strLocalPort.toInt(&bLocalPortOk);
		if(!bLocalPortOk) pfs.nLocalPort = 0;

		QTableWidgetItem *itemDestHost = m_pTable->item(row, COL_DESTHOST);
		pfs.strDestinationHost = itemDestHost->text().trimmed();

		QTableWidgetItem *itemDestPort = m_pTable->item(row, COL_DESTPORT);
		QString strDestPort = itemDestPort->text().trimmed();
		if(strDestPort.isEmpty()) strDestPort = "0";
		bool bDestPortOk;
		pfs.nDestinationPort = strDestPort.toInt(&bDestPortOk);
		if(!bDestPortOk) pfs.nDestinationPort = 0;

		QTableWidgetItem *itemDescription = m_pTable->item(row, COL_DESCRIPTION);
		pfs.strDescription = itemDescription->text().trimmed();

		data.append(pfs);
	}

	return data;
}

PortForwardStruct MoreTunnelsEditWidget::getOrigStruct(int row)
{
	QTableWidgetItem *item = m_pTable->item(row,COL_NAME);
	QVariant v = item->data(QT_USERROLE_ORIG);
	if(!v.isNull()) {
		PortForwardStruct pfs = v.value<PortForwardStruct>();
		return pfs;
	} else {
		return PortForwardStruct();
	}
}

void MoreTunnelsEditWidget::setOrigStruct(int row, PortForwardStruct pfs)
{
	QTableWidgetItem *item = m_pTable->item(row,COL_NAME);
	if(item == NULL) {
		item = new QTableWidgetItem();
		m_pTable->setItem(row,COL_NAME,item);
	}
	item->setData( QT_USERROLE_ORIG, QVariant::fromValue(pfs) );
}

QString MoreTunnelsEditWidget::ensureIsValidVarName(QTableWidgetItem *currTwi, const QString& text)
{
	QString itemText = text.trimmed();

	//Cannot be empty
	if(itemText.isEmpty()) {
		itemText = "var_name";
	}

	QString itemTextUnique = itemText;
	for(int i=2 ; m_pSkeletonWindow->isDuplicatePortForwardVariableName(itemTextUnique,currTwi) 
		|| m_pSkeletonWindow->isDuplicateBuiltinVariableName(itemTextUnique) 
		|| m_pSkeletonWindow->isDuplicateVariableName(itemTextUnique); i++) {
		itemTextUnique = QString("%1_%2").arg(itemText).arg(i);
	}
	return itemTextUnique;	
}

void MoreTunnelsEditWidget::slotUpdateControls()
{
	//enable/disable buttons
	m_pBtnDelete->setEnabled(false);
	m_pBtnMoveUp->setEnabled(false);
	m_pBtnMoveDown->setEnabled(false);
	int row = m_pTable->getSelectedRowIndex();
	if(row != -1) {
		m_pBtnDelete->setEnabled(true);
		if(!m_pTable->isFirstRow(row)) {
			m_pBtnMoveUp->setEnabled(true);
		}
		if(!m_pTable->isLastRow(row)) {
			m_pBtnMoveDown->setEnabled(true);
		}
	}
}

void MoreTunnelsEditWidget::slotItemChanged(QTableWidgetItem* item)
{
	bool blocked = m_pTable->blockSignals(true);

	//Trim
	QString itemText = item->text();
	if(itemText.startsWith(" ") || itemText.endsWith(" ")) {
		itemText = itemText.trimmed();
		if(item->column() != COL_ENABLED && item->column() != COL_TYPE) {
			item->setText(itemText);
		}
	}

	if(item->column() == COL_NAME) {
		//Ensures
		// - unique
		if(!itemText.isEmpty()) {
			QString itemTextUnique = ensureIsValidVarName(item, itemText);
			if(itemTextUnique != itemText) {
				item->setText(itemTextUnique);
			}
		}
	}

	if(item->column() == COL_TYPE) {
		updateRowStyle(item->row());
	}

	m_pTable->blockSignals(blocked);
	emit signalModified();
}

void MoreTunnelsEditWidget::updateRowStyle(int row)
{
	bool blocked = m_pTable->blockSignals(true);
	QTableWidgetItem *itemType = m_pTable->item(row, COL_TYPE);
	QTableWidgetItem *itemDestHost = m_pTable->item(row, COL_DESTHOST);
	QTableWidgetItem *itemDestPort = m_pTable->item(row, COL_DESTPORT);
	if(itemType != NULL && itemDestHost != NULL && itemDestPort != NULL) {
		if(itemType->text() == PORTFORWARD_TYPE_DYNAMIC) {
			QBrush brush(QColor(232, 232, 232, 255));
			brush.setStyle(Qt::DiagCrossPattern);
			itemDestHost->setBackground(brush);
			itemDestPort->setBackground(brush);
			itemDestHost->setFlags(itemDestHost->flags() ^ Qt::ItemIsEnabled);
			itemDestPort->setFlags(itemDestPort->flags() ^ Qt::ItemIsEnabled);
		} else {
			itemDestHost->setBackground(QBrush());
			itemDestPort->setBackground(QBrush());
			itemDestHost->setFlags(itemDestHost->flags() | Qt::ItemIsEnabled);
			itemDestPort->setFlags(itemDestPort->flags() | Qt::ItemIsEnabled);
		}
	}
	m_pTable->blockSignals(blocked);
}

void MoreTunnelsEditWidget::slotAdd()
{
	PortForwardStruct pfs;
	pfs.uUid = QUuid::createUuid();
	pfs.bEnabled = true;
	pfs.nType = PortForwardStruct::LOCAL;
	pfs.strName = "";
	pfs.strLocalIP = "localhost";
	pfs.nLocalPort = m_pSkeletonWindow->proposeNewLocalPort(m_pSkeletonWindow->m_pTreeTunnelsItemEdit);
	pfs.strDestinationHost = "";
	pfs.nDestinationPort = 0;
	pfs.strDescription = "";

	bool blocked = m_pTable->blockSignals(true);
	int row = m_pTable->addRowBelowSelected();
	setRowData(row, pfs);
	m_pTable->blockSignals(blocked);
	m_pTable->resizeRowsToContents();

	QTableWidgetItem *itemType = m_pTable->item(row, COL_TYPE);
	m_pTable->setCurrentItem(itemType);

	emit signalModified();
}

void MoreTunnelsEditWidget::slotDelete()
{
	int row = m_pTable->getSelectedRowIndex();
	if(row == -1) return;
	QTableWidgetItem *itemName = m_pTable->item(row,COL_NAME);
    QMessageBox::StandardButton iRet = QMessageBox::question( m_pSkeletonWindow->ui.widgetEditTunnel, "", "Confirm delete'"+itemName->text()+"' ?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes );
 	if ( iRet == QMessageBox::Yes ) {
		bool blocked = m_pTable->blockSignals(true);
		m_pTable->removeRow(row);
		m_pTable->resizeRowsToContents();
		m_pTable->blockSignals(blocked);
		m_pTable->setFocus(Qt::OtherFocusReason);
		emit signalModified();
	}
}

void MoreTunnelsEditWidget::slotMoveUp()
{
	bool blocked = m_pTable->blockSignals(true);
	m_pTable->moveSelectedRowUp();
	slotUpdateControls();
	m_pTable->blockSignals(blocked);
	emit signalModified();
}

void MoreTunnelsEditWidget::slotMoveDown()
{
	bool blocked = m_pTable->blockSignals(true);
	m_pTable->moveSelectedRowDown();
	slotUpdateControls();
	m_pTable->blockSignals(blocked);
	emit signalModified();
}

void MoreTunnelsEditWidget::slotMoreToggled(bool checked)
{
	bool blocked = m_pTable->blockSignals(true);
	if(checked) {
		m_pTable->showColumn(COL_ENABLED); 
		m_pTable->showColumn(COL_NAME);   //Enable this when we have implementet usage of the variable name
		m_pBtnMoreTunnelsMore->setText("less");
		m_pTable->resizeColumnToContents(COL_ENABLED);
		m_pTable->resizeColumnToContents(COL_NAME);
	} else {
		m_pTable->hideColumn(COL_ENABLED);
		m_pTable->hideColumn(COL_NAME);
		m_pBtnMoreTunnelsMore->setText("more");
	}
	m_pTable->blockSignals(blocked);
}







