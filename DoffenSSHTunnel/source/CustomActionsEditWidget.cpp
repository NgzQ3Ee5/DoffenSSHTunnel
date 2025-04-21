#include "ATSkeleton.h"
#include "CustomActionsEditWidget.h"
#include "CustomAction.h"
#include "CustomButtonDialog.h"
#include "Backup.h"
#include "Images.h"

CustomActionsEditWidget::CustomActionsEditWidget(QWidget *parent)
    : QWidget(parent)
{
	m_pTunnel = NULL;
	m_pSkeletonWindow = NULL;
	m_pActionExecute = NULL;
}

void CustomActionsEditWidget::setup(ATSkeletonWindow *pSkeletonWindow)
{
	m_pSkeletonWindow = pSkeletonWindow;
	m_pDlgEdit = new CustomButtonDialog(this, pSkeletonWindow);
	m_pDlgEdit->setModal(true);

	m_pSkeletonWindow->ui.treeCustomActionsEdit->setColumnWidth(COL_LABEL,150);
	m_pSkeletonWindow->ui.treeCustomActionsEdit->setColumnWidth(COL_COMMAND,300);

	m_pSkeletonWindow->ui.treeCustomActionsEdit->setAllowItemsToBeParents(false);
	m_pSkeletonWindow->ui.treeCustomActionsEdit->setupStandardControls();

	QMenu *treeContextMenu = m_pSkeletonWindow->ui.treeCustomActionsEdit->contextMenu();
	QAction *pActionEdit = m_pSkeletonWindow->ui.treeCustomActionsEdit->actionEdit();
	pActionEdit->setText("&Edit Wizard...");
    m_pSkeletonWindow->ui.treeCustomActionsEdit->actionInsertAfter()->setText("&Insert Action After");
#ifdef Q_OS_MACOS
    m_pSkeletonWindow->ui.treeCustomActionsEdit->actionInsertAfter()->setText(
        m_pSkeletonWindow->ui.treeCustomActionsEdit->actionInsertAfter()->text() + " (Cmd+N)");
#else
    m_pSkeletonWindow->ui.treeCustomActionsEdit->actionInsertAfter()->setText(
        m_pSkeletonWindow->ui.treeCustomActionsEdit->actionInsertAfter()->text() + " (Ctrl+N)");
#endif
	m_pSkeletonWindow->ui.treeCustomActionsEdit->actionInsertBefore()->setText("Insert Action Before");
	m_pSkeletonWindow->ui.treeCustomActionsEdit->actionInsertChild()->setText("Insert Action Child");
	//add my actions
	QAction *pSeparator = treeContextMenu->insertSeparator(pActionEdit);
    m_pActionExecute = new QAction("E&xecute",treeContextMenu);
#ifdef Q_OS_MACOS
    m_pActionExecute->setText(m_pActionExecute->text() + " (Dbl Click)");
#else
     m_pActionExecute->setText(m_pActionExecute->text() + " (Return Key / Dbl Click");
#endif
	treeContextMenu->insertAction(pSeparator, m_pActionExecute);

	ATVERIFY( connect( m_pDlgEdit, SIGNAL( accepted() ), this, SLOT( slotEditDialogAccepted() ), Qt::QueuedConnection ) );

	ATVERIFY( connect( m_pSkeletonWindow->ui.treeCustomActionsEdit,	SIGNAL( signalUpdateControls() ),	this, SLOT( slotUpdateControls() ), Qt::UniqueConnection ) );
	ATVERIFY( connect( m_pSkeletonWindow->ui.treeCustomActionsEdit,	SIGNAL( signalBeforeShowingContextMenu() ),	this, SLOT( slotBeforeShowingContextMenu() ), Qt::UniqueConnection ) );
	ATVERIFY( connect( m_pSkeletonWindow->ui.treeCustomActionsEdit,	SIGNAL( signalAddItem(int) ),		this, SLOT( slotAddItem(int) ), Qt::UniqueConnection ) );
	ATVERIFY( connect( m_pSkeletonWindow->ui.treeCustomActionsEdit,	SIGNAL( signalCreatedNewFolder(QTreeWidgetItem*) ), this, SLOT( slotCreatedNewFolder(QTreeWidgetItem*) ), Qt::UniqueConnection ) );
	ATVERIFY( connect( m_pSkeletonWindow->ui.treeCustomActionsEdit,	SIGNAL( signalAfterPasteItem(QTreeWidgetItem*,QTreeWidgetItem*,int,int) ), this, SLOT( slotAfterPasteItem(QTreeWidgetItem*,QTreeWidgetItem*,int,int) ), Qt::UniqueConnection ) );
	ATVERIFY( connect( m_pSkeletonWindow->ui.btnCustomActionAdd,	SIGNAL( clicked() ),				this, SLOT( slotAddItem() ), Qt::UniqueConnection ) );
	ATVERIFY( connect( m_pSkeletonWindow->ui.treeCustomActionsEdit,	SIGNAL( signalEditItem() ),			this, SLOT( slotEditItem() ), Qt::UniqueConnection ) );
	ATVERIFY( connect( m_pDlgEdit,								SIGNAL( signalActionExec(const CustomActionStruct&) ),	this, SLOT( slotExecute(const CustomActionStruct&) ), Qt::UniqueConnection ) );
	ATVERIFY( connect( m_pActionExecute,							SIGNAL( triggered() ),				this, SLOT( slotExecute() ), Qt::UniqueConnection ) );
	ATVERIFY( connect( m_pSkeletonWindow->ui.treeCustomActionsEdit,	SIGNAL( itemActivated(QTreeWidgetItem*, int) ),	this, SLOT( slotExecute() ), Qt::UniqueConnection ) );
	ATVERIFY( connect( m_pSkeletonWindow->ui.btnCustomActionEdit,	SIGNAL( clicked() ),				this, SLOT( slotEditItem() ), Qt::UniqueConnection ) );
	ATVERIFY( connect( m_pSkeletonWindow->ui.btnCustomActionDelete, SIGNAL( clicked() ),				m_pSkeletonWindow->ui.treeCustomActionsEdit, SLOT( slotDelete() ), Qt::UniqueConnection ) );
	ATVERIFY( connect( m_pSkeletonWindow->ui.btnCustomActionDuplicate, SIGNAL( clicked() ),				m_pSkeletonWindow->ui.treeCustomActionsEdit, SLOT( slotDuplicate() ), Qt::UniqueConnection ) );
	ATVERIFY( connect( m_pSkeletonWindow->ui.btnCustomActionUp,		SIGNAL( clicked() ),				m_pSkeletonWindow->ui.treeCustomActionsEdit, SLOT( slotMoveUp() ), Qt::UniqueConnection ) );
	ATVERIFY( connect( m_pSkeletonWindow->ui.btnCustomActionDown,	SIGNAL( clicked() ),				m_pSkeletonWindow->ui.treeCustomActionsEdit, SLOT( slotMoveDown() ), Qt::UniqueConnection ) );
	ATVERIFY( connect( m_pSkeletonWindow->ui.btnCustomActionLeft,	SIGNAL( clicked() ),				m_pSkeletonWindow->ui.treeCustomActionsEdit, SLOT( slotMoveLeft() ), Qt::UniqueConnection ) );
	ATVERIFY( connect( m_pSkeletonWindow->ui.btnCustomActionRight,	SIGNAL( clicked() ),				m_pSkeletonWindow->ui.treeCustomActionsEdit, SLOT( slotMoveRight() ), Qt::UniqueConnection ) );

	ATVERIFY( connect( m_pSkeletonWindow->ui.treeCustomActionsEdit, SIGNAL( signalSave() ), this, SLOT( slotSave() ), Qt::QueuedConnection ) );

	ATVERIFY( connect( m_pSkeletonWindow->ui.treeCustomActionsEdit, SIGNAL( itemCollapsed(QTreeWidgetItem*) ), this, SLOT( slotItemCollapsed(QTreeWidgetItem*) ) ) );
	ATVERIFY( connect( m_pSkeletonWindow->ui.treeCustomActionsEdit, SIGNAL( itemExpanded(QTreeWidgetItem*) ), this, SLOT( slotItemExpanded(QTreeWidgetItem*) ) ) );

	slotUpdateControls();
}
 
