#include "VariableSettingsDialog.h"
#include "ATSkeleton.h"
#include "Images.h"
#include "PasswordDialog.h"
#include "PasswordDb.h"

VariableSettingsDialog::VariableSettingsDialog(ATSkeletonWindow *parent)
: QDialog(parent)
{
	setupUi(this);
	m_pSkeletonWindow = parent;
	m_bEditingEnabled = true;
    m_masterPassword = "";
    tabWidget->setCurrentIndex(0); //Executable variables

    if(!g_bPwdFileEnabled) {
        tabPasswords->setEnabled(false);

        // Insert the label into the center of the grid (e.g., row 0, column 0, spanning all rows/cols)
        QGridLayout* grid = qobject_cast<QGridLayout*>(tabPasswords->layout());
        if (grid) {
            // Clear the layout visually by hiding children (if needed)
            for (int i = 0; i < grid->count(); ++i) {
                QWidget* widget = grid->itemAt(i)->widget();
                if (widget)
                    widget->hide();
            }

            // Add the label with full span
            int rowCount = grid->rowCount();
            int colCount = grid->columnCount();

            QLabel* disabledLabel = new QLabel("Passwords are disabled by command-line option -p disabled", tabPasswords);
            disabledLabel->setAlignment(Qt::AlignCenter);
                disabledLabel->setStyleSheet(R"(
                color: red;
                font-weight: normal;
                font-size: 14px;
                background-color: transparent;
            )");
            grid->addWidget(disabledLabel, 0, 0, rowCount > 0 ? rowCount : 1, colCount > 0 ? colCount : 1, Qt::AlignCenter);
        }
    }

    //Dialog
    ATVERIFY( connect( this, &VariableSettingsDialog::accepted, this, &VariableSettingsDialog::slotAccepted ) );
    ATVERIFY( connect( this, &VariableSettingsDialog::rejected, this, &VariableSettingsDialog::slotRejected ) );

    ATVERIFY( connect( tabWidget, &QTabWidget::currentChanged,  this, &VariableSettingsDialog::slotTabChanged ) );

	//Passwords table
	tableIdents->clear();
	tableIdents->setColumnCount(5);
	tableExecutables->horizontalHeader()->show();
	tableExecutables->verticalHeader()->hide();

	QStringList identHeaders;
	identHeaders << "Used" << "Variable Name" << "User ID" << "Password" << "";
	tableIdents->setHorizontalHeaderLabels(identHeaders);

    ATVERIFY( connect( tableIdents,         &TableWidget::itemChanged,              this, &VariableSettingsDialog::slotPwdItemChanged ) );
    ATVERIFY( connect( tableIdents,         &TableWidget::itemSelectionChanged,     this, &VariableSettingsDialog::slotPwdUpdateControls ) );
    ATVERIFY( connect( buttonPwdShowAll,    &QAbstractButton::toggled,              this, &VariableSettingsDialog::slotPwdShowPasswords ) );
    ATVERIFY( connect( btnPwdAdd,           &QAbstractButton::clicked,              this, &VariableSettingsDialog::slotPwdAdd ) );
    ATVERIFY( connect( tableIdents,         &TableWidget::signalKeyCtrlNPressed,    this, &VariableSettingsDialog::slotPwdAdd ) );
    ATVERIFY( connect( btnPwdDelete,        &QAbstractButton::clicked,              this, &VariableSettingsDialog::slotPwdDelete ) );
    ATVERIFY( connect( tableIdents,         &TableWidget::signalKeyDeletePressed,   this, &VariableSettingsDialog::slotPwdDelete ) );
    ATVERIFY( connect( btnPwdEnableEdit,    &QAbstractButton::toggled,              this, &VariableSettingsDialog::slotPwdEnableEditView ) );

#ifdef Q_OS_MACOS
    btnPwdAdd->setToolTip("Add (Cmd+N))");
    btnPwdDelete->setToolTip("Delete");
#else
    btnPwdAdd->setToolTip("Add (Ctrl+N))");
    btnPwdDelete->setToolTip("Delete (Del)");
#endif

	//Executables table
	tableExecutables->clear();
    tableExecutables->setColumnCount(6);
	tableExecutables->horizontalHeader()->show();
	tableExecutables->verticalHeader()->hide();
	tableExecutables->setColumnWidth(EXECUTABLES_COL_VALUE,150);

	QStringList execHeaders;
    execHeaders << "Used" << "Variable Name" << "Executable File" << "Args" << "" << "Description";
	tableExecutables->setHorizontalHeaderLabels(execHeaders);
	
    ATVERIFY( connect( tableExecutables,    &TableWidget::itemChanged,              this, &VariableSettingsDialog::slotExecItemChanged ) );
    ATVERIFY( connect( tableExecutables,    &TableWidget::itemSelectionChanged,     this, &VariableSettingsDialog::slotExecUpdateControls ) );
    ATVERIFY( connect( btnExecAdd,          &QAbstractButton::clicked,              this, &VariableSettingsDialog::slotExecAdd ) );
    ATVERIFY( connect( tableExecutables,    &TableWidget::signalKeyCtrlNPressed,    this, &VariableSettingsDialog::slotExecAdd ) );
    ATVERIFY( connect( btnExecDelete,       &QAbstractButton::clicked,              this, &VariableSettingsDialog::slotExecDelete ) );
    ATVERIFY( connect( tableExecutables,    &TableWidget::signalKeyDeletePressed,   this, &VariableSettingsDialog::slotExecDelete ) );
    ATVERIFY( connect( btnExecMoveUp,       &QAbstractButton::clicked,              this, &VariableSettingsDialog::slotExecMoveUp ) );
    ATVERIFY( connect( tableExecutables,    &TableWidget::signalKeyCtrlUpPressed,   this, &VariableSettingsDialog::slotExecMoveUp ) );
    ATVERIFY( connect( btnExecMoveDown,     &QAbstractButton::clicked,              this, &VariableSettingsDialog::slotExecMoveDown ) );
    ATVERIFY( connect( tableExecutables,    &TableWidget::signalKeyCtrlDownPressed, this, &VariableSettingsDialog::slotExecMoveDown ) );

#ifdef Q_OS_MACOS
    btnExecAdd->setToolTip("Add (Cmd+N)");
    btnExecDelete->setToolTip("Delete");
    btnExecMoveUp->setToolTip("Move Up");
    btnExecMoveDown->setToolTip("Move Down");
#else
    btnExecAdd->setToolTip("Add (Ctrl+N)");
    btnExecDelete->setToolTip("Delete (Del)");
    btnExecMoveUp->setToolTip("Move Up (Ctrl+Up)");
    btnExecMoveDown->setToolTip("Move Down (Ctrl+Down)");
#endif

    //Userdefined table
	tableUser->clear();
	tableUser->setColumnCount(5);
	tableUser->horizontalHeader()->show();
	tableUser->verticalHeader()->hide();
	tableUser->setColumnWidth(USERDEF_COL_VALUE,150);

	QStringList userHeaders;
	userHeaders << "Used" << "Variable Name" << "Variable Value" << "" << "Description";
	tableUser->setHorizontalHeaderLabels(userHeaders);

    ATVERIFY( connect( tableUser,       &TableWidget::itemChanged,              this, &VariableSettingsDialog::slotUserItemChanged ) );
    ATVERIFY( connect( tableUser,       &TableWidget::itemSelectionChanged,     this, &VariableSettingsDialog::slotUserUpdateControls ) );
    ATVERIFY( connect( btnUserAdd,      &QAbstractButton::clicked,              this, &VariableSettingsDialog::slotUserAdd ) );
    ATVERIFY( connect( tableUser,       &TableWidget::signalKeyCtrlNPressed,    this, &VariableSettingsDialog::slotUserAdd ) );
    ATVERIFY( connect( btnUserDelete,   &QAbstractButton::clicked,              this, &VariableSettingsDialog::slotUserDelete ) );
    ATVERIFY( connect( tableUser,       &TableWidget::signalKeyDeletePressed,   this, &VariableSettingsDialog::slotUserDelete ) );
    ATVERIFY( connect( btnUserMoveUp,   &QAbstractButton::clicked,              this, &VariableSettingsDialog::slotUserMoveUp ) );
    ATVERIFY( connect( tableUser,       &TableWidget::signalKeyCtrlUpPressed,   this, &VariableSettingsDialog::slotUserMoveUp ) );
    ATVERIFY( connect( btnUserMoveDown, &QAbstractButton::clicked,              this, &VariableSettingsDialog::slotUserMoveDown ) );
    ATVERIFY( connect( tableUser,       &TableWidget::signalKeyCtrlDownPressed, this, &VariableSettingsDialog::slotUserMoveDown ) );

#ifdef Q_OS_MACOS
    btnUserAdd->setToolTip("Add (Cmd+N)");
    btnUserDelete->setToolTip("Delete (Fn+Backspace)");
    btnUserMoveUp->setToolTip("Move Up");
    btnUserMoveDown->setToolTip("Move Down");
#else
    btnUserAdd->setToolTip("Add (Ctrl+N)");
    btnUserDelete->setToolTip("Delete (Del)");
    btnUserMoveUp->setToolTip("Move Up (Ctrl+Up)");
    btnUserMoveDown->setToolTip("Move Down (Ctrl+Down)");
#endif
}

VariableSettingsDialog::~VariableSettingsDialog()
{

}

void VariableSettingsDialog::enableEditingToggled(bool enabled)
{
    qDebug( "%s", Q_FUNC_INFO );
	m_bEditingEnabled = enabled;
	buttonBox->button(QDialogButtonBox::Ok)->setEnabled(enabled);
	if(m_bEditingEnabled) {
		setWindowTitle("Setup variables");
	} else {
		setWindowTitle("Setup variables - EDITING DISABLED");
    }
}

