
#include "CustomButtonDialog.h"
#include "CustomActionsEditWidget.h"
#include "PasswordDb.h"

CustomButtonDialog::CustomButtonDialog(QWidget *parent, ATSkeletonWindow *pMainWindow)
: QDialog(parent)
{
    m_pTunnel = NULL;

	setupUi(this);

	tabWidgetHelp->setCurrentIndex(0);

	m_pMainWindow = pMainWindow;

	editCommand->setIgnoreReturnKey(true);

	tableVarHelp->setContextMenuPolicy(Qt::CustomContextMenu);
	m_pTableVarContextMenu = new QMenu(tableVarHelp);
	m_pTableVarActionCopy = new QAction("&Copy (Ctrl+C)",m_pTableVarContextMenu);
#ifdef Q_OS_MACOS
    m_pTableVarActionCopy->setText(m_pTableVarActionCopy->text() + " (Cmd+C)");
#else
    m_pTableVarActionCopy->setText(m_pTableVarActionCopy->text() + " (Ctrl+C)");
#endif
	m_pTableVarContextMenu->addAction(m_pTableVarActionCopy);

    ATVERIFY( connect( tableVarHelp,          &QWidget::customContextMenuRequested,   this, &CustomButtonDialog::slotTableVarCustomContextMenuRequested ) );
    ATVERIFY( connect( m_pTableVarActionCopy, &QAction::triggered,                    this, &CustomButtonDialog::slotTableVarCopy ) );
    ATVERIFY( connect( editCommand,           &PlainTextEdit::signalReturnKeyPressed, this, &CustomButtonDialog::accept ) );
    ATVERIFY( connect( editCommand,           &PlainTextEdit::textChanged,            this, &CustomButtonDialog::slotCommandTextChanged ) );
    ATVERIFY( connect( commandPreview,        &PlainTextEdit::signalReturnKeyPressed, this, &CustomButtonDialog::accept ) );
    ATVERIFY( connect( btnExecute,            &QAbstractButton::clicked,              this, &CustomButtonDialog::slotExecute ) );
}

CustomButtonDialog::~CustomButtonDialog()
{

}

void CustomButtonDialog::showEvent(QShowEvent * event)
{
	qDebug("CustomButtonDialog::showEvent()");
	QDialog::showEvent(event);
	if(m_pos.x() != 0 && m_pos.y() != 0) {
		move(m_pos);
	}
}

void CustomButtonDialog::hideEvent(QHideEvent * event) 
{
	qDebug("CustomButtonDialog::hideEvent()");
	m_pos = pos();
	QDialog::hideEvent(event);
}

void CustomButtonDialog::init(Tunnel_c* pt, QTreeWidgetItem* twi, CustomActionStruct &cas)
{
    m_pTunnel = pt;
	m_pTwi = twi;
	m_cas = cas;

    editLabel->setText(m_cas.sLabel);
	editCommand->setPlainText(m_cas.sCmd);

	//Setup variable table

	tableVarHelp->setColumnCount(3);
	tableVarHelp->horizontalHeader()->show();
	tableVarHelp->verticalHeader()->hide();
	tableVarHelp->setColumnWidth(0,150); //variable name
	tableVarHelp->setColumnWidth(1,150); //variable value

	QStringList headers;
	headers << "Name" << "Value" << "Description";
	tableVarHelp->setHorizontalHeaderLabels(headers);

	setVarTableData();
}