void CustomActionsEditWidget::setData(Tunnel_c *pTunnel)
{
	ATASSERT(pTunnel);

    qDebug() << Q_FUNC_INFO << "Host:" << pTunnel->strName;

	m_pTunnel = pTunnel;

	m_pSkeletonWindow->ui.treeCustomActionsEdit->setModifiedAndSaveSignalEnabled(false);

	m_pSkeletonWindow->ui.treeCustomActionsEdit->clear(); 
	QTreeWidgetItem* lastTwi = NULL;
	int lastLevel = 0;
	for(int i=0;i<m_pTunnel->customActionList.size();i++) {
		CustomActionStruct actionInfo = m_pTunnel->customActionList.at(i);
		int level = actionInfo.iLevel;
		QTreeWidgetItem *twi = NULL;
		if(lastTwi != NULL && level > 0) {
			if(level > lastLevel) {
				twi = new QTreeWidgetItem( lastTwi );
			} else if(level == lastLevel) {
				QTreeWidgetItem* parentTwi = lastTwi->parent();
				if(parentTwi != NULL) {
					twi = new QTreeWidgetItem( parentTwi );
				} else {
					twi = new QTreeWidgetItem( m_pSkeletonWindow->ui.treeCustomActionsEdit );
				}
			} else {
				QTreeWidgetItem* parentTwi = lastTwi->parent();
				while(parentTwi != NULL && level <= TreeWidget::getItemLevel(parentTwi) ) {
					parentTwi = parentTwi->parent();
				}
				if(parentTwi != NULL) {
					twi = new QTreeWidgetItem( parentTwi );
				} else {
					twi = new QTreeWidgetItem( m_pSkeletonWindow->ui.treeCustomActionsEdit );
				}
			}
		} else {
			twi = new QTreeWidgetItem( m_pSkeletonWindow->ui.treeCustomActionsEdit );
		}

		twi->setText(COL_LABEL, actionInfo.sLabel);
		//twi->setFont(COL_LABEL,QFont(QString::fromUtf8("Tahoma"),10));

		twi->setText(COL_COMMAND, actionInfo.sCmd);
		//twi->setFont(COL_COMMAND,QFont(QString::fromUtf8("Courier New"),10));

		lastTwi = twi;
		lastLevel = level;
		twi->setExpanded(actionInfo.bExpanded);
		twi->setFlags(twi->flags() | Qt::ItemIsEditable);
		setUUuid(twi,actionInfo.uUid);
		if(actionInfo.iType == CUSTOM_ACTION_TYPE_FOLDER) {
			m_pSkeletonWindow->ui.treeCustomActionsEdit->setIsFolder(twi, true);
			if(twi->isExpanded()) {
				twi->setIcon( COL_LABEL, Images::icon("ca_090_folder_open") );
			} else {
				twi->setIcon( COL_LABEL, Images::icon("ca_100_folder_closed") );
			}
		} else {
			twi->setIcon( COL_LABEL, Images::icon("ca_110_action") );
		}
	}

	slotUpdateControls();

	m_pSkeletonWindow->ui.treeCustomActionsEdit->setModifiedAndSaveSignalEnabled(true);
}