QString VariableSettingsDialog::getMasterPassword()
{
    return m_masterPassword;
}

void VariableSettingsDialog::clearMasterPassword()
{
    qDebug() << "VariableSettingsDialog::clearMasterPassword()";
    m_masterPassword.clear();
}

void VariableSettingsDialog::clearAllData()
{
    qDebug() << "VariableSettingsDialog::clearAllData()";
    m_masterPassword.clear();
    m_statMap.clear();

    tableIdents->blockSignals(true);
    tableIdents->clearContents();
    tableIdents->setRowCount(0);
    buttonPwdShowAll->setChecked(false);
    btnPwdEnableEdit->setChecked(false);
    tableIdents->blockSignals(false);

    tableExecutables->blockSignals(true);
    tableExecutables->clearContents();
    tableExecutables->setRowCount(0);
    tableExecutables->blockSignals(false);

    tableUser->blockSignals(true);
    tableUser->clearContents();
    tableUser->setRowCount(0);
    tableUser->blockSignals(false);

}

void VariableSettingsDialog::slotLoadIcons()
{
	//Passwords
	btnPwdAdd->setIcon(Images::icon("pw_010_btn_add"));
	buttonPwdShowAll->setIcon(Images::icon("pw_020_btn_view_all"));
	btnPwdDelete->setIcon(Images::icon("pw_030_btn_del"));
	for(int row=0;row<tableIdents->rowCount();row++) {
		TableWidgetToolButton *btn = qobject_cast<TableWidgetToolButton*>( tableIdents->cellWidget(row,PWD_COL_BUTTON) );
		if(btn != NULL) {
			btn->setIcon(Images::icon("pw_040_btn_view"));
		}
	}

	//Executables
	btnExecAdd->setIcon(Images::icon("ea_010_btn_add"));
	btnExecDelete->setIcon(Images::icon("ea_020_btn_del"));
	btnExecMoveUp->setIcon(Images::icon("ea_030_btn_up"));
	btnExecMoveDown->setIcon(Images::icon("ea_040_btn_down"));


	//UserDefined
	btnUserAdd->setIcon(Images::icon("uv_010_btn_add"));
	btnUserDelete->setIcon(Images::icon("uv_020_btn_del"));
	btnUserMoveUp->setIcon(Images::icon("uv_030_btn_up"));
    btnUserMoveDown->setIcon(Images::icon("uv_040_btn_down"));
}

void VariableSettingsDialog::slotAccepted()
{
    qDebug() << "VariableSettingsDialog::slotAccepted()";
}

void VariableSettingsDialog::slotRejected()
{
    qDebug() << "VariableSettingsDialog::slotRejected()";
    clearAllData();
}

void VariableSettingsDialog::slotTabChanged(int index)
{
    Q_UNUSED(index)
}

void VariableSettingsDialog::showEvent(QShowEvent * event)
{
	qDebug("VariableSettingsDialog::showEvent()");
    buttonPwdShowAll->setChecked(false);
    btnPwdEnableEdit->setChecked(false);
    m_masterPassword.clear();
	QDialog::showEvent(event);
	if(m_pos.x() != 0 && m_pos.y() != 0) {
		//move(m_pos);
	}
}

void VariableSettingsDialog::hideEvent(QHideEvent * event) 
{
	qDebug("VariableSettingsDialog::hideEvent()");
	m_pos = pos();
	QDialog::hideEvent(event);
}

//Used for statistics.
//QUuid: VariableStruct.uUid
//VariableStatStruct: The stat values
void VariableSettingsDialog::setVariableStatistics(QMap<QUuid,VariableStatStruct> statMap)
{
	m_statMap = statMap;
}

QUuid VariableSettingsDialog::getPwdUuid(int row)
{
	QTableWidgetItem *item = tableIdents->item(row,PWD_COL_NAME);
	QVariant v = item->data(QT_USERROLE_VARUUID);
	if(!v.isNull()) {
		QString strUuid = v.value<QString>();
		QUuid uuid = QUuid(strUuid);
		return uuid;
	} else {
		return QUuid();
	}
}

QUuid VariableSettingsDialog::getExecutableUuid(int row)
{
	QTableWidgetItem *item = tableExecutables->item(row,EXECUTABLES_COL_NAME);
	QVariant v = item->data(QT_USERROLE_VARUUID);
	if(!v.isNull()) {
		QString strUuid = v.value<QString>();
		QUuid uuid = QUuid(strUuid);
		return uuid;
	} else {
		return QUuid();
	}
}

QUuid VariableSettingsDialog::getUserDefinedUuid(int row)
{
	QTableWidgetItem *item = tableUser->item(row,USERDEF_COL_NAME);
	QVariant v = item->data(QT_USERROLE_VARUUID);
	if(!v.isNull()) {
		QString strUuid = v.value<QString>();
		QUuid uuid = QUuid(strUuid);
		return uuid;
	} else {
		return QUuid();
	}
}

void VariableSettingsDialog::updateStatistics()
{
	//Update Used counters in m_statMap ...

	//Password vars used in listExecutables AND/OR listUser
	for(int i=0;i<tableIdents->rowCount();i++) {

		QUuid uuid = getPwdUuid(i);
		ATASSERT( !uuid.isNull() );

		//returns default when user added new variable
		VariableStatStruct varStat = m_statMap.value( uuid, VariableStatStruct(uuid) ); 
		varStat.listExecVarables.clear(); //reset stats for for this variable
		varStat.listUserDefVarables.clear(); //reset stats for for this variable

		QTableWidgetItem *itemName = tableIdents->item(i,PWD_COL_NAME);
		QString name = itemName->text();
		QString varNameUid1 = "${"+name+".uid}";
		QString varNameUid2 = "$"+name+".uid";
		QString varNamePwd1 = "${"+name+".pwd}";
		QString varNamePwd2 = "$"+name+".pwd";
		
        //Usage of this executable var i tableExecutables
		for(int j=0;j<tableExecutables->rowCount();j++) {
			QTableWidgetItem *itemValueCheck = tableExecutables->item(j,EXECUTABLES_COL_VALUE);
			QString valueCheck = itemValueCheck->text();
            QTableWidgetItem *itemValue2Check = tableExecutables->item(j,EXECUTABLES_COL_ARGS);
            QString value2Check = itemValue2Check->text();
			if(valueCheck.contains(varNameUid1) || valueCheck.contains(varNameUid2)
				|| valueCheck.contains(varNamePwd1) || valueCheck.contains(varNamePwd2)) {
				QUuid uuidCheck = getExecutableUuid(j);
				varStat.listExecVarables.append(uuidCheck);
            } else if(value2Check.contains(varNameUid1) || value2Check.contains(varNameUid2)
                || value2Check.contains(varNamePwd1) || value2Check.contains(varNamePwd2)) {
                QUuid uuidCheck = getExecutableUuid(j);
                varStat.listExecVarables.append(uuidCheck);
            }
		}

		//Usage of this executable var i tableUser
		for(int j=0;j<tableUser->rowCount();j++) {
			QTableWidgetItem *itemValueCheck = tableUser->item(j,USERDEF_COL_VALUE);
			QString valueCheck = itemValueCheck->text();
			if(valueCheck.contains(varNameUid1) || valueCheck.contains(varNameUid2)
				|| valueCheck.contains(varNamePwd1) || valueCheck.contains(varNamePwd2)) {
				QUuid uuidCheck = getUserDefinedUuid(j);
				varStat.listUserDefVarables.append(uuidCheck);
			}
		}

		m_statMap.insert(uuid, varStat);
	}

	//Executable vars used in listExecutables AND/OR listUser
	for(int i=0;i<tableExecutables->rowCount();i++) {

		QUuid uuid = getExecutableUuid(i);
		ATASSERT( !uuid.isNull() );

		//returns default when user added new variable
		VariableStatStruct varStat = m_statMap.value( uuid, VariableStatStruct(uuid) ); 
		varStat.listExecVarables.clear(); //reset stats for for this variable
		varStat.listUserDefVarables.clear(); //reset stats for for this variable

		QTableWidgetItem *itemName = tableExecutables->item(i,EXECUTABLES_COL_NAME);
		QString name = itemName->text();
		QString varName1 = "${"+name+"}";
		QString varName2 = "$"+name;
		
		//Usage of this executable var i tableExecutables
		for(int j=0;j<tableExecutables->rowCount();j++) {
            QTableWidgetItem *itemValueCheck = tableExecutables->item(j,EXECUTABLES_COL_VALUE);
            QString valueCheck = itemValueCheck->text();
            QTableWidgetItem *itemArgsCheck = tableExecutables->item(j,EXECUTABLES_COL_ARGS);
            QString argsCheck = itemArgsCheck->text();
            bool varUsed = false;
            if(valueCheck.contains(varName1) || valueCheck.contains(varName2)) {
                varUsed = true;
            } else if(argsCheck.contains(varName1) || argsCheck.contains(varName2)) {
                varUsed = true;
            }
            if(varUsed) {
                QUuid uuidCheck = getExecutableUuid(j);
                varStat.listExecVarables.append(uuidCheck);
            }
		}

		//Usage of this executable var i tableUser
		for(int j=0;j<tableUser->rowCount();j++) {
			QTableWidgetItem *itemValueCheck = tableUser->item(j,USERDEF_COL_VALUE);
			QString valueCheck = itemValueCheck->text();
			if(valueCheck.contains(varName1) || valueCheck.contains(varName2)) {
				QUuid uuidCheck = getUserDefinedUuid(j);
				varStat.listUserDefVarables.append(uuidCheck);
			}
		}

		m_statMap.insert(uuid, varStat);
	}

	//UserDefined vars used in listExecutables AND/OR listUser
	for(int i=0;i<tableUser->rowCount();i++) {

		QUuid uuid = getUserDefinedUuid(i);

		//returns default when user added new variable
		VariableStatStruct varStat = m_statMap.value( uuid, VariableStatStruct(uuid) ); 
		varStat.listExecVarables.clear(); //reset stats for for this variable
		varStat.listUserDefVarables.clear(); //reset stats for for this variable

		QTableWidgetItem *itemName = tableUser->item(i,USERDEF_COL_NAME);
		QString name = itemName->text();
		QString varName1 = "${"+name+"}";
		QString varName2 = "$"+name;
		
		//Usage of this user defined var i tableExecutables
		for(int j=0;j<tableExecutables->rowCount();j++) {
            QTableWidgetItem *itemValueCheck = tableExecutables->item(j,EXECUTABLES_COL_VALUE);
            QString valueCheck = itemValueCheck->text();
            QTableWidgetItem *itemArgsCheck = tableExecutables->item(j,EXECUTABLES_COL_ARGS);
            QString argsCheck = itemArgsCheck->text();
            bool varUsed = false;
            if(valueCheck.contains(varName1) || valueCheck.contains(varName2)) {
                varUsed = true;
            } else if(argsCheck.contains(varName1) || argsCheck.contains(varName2)) {
                varUsed = true;
            }
            if(varUsed) {
                QUuid uuidCheck = getExecutableUuid(j);
                varStat.listExecVarables.append(uuidCheck);
            }
		}

		//Usage of this user defined var i tableUser
		for(int j=0;j<tableUser->rowCount();j++) {
			QTableWidgetItem *itemValueCheck = tableUser->item(j,USERDEF_COL_VALUE);
			QString valueCheck = itemValueCheck->text();
			if(valueCheck.contains(varName1) || valueCheck.contains(varName2)) {
				QUuid uuidCheck = getUserDefinedUuid(j);
				varStat.listUserDefVarables.append(uuidCheck);
			}
		}

		m_statMap.insert(uuid, varStat);
	}


	//Update the visual counters ...
	for(int i=0;i<tableIdents->rowCount();i++) {
		QUuid uuid = getPwdUuid(i);
		ATASSERT( m_statMap.contains(uuid) );
		VariableStatStruct varStat = m_statMap.value( uuid ); 
		QTableWidgetItem *itemInUse = tableIdents->item(i,PWD_COL_INUSE);
		itemInUse->setText( QString("%1").arg(varStat.totalUsageCount()) );
	}
	for(int i=0;i<tableExecutables->rowCount();i++) {
		QUuid uuid = getExecutableUuid(i);
		ATASSERT( m_statMap.contains(uuid) );
		VariableStatStruct varStat = m_statMap.value( uuid ); 
		QTableWidgetItem *itemInUse = tableExecutables->item(i,EXECUTABLES_COL_INUSE);
		itemInUse->setText( QString("%1").arg(varStat.totalUsageCount()) );
	}
	for(int i=0;i<tableUser->rowCount();i++) {
		QUuid uuid = getUserDefinedUuid(i);
		ATASSERT( m_statMap.contains(uuid) );
		VariableStatStruct varStat = m_statMap.value( uuid ); 
		QTableWidgetItem *itemInUse = tableUser->item(i,USERDEF_COL_INUSE);
		itemInUse->setText( QString("%1").arg(varStat.totalUsageCount()) );
	}

}