void CustomButtonDialog::setVarTableData()
{
    //Special variables
    QList<VariableStruct> specialListEditPane;

    specialListEditPane.append(
        VariableStruct(QUuid(),
        "Executes the command in the built-in output window",
        "OW",
        "OutputWindow",
        "",
        VariableStruct::NONE));

    specialListEditPane.append(
        VariableStruct(QUuid(),
       "Executes the command in the built-in output window and automatically closes it if the command exits with code 0",
       "OWAC",
       "OutputWindowAutoClose",
       "",
       VariableStruct::NONE));

    specialListEditPane.append(
        VariableStruct(QUuid(),
        "The directory that contains the application executable",
        "${appdir}",
        g_strAppDirectoryPath,
        "",
        VariableStruct::NONE));

    specialListEditPane.append(
        VariableStruct(QUuid(),
        "The directory that contains the application data",
        "${datadir}",
        g_strDataDirectoryPath,
        "",
        VariableStruct::NONE));

	//Variables from EditPane

	QList<VariableStruct> helpListEditPane;

    helpListEditPane.append(
        VariableStruct(QUuid(),
        m_pMainWindow->ui.btnTunnelNameMenu->text().replace(":",""),
        "${name}",
        m_pTunnel->strName,
        "",
        VariableStruct::EDITPANE));

	helpListEditPane.append(
		VariableStruct(QUuid(),
		m_pMainWindow->ui.labelEditSSHHost->text().replace(":",""),
        "${host}",
        m_pTunnel->getSelectedSSHHost(),
        "",
		VariableStruct::EDITPANE));

	helpListEditPane.append(
		VariableStruct(QUuid(),
		m_pMainWindow->ui.labelEditSSHUsername->text().replace(":",""),
        "${uid}",
        m_pTunnel->strUsername,
        "",
		VariableStruct::EDITPANE));

	helpListEditPane.append(
		VariableStruct(QUuid(),
		m_pMainWindow->ui.labelEditSSHPassword->text().replace(":",""),
        "${pwd}",
        m_pTunnel->strPassword.trimmed().isEmpty()?"":"*****",
        "",
		VariableStruct::EDITPANE));

	helpListEditPane.append(
		VariableStruct(QUuid(),
		m_pMainWindow->ui.labelSSHKeyFile->text().replace(":",""),
        "${kfile}",
        m_pTunnel->strSSHKeyFile,
        "",
		VariableStruct::EDITPANE));

	helpListEditPane.append(
		VariableStruct(QUuid(),
		m_pMainWindow->ui.labelSSHKeyPassword->text().replace(":",""),
        "${kpwd}",
        m_pTunnel->strSSHKeyFilePassword.trimmed().isEmpty()?"":"*****",
        "",
		VariableStruct::EDITPANE));

    QString strLocalIP = m_pTunnel->strLocalIP;
	if(strLocalIP.isEmpty()) {
		strLocalIP = "localhost";
	}
	helpListEditPane.append(
		VariableStruct(QUuid(),
		m_pMainWindow->ui.labelEditMainTunnelSourceIP->text().replace(":",""),
        "${ip}",
		QString("%1").arg(strLocalIP),
        "",
		VariableStruct::EDITPANE));

	helpListEditPane.append(
		VariableStruct(QUuid(),
		m_pMainWindow->ui.labelEditMainTunnelSourcePort->text().replace(":",""),
        "${port}",
        QString("%1").arg(m_pTunnel->iLocalPort),
        "",
		VariableStruct::EDITPANE));

	helpListEditPane.append(
		VariableStruct(QUuid(),
		m_pMainWindow->ui.labelEditMainTunnelDestHost->text().replace(":",""),
        "${rhost}",
        m_pTunnel->getSelectedRemoteHost(),
        "",
		VariableStruct::EDITPANE));

	helpListEditPane.append(
		VariableStruct(QUuid(),
		m_pMainWindow->ui.labelEditMainTunnelDestinationPort->text().replace(":",""),
        "${rport}",
        QString("%1").arg(m_pTunnel->iRemotePort),
        "",
		VariableStruct::EDITPANE));

    //Variables from EditPane More Tunnels

    QList<VariableStruct> helpListEditPaneMoreTunnels;

    for(int i=0;i<m_pTunnel->portForwardList.size();i++) {
        PortForwardStruct varIn = m_pTunnel->portForwardList.at(i);
        if(!varIn.strName.trimmed().isEmpty()) {
            helpListEditPaneMoreTunnels.append(VariableStruct(varIn.uUid,
              "Local Port",
              "${" + varIn.strName.trimmed() +  + ".port}",
              QString::number(varIn.nLocalPort),
              "",
              VariableStruct::EDITPANE));

            helpListEditPaneMoreTunnels.append(VariableStruct(varIn.uUid,
              "Local IP",
              "${" + varIn.strName.trimmed() +  + ".ip}",
              varIn.strLocalIP,
              "",
              VariableStruct::EDITPANE));
        }
    }

	//Variable help Executables
	QList<VariableStruct> helpListExecutables;
	for(int i=0;i<m_pMainWindow->m_listExecutableVariables.size();i++) {
		VariableStruct varIn = m_pMainWindow->m_listExecutableVariables.at(i);
		VariableStruct var(varIn.uUid,
			varIn.strDescription,
			varIn.strName.trimmed(),
			varIn.strValue,
            varIn.strArgs,
			varIn.nType
			);
		if(!var.strName.isEmpty()) {
            var.strName = "${" + var.strName + "}";
		}
		helpListExecutables.append(var);
	}

	//Variable help UserDefined
	QList<VariableStruct> helpListUserDefined;
	for(int i=0;i<m_pMainWindow->m_listUserDefinedVariables.size();i++) {
		VariableStruct varIn = m_pMainWindow->m_listUserDefinedVariables.at(i);
		VariableStruct var(varIn.uUid,
			varIn.strDescription,
			varIn.strName.trimmed(),
			varIn.strValue,
            varIn.strArgs,
			varIn.nType
			);
		if(!var.strName.isEmpty()) {
            var.strName = "${" + var.strName + "}";
		}
		helpListUserDefined.append(var);
	}

	//Variable help Passwords

	QList<VariableStruct> helpListPasswords;

    PasswordDb *pdb = PasswordDb::getInstance();
    for(unsigned int i = 0; i < pdb->size(); ++i) {
        Identifier ident = pdb->at(i);
		QString name = ident.getDescription();
		QString login = ident.getLogin();
		QString pass = ident.getPassword();
		pass = pass.trimmed().isEmpty()?"":"*****";
		QString loginVar = "${" + name + ".uid}";
		QString passVar = "${" + name + ".pwd}";

		helpListPasswords.append(
			VariableStruct(QUuid(),
			"Username for: " + name,
			loginVar,
			login,
            "",
			VariableStruct::PASSWORDS));

		helpListPasswords.append(
			VariableStruct(QUuid(),
			"Password for: " + name,
			passVar,
			pass,
            "",
			VariableStruct::PASSWORDS));
	}

	tableVarHelp->clearContents();
	tableVarHelp->setRowCount(0);

    addHeaderToVarTable("Special built in variables");
    addDataToVarTable(specialListEditPane);

    if(m_pTunnel->iType == TUNNEL_TYPE_TUNNEL) {
        addHeaderToVarTable("Built in variables from the Edit pane (Selected Host)");
        addDataToVarTable(helpListEditPane);

        addHeaderToVarTable("More Tunnels from the Edit pane (Selected Host)");
        addDataToVarTable(helpListEditPaneMoreTunnels);
    }

	addHeaderToVarTable("Executable Applications (go to Settings -> Passwords & variables... to edit them)");
	addDataToVarTable(helpListExecutables);

	addHeaderToVarTable("User Defined Variables (go to Settings -> Passwords & variables... to edit them)");
	addDataToVarTable(helpListUserDefined);

	addHeaderToVarTable("Passwords (go to Settings -> Passwords & variables... to edit them)");
	addDataToVarTable(helpListPasswords);

	//Port forwarding
	/*
	addHeaderToVarTable("Port Forward Variables from the Edit pane (Selected Host)");
	int count1 = 0;
	QTreeWidgetItem *twiCurrent = m_pMainWindow->m_pTreeTunnelsItemEdit;
	Tunnel_c* ptCurrent = ATSkeletonWindow::getTunnel(twiCurrent); 
	ATASSERT(ptCurrent);
	for(int j=0; j<ptCurrent->portForwardList.size(); j++) {
		PortForwardStruct pfs = ptCurrent->portForwardList.at(j);
		count1 += addDataToVarTable(NULL, pfs);
	}
	if(count1 == 0) {
		addEmptyToVarTable();
	}*/
	/*
	addHeaderToVarTable("Port Forward Variables from Parent Hosts");
	int count2 = 0;
	QTreeWidgetItem *twiParent = m_pMainWindow->findParentTunnelNode(twiCurrent);
	while(twiParent != NULL) {
		Tunnel_c* ptParent = ATSkeletonWindow::getTunnel(twiParent); 
		ATASSERT(ptParent);
		for(int j=0; j<ptParent->portForwardList.size(); j++) {
			PortForwardStruct pfs = ptParent->portForwardList.at(j);
			count2 += addDataToVarTable(ptParent, pfs);
		}
		twiParent = m_pMainWindow->findParentTunnelNode(twiParent);
	}
	if(count2 == 0) {
		addEmptyToVarTable();
	}*/
}