void CustomActionsEditWidget::slotLoadIcons()
{
	bool enabled = m_pSkeletonWindow->ui.treeCustomActionsEdit->setModifiedAndSaveSignalEnabled(false);
    QList<QTreeWidgetItem*> treeItems = m_pSkeletonWindow->ui.treeCustomActionsEdit->findItems(".*", Qt::MatchFlags(Qt::MatchRegularExpression | Qt::MatchRecursive), 0);
	for(int i=0;i<treeItems.size();i++) {
		QTreeWidgetItem* twi = treeItems.at(i);
		if(TreeWidget::isFolder(twi)) {
			if(twi->isExpanded()) {
				twi->setIcon( COL_LABEL, Images::icon("ca_090_folder_open") );
			} else {
				twi->setIcon( COL_LABEL, Images::icon("ca_100_folder_closed") );
			}
		} else {
			twi->setIcon( COL_LABEL, Images::icon("ca_110_action") );
		}
	}
	m_pSkeletonWindow->ui.treeCustomActionsEdit->setModifiedAndSaveSignalEnabled(enabled);

	m_pSkeletonWindow->ui.btnCustomActionAdd->setIcon(Images::icon("ca_010_btn_add_action"));
	m_pSkeletonWindow->ui.btnCustomActionEdit->setIcon(Images::icon("ca_020_btn_edit"));
	m_pSkeletonWindow->ui.btnCustomActionDuplicate->setIcon(Images::icon("ca_030_btn_dup"));
	m_pSkeletonWindow->ui.btnCustomActionDelete->setIcon(Images::icon("ca_040_btn_del"));
	m_pSkeletonWindow->ui.btnCustomActionLeft->setIcon(Images::icon("ca_050_btn_left"));
	m_pSkeletonWindow->ui.btnCustomActionUp->setIcon(Images::icon("ca_060_btn_up"));
	m_pSkeletonWindow->ui.btnCustomActionDown->setIcon(Images::icon("ca_070_btn_down"));
	m_pSkeletonWindow->ui.btnCustomActionRight->setIcon(Images::icon("ca_080_btn_right"));
}