//Stored in app settings DoffenSSHTunnel.pwd
void VariableSettingsDialog::setPasswordVariables(QList<VariableStruct> varList)
{
	tableIdents->blockSignals(true);

	tableIdents->clearContents();
	tableIdents->setRowCount(0);

	for(int i=0;i<varList.size();i++) {
		VariableStruct var = varList.at(i);
		int row = tableIdents->rowCount();
		tableIdents->insertRow(row);	

        QTableWidgetItem *itemInUse = new QTableWidgetItem();
        setReadOnly(itemInUse,  true);
		itemInUse->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
		if(m_statMap.contains(var.uUid)) {
			VariableStatStruct varStat = m_statMap.value(var.uUid);
			itemInUse->setText( QString("%1").arg(varStat.totalUsageCount()) );
		} else {
			itemInUse->setText("0");
		}
		tableIdents->setItem(row,PWD_COL_INUSE,itemInUse);
		
		QTableWidgetItem *itemName = new QTableWidgetItem();
        //must click button 'click me to enable editing'
        setReadOnly(itemName,  true);
		itemName->setText(var.strDescription);
		tableIdents->setItem(row, PWD_COL_NAME, itemName);
		//store the UUID. Used to lookup statistics in m_statMap.
		itemName->setData( QT_USERROLE_VARUUID, QVariant::fromValue(var.uUid.toString()) );

		//store the original value so we can check if the name changed
		itemName->setData( QT_USERROLE_ORIGVAR, QVariant::fromValue(var) );

		//store the variable name. Used in slotExecItemChanged() to see
		//the original value
		itemName->setData( QT_USERROLE_VARNAME, QVariant::fromValue(var.strDescription) );

		QTableWidgetItem *itemLogin = new QTableWidgetItem();
        //must click button 'click me to enable editing'
        setReadOnly(itemLogin,  true);
		itemLogin->setText(var.strName);
		tableIdents->setItem(row, PWD_COL_LOGIN, itemLogin);

		QTableWidgetItem *itemPwd = new QTableWidgetItem();
        //must click button 'click me to enable editing'
        setReadOnly(itemPwd,  true);
		itemPwd->setText(var.strValue);
		tableIdents->setItem(row, PWD_COL_PWD, itemPwd);

		QTableWidgetItem *itemButton = new QTableWidgetItem();
        setReadOnly(itemButton,  true);
		itemButton->setText("");
		tableIdents->setItem(row, PWD_COL_BUTTON, itemButton);
	}

	//Adds the show pwd button to all rows
	slotPwdUpdateTableWidgets();

	tableIdents->resizeColumnToContents(PWD_COL_INUSE);
	tableIdents->resizeColumnToContents(PWD_COL_NAME);
	tableIdents->resizeColumnToContents(PWD_COL_LOGIN);
	tableIdents->resizeColumnToContents(PWD_COL_PWD);
	tableIdents->resizeColumnToContents(PWD_COL_BUTTON);
	slotPwdUpdateControls();

	tableIdents->blockSignals(false);
}

//Stored in app settings DoffenSSHTunnel.pwd
//QPair.first = The original VariableStruct passed to setPasswordVariables(..)
// IF NEW var then QPair.first.uUid.isNull() == true
//QPair.second = VariableStruct with edited values (from this dialog)
QList<QPair<VariableStruct,VariableStruct> > VariableSettingsDialog::getPasswordVariables()
{
    QList<QPair<VariableStruct,VariableStruct> > varList;
	for(int row=0;row<tableIdents->rowCount();row++) {
		QTableWidgetItem *itemName = tableIdents->item(row,PWD_COL_NAME);
		QTableWidgetItem *itemLogin = tableIdents->item(row,PWD_COL_LOGIN);
		//QTableWidgetItem *itemPwd = tableIdents->item(row,PWD_COL_PWD);
		TableWidgetLineEdit *editPwd = qobject_cast<TableWidgetLineEdit*>(tableIdents->cellWidget(row,PWD_COL_PWD));

		//New or edited
		VariableStruct varNew;
		varNew.nType = VariableStruct::PASSWORDS;
		varNew.uUid = QUuid();
		varNew.strDescription = itemName->text().trimmed();
		varNew.strName = itemLogin->text().trimmed();
		varNew.strValue = editPwd->text().trimmed();

		//The original
		VariableStruct varOrig;
		QVariant v = itemName->data(QT_USERROLE_ORIGVAR);
		if(!v.isNull()) {
			//existing var
			varOrig = v.value<VariableStruct>(); //The original VariableStruct passed to setPasswordVariables(..)
			varNew.uUid = varOrig.uUid;
		} else {
			//new var: varOrig below will have uUid.isNull() = true
			QVariant vUuid = itemName->data(QT_USERROLE_VARUUID);
			ATASSERT(!vUuid.isNull());
			QString strUuid = vUuid.value<QString>();
			ATASSERT(!strUuid.isEmpty());
			varNew.uUid = QUuid(strUuid);
		}
		
		//if new var then varOrig.uUid.isNull() == true
		varList.append( QPair<VariableStruct,VariableStruct>(varOrig,varNew) );
	}
	return varList;
}