void CustomButtonDialog::addHeaderToVarTable(const QString & strHeader)
{
	QTableWidgetItem *item = new QTableWidgetItem();
	item->setText(strHeader);
	item->setFont(QFont(QString::fromUtf8("Tahoma"),8, QFont::Bold));
	int row = tableVarHelp->rowCount();
	tableVarHelp->insertRow(row);
	tableVarHelp->setSpan(row,0,1,tableVarHelp->columnCount());
	tableVarHelp->setItem(row,0,item);
}

void CustomButtonDialog::addEmptyToVarTable()
{
	QTableWidgetItem *item = new QTableWidgetItem();
	item->setText("empty");
	item->setFont(QFont(QString::fromUtf8("Tahoma"),8, QFont::Cursive));
	int row = tableVarHelp->rowCount();
	tableVarHelp->insertRow(row);
	tableVarHelp->setSpan(row,0,1,tableVarHelp->columnCount());
	tableVarHelp->setItem(row,0,item);
}

void CustomButtonDialog::addDataToVarTable(const QList<VariableStruct> & varList)
{
	for(int i=0;i<varList.size();i++) {
		VariableStruct var = varList.at(i);
		int row = tableVarHelp->rowCount();
		tableVarHelp->insertRow(row);

		QTableWidgetItem *itemName = new QTableWidgetItem();
		itemName->setText(var.strName);
		tableVarHelp->setItem(row,0,itemName);

		QTableWidgetItem *itemValue = new QTableWidgetItem();
        if(!var.strArgs.isEmpty()) {
            itemValue->setText(var.strValue + " " + var.strArgs);
        } else {
            itemValue->setText(var.strValue);
        }
		tableVarHelp->setItem(row,1,itemValue);

		QTableWidgetItem *itemDescr = new QTableWidgetItem();
		itemDescr->setText(var.strDescription);
		tableVarHelp->setItem(row,2,itemDescr);
	}
}