QList<CustomActionStruct> CustomActionsEditWidget::getData()
{
	qDebug() << Q_FUNC_INFO;

	QList<CustomActionStruct> data;

    QList<QTreeWidgetItem*> treeItems = m_pSkeletonWindow->ui.treeCustomActionsEdit->findItems(".*", Qt::MatchFlags(Qt::MatchRegularExpression | Qt::MatchRecursive), 0);
	for(int i=0;i<treeItems.size();i++) {
		QTreeWidgetItem* twi = treeItems.at(i);
		CustomActionStruct cas;
		QUuid uuid = getUUuid(twi);
		if(uuid.isNull()) {
			uuid = QUuid::createUuid();
		}
		cas.uUid = uuid;
		cas.sLabel = twi->text(COL_LABEL);
		cas.sCmd = twi->text(COL_COMMAND);
		cas.iLevel = TreeWidget::getItemLevel(twi);
		cas.bExpanded = twi->isExpanded();
		if(twi->childCount() > 0 || TreeWidget::isFolder(twi)) {
			cas.iType = CUSTOM_ACTION_TYPE_FOLDER;
		}
		data.append(cas);
	}

	return data;
}

void CustomActionsEditWidget::slotSave()
{
	//qDebug() << "SAVE SAVE SAVE SAVE SAVE SAVE SAVE SAVE";
	m_pTunnel->customActionList.clear();
	m_pTunnel->customActionList.append(getData());
    m_pSkeletonWindow->saveEditPane();
}