//Stored in app settings DoffenSSHTunnelApp.ini
void VariableSettingsDialog::setExecutableVariables(QList<VariableStruct> varList)
{
	tableExecutables->blockSignals(true);

	tableExecutables->clearContents();
	tableExecutables->setRowCount(0);

	for(int i=0;i<varList.size();i++) {
		VariableStruct var = varList.at(i);
		int row = tableExecutables->rowCount();
		tableExecutables->insertRow(row);

		QTableWidgetItem *itemInUse = new QTableWidgetItem();
        setReadOnly(itemInUse,  true);
		
		itemInUse->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
		if(m_statMap.contains(var.uUid)) {
			VariableStatStruct varStat = m_statMap.value(var.uUid);
			itemInUse->setText( QString("%1").arg(varStat.totalUsageCount()) );
		} else {
			itemInUse->setText("0");
		}
		tableExecutables->setItem(row,EXECUTABLES_COL_INUSE,itemInUse);

		QTableWidgetItem *itemName = new QTableWidgetItem();
        if(!m_bEditingEnabled) {
            setReadOnly(itemName,  true);
        }
		itemName->setText(var.strName);
		tableExecutables->setItem(row,EXECUTABLES_COL_NAME,itemName);

		//store the UUID. Used to lookup statistics in m_statMap.
		itemName->setData( QT_USERROLE_VARUUID, QVariant::fromValue(var.uUid.toString()) );

		//store the original value so we can check if the name changed
		itemName->setData( QT_USERROLE_ORIGVAR, QVariant::fromValue(var) );

		//store the variable name. Used in slotExecItemChanged() to see
		//the original value
		itemName->setData( QT_USERROLE_VARNAME, QVariant::fromValue(var.strName) );

		QTableWidgetItem *itemValue = new QTableWidgetItem();
        if(!m_bEditingEnabled) {
            setReadOnly(itemValue,  true);
        }
		itemValue->setText(var.strValue);
		tableExecutables->setItem(row,EXECUTABLES_COL_VALUE,itemValue);

        QTableWidgetItem *itemArgs = new QTableWidgetItem();
        if(!m_bEditingEnabled) {
            setReadOnly(itemArgs,  true);
        }
        itemArgs->setText(var.strArgs);
        tableExecutables->setItem(row,EXECUTABLES_COL_ARGS,itemArgs);

        QTableWidgetItem *itemBrowse = new QTableWidgetItem();
        setReadOnly(itemBrowse,  true);
        itemBrowse->setText("");
        tableExecutables->setItem(row,EXECUTABLES_COL_BUTTON,itemBrowse);

		QTableWidgetItem *itemDescr = new QTableWidgetItem();
        if(!m_bEditingEnabled) {
            setReadOnly(itemDescr,  true);
        }
		itemDescr->setText(var.strDescription);
		tableExecutables->setItem(row,EXECUTABLES_COL_DESCRIPTION,itemDescr);		
	}

	//Adds the browse button to all rows
	slotExecUpdateBrowseButtons();

	tableExecutables->resizeColumnToContents(EXECUTABLES_COL_INUSE);
	tableExecutables->resizeColumnToContents(EXECUTABLES_COL_NAME);
	tableExecutables->resizeColumnToContents(EXECUTABLES_COL_BUTTON);
	tableExecutables->resizeColumnToContents(EXECUTABLES_COL_DESCRIPTION);
	slotExecUpdateControls();

	tableExecutables->blockSignals(false);
}

//Stored in app settings DoffenSSHTunnelApp.ini
//QPair.first = The original VariableStruct passed to setExecutableVariables(..)
// IF NEW var then QPair.first.uUid.isNull() == true
//QPair.second = VariableStruct with edited values (from this dialog)
QList<QPair<VariableStruct,VariableStruct> > VariableSettingsDialog::getExecutableVariables()
{
    QList<QPair<VariableStruct,VariableStruct> > varList;
	for(int row=0;row<tableExecutables->rowCount();row++) {
		QTableWidgetItem *itemName = tableExecutables->item(row,EXECUTABLES_COL_NAME);
		QTableWidgetItem *itemValue = tableExecutables->item(row,EXECUTABLES_COL_VALUE);
        QTableWidgetItem *itemArgs = tableExecutables->item(row,EXECUTABLES_COL_ARGS);
		QTableWidgetItem *itemDescr = tableExecutables->item(row,EXECUTABLES_COL_DESCRIPTION);

		//New or edited
        VariableStruct varNew(QUuid(), itemDescr->text(), itemName->text(), itemValue->text(),
                              itemArgs->text(), VariableStruct::EXECUTABLE);
		varNew.strDescription = varNew.strDescription.trimmed();
		varNew.strName = varNew.strName.trimmed();
		varNew.strValue = varNew.strValue.trimmed();
        varNew.strArgs = varNew.strArgs.trimmed();

		//The original
		VariableStruct varOrig;
		QVariant v = itemName->data(QT_USERROLE_ORIGVAR);
		if(!v.isNull()) {
			//existing var
			varOrig = v.value<VariableStruct>(); //The original VariableStruct passed to setExecutableVariables(..)
			varNew.uUid = varOrig.uUid;
		} else {
			//new var: varOrig below will have uUid.isNull() = true
			QVariant vUuid = itemName->data(QT_USERROLE_VARUUID);
			ATASSERT(!vUuid.isNull());
			QString strUuid = vUuid.value<QString>();
			ATASSERT(!strUuid.isEmpty());
			varNew.uUid = QUuid(strUuid);
		}
		
		//if new var then varOrig.uUid.isNull() == true
		varList.append( QPair<VariableStruct,VariableStruct>(varOrig,varNew) );
	}
	return varList;
}

//Stored in tunnel settings DoffenSSHTunnel.ini
void VariableSettingsDialog::setUserDefinedVariables(QList<VariableStruct> varList)
{
	tableUser->blockSignals(true);

	tableUser->clearContents();
	tableUser->setRowCount(0);

	for(int i=0;i<varList.size();i++) {
		VariableStruct var = varList.at(i);
		int row = tableUser->rowCount();
		tableUser->insertRow(row);

		QTableWidgetItem *itemInUse = new QTableWidgetItem();
        setReadOnly(itemInUse,  true);
		itemInUse->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
		if(m_statMap.contains(var.uUid)) {
			VariableStatStruct varStat = m_statMap.value(var.uUid);
			itemInUse->setText( QString("%1").arg(varStat.totalUsageCount()) );
		} else {
			itemInUse->setText("0");
		}
		tableUser->setItem(row,USERDEF_COL_INUSE,itemInUse);

		QTableWidgetItem *itemName = new QTableWidgetItem();
		if(!m_bEditingEnabled) {
            setReadOnly(itemName,  true);
		}
		itemName->setText(var.strName);
		tableUser->setItem(row,USERDEF_COL_NAME,itemName);

		//store the UUID. Used to lookup statistics in m_statMap.
		itemName->setData( QT_USERROLE_VARUUID, QVariant::fromValue(var.uUid.toString()) );

		//store the original value so we can find the ones that changed on Dialog::Accept
		itemName->setData( QT_USERROLE_ORIGVAR, QVariant::fromValue(var) );

		//store the variable name. Used in slotUserItemChanged() to see
		//the original value
		itemName->setData( QT_USERROLE_VARNAME, QVariant::fromValue(var.strName) );

		QTableWidgetItem *itemValue = new QTableWidgetItem();
		if(!m_bEditingEnabled) {
            setReadOnly(itemValue,  true);
		}
		itemValue->setText(var.strValue);
		tableUser->setItem(row,USERDEF_COL_VALUE,itemValue);

        /*
		QTableWidgetItem *itemBrowse = new QTableWidgetItem();
        setReadOnly(itemBrowse,  true);
		itemBrowse->setText("");
		tableUser->setItem(row,USERDEF_COL_BUTTON,itemBrowse);
        */

		QTableWidgetItem *itemDescr = new QTableWidgetItem();
		if(!m_bEditingEnabled) {
            setReadOnly(itemDescr,  true);
		}
		itemDescr->setText(var.strDescription);
		tableUser->setItem(row,USERDEF_COL_DESCRIPTION,itemDescr);		
	}

	//Adds the browse button to all rows
	slotUserUpdateBrowseButtons();

	tableUser->resizeColumnToContents(USERDEF_COL_INUSE);
	tableUser->resizeColumnToContents(USERDEF_COL_NAME);
	tableUser->resizeColumnToContents(USERDEF_COL_BUTTON);
	tableUser->resizeColumnToContents(USERDEF_COL_DESCRIPTION);
	slotUserUpdateControls();

	tableUser->blockSignals(false);
}

//Stored in tunnel settings DoffenSSHTunnel.ini
//QPair.first = The original VariableStruct passed to setUserVariables(..)
// IF NEW var then QPair.first.uUid.isNull() == true
//QPair.second = VariableStruct with edited values (from this dialog)
QList<QPair<VariableStruct,VariableStruct> > VariableSettingsDialog::getUserDefinedVariables()
{
    QList<QPair<VariableStruct,VariableStruct> > varList;
	for(int row=0;row<tableUser->rowCount();row++) {
		QTableWidgetItem *itemName = tableUser->item(row,USERDEF_COL_NAME);
		QTableWidgetItem *itemValue = tableUser->item(row,USERDEF_COL_VALUE);
		QTableWidgetItem *itemDescr = tableUser->item(row,USERDEF_COL_DESCRIPTION);

		//New or edited
        VariableStruct varNew(QUuid(), itemDescr->text(), itemName->text(), itemValue->text(), "", VariableStruct::USERDEFINED);
		varNew.strDescription = varNew.strDescription.trimmed();
		varNew.strName = varNew.strName.trimmed();
		varNew.strValue = varNew.strValue.trimmed();

		//The original
		VariableStruct varOrig;
		QVariant v = itemName->data(QT_USERROLE_ORIGVAR);
		if(!v.isNull()) {
			//existing var
			varOrig = v.value<VariableStruct>(); //The original VariableStruct passed to setUserDefinedVariables(..)
			varNew.uUid = varOrig.uUid;
		} else {
			//new var: varOrig below will have uUid.isNull() = true
			QVariant vUuid = itemName->data(QT_USERROLE_VARUUID);
			ATASSERT(!vUuid.isNull());
			QString strUuid = vUuid.value<QString>();
			ATASSERT(!strUuid.isEmpty());
			varNew.uUid = QUuid(strUuid);
		}
		
		//if new var then varOrig.uUid.isNull() == true
		varList.append( QPair<VariableStruct,VariableStruct>(varOrig,varNew) );
	}
	return varList;
}