int CustomButtonDialog::addDataToVarTable(Tunnel_c* pt, const PortForwardStruct& pfs)
{
	QString varName = pfs.strName.trimmed();
	//if(varName.isEmpty()) return 0;

	int row = tableVarHelp->rowCount();
	tableVarHelp->insertRow(row);

	QTableWidgetItem *itemName = new QTableWidgetItem();
	itemName->setText(pfs.strName);
	tableVarHelp->setItem(row,0,itemName);

	QTableWidgetItem *itemValue = new QTableWidgetItem();
	itemValue->setText(QString("%1").arg(pfs.nLocalPort));
	tableVarHelp->setItem(row,1,itemValue);

	QString descr = "";
	if(pt != NULL) {
		descr += pt->strName + "\n";
	}
	descr += pfs.strDescription;
	QTableWidgetItem *itemDescr = new QTableWidgetItem();
	itemDescr->setText(descr);
	tableVarHelp->setItem(row,2,itemDescr);

	return 1;
}


void CustomButtonDialog::slotTableVarCustomContextMenuRequested(const QPoint& pos)
{
	m_pTableVarContextMenu->exec(tableVarHelp->mapToGlobal(pos));
}

void CustomButtonDialog::slotTableVarCopy()
{
	QClipboard *clipboard = QApplication::clipboard();
	QTableWidgetItem *item = tableVarHelp->currentItem();
	if(item != NULL) {
		clipboard->setText(item->text());
	}
}

void CustomButtonDialog::slotCommandTextChanged()
{
	updateCommandPreview();
}

void CustomButtonDialog::updateCommandPreview()
{
    QString strPreview = m_pMainWindow->replaceVarsLog(*m_pTunnel,editCommand->toPlainText());
	commandPreview->setPlainText(strPreview);
}

void CustomButtonDialog::slotExecute()
{
	m_cas.sLabel = editLabel->text().trimmed();
	m_cas.sCmd = editCommand->toPlainText().trimmed();
	if(!m_cas.sCmd.trimmed().isEmpty()) {
		emit signalActionExec(m_cas);
	}
}