void CustomActionsEditWidget::slotUpdateControls()
{
	m_pSkeletonWindow->ui.btnCustomActionDelete->setEnabled(false);
	m_pSkeletonWindow->ui.btnCustomActionEdit->setEnabled(false);
	m_pSkeletonWindow->ui.btnCustomActionDuplicate->setEnabled(false);

	m_pSkeletonWindow->ui.btnCustomActionUp->setEnabled(false);
	m_pSkeletonWindow->ui.btnCustomActionDown->setEnabled(false);
	m_pSkeletonWindow->ui.btnCustomActionLeft->setEnabled(false);
	m_pSkeletonWindow->ui.btnCustomActionRight->setEnabled(false);

	QList<QTreeWidgetItem*> twiList = m_pSkeletonWindow->ui.treeCustomActionsEdit->selectedItems();
	if(twiList.size() == 1) {
		//1 item selected
		QTreeWidgetItem* twi = twiList.at(0);
		if(!m_pSkeletonWindow->ui.treeCustomActionsEdit->isFolder(twi)) {
			m_pSkeletonWindow->ui.btnCustomActionEdit->setEnabled(true);
		}
	}
	if(twiList.size() > 0) {
		//1 or more items selected
		m_pSkeletonWindow->ui.btnCustomActionDuplicate->setEnabled(true);
		m_pSkeletonWindow->ui.btnCustomActionDelete->setEnabled(true);
		if(m_pSkeletonWindow->ui.treeCustomActionsEdit->canMoveUp(twiList)) {
			m_pSkeletonWindow->ui.btnCustomActionUp->setEnabled(true);
		}
		if(m_pSkeletonWindow->ui.treeCustomActionsEdit->canMoveDown(twiList)) {
			m_pSkeletonWindow->ui.btnCustomActionDown->setEnabled(true);
		}
		if(m_pSkeletonWindow->ui.treeCustomActionsEdit->canMoveLeft(twiList)) {
			m_pSkeletonWindow->ui.btnCustomActionLeft->setEnabled(true);
		}
		if(m_pSkeletonWindow->ui.treeCustomActionsEdit->canMoveRight(twiList)) {
			m_pSkeletonWindow->ui.btnCustomActionRight->setEnabled(true);
		}
	}
}

void CustomActionsEditWidget::slotBeforeShowingContextMenu()
{
	QList<QTreeWidgetItem*> twiList = m_pSkeletonWindow->ui.treeCustomActionsEdit->selectedItems();
	m_pActionExecute->setEnabled(false);
	m_pSkeletonWindow->ui.treeCustomActionsEdit->actionEdit()->setEnabled(false); //Edit is default enabled when size() == 1
	if(twiList.size() == 1) {
		QTreeWidgetItem* twi = twiList.at(0);
		if(!TreeWidget::isFolder(twi)) {
			m_pActionExecute->setEnabled(true);
			m_pSkeletonWindow->ui.treeCustomActionsEdit->actionEdit()->setEnabled(true);
		}
	}
}

void CustomActionsEditWidget::slotCreatedNewFolder(QTreeWidgetItem* twi)
{
	twi->setIcon( COL_LABEL, Images::icon("ca_100_folder_closed") );
}

void CustomActionsEditWidget::slotAfterPasteItem(QTreeWidgetItem *twiPasteTo, QTreeWidgetItem *twiInserted, int pasteMode, int insertMode)
{
	Q_UNUSED(twiPasteTo);
	if(pasteMode == TreeWidget::PASTE_COPY) {
		setUUuid(twiInserted, QUuid::createUuid());
		//recursive update of all children
		for(int i=0;i<twiInserted->childCount();i++) {
			QTreeWidgetItem *twiChild = twiInserted->child(i);
			slotAfterPasteItem(NULL, twiChild, pasteMode, insertMode);
		}
	}
}

void CustomActionsEditWidget::slotAddItem(int insertMode/*=TreeWidget::INSERT_AFTER*/)
{
	m_pSkeletonWindow->ui.treeCustomActionsEdit->setFocus(Qt::OtherFocusReason);

	QTreeWidgetItem *twi = new QTreeWidgetItem();
	
	twi->setText(COL_LABEL, "Untitled");
	//twi->setFont(COL_LABEL,QFont(QString::fromUtf8("Tahoma"),10));

	twi->setText(COL_COMMAND, "");
	//twi->setFont(COL_COMMAND,QFont(QString::fromUtf8("Courier New"),10));

	twi->setIcon( COL_LABEL, Images::icon("ca_110_action") );

	twi->setFlags(twi->flags() | Qt::ItemIsEditable);
	
	m_pSkeletonWindow->ui.treeCustomActionsEdit->insertAtCurrentItem(twi, insertMode);
}