//public
bool VariableSettingsDialog::varNameExists(const QString& text)
{
	return isDuplicateVarName(NULL, text);
}

//private
bool VariableSettingsDialog::isDuplicateVarName(QTableWidgetItem *currTwi, const QString& text)
{
	if(isDuplicateVarName(tableIdents, currTwi, text, PWD_COL_NAME)) return true;
	if(isDuplicateVarName(tableExecutables, currTwi, text, EXECUTABLES_COL_NAME)) return true;
	if(isDuplicateVarName(tableUser, currTwi, text, USERDEF_COL_NAME)) return true;
	return false;
}

//private
bool VariableSettingsDialog::isDuplicateVarName(QTableWidget *tw, QTableWidgetItem *currTwi, const QString& text, int col)
{
	for(int i=0;i<tw->rowCount();i++) {
		QTableWidgetItem *_twi = tw->item(i,col);
		if(currTwi != NULL && _twi == currTwi) continue;
		if(_twi->text().trimmed().toUpper() == text.trimmed().toUpper()) {
			return true;
		}
	}
	return false;
}

QString VariableSettingsDialog::ensureIsValidVarName(QTableWidgetItem *currTwi, const QString& text)
{
	QString itemText = text.trimmed();

	//Cannot be empty
	if(itemText.isEmpty()) {
		itemText = "var_name";
	}

	//These variable names MUST be unique. We just change it here.
	QString itemTextUnique = itemText;
	for(int i=2 ; isDuplicateVarName(currTwi, itemTextUnique) 
		|| m_pSkeletonWindow->isDuplicateBuiltinVariableName(itemTextUnique)
        || m_pSkeletonWindow->isDuplicatePortForwardVariableName(itemTextUnique, true); i++) {
		itemTextUnique = QString("%1_%2").arg(itemText).arg(i);
	}
    return itemTextUnique;
}

void VariableSettingsDialog::setReadOnly(QTableWidgetItem *item, bool readOnly)
{
    if(readOnly) {
        if(item->flags() & Qt::ItemIsEditable) {
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        }
    } else {
        if(!(item->flags() & Qt::ItemIsEditable)) {
            item->setFlags(item->flags() | Qt::ItemIsEditable);
        }
    }
}

QString VariableSettingsDialog::replaceExecutableVarsFile(const QString &str)
{
    QList<QPair<QString,QString> > varList;

    for(int i=0;i<tableExecutables->rowCount();i++) {
        QTableWidgetItem *itemName = tableExecutables->item(i,EXECUTABLES_COL_NAME);
        QTableWidgetItem *itemValue = tableExecutables->item(i,EXECUTABLES_COL_VALUE);
        QString name = itemName->text().trimmed();
        QString value = itemValue->text().trimmed();
        if(name.isEmpty()) {
            continue;
        }
        QString name1 = "${" + name + "}";
        QString name2 = "$" + name;
        varList.append(QPair<QString,QString>(name1,value));
        varList.append(QPair<QString,QString>(name2,value));
    }

    varList.append(QPair<QString,QString>("$appdir",g_strAppDirectoryPath));
    varList.append(QPair<QString,QString>("${appdir}",g_strAppDirectoryPath));

    varList.append(QPair<QString,QString>("$datadir",g_strDataDirectoryPath));
    varList.append(QPair<QString,QString>("${datadir}",g_strDataDirectoryPath));

    QString replaced = str;
    for(int i = 0; i < varList.size(); i++) {
        replaced = replaced.replace( varList.at(i).first, varList.at(i).second );
    }
    for(int i = 0; i < varList.size(); i++) {
        replaced = replaced.replace( varList.at(i).first, varList.at(i).second );
    }
    for(int i = 0; i < varList.size(); i++) {
        replaced = replaced.replace( varList.at(i).first, varList.at(i).second );
    }

    return replaced;
}

QString VariableSettingsDialog::replaceUserDefVars(const QString &str)
{
    QList<QPair<QString,QString> > varList;

    for(int i=0;i<tableUser->rowCount();i++) {
        QTableWidgetItem *itemName = tableUser->item(i,USERDEF_COL_NAME);
        QTableWidgetItem *itemValue = tableUser->item(i,USERDEF_COL_VALUE);
        QString name = itemName->text().trimmed();
        QString value = itemValue->text().trimmed();
        if(name.isEmpty()) {
            continue;
        }
        QString name1 = "${" + name + "}";
        QString name2 = "$" + name;
        varList.append(QPair<QString,QString>(name1,value));
        varList.append(QPair<QString,QString>(name2,value));
    }

    varList.append(QPair<QString,QString>("$appdir",g_strAppDirectoryPath));
    varList.append(QPair<QString,QString>("${appdir}",g_strAppDirectoryPath));

    varList.append(QPair<QString,QString>("$datadir",g_strDataDirectoryPath));
    varList.append(QPair<QString,QString>("${datadir}",g_strDataDirectoryPath));

    QString replaced = str;
    for(int i = 0; i < varList.size(); i++) {
        replaced = replaced.replace( varList.at(i).first, varList.at(i).second );
    }
    for(int i = 0; i < varList.size(); i++) {
        replaced = replaced.replace( varList.at(i).first, varList.at(i).second );
    }
    for(int i = 0; i < varList.size(); i++) {
        replaced = replaced.replace( varList.at(i).first, varList.at(i).second );
    }

    return replaced;
}

void VariableSettingsDialog::slotPwdItemChanged(QTableWidgetItem *item)
{
	//since we change ourselves, _DO_NOT_ trigger change event while we are in here
	tableIdents->blockSignals(true);

	//Trim
	QString itemText = item->text();
	if(itemText.startsWith(" ") || itemText.endsWith(" ")) {
		itemText = itemText.trimmed();
		item->setText(itemText);
	}

	if(item->column() == PWD_COL_NAME) {

		//Ensures
		// - not empty
		// - unique
		QString itemTextUnique = ensureIsValidVarName(item, itemText);
		if(itemTextUnique != itemText) {
			item->setText(itemTextUnique);
		}

		//We are dealing with variable names only here

		QString newName = item->text();
		
		QVariant v = item->data(QT_USERROLE_VARNAME);
		ATASSERT(!v.isNull());
		QString origName = v.value<QString>();
		item->setData( QT_USERROLE_VARNAME, QVariant::fromValue(newName) );

		if(!origName.isEmpty() && !newName.isEmpty() && origName != newName)
		{
			QString origVarNameUid1 = "${" + origName + ".uid}";
			QString newVarNameUid1 = "${" + newName + ".uid}";
			QString origVarNameUid2 = "$" + origName + ".uid";
			QString newVarNameUid2 = "$" + newName + ".uid";
			if(newVarNameUid2.contains(" ")) {
				newVarNameUid2 = "${" + newName + ".uid}";
			}

			QString origVarNamePwd1 = "${" + origName + ".pwd}";
			QString newVarNamePwd1 = "${" + newName + ".pwd}";
			QString origVarNamePwd2 = "$" + origName + ".pwd";
			QString newVarNamePwd2 = "$" + newName + ".pwd";
			if(newVarNamePwd2.contains(" ")) {
				newVarNamePwd2 = "${" + newName + ".pwd}";
			}

            //Executable variables filename can reference identifiers
			for(int i=0;i<tableExecutables->rowCount();i++) {
				QTableWidgetItem *itemValue = tableExecutables->item(i,EXECUTABLES_COL_VALUE);
				QString value = itemValue->text();
				if(value.contains(origVarNameUid1) || value.contains(origVarNameUid2)) {
					value = value.replace(origVarNameUid1,newVarNameUid1);
					value = value.replace(origVarNameUid2,newVarNameUid2);
					itemValue->setText(value);
				}
				if(value.contains(origVarNamePwd1) || value.contains(origVarNamePwd2)) {
					value = value.replace(origVarNamePwd1,newVarNamePwd1);
					value = value.replace(origVarNamePwd2,newVarNamePwd2);
					itemValue->setText(value);
				}
			}
            //Executable variables args can reference identifiers
            for(int i=0;i<tableExecutables->rowCount();i++) {
                QTableWidgetItem *itemValue = tableExecutables->item(i,EXECUTABLES_COL_ARGS);
                QString value = itemValue->text();
                if(value.contains(origVarNameUid1) || value.contains(origVarNameUid2)) {
                    value = value.replace(origVarNameUid1,newVarNameUid1);
                    value = value.replace(origVarNameUid2,newVarNameUid2);
                    itemValue->setText(value);
                }
                if(value.contains(origVarNamePwd1) || value.contains(origVarNamePwd2)) {
                    value = value.replace(origVarNamePwd1,newVarNamePwd1);
                    value = value.replace(origVarNamePwd2,newVarNamePwd2);
                    itemValue->setText(value);
                }
            }

			//User Defined Variables can reference identifiers
			for(int i=0;i<tableUser->rowCount();i++) {
				QTableWidgetItem *itemValue = tableUser->item(i,USERDEF_COL_VALUE);
				QString value = itemValue->text();
				if(value.contains(origVarNameUid1) || value.contains(origVarNameUid2)) {
					value = value.replace(origVarNameUid1,newVarNameUid1);
					value = value.replace(origVarNameUid2,newVarNameUid2);
					itemValue->setText(value);
				}
				if(value.contains(origVarNamePwd1) || value.contains(origVarNamePwd2)) {
					value = value.replace(origVarNamePwd1,newVarNamePwd1);
					value = value.replace(origVarNamePwd2,newVarNamePwd2);
					itemValue->setText(value);
				}
			}
		}

	} // end - if(item->column() == PWD_COL_NAME) {

	tableIdents->blockSignals(false);
}

void VariableSettingsDialog::slotPwdAdd()
{
	if(!m_bEditingEnabled) return;
	tableIdents->blockSignals(true);

	int row = tableIdents->addRowBelowSelected();
	QTableWidgetItem *itemInUse = tableIdents->item(row,PWD_COL_INUSE);
    setReadOnly(itemInUse,  true);
	itemInUse->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
	itemInUse->setText("0");

	//create and store the UUID. Used to lookup statistics in m_statMap.
	QTableWidgetItem *itemName = tableIdents->item(row,PWD_COL_NAME);
	itemName->setData( QT_USERROLE_VARUUID, QVariant::fromValue(QUuid::createUuid().toString()) );
	tableIdents->setCurrentCell(row,PWD_COL_NAME);

	//store the name. Used to replace old var name with new var name in references
	QString itemTextUnique = ensureIsValidVarName(itemName, "new_var_name");
	itemName->setText(itemTextUnique);
	itemName->setData( QT_USERROLE_VARNAME, QVariant::fromValue(itemTextUnique) );

	QTableWidgetItem *itemLogin = tableIdents->item(row,PWD_COL_LOGIN);
	itemLogin->setText("username");

	QTableWidgetItem *itemPwd = tableIdents->item(row,PWD_COL_PWD);
	itemPwd->setText("password");

	//Updates the browse button rowIndex in all rows
	slotPwdUpdateTableWidgets();

	tableIdents->blockSignals(false);
}