void CustomActionsEditWidget::slotEditItem()
{
	QTreeWidgetItem *twi = m_pSkeletonWindow->ui.treeCustomActionsEdit->currentItem();
	if(twi == NULL) return;
	ATASSERT(m_pDlgEdit);

	CustomActionStruct cas(getUUuid(twi), twi->text(COL_LABEL).trimmed(), twi->text(COL_COMMAND).trimmed());

    m_pDlgEdit->init(m_pTunnel,twi,cas);
	m_pDlgEdit->show();
}

void CustomActionsEditWidget::slotEditDialogAccepted()
{
	QTreeWidgetItem *twi = m_pDlgEdit->getTwi();
	if(twi == NULL) return;
	twi->setText(COL_LABEL, m_pDlgEdit->getLabel());
	twi->setText(COL_COMMAND, m_pDlgEdit->getCommand());
}


void CustomActionsEditWidget::slotItemCollapsed(QTreeWidgetItem * twi)
{
	if(TreeWidget::isFolder(twi)) {
		bool enabled = m_pSkeletonWindow->ui.treeCustomActionsEdit->setModifiedAndSaveSignalEnabled(false);
		twi->setIcon(COL_LABEL, Images::icon("ca_100_folder_closed"));
		m_pSkeletonWindow->ui.treeCustomActionsEdit->setModifiedAndSaveSignalEnabled(enabled);
	}

	QUuid uuid = getUUuid(twi);
	for(int i=0;i<m_pTunnel->customActionList.size();i++) {
		CustomActionStruct cas = m_pTunnel->customActionList.at(i);
		if(cas.uUid == uuid) {
			cas.bExpanded = false;
			m_pTunnel->customActionList.replace(i,cas);
		}
	}
}

void CustomActionsEditWidget::slotItemExpanded(QTreeWidgetItem * twi)
{
	if(TreeWidget::isFolder(twi)) {
		bool enabled = m_pSkeletonWindow->ui.treeCustomActionsEdit->setModifiedAndSaveSignalEnabled(false);
		twi->setIcon(COL_LABEL, Images::icon("ca_090_folder_open"));
		m_pSkeletonWindow->ui.treeCustomActionsEdit->setModifiedAndSaveSignalEnabled(enabled);
	}

	QUuid uuid = getUUuid(twi);
	for(int i=0;i<m_pTunnel->customActionList.size();i++) {
		CustomActionStruct cas = m_pTunnel->customActionList.at(i);
		if(cas.uUid == uuid) {
			cas.bExpanded = true;
			m_pTunnel->customActionList.replace(i,cas);
		}
	}
}

void CustomActionsEditWidget::slotExecute()
{
	QTreeWidgetItem *twi = m_pSkeletonWindow->ui.treeCustomActionsEdit->currentItem();
	if(twi == NULL) return;
	if(TreeWidget::isFolder(twi)) return;
	CustomActionStruct cas(getUUuid(twi), twi->text(COL_LABEL).trimmed(), twi->text(COL_COMMAND).trimmed());
	if(!cas.sCmd.trimmed().isEmpty()) {
		emit signalActionExec(cas);
	}
}

void CustomActionsEditWidget::slotExecute(const CustomActionStruct& cas)
{
	emit signalActionExec(cas);
}

// private
QUuid CustomActionsEditWidget::getUUuid(QTreeWidgetItem * twi)
{
	QVariant v = twi->data(COL_LABEL, Qt::UserRole);
	if(!v.isNull()) {
		QString strUuid = v.value<QString>();
		QUuid uuid = QUuid(strUuid);
		return uuid;
	} else {
		return QUuid();
	}
}

// private
void CustomActionsEditWidget::setUUuid(QTreeWidgetItem * twi, const QUuid& uuid)
{
	twi->setData( COL_LABEL, Qt::UserRole, QVariant::fromValue(uuid.toString()) );
}