void VariableSettingsDialog::slotPwdDelete()
{
	if(!m_bEditingEnabled) return;
	int row = tableIdents->getSelectedRowIndex();
	if(row == -1) return;
	QTableWidgetItem *itemName = tableIdents->item(row,PWD_COL_NAME);
    if(!(itemName->flags() & Qt::ItemIsEditable)) {
        return;
    }
	QMessageBox::StandardButton iRet = QMessageBox::question( this, "", "Confirm delete '"+itemName->text()+"' ?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes );
 	if ( iRet == QMessageBox::Yes ) {
		tableIdents->removeRow(row);
		//Updates the show pwd button rowIndex in all rows
		slotPwdUpdateTableWidgets();
	}
	tableIdents->setFocus(Qt::OtherFocusReason);
}

void VariableSettingsDialog::slotPwdShowPasswords(bool show)
{
    if(show) {
        bool doShow = true;
        if(m_masterPassword.isEmpty()) {
            doShow = false;
            //Ask for password
            PasswordDialog dlg(this);
            dlg.setMessage("Enter master password");
            int dlgStatus = dlg.exec();
            if(dlgStatus == QDialog::Accepted) {
                PasswordDb *pdb = PasswordDb::getInstance();
                if(pdb->comparePassword(dlg.password)) {
                    m_masterPassword = dlg.password;
                    doShow = true;
                } else {
                    buttonPwdShowAll->setChecked(false);
                    QMessageBox::critical(this, "Oops!", "Wrong password");
                }
            } else {
                buttonPwdShowAll->setChecked(false);
            }
        }
        if(doShow) {
            for(int row=0;row<tableIdents->rowCount();row++) {
                TableWidgetToolButton *btnShowPwd = qobject_cast<TableWidgetToolButton*>( tableIdents->cellWidget(row,PWD_COL_BUTTON) );
                btnShowPwd->setChecked(true);
            }
        }
    } else {
        for(int row=0;row<tableIdents->rowCount();row++) {
            TableWidgetToolButton *btnShowPwd = qobject_cast<TableWidgetToolButton*>( tableIdents->cellWidget(row,PWD_COL_BUTTON) );
            btnShowPwd->setChecked(false);
        }
    }
}

void VariableSettingsDialog::slotPwdEnableEditView(bool enable)
{

    if(enable) {
        //ask for master password ?
        if(m_masterPassword.isEmpty()) {
            enable = false;
            //Ask for password
            PasswordDialog dlg(this);
            dlg.setMessage("Enter master password");
            int dlgStatus = dlg.exec();
            if(dlgStatus == QDialog::Accepted) {
                PasswordDb *pdb = PasswordDb::getInstance();
                if(pdb->comparePassword(dlg.password)) {
                    m_masterPassword = dlg.password;
                    enable = true;
                } else {
                    btnPwdEnableEdit->blockSignals(true);
                    btnPwdEnableEdit->setChecked(false);
                    btnPwdEnableEdit->blockSignals(false);
                    QMessageBox::critical(this, "Oops!", "Wrong password");
                }
            } else {
                btnPwdEnableEdit->blockSignals(true);
                btnPwdEnableEdit->setChecked(false);
                btnPwdEnableEdit->blockSignals(false);
            }
        }
    }


    tableIdents->blockSignals(true);
    for(int row=0;row<tableIdents->rowCount();row++) {
        QTableWidgetItem *itemName = tableIdents->item(row,PWD_COL_NAME);
        QTableWidgetItem *itemLogin = tableIdents->item(row,PWD_COL_LOGIN);
        QTableWidgetItem *itemPwd = tableIdents->item(row,PWD_COL_PWD);
        TableWidgetLineEdit *editPwd = qobject_cast<TableWidgetLineEdit*>(tableIdents->cellWidget(row,PWD_COL_PWD));
        if(enable) {
            setReadOnly(itemName,  false);
            setReadOnly(itemLogin,  false);
            setReadOnly(itemPwd,  false);
            editPwd->setReadOnly(false);
        } else {
            setReadOnly(itemName,  true);
            setReadOnly(itemLogin,  true);
            setReadOnly(itemPwd,  true);
            editPwd->setReadOnly(true);
        }
    }

    tableIdents->blockSignals(false);

    if(enable) {
        btnPwdEnableEdit->setText("click me to disable editing");
    } else {
        btnPwdEnableEdit->setText("click me to enable editing");
    }

    slotPwdUpdateControls();

}

void VariableSettingsDialog::slotPwdShowPassword(bool show, int row, int col)
{
	Q_UNUSED(col);
	TableWidgetLineEdit *editPwd = qobject_cast<TableWidgetLineEdit*>(tableIdents->cellWidget(row,PWD_COL_PWD));
    TableWidgetToolButton *btn = qobject_cast<TableWidgetToolButton*>( tableIdents->cellWidget(row,PWD_COL_BUTTON) );
	if(show) {
        bool doShow = true;
        if(m_masterPassword.isEmpty()) {
            doShow = false;
            //Ask for password
            PasswordDialog dlg(this);
            dlg.setMessage("Enter master password");
            int dlgStatus = dlg.exec();
            if(dlgStatus == QDialog::Accepted) {
                PasswordDb *pdb = PasswordDb::getInstance();
                if(pdb->comparePassword(dlg.password)) {
                    m_masterPassword = dlg.password;
                    doShow = true;
                } else {
                    btn->setChecked(false);
                    QMessageBox::critical(this, "Oops!", "Wrong password");
                }
            } else {
                btn->setChecked(false);
            }
        }
        if(doShow) {
            editPwd->setEchoMode(QLineEdit::Normal);
        }
	} else {
        qDebug() << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
		editPwd->setEchoMode(QLineEdit::Password);
	}
}

void VariableSettingsDialog::slotPwdUpdateControls()
{
	//enable/disable buttons
	btnPwdAdd->setEnabled(false);
	btnPwdDelete->setEnabled(false);
    btnPwdEnableEdit->setEnabled(false);
    if(m_bEditingEnabled) {
        btnPwdEnableEdit->setEnabled(true);
        if(btnPwdEnableEdit->isChecked()) {
            btnPwdAdd->setEnabled(true);
            int row = tableIdents->getSelectedRowIndex();
            if(row != -1) {
                btnPwdDelete->setEnabled(true);
            }
        }
	}
}

void VariableSettingsDialog::slotPwdUpdateTableWidgets()
{
	//update our show password buttons
	for(int row=0;row<tableIdents->rowCount();row++) {
		TableWidgetToolButton *btn = qobject_cast<TableWidgetToolButton*>( tableIdents->cellWidget(row,PWD_COL_BUTTON) );
		if(btn != NULL) {
			btn->setRowIndex(row);
		} else {
			btn = new TableWidgetToolButton(tableIdents,row,PWD_COL_BUTTON);
			btn->setIcon(Images::icon("pw_040_btn_view"));
			btn->setCheckable(true);
			btn->setChecked(false);
			btn->setToolTip("Click to show the password");
			btn->setAutoRaise(true);
			tableIdents->setCellWidget(row,PWD_COL_BUTTON,btn);
            ATVERIFY( connect( btn,	&TableWidgetToolButton::signalToggled,  this, &VariableSettingsDialog::slotPwdShowPassword ) );
            ATVERIFY( connect( btn,	&TableWidgetToolButton::signalClicked,  tableIdents, &TableWidget::slotSetCurrentCell ) );
		}

		TableWidgetLineEdit *editPwd = qobject_cast<TableWidgetLineEdit*>(tableIdents->cellWidget(row,PWD_COL_PWD));
		if(editPwd != NULL) {
			editPwd->setRowIndex(row);
		} else {
			editPwd = new TableWidgetLineEdit(tableIdents,row,PWD_COL_PWD);
			editPwd->setFrame(false);
			editPwd->setText(tableIdents->item(row, PWD_COL_PWD)->text());
			tableIdents->item(row, PWD_COL_PWD)->setText("");
			tableIdents->setCellWidget(row, PWD_COL_PWD, editPwd);
            ATVERIFY( connect( editPwd,	&TableWidgetLineEdit::setCurrentCell, tableIdents, &TableWidget::slotSetCurrentCell ) );
		}

        editPwd->setReadOnly(!(m_bEditingEnabled && btnPwdEnableEdit->isChecked()));

		if(buttonPwdShowAll->isChecked()) {
			btn->setChecked(true);
			editPwd->setEchoMode(QLineEdit::Normal);
		} else {
			btn->setChecked(false);
			editPwd->setEchoMode(QLineEdit::Password);
		}
    }
}

void VariableSettingsDialog::slotExecUpdateBrowseButtons()
{
	//update our browse buttons
#ifdef Q_OS_MAC
    tableExecutables->hideColumn(EXECUTABLES_COL_BUTTON);
#else
	for(int row=0;row<tableExecutables->rowCount();row++) {
		TableWidgetToolButton *btn = static_cast<TableWidgetToolButton*>( tableExecutables->cellWidget(row,EXECUTABLES_COL_BUTTON) );
		if(btn != NULL) {
			btn->setRowIndex(row);
		} else {
			btn = new TableWidgetToolButton(this,row,EXECUTABLES_COL_BUTTON);
			btn->setText("...");
			btn->setToolTip("Click to browse for file...");
			btn->setAutoRaise(true);
			tableExecutables->setCellWidget(row,EXECUTABLES_COL_BUTTON,btn);
            ATVERIFY( connect( btn,	&TableWidgetToolButton::signalClicked, this, &VariableSettingsDialog::slotExecBrowse ) );
		}
		btn->setEnabled(m_bEditingEnabled);
    }
#endif
}

void VariableSettingsDialog::slotExecBrowse(int row, int column)
{
	Q_UNUSED(column);
	tableExecutables->setCurrentCell(row,EXECUTABLES_COL_VALUE);

	QTableWidgetItem *itemValue = tableExecutables->item(row,EXECUTABLES_COL_VALUE);
	QString strCurrentFilename = itemValue->text();
    for(int i=0;i<3;i++) {
        strCurrentFilename = replaceExecutableVarsFile(strCurrentFilename);
        strCurrentFilename = replaceUserDefVars(strCurrentFilename);
    }

	QFileInfo fi(strCurrentFilename);

    QString strCurrentDir;
    //see if parent folder exists
    if(QFile::exists(fi.absolutePath())) {
        strCurrentDir = fi.absolutePath();
    }

    if(QFile::exists(fi.absoluteFilePath())) {
        //exists and is either dir or file
        if(fi.isDir()) {
            QString strFilename = QFileDialog::getExistingDirectory( this, "Select directory", strCurrentDir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks );
            if ( !strFilename.isNull() ) {
                itemValue->setText( strFilename );
            }
        } else {
            QString strFilename = QFileDialog::getOpenFileName( this, "Select executable file", strCurrentDir, QString(), 0, QFileDialog::DontResolveSymlinks );
            if ( !strFilename.isNull() ) {
                itemValue->setText( strFilename );
            }
        }
    } else {
        //folder or file does not exist
        QString strFilename = QFileDialog::getOpenFileName( this, "Select executable file", strCurrentDir, QString(), 0, QFileDialog::DontResolveSymlinks );
        if ( !strFilename.isNull() ) {
            itemValue->setText( strFilename );
        }
    }
}

void VariableSettingsDialog::slotExecAdd()
{
	if(!m_bEditingEnabled) return;
    disconnect( tableExecutables, &TableWidget::itemChanged, this, &VariableSettingsDialog::slotExecItemChanged );

	int row = tableExecutables->addRowBelowSelected();
	QTableWidgetItem *itemInUse = tableExecutables->item(row,EXECUTABLES_COL_INUSE);
    setReadOnly(itemInUse,  true);
	itemInUse->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
	itemInUse->setText("0");
	tableExecutables->setCurrentCell(row,EXECUTABLES_COL_NAME);

	//create and store the UUID. Used to lookup statistics in m_statMap.
	QTableWidgetItem *itemName = tableExecutables->item(row,EXECUTABLES_COL_NAME);
	itemName->setData( QT_USERROLE_VARUUID, QVariant::fromValue(QUuid::createUuid().toString()) );

	//store the name. Used to replace old var name with new var name in references
	QString itemTextUnique = ensureIsValidVarName(itemName, "new_var_name");
	itemName->setText(itemTextUnique);
	itemName->setData( QT_USERROLE_VARNAME, QVariant::fromValue(itemTextUnique) );

	QTableWidgetItem *itemValue = tableExecutables->item(row,EXECUTABLES_COL_VALUE);
	itemValue->setText("new var value");

    QTableWidgetItem *itemArgs = tableExecutables->item(row,EXECUTABLES_COL_ARGS);
    itemArgs->setText("");

	//Updates the browse button rowIndex in all rows
	slotExecUpdateBrowseButtons();

    ATVERIFY( connect( tableExecutables, &TableWidget::itemChanged, this, &VariableSettingsDialog::slotExecItemChanged ) );
}

void VariableSettingsDialog::slotExecDelete()
{
	if(!m_bEditingEnabled) return;
	int row = tableExecutables->getSelectedRowIndex();
	if(row == -1) return;
	QTableWidgetItem *itemName = tableExecutables->item(row,EXECUTABLES_COL_NAME);
	QMessageBox::StandardButton iRet = QMessageBox::question( this, "", "Confirm delete '"+itemName->text()+"' ?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes );
 	if ( iRet == QMessageBox::Yes ) {
		tableExecutables->removeRow(row);
		//Updates the browse button rowIndex in all rows
		slotExecUpdateBrowseButtons();
	}
	tableExecutables->setFocus(Qt::OtherFocusReason);
}

void VariableSettingsDialog::slotExecMoveUp()
{
	if(!m_bEditingEnabled) return;
    disconnect( tableExecutables, &TableWidget::itemChanged, this, &VariableSettingsDialog::slotExecItemChanged );

	tableExecutables->moveSelectedRowUp();

	//Updates the browse button rowIndex in all rows
	slotExecUpdateBrowseButtons();

    ATVERIFY( connect( tableExecutables, &TableWidget::itemChanged, this, &VariableSettingsDialog::slotExecItemChanged ) );
}

void VariableSettingsDialog::slotExecMoveDown()
{
	if(!m_bEditingEnabled) return;
    disconnect( tableExecutables, &TableWidget::itemChanged, this, &VariableSettingsDialog::slotExecItemChanged );

	tableExecutables->moveSelectedRowDown();

	//Updates the browse button rowIndex in all rows
	slotExecUpdateBrowseButtons();

    ATVERIFY( connect( tableExecutables, &TableWidget::itemChanged, this, &VariableSettingsDialog::slotExecItemChanged ) );
}

void VariableSettingsDialog::slotExecUpdateControls()
{
	//enable/disable buttons
	btnExecAdd->setEnabled(false);
	btnExecDelete->setEnabled(false);
	btnExecMoveUp->setEnabled(false);
	btnExecMoveDown->setEnabled(false);
	if(m_bEditingEnabled) {
		btnExecAdd->setEnabled(true);
		int row = tableExecutables->getSelectedRowIndex();
		if(row != -1) {
			btnExecDelete->setEnabled(true);
			if(!tableExecutables->isFirstRow(row)) {
				btnExecMoveUp->setEnabled(true);
			}
			if(!tableExecutables->isLastRow(row)) {
				btnExecMoveDown->setEnabled(true);
			}
		}
	}
}

void VariableSettingsDialog::slotExecItemChanged(QTableWidgetItem *item)
{
	qDebug() << "slotExecItemChanged slotExecItemChanged slotExecItemChanged " << item->text();

	//since we change ourselves, _DO_NOT_ trigger change event while we are in here
    disconnect( tableExecutables, &TableWidget::itemChanged, this, &VariableSettingsDialog::slotExecItemChanged );

	//Trim
	QString itemText = item->text();
	if(itemText.startsWith(" ") || itemText.endsWith(" ")) {
		itemText = itemText.trimmed();
		item->setText(itemText);
	}

	if(item->column() == EXECUTABLES_COL_NAME) {

		//We are dealing with variable names only here

		//Ensures
		// - not empty
		// - unique
		QString itemTextUnique = ensureIsValidVarName(item, itemText);
		if(itemTextUnique != itemText) {
			item->setText(itemTextUnique);
		}

		QString newName = item->text();
		
		QVariant v = item->data(QT_USERROLE_VARNAME);
		ATASSERT(!v.isNull());
		QString origName = v.value<QString>();
		item->setData( QT_USERROLE_VARNAME, QVariant::fromValue(newName) );

		if(!origName.isEmpty() && !newName.isEmpty() && origName != newName)
		{
			QString origVarName1 = "${" + origName + "}";
			QString newVarName1 = "${" + newName + "}";
			QString origVarName2 = "$" + origName;
			QString newVarName2 = "$" + newName;
			if(newName.contains(" ")) {
				newVarName2 = "${" + newName + "}";
			}

            //Executable variable filename can reference other executable variables
			for(int i=0;i<tableExecutables->rowCount();i++) {
				QTableWidgetItem *itemValue = tableExecutables->item(i,EXECUTABLES_COL_VALUE);
				QString value = itemValue->text();
				if(value.contains(origVarName1) || value.contains(origVarName2)) {
					value = value.replace(origVarName1,newVarName1);
					value = value.replace(origVarName2,newVarName2);
					itemValue->setText(value);
                }
			}
            //Executable variable args can reference other executable variables
            for(int i=0;i<tableExecutables->rowCount();i++) {
                QTableWidgetItem *itemValue = tableExecutables->item(i,EXECUTABLES_COL_ARGS);
                QString value = itemValue->text();
                if(value.contains(origVarName1) || value.contains(origVarName2)) {
                    value = value.replace(origVarName1,newVarName1);
                    value = value.replace(origVarName2,newVarName2);
                    itemValue->setText(value);
                }
            }

			//User Defined Variables can reference executable variables
			for(int i=0;i<tableUser->rowCount();i++) {
				QTableWidgetItem *itemValue = tableUser->item(i,USERDEF_COL_VALUE);
				QString value = itemValue->text();
				if(value.contains(origVarName1) || value.contains(origVarName2)) {
					value = value.replace(origVarName1,newVarName1);
					value = value.replace(origVarName2,newVarName2);
					itemValue->setText(value);
				}
			}
		}

	} // end - if(item->column() == EXECUTABLES_COL_NAME) {
	else if(item->column() == EXECUTABLES_COL_VALUE) {

		updateStatistics();

	} // end - if(item->column() == EXECUTABLES_COL_VALUE) {
    else if(item->column() == EXECUTABLES_COL_ARGS) {

        updateStatistics();

    } // end - if(item->column() == EXECUTABLES_COL_ARGS) {

    ATVERIFY( connect( tableExecutables, &TableWidget::itemChanged, this, &VariableSettingsDialog::slotExecItemChanged ) );
}

void VariableSettingsDialog::slotUserBrowse(int row, int column)
{
	Q_UNUSED(column);
	tableUser->setCurrentCell(row,USERDEF_COL_VALUE);

	QTableWidgetItem *itemValue = tableUser->item(row,USERDEF_COL_VALUE);
	QString strCurrentFilename = itemValue->text();
	QString strCurrentDir;
	QFileInfo fi(strCurrentFilename);
	if(QFile::exists(fi.absolutePath())) {
		strCurrentDir = fi.absolutePath();
	}

	QString strFilename = QFileDialog::getOpenFileName( this, "Select file", strCurrentDir );

	if ( !strFilename.isNull() )
	{
		itemValue->setText( strFilename );
	}
}

void VariableSettingsDialog::slotUserAdd()
{
	if(!m_bEditingEnabled) return;
    disconnect( tableUser, &TableWidget::itemChanged, this, &VariableSettingsDialog::slotUserItemChanged );

	int row = tableUser->addRowBelowSelected();
	QTableWidgetItem *itemInUse = tableUser->item(row,USERDEF_COL_INUSE);
     setReadOnly(itemInUse,  true);
	itemInUse->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter|Qt::AlignCenter);
	itemInUse->setText("0");
	tableUser->setCurrentCell(row,USERDEF_COL_NAME);

	//create and store the UUID. Used to lookup statistics in m_statMap.
	QTableWidgetItem *itemName = tableUser->item(row,USERDEF_COL_NAME);
	itemName->setData( QT_USERROLE_VARUUID, QVariant::fromValue(QUuid::createUuid().toString()) );

	//store the name. Used to replace old var name with new var name in references
	QString itemTextUnique = ensureIsValidVarName(itemName, "new_var_name");
	itemName->setText(itemTextUnique);
	itemName->setData( QT_USERROLE_VARNAME, QVariant::fromValue(itemTextUnique) );

	QTableWidgetItem *itemValue = tableUser->item(row,USERDEF_COL_VALUE);
	itemValue->setText("new var value");

	//Updates the browse button rowIndex in all rows
	slotUserUpdateBrowseButtons();

    ATVERIFY( connect( tableUser, &TableWidget::itemChanged, this, &VariableSettingsDialog::slotUserItemChanged ) );
}
 
void VariableSettingsDialog::slotUserDelete()
{
	if(!m_bEditingEnabled) return;
	int row = tableUser->getSelectedRowIndex();
	if(row == -1) return;
	QTableWidgetItem *itemName = tableUser->item(row,USERDEF_COL_NAME);
	QMessageBox::StandardButton iRet = QMessageBox::question( this, "", "Confirm delete '"+itemName->text()+"' ?", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes );
 	if ( iRet == QMessageBox::Yes ) {
		tableUser->removeRow(row);
		//Updates the browse button rowIndex in all rows
		slotUserUpdateBrowseButtons();
	}
	tableUser->setFocus(Qt::OtherFocusReason);
}

void VariableSettingsDialog::slotUserMoveUp()
{
	if(!m_bEditingEnabled) return;
    disconnect( tableUser, &TableWidget::itemChanged, this, &VariableSettingsDialog::slotUserItemChanged );

	tableUser->moveSelectedRowUp();

	//Updates the browse button rowIndex in all rows
	slotUserUpdateBrowseButtons();

    ATVERIFY( connect( tableUser, &TableWidget::itemChanged, this, &VariableSettingsDialog::slotUserItemChanged ) );
}

void VariableSettingsDialog::slotUserMoveDown()
{
	if(!m_bEditingEnabled) return;
    disconnect( tableUser, &TableWidget::itemChanged, this, &VariableSettingsDialog::slotUserItemChanged );

	tableUser->moveSelectedRowDown();

	//Updates the browse button rowIndex in all rows
	slotUserUpdateBrowseButtons();

    ATVERIFY( connect( tableUser, &TableWidget::itemChanged, this, &VariableSettingsDialog::slotUserItemChanged ) );
}

void VariableSettingsDialog::slotUserUpdateControls()
{
	//enable/disable buttons
	btnUserAdd->setEnabled(false);
	btnUserDelete->setEnabled(false);
	btnUserMoveUp->setEnabled(false);
	btnUserMoveDown->setEnabled(false);
	int row = tableUser->getSelectedRowIndex();
	if(m_bEditingEnabled) {
		btnUserAdd->setEnabled(true);
		if(row != -1) {
			btnUserDelete->setEnabled(true);
			if(!tableUser->isFirstRow(row)) {
				btnUserMoveUp->setEnabled(true);
			}
			if(!tableUser->isLastRow(row)) {
				btnUserMoveDown->setEnabled(true);
			}
		}
	}
}

void VariableSettingsDialog::slotUserUpdateBrowseButtons()
{
	//update our browse buttons
    tableUser->hideColumn(USERDEF_COL_BUTTON);
}

void VariableSettingsDialog::slotUserItemChanged(QTableWidgetItem *item)
{
	qDebug() << "slotUserItemChanged slotUserItemChanged slotUserItemChanged " << item->text();

	//since we change ourselves, _DO_NOT_ trigger change event while we are in here
    disconnect( tableUser, &TableWidget::itemChanged, this, &VariableSettingsDialog::slotUserItemChanged );

	//Trim
	QString itemText = item->text();
	if(itemText.startsWith(" ") || itemText.endsWith(" ")) {
		itemText = itemText.trimmed();
		item->setText(itemText);
	}

	if(item->column() == USERDEF_COL_NAME) {

		//We are dealing with variable names only here

		//Ensures
		// - not empty
		// - unique
		QString itemTextUnique = ensureIsValidVarName(item, itemText);
		if(itemTextUnique != itemText) {
			item->setText(itemTextUnique);
		}

		QString newName = item->text();
		
		QVariant v = item->data(QT_USERROLE_VARNAME);
		ATASSERT(!v.isNull());
		QString origName = v.value<QString>();
		item->setData( QT_USERROLE_VARNAME, QVariant::fromValue(newName) );

		if(!origName.isEmpty() && !newName.isEmpty() && origName != newName)
		{
			QString origVarName1 = "${" + origName + "}";
			QString newVarName1 = "${" + newName + "}";
			QString origVarName2 = "$" + origName;
			QString newVarName2 = "$" + newName;
			if(newName.contains(" ")) {
				newVarName2 = "${" + newName + "}";
			}

            //User defined variables can reference other user defined variables
			for(int i=0;i<tableUser->rowCount();i++) {
				QTableWidgetItem *itemValue = tableUser->item(i,USERDEF_COL_VALUE);
				QString value = itemValue->text();
				if(value.contains(origVarName1) || value.contains(origVarName2)) {
					value = value.replace(origVarName1,newVarName1);
					value = value.replace(origVarName2,newVarName2);
					itemValue->setText(value);
				}
			}

            //Executable variables filename can reference user defined variables
			for(int i=0;i<tableExecutables->rowCount();i++) {
				QTableWidgetItem *itemValue = tableExecutables->item(i,EXECUTABLES_COL_VALUE);
				QString value = itemValue->text();
				if(value.contains(origVarName1) || value.contains(origVarName2)) {
					value = value.replace(origVarName1,newVarName1);
					value = value.replace(origVarName2,newVarName2);
					itemValue->setText(value);
				}
			}
            //Executable variables args can reference user defined variables
            for(int i=0;i<tableExecutables->rowCount();i++) {
                QTableWidgetItem *itemValue = tableExecutables->item(i,EXECUTABLES_COL_ARGS);
                QString value = itemValue->text();
                if(value.contains(origVarName1) || value.contains(origVarName2)) {
                    value = value.replace(origVarName1,newVarName1);
                    value = value.replace(origVarName2,newVarName2);
                    itemValue->setText(value);
                }
            }

		}

	} // end - if(item->column() == USERDEF_COL_NAME) {
	else if(item->column() == USERDEF_COL_VALUE) {

		updateStatistics();

	} // end - if(item->column() == USERDEF_COL_VALUE) {

    ATVERIFY( connect( tableUser, &TableWidget::itemChanged, this, &VariableSettingsDialog::slotUserItemChanged ) );
}



