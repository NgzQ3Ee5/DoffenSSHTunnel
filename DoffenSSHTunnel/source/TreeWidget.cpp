#include "TreeWidget.h"
#include "pch.h"

#include <QtWidgets/QTreeWidgetItem>
#include <QtCore/QModelIndex>
#include <QtCore/QDebug>
#include <QtWidgets/QMessageBox>

#include <QtWidgets/QMenu>
#include <QtGui/QAction>
#include <QtGui/QKeyEvent>
#include <QtGui/QFocusEvent>


TreeWidget::TreeWidget(QWidget *parent)
    : QTreeWidget(parent)
{
	m_bEditingEnabled = true;
	m_bUsingStandardControls = false;
	m_bAllowItemsToBeParents = false;
	m_bUsingCheckControls = false;
	m_bCheckStateInheritanceEnabled = false;
	m_bModifiedAndSaveSignalEnabled = false;
	m_nPasteMode = PASTE_NONE;

	m_timerModifiedSignal.setSingleShot(true);
	m_timerModifiedSignal.setInterval(100);
	m_timerSaveSignal.setSingleShot(true);
	m_timerSaveSignal.setInterval(500);

	//setContextMenuPolicy(Qt::ActionsContextMenu);
	setContextMenuPolicy(Qt::CustomContextMenu);
	m_pContextMenu = new QMenu(this);

    ATVERIFY( connect( &m_timerModifiedSignal,  &QTimer::timeout,       this, &TreeWidget::slotModifiedTimerTimeout, Qt::QueuedConnection ) );
    ATVERIFY( connect( &m_timerSaveSignal,      &QTimer::timeout,       this, &TreeWidget::slotSaveTimerTimeout, Qt::QueuedConnection ) );

    ATVERIFY( connect( this, &TreeWidget::customContextMenuRequested,   this, &TreeWidget::slotCustomContextMenuRequested, Qt::QueuedConnection ) );
    ATVERIFY( connect( this, &TreeWidget::itemClicked,                  this, &TreeWidget::slotItemClicked, Qt::QueuedConnection ) );
    ATVERIFY( connect( this, &TreeWidget::signalKeySpacePressed,		this, &TreeWidget::slotUpdateCheckStateInheritanceCurrentItem, Qt::QueuedConnection ) );
    ATVERIFY( connect( this, &TreeWidget::itemChanged,                  this, &TreeWidget::slotItemChanged, Qt::QueuedConnection ) );
}

TreeWidget::~TreeWidget()
{
	if(m_nPasteMode == PASTE_COPY) {
		//we cloned them
		for(int i=0;i<m_listItemsToPaste.size();i++) {
			qDebug() << "TreeWidget::~TreeWidget() DELETE: " << m_listItemsToPaste.at(i)->text(0);
			delete m_listItemsToPaste.at(i);
		}
	}
}

void TreeWidget::setCheckStateInheritanceEnabled(bool enable)
{ 
	if(enable != m_bCheckStateInheritanceEnabled) {
		if(enable) {
			//Find lowest level parents
			QList<QTreeWidgetItem*> parentList;
            QList<QTreeWidgetItem*> allItems = findItems(".*", Qt::MatchFlags(Qt::MatchRegularExpression | Qt::MatchRecursive), 0);
			for(int i=0;i<allItems.size();i++) {
				QTreeWidgetItem *twi = allItems.at(i);
				if(twi->childCount() == 0) {
					if(twi->parent() != NULL) {
						if(!parentList.contains(twi->parent())) {
							parentList.append(twi->parent());
						}
					}
				}
			}
			for(int i=0;i<parentList.size();i++) {
				QTreeWidgetItem *twi = parentList.at(i);
				updateParentCheckState(twi,0);
				updateParentsCheckState(twi,0);
			}
			//updateChildrenCheckState(twi,0);
			//updateParentCheckState(twi,0);
		} else {
			//Just replace PartiallyChecked with Checked
            QList<QTreeWidgetItem*> allItems = findItems(".*", Qt::MatchFlags(Qt::MatchRegularExpression | Qt::MatchRecursive), 0);
			for(int i=0;i<allItems.size();i++) {
				QTreeWidgetItem *twi = allItems.at(i);
				Qt::CheckState checkState = twi->checkState(0);
				if(checkState == Qt::PartiallyChecked) {
					twi->setCheckState(0, Qt::Checked);
				}
			}
		}
	}
    m_bCheckStateInheritanceEnabled = enable;
}

bool TreeWidget::checkStateInheritanceEnabled()
{
    return m_bCheckStateInheritanceEnabled;
}

//public
void TreeWidget::setupCheckControls()
{
	m_bUsingCheckControls = true;

	m_pCheckSelected = new QAction("C&heck Selected",m_pContextMenu);
	m_pUnCheckSelected = new QAction("UnCheck Selected",m_pContextMenu);

	m_pCheckAllChildren = new QAction("Check Selected and &All Children",m_pContextMenu);
	m_pUnCheckAllChildren = new QAction("UnCheck Selected and All Children",m_pContextMenu);

	if(m_bUsingStandardControls) {
		//adding to bottom of menu
		m_pContextMenu->addSeparator();
	}

	m_pContextMenu->addAction(m_pCheckSelected);
	m_pContextMenu->addAction(m_pUnCheckSelected);

	m_pContextMenu->addAction(m_pCheckAllChildren);
	m_pContextMenu->addAction(m_pUnCheckAllChildren);

    ATVERIFY( connect( m_pCheckSelected,		&QAction::triggered, this, &TreeWidget::slotCheckSelected, Qt::UniqueConnection ) );
    ATVERIFY( connect( m_pUnCheckSelected,		&QAction::triggered, this, &TreeWidget::slotUnCheckSelected, Qt::UniqueConnection ) );
    ATVERIFY( connect( m_pCheckAllChildren,		&QAction::triggered, this, &TreeWidget::slotCheckAllChildren, Qt::UniqueConnection ) );
    ATVERIFY( connect( m_pUnCheckAllChildren,	&QAction::triggered, this, &TreeWidget::slotUnCheckAllChildren, Qt::UniqueConnection ) );
}

//public
void TreeWidget::setupStandardControls()
{
	m_bUsingStandardControls = true;

	m_pActionEdit = new QAction("&Edit",m_pContextMenu);
	m_pActionRename = new QAction("Rena&me",m_pContextMenu);
	m_pActionDuplicate = new QAction("D&uplicate",m_pContextMenu);
	m_pActionInsertFolderAfter = new QAction("&Insert Folder After",m_pContextMenu);
	m_pActionInsertFolderBefore = new QAction("Insert Folder Before",m_pContextMenu);
	m_pActionInsertFolderChild = new QAction("Insert Folder Child",m_pContextMenu);
    m_pActionInsertAfter = new QAction("&Insert Item After",m_pContextMenu);
#ifdef Q_OS_MACOS
    m_pActionInsertAfter->setText(m_pActionInsertAfter->text() + " (Cmd+N)");
#else
    m_pActionInsertAfter->setText(m_pActionInsertAfter->text() + " (Ctrl+N)");
#endif
	m_pActionInsertBefore = new QAction("Insert Item Before",m_pContextMenu);
	m_pActionInsertChild = new QAction("Insert Item Child",m_pContextMenu);
    m_pActionCut = new QAction("Cu&t",m_pContextMenu);
#ifdef Q_OS_MACOS
    m_pActionCut->setText(m_pActionCut->text() + " (Cmd+X)");
#else
    m_pActionCut->setText(m_pActionCut->text() + " (Ctrl+X)");
#endif
    m_pActionCopy = new QAction("&Copy",m_pContextMenu);
#ifdef Q_OS_MACOS
    m_pActionCopy->setText(m_pActionCopy->text() + " (Cmd+C)");
#else
    m_pActionCopy->setText(m_pActionCopy->text() + " (Ctrl+C)");
#endif
    m_pActionPasteAfter = new QAction("&Paste After",m_pContextMenu);
#ifdef Q_OS_MACOS
    m_pActionPasteAfter->setText(m_pActionPasteAfter->text() + " (Cmd+V)");
#else
    m_pActionPasteAfter->setText(m_pActionPasteAfter->text() + " (Ctrl+V)");
#endif
	m_pActionPasteBefore = new QAction("Paste Before",m_pContextMenu);
	m_pPasteChild = new QAction("Paste Child",m_pContextMenu);
	m_pActionDelete = new QAction("&Delete",m_pContextMenu);

	if(m_bUsingCheckControls) {
		//adding to bottom of menu
		m_pContextMenu->addSeparator();
	}

	m_pContextMenu->addAction(m_pActionEdit);
	m_pContextMenu->addAction(m_pActionRename);
	m_pContextMenu->addSeparator();
	m_pContextMenu->addAction(m_pActionDuplicate);
	m_pContextMenu->addSeparator();
	m_pContextMenu->addAction(m_pActionInsertFolderAfter);
	m_pContextMenu->addAction(m_pActionInsertFolderBefore);
	m_pContextMenu->addAction(m_pActionInsertFolderChild);
	m_pContextMenu->addSeparator();
	m_pContextMenu->addAction(m_pActionInsertAfter);
	m_pContextMenu->addAction(m_pActionInsertBefore);
	m_pContextMenu->addAction(m_pActionInsertChild);
	m_pContextMenu->addSeparator();
	m_pContextMenu->addAction(m_pActionCut);
	m_pContextMenu->addAction(m_pActionCopy);
	m_pContextMenu->addAction(m_pActionPasteAfter);
	m_pContextMenu->addAction(m_pActionPasteBefore);
	m_pContextMenu->addAction(m_pPasteChild);
	m_pContextMenu->addSeparator();
	m_pContextMenu->addAction(m_pActionDelete);

    ATVERIFY( connect( this, &TreeWidget::itemSelectionChanged,             this, &TreeWidget::slotUpdateControls, Qt::UniqueConnection ) );
    ATVERIFY( connect( this, &TreeWidget::signalKeyCtrlUpPressed,           this, &TreeWidget::slotMoveUp, Qt::UniqueConnection ) );
    ATVERIFY( connect( this, &TreeWidget::signalKeyCtrlDownPressed,         this, &TreeWidget::slotMoveDown, Qt::UniqueConnection ) );
    ATVERIFY( connect( this, &TreeWidget::signalKeyCtrlLeftPressed,         this, &TreeWidget::slotMoveLeft, Qt::UniqueConnection ) );
    ATVERIFY( connect( this, &TreeWidget:: signalKeyCtrlRightPressed,       this, &TreeWidget::slotMoveRight, Qt::UniqueConnection ) );
    ATVERIFY( connect( this, &TreeWidget::signalKeyEscapePressed,           this, &TreeWidget:: slotClearPasteItems, Qt::UniqueConnection ) );
    ATVERIFY( connect( this, &TreeWidget::signalKeyDeletePressed,           this, &TreeWidget::slotDelete, Qt::UniqueConnection ) );
    ATVERIFY( connect( this, &TreeWidget::signalKeyCtrlCPressed,            this, &TreeWidget::slotCopy, Qt::UniqueConnection ) );
    ATVERIFY( connect( this, &TreeWidget::signalKeyCtrlXPressed,            this, &TreeWidget::slotCut, Qt::UniqueConnection ) );
    ATVERIFY( connect( this, &TreeWidget::signalKeyCtrlVPressed,            this, &TreeWidget::slotPasteAfter, Qt::UniqueConnection ) );
    ATVERIFY( connect( this, &TreeWidget::signalPasteEnabled,               this, &TreeWidget::slotUpdateControls, Qt::UniqueConnection ) );
    ATVERIFY( connect( this, &TreeWidget::signalPasteDisabled,              this, &TreeWidget::slotUpdateControls, Qt::UniqueConnection ) );
    ATVERIFY( connect( m_pActionEdit,               &QAction::triggered,    this, &TreeWidget::slotEdit, Qt::UniqueConnection ) );
    ATVERIFY( connect( m_pActionDuplicate,          &QAction::triggered,    this, &TreeWidget::slotDuplicate, Qt::UniqueConnection ) );
    ATVERIFY( connect( m_pActionInsertFolderAfter,  &QAction::triggered,    this, &TreeWidget::slotInsertFolderAfter, Qt::UniqueConnection ) );
    ATVERIFY( connect( m_pActionInsertFolderBefore, &QAction::triggered,    this, &TreeWidget::slotInsertFolderBefore, Qt::UniqueConnection ) );
    ATVERIFY( connect( m_pActionInsertFolderChild,  &QAction::triggered,    this, &TreeWidget::slotInsertFolderChild, Qt::UniqueConnection ) );
    ATVERIFY( connect( m_pActionInsertAfter,        &QAction::triggered,    this, &TreeWidget::slotInsertAfter, Qt::UniqueConnection ) );
    ATVERIFY( connect( this, &TreeWidget::signalKeyCtrlNPressed,            this, &TreeWidget::slotInsertAfter, Qt::UniqueConnection ) );
    ATVERIFY( connect( m_pActionInsertBefore,       &QAction::triggered,    this, &TreeWidget::slotInsertBefore, Qt::UniqueConnection ) );
    ATVERIFY( connect( m_pActionInsertChild,        &QAction::triggered,    this, &TreeWidget::slotInsertChild, Qt::UniqueConnection ) );
    ATVERIFY( connect( m_pActionCut,                &QAction::triggered,    this, &TreeWidget::slotCut, Qt::UniqueConnection ) );
    ATVERIFY( connect( m_pActionCopy,               &QAction::triggered,    this, &TreeWidget::slotCopy, Qt::UniqueConnection ) );
    ATVERIFY( connect( m_pActionPasteAfter,         &QAction::triggered,    this, &TreeWidget::slotPasteAfter, Qt::UniqueConnection ) );
    ATVERIFY( connect( m_pActionPasteBefore,        &QAction::triggered,    this, &TreeWidget::slotPasteBefore, Qt::UniqueConnection ) );
    ATVERIFY( connect( m_pPasteChild,               &QAction::triggered,    this, &TreeWidget::slotPasteChild, Qt::UniqueConnection ) );
    ATVERIFY( connect( m_pActionDelete,             &QAction::triggered,    this, &TreeWidget::slotDelete, Qt::UniqueConnection ) );
    ATVERIFY( connect( m_pActionRename,             &QAction::triggered,    this, &TreeWidget::slotRename, Qt::UniqueConnection ) );
}

void TreeWidget::setAllowItemsToBeParents(bool allow)
{
    m_bAllowItemsToBeParents = allow;
}

bool TreeWidget::allowItemsToBeParents()
{
    return m_bAllowItemsToBeParents;
}

void TreeWidget::slotEnableEditing(bool enable)
{
    m_bEditingEnabled = enable;
}

void TreeWidget::slotModifiedTimerTimeout()
{
    emit signalModified();
}

void TreeWidget::slotSaveTimerTimeout()
{
    emit signalSave();
}

//private
void TreeWidget::slotItemClicked( QTreeWidgetItem* twi, int col ) 
{ 
	if(m_bCheckStateInheritanceEnabled && col == 0) {
		slotUpdateCheckStateInheritance(twi,col);
    }
}

void TreeWidget::slotItemChanged(QTreeWidgetItem *, int)
{
    handleModifiedAndSave();
}

//public connected to signalKeySpacePressed() Qt::QueuedConnection
void TreeWidget::slotUpdateCheckStateInheritanceCurrentItem()
{
	if(m_bCheckStateInheritanceEnabled) {
		QTreeWidgetItem *twi = currentItem();
		if(twi != NULL) {
			slotUpdateCheckStateInheritance(twi, 0);
		}
	}
}

//public called by slotItemClicked(..) AND slotUpdateCheckStateInheritance()
void TreeWidget::slotUpdateCheckStateInheritance(QTreeWidgetItem* twi, int col)
{
	//http://stackoverflow.com/questions/6807291/maintaining-checkedstatus-inheritance-in-a-qtreeview
	updateParentsCheckState(twi, col);
	updateChildrenCheckState(twi, col);
}

//public called by context-menu check/uncheck selected
void TreeWidget::updateParentsCheckState(QList<QTreeWidgetItem*> twiList, int col)
{
	//Find unique parents
	QList<QTreeWidgetItem*> twiParentList;
	for(int i=0;i<twiList.size();i++) {
		QTreeWidgetItem* twiParent = twiList.at(i);
		if(twiParent != NULL && twiParentList.indexOf(twiParent) == -1) {
			twiParentList.append(twiParent);
		}
	}
	//Update parents state
	for(int i=0;i<twiParentList.size();i++) {
		QTreeWidgetItem* twiParent = twiParentList.at(i);
		updateParentsCheckState(twiParent, col);
	}
}

void TreeWidget::updateChildrenCheckState(QList<QTreeWidgetItem*> twiList, int col)
{
	for(int i=0;i<twiList.size();i++) {
		QTreeWidgetItem* twi = twiList.at(i);
		updateChildrenCheckState(twi, col);
	}
}

//public called by slotUpdateCheckStateInheritance(..)
void TreeWidget::updateParentsCheckState(QTreeWidgetItem* twi, int col)
{
	//Qt::UnChecked=0, Qt::PartiallyChecked=1, Qt::Checked=2
	QTreeWidgetItem* twiParent = twi->parent();
	while(twiParent != NULL) {
		updateParentCheckState(twiParent, col);
		twiParent = twiParent->parent();
	}	
}

//public called by updateParentsCheckState(..)
void TreeWidget::updateParentCheckState(QTreeWidgetItem* twiParent, int col)
{
	//Qt::UnChecked=0, Qt::PartiallyChecked=1, Qt::Checked=2
	if(twiParent != NULL) {
		int checkCount = 0;
		for (int i = 0; i < twiParent->childCount(); i++) {
			if (twiParent->child(i)->checkState(col) == Qt::Checked) {
                checkCount++;
			}
        }
		if(checkCount ==  twiParent->childCount()) {
			twiParent->setCheckState(col, Qt::Checked);
		} else {
			twiParent->setCheckState(col, Qt::PartiallyChecked);
		}
	}	
}

//public
void TreeWidget::updateChildrenCheckState(QTreeWidgetItem* twi, int col)
{
	Qt::CheckState checkState = twi->checkState(col);
	if(checkState == Qt::Checked || checkState == Qt::Unchecked) {
		for(int i=0; i<twi->childCount(); i++) {
			QTreeWidgetItem* twiChild = twi->child(i);
			twiChild->setCheckState(col, checkState);
			if(twiChild->childCount() > 0) {
				updateChildrenCheckState(twiChild, col);
			}
		}
	}
}

//public
QList<QTreeWidgetItem*> TreeWidget::slotDuplicate()
{
	QList<QTreeWidgetItem*> newItems;
	if(!m_bEditingEnabled) return newItems;
	slotCopy();
	newItems = slotPasteAfter();
	clearPasteItems();
	return newItems;
}

//public
void TreeWidget::slotDelete()
{
	if(!m_bEditingEnabled) return;

	QList<QTreeWidgetItem*> twiList = selectedItems();
	if(twiList.size() == 0) return;

    QString strQuestion = QString( "Confirm delete ?" );

 	QMessageBox::StandardButton iRet = QMessageBox::question( this, "", strQuestion, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes );
 
 	if ( iRet == QMessageBox::Yes )
 	{
		if(m_bUsingStandardControls) {
            ATVERIFY ( disconnect( this, &TreeWidget::itemSelectionChanged, this, &TreeWidget::slotUpdateControls ) );
		}

		deleteItems(twiList);

		QTreeWidgetItem *twiCurrent = currentItem();
		if(twiCurrent != NULL) {
			setCurrentItem(twiCurrent);
		}
		setFocus(Qt::OtherFocusReason);
		
		slotUpdateControls(); //notifies listener

		if(m_bUsingStandardControls) {
            ATVERIFY( connect( this, &TreeWidget::itemSelectionChanged, this, &TreeWidget::slotUpdateControls ) );
		}
	}
}

//public
void TreeWidget::slotRename()
{
	if(!m_bEditingEnabled) return;
	if(selectedItems().size() == 1) {
		QTreeWidgetItem *twi = selectedItems().at(0);
		editItem(twi, 0);
	}
}

//private
void TreeWidget::slotCustomContextMenuRequested(const QPoint& pos)
{
	qDebug() << "TreeWidget::slotCustomContextMenuRequested()";

	if(m_bUsingStandardControls)
	{
		m_pActionEdit->setVisible(m_bEditingEnabled);
		m_pActionDuplicate->setVisible(m_bEditingEnabled);
		m_pActionInsertFolderAfter->setVisible(m_bEditingEnabled);
		m_pActionInsertFolderBefore->setVisible(m_bEditingEnabled);
		m_pActionInsertFolderChild->setVisible(m_bEditingEnabled);
		m_pActionInsertAfter->setVisible(m_bEditingEnabled);
		m_pActionInsertBefore->setVisible(m_bEditingEnabled);
		m_pActionInsertChild->setVisible(m_bEditingEnabled);
		m_pActionCut->setVisible(m_bEditingEnabled);
		m_pActionCopy->setVisible(m_bEditingEnabled);
		m_pActionPasteAfter->setVisible(m_bEditingEnabled);
		m_pActionPasteBefore->setVisible(m_bEditingEnabled);
		m_pPasteChild->setVisible(m_bEditingEnabled);
		m_pActionDelete->setVisible(m_bEditingEnabled);
		m_pActionRename->setVisible(m_bEditingEnabled);

		m_pActionEdit->setEnabled(false);
		m_pActionDuplicate->setEnabled(false);
		m_pActionInsertFolderAfter->setEnabled(false);
		m_pActionInsertFolderBefore->setEnabled(false);
		m_pActionInsertFolderChild->setEnabled(false);
		m_pActionInsertAfter->setEnabled(false);
		m_pActionInsertBefore->setEnabled(false);
		m_pActionInsertChild->setEnabled(false);
		m_pActionCut->setEnabled(false);
		m_pActionCopy->setEnabled(false);
		m_pActionPasteBefore->setEnabled(false);
		m_pActionPasteAfter->setEnabled(false);
		m_pPasteChild->setEnabled(false);
		m_pActionDelete->setEnabled(false);
		m_pActionRename->setEnabled(false);

		if(selectedItems().size() <= 1) {
			m_pActionInsertFolderAfter->setEnabled(true);
			m_pActionInsertAfter->setEnabled(true);
			if(m_nPasteMode != PASTE_NONE) {
				m_pActionPasteAfter->setEnabled(true);
			}
		}
		

		if(selectedItems().size() > 0) 
		{
			if(selectedItems().size() == 1) {
				bool insertChildEnabled = true;
				if(!m_bAllowItemsToBeParents && !isFolder(selectedItems().at(0))) {
					insertChildEnabled = false;
				}
				m_pActionEdit->setEnabled(true);
				m_pActionDuplicate->setEnabled(true);
				m_pActionInsertFolderBefore->setEnabled(true);
				m_pActionInsertBefore->setEnabled(true);
				if(insertChildEnabled) m_pActionInsertFolderChild->setEnabled(true);
				if(insertChildEnabled) m_pActionInsertChild->setEnabled(true);

				if(m_nPasteMode != PASTE_NONE) {
					m_pActionPasteBefore->setEnabled(true);
					if(insertChildEnabled) m_pPasteChild->setEnabled(true);
				}

				m_pActionRename->setEnabled(true);
			}

			m_pActionCut->setEnabled(true);
			m_pActionCopy->setEnabled(true);
			m_pActionDelete->setEnabled(true);
		}
	}

	if(m_bUsingCheckControls) 
	{
		m_pCheckSelected->setEnabled(false);
		m_pUnCheckSelected->setEnabled(false);
		m_pCheckAllChildren->setEnabled(false);
		m_pUnCheckAllChildren->setEnabled(false);

		QList<QTreeWidgetItem*> twiList = selectedItems();
		QTreeWidgetItem* twiCurrent = currentItem();
		if(twiCurrent != NULL && twiList.indexOf(twiCurrent) == -1) {
			twiList.append(twiCurrent);
		}
		if(twiList.size() > 0) 
		{
			if(!hasAllCheckState(twiList, Qt::Checked)) {
				m_pCheckSelected->setEnabled(true);
			}
			if(!hasAllChildrenCheckState(twiList, Qt::Checked)) { //also checks current item (not only its children)
				m_pCheckAllChildren->setEnabled(true);
			}
			if(!hasAllCheckState(twiList, Qt::Unchecked)) {
				m_pUnCheckSelected->setEnabled(true);
			}
			if(!hasAllChildrenCheckState(twiList, Qt::Unchecked)) { //also checks current item (not only its children)
				m_pUnCheckAllChildren->setEnabled(true);
			}
		}
		if(m_bCheckStateInheritanceEnabled) {
			//No point in having them since all childre _will_ be checked
			m_pCheckAllChildren->setVisible(false);
			m_pUnCheckAllChildren->setVisible(false);
		} else {
			m_pCheckAllChildren->setVisible(true);
			m_pUnCheckAllChildren->setVisible(true);
		}
	}

	if(m_bUsingStandardControls || m_bUsingCheckControls) {
		emit signalBeforeShowingContextMenu();
		m_pContextMenu->exec(mapToGlobal(pos));
	}
}

bool TreeWidget::hasAllCheckState(QList<QTreeWidgetItem*> twiList, Qt::CheckState checkCheckState)
{
	for(int i=0;i<twiList.size();i++) {
		QTreeWidgetItem* twi = twiList.at(i);
		if(!hasCheckState(twi, checkCheckState)) {
			return false;
		}
	}
	return true;
}

bool TreeWidget::hasCheckState(QTreeWidgetItem* twi, Qt::CheckState checkCheckState)
{
	Qt::CheckState checkState = twi->checkState(0);
	if(checkState == checkCheckState) {
		return true;
	} else {
		return false;
	}
}

bool TreeWidget::hasAllChildrenCheckState(QList<QTreeWidgetItem*> twiList, Qt::CheckState checkCheckState)
{
	for(int i=0;i<twiList.size();i++) {
		QTreeWidgetItem* twi = twiList.at(i);
		bool ret = hasAllChildrenCheckState(twi, checkCheckState);
		if(!ret) {
			return false;
		}
	}
	return true;
}

bool TreeWidget::hasAllChildrenCheckState(QTreeWidgetItem* twi, Qt::CheckState checkCheckState)
{
	if(!hasCheckState(twi,checkCheckState)) {
		return false;
	}
	for(int i=0;i<twi->childCount();i++) {
		QTreeWidgetItem* twiChild = twi->child(i);
		if(!hasCheckState(twiChild, checkCheckState)) {
			return false;
		}
		for(int j=0;j<twiChild->childCount();j++) {
			QTreeWidgetItem* twiChildChild = twiChild->child(j);
			bool ret = hasAllChildrenCheckState(twiChildChild, checkCheckState);
			if(ret == false) {
				return false;
			}
		}
	}
	return true;
}

void TreeWidget::setAllCheckState(QList<QTreeWidgetItem*> twiList, Qt::CheckState checkState)
{
	for(int i=0;i<twiList.size();i++) {
		QTreeWidgetItem* twi = twiList.at(i);
		twi->setCheckState(0, checkState);
	}
}

void TreeWidget::setAllChildrenCheckState(QList<QTreeWidgetItem*> twiList, Qt::CheckState checkState)
{
	for(int i=0;i<twiList.size();i++) {
		QTreeWidgetItem* twi = twiList.at(i);
		setAllChildrenCheckState(twi, checkState);
	}
}

void TreeWidget::setAllChildrenCheckState(QTreeWidgetItem* twi, Qt::CheckState checkState)
{
	twi->setCheckState(0, checkState);
	for(int i=0;i<twi->childCount();i++) {
		QTreeWidgetItem* twiChild = twi->child(i);
		setAllChildrenCheckState(twiChild, checkState);
    }
}

QMenu *TreeWidget::contextMenu()
{
    return m_pContextMenu;
}

QAction *TreeWidget::actionEdit()
{
    return m_pActionEdit;
}

QAction *TreeWidget::actionReanme()
{
    return m_pActionRename;
}

QAction *TreeWidget::actionDuplicate()
{
    return m_pActionDuplicate;
}

QAction *TreeWidget::actionInsertFolderAfter()
{
    return m_pActionInsertFolderAfter;
}

QAction *TreeWidget::actionInsertFolderBefore()
{
    return m_pActionInsertFolderBefore;
}

QAction *TreeWidget::actionInsertFolderChild()
{
    return m_pActionInsertFolderChild;
}

QAction *TreeWidget::actionInsertAfter()
{
    return m_pActionInsertAfter;
}

QAction *TreeWidget::actionInsertBefore()
{
    return m_pActionInsertBefore;
}

QAction *TreeWidget::actionInsertChild()
{
    return m_pActionInsertChild;
}

QAction *TreeWidget::actionCut()
{
    return m_pActionCut;
}

QAction *TreeWidget::actionCopy() { return m_pActionCopy; }

QAction *TreeWidget::actionPasteAfter() { return m_pActionPasteAfter; }

QAction *TreeWidget::actionPasteBefore() { return m_pActionPasteBefore; }

QAction *TreeWidget::actionPasteChild() { return m_pPasteChild; }

QAction *TreeWidget::actionDelete() { return m_pActionDelete; }

bool TreeWidget::setModifiedAndSaveSignalEnabled(bool enabled)
{
    bool previous = m_bModifiedAndSaveSignalEnabled;
    m_bModifiedAndSaveSignalEnabled = enabled;
    if(!enabled) { m_timerModifiedSignal.stop(); m_timerSaveSignal.stop(); }
    return previous;
}

void TreeWidget::clear()
{
    updatePasteStatus();
    QTreeWidget::clear();
}


//private internal signal
void TreeWidget::slotUpdateControls()
{

	qDebug() << "TreeWidget::slotUpdateControls() emit signalUpdateControls()";
	
	if(m_bUsingStandardControls)
	{

	}

	emit signalUpdateControls(); //notify listener
}



//private internal signal
void TreeWidget::slotEdit()
{
	if(!m_bEditingEnabled) return;
	qDebug() << "TreeWidget::slotEdit() emit signalEdit()";
	emit signalEditItem(); //nofity listener
}

void TreeWidget::slotInsertAfter()
{
	if(!m_bEditingEnabled) return;
	qDebug() << "TreeWidget::slotInsertAfter() emit signalAddItem(INSERT_AFTER)";
	emit signalAddItem(TreeWidget::INSERT_AFTER);
}

void TreeWidget::slotInsertBefore()
{
	if(!m_bEditingEnabled) return;
	qDebug() << "TreeWidget::slotInsertBefore() emit signalAddItem(INSERT_BEFORE)";
	emit signalAddItem(TreeWidget::INSERT_BEFORE);
}

void TreeWidget::slotInsertChild()
{
	if(!m_bEditingEnabled) return;
	qDebug() << "TreeWidget::slotInsertChild() emit signalAddItem(INSERT_CHILD)";
	emit signalAddItem(TreeWidget::INSERT_CHILD);
}

void TreeWidget::slotInsertFolderAfter()
{
	if(!m_bEditingEnabled) return;
	createNewFolder(TreeWidget::INSERT_AFTER);
}

void TreeWidget::slotInsertFolderBefore()
{
	if(!m_bEditingEnabled) return;
	createNewFolder(TreeWidget::INSERT_BEFORE);
}

void TreeWidget::slotInsertFolderChild()
{
	if(!m_bEditingEnabled) return;
	createNewFolder(TreeWidget::INSERT_CHILD);
}

//protected
QTreeWidgetItem* TreeWidget::createNewFolder(int insertMode)
{
	QTreeWidgetItem *twi = new QTreeWidgetItem();
	setIsFolder(twi);
	twi->setText(0, "Untitled");
	twi->setFlags(twi->flags() | Qt::ItemIsEditable);
	insertAtCurrentItem(twi, insertMode);	
	emit signalCreatedNewFolder(twi);
	return twi;
}

void TreeWidget::setIsFolder(QTreeWidgetItem *twi, bool isFolder/*=true*/)
{
	twi->setData( 0, Qt::UserRole + 10, QVariant(isFolder) );
}

bool TreeWidget::isFolder(const QModelIndex& index)
{
	if(!index.isValid()) return false;
	QAbstractItemModel *aModel = model();
	QVariant v = aModel->data(index, Qt::UserRole + 10);
//	if(v != NULL) {
		bool isFolder = (bool)v.value<bool>();
		return isFolder;
//	}
	return false;	
}

bool TreeWidget::isFolder(QTreeWidgetItem *twi)
{
	QVariant v = twi->data(0, Qt::UserRole + 10);
//	if(v != NULL) {
		bool isFolder = (bool)v.value<bool>();
		return isFolder;
//	}
	return false;
}

void TreeWidget::keyPressEvent(QKeyEvent *e)
{
	qDebug() << "keyPressEvent() key: " << e->key();
	switch (e->key()) {
		case Qt::Key_Up:
			if(e->modifiers() == Qt::ControlModifier) {
				e->ignore();
				emit signalKeyCtrlUpPressed();
			}
			break;
		case Qt::Key_Down:
			if(e->modifiers() == Qt::ControlModifier) {
				e->ignore();
				emit signalKeyCtrlDownPressed();
			}
			break;
		case Qt::Key_Left:
			if(e->modifiers() == Qt::ControlModifier) {
				e->ignore();
				emit signalKeyCtrlLeftPressed();
			}
			break;
		case Qt::Key_Right:
			if(e->modifiers() == Qt::ControlModifier) {
				e->ignore();
				emit signalKeyCtrlRightPressed();
			}
			break;
		case Qt::Key_C:
			if(e->modifiers() == Qt::ControlModifier) {
				e->ignore();
				emit signalKeyCtrlCPressed();
			}
			break;
		case Qt::Key_X:
			if(e->modifiers() == Qt::ControlModifier) {
				e->ignore();
				emit signalKeyCtrlXPressed();
			}
			break;
		case Qt::Key_V:
			if(e->modifiers() == Qt::ControlModifier) {
				e->ignore();
				emit signalKeyCtrlVPressed();
			}
			break;
		case Qt::Key_N:
			if(e->modifiers() == Qt::ControlModifier) {
				e->ignore();
				emit signalKeyCtrlNPressed();
			}
			break;
		case Qt::Key_Delete:
			e->ignore();
			emit signalKeyDeletePressed();
			break;
		case Qt::Key_Space:
			e->accept();
			emit signalKeySpacePressed();
			break;
		case Qt::Key_Escape:
			//Not called if the parent widget has registered shortcut key ESC
			e->ignore();
			emit signalKeyEscapePressed();
			break;
		default:
			break;
	}

	if(e->isAccepted()) {
		QTreeWidget::keyPressEvent(e);
    }
}

void TreeWidget::rowsRemoved(const QModelIndex &i, int s, int e)
{
    updatePasteStatus();
    handleModifiedAndSave();
    QTreeWidget::rowsRemoved(i,s,e);
}

void TreeWidget::rowsAboutToBeRemoved(const QModelIndex &i, int s, int e)
{
    updatePasteStatus();
    handleModifiedAndSave();
    QTreeWidget::rowsAboutToBeRemoved(i,s,e);
}

void TreeWidget::rowsInserted(const QModelIndex &i, int s, int e)
{
    updatePasteStatus();
    handleModifiedAndSave();
    QTreeWidget::rowsInserted(i,s,e);
}

void TreeWidget::dataChanged(const QModelIndex &l, const QModelIndex &r)
{
    updatePasteStatus();
    handleModifiedAndSave();
    QTreeWidget::dataChanged(l,r);
}

void TreeWidget::focusOutEvent(QFocusEvent *e)
{
    if(e->reason() != Qt::PopupFocusReason && e->reason() != Qt::ActiveWindowFocusReason) {
        updatePasteStatus(); QTreeWidget::focusOutEvent(e);
    }
}

void TreeWidget::dragMoveEvent(QDragMoveEvent* event)
{
	QTreeWidget::dragMoveEvent(event);
	if(event->isAccepted()) {
		if(m_bUsingStandardControls) {
            QPoint pos = event->position().toPoint();
			QModelIndex index = indexAt(pos);
			if(!m_bAllowItemsToBeParents && !isFolder(index)) {
				if(dropIndicatorPosition() == QAbstractItemView::OnItem) {
					event->ignore();
				}
			}
		}
	}
}

void TreeWidget::dropEvent(QDropEvent* event)
{

	QTreeWidget::dropEvent(event);
}

void TreeWidget::slotCheckSelected()
{
	QList<QTreeWidgetItem*> twiList = selectedItems();
	QTreeWidgetItem* twiCurrent = currentItem();
	if(twiCurrent != NULL && twiList.indexOf(twiCurrent) == -1) {
		twiList.append(twiCurrent);
	}
	setAllCheckState(twiList, Qt::Checked);
	if(m_bCheckStateInheritanceEnabled) {
		updateParentsCheckState(twiList, 0);
		updateChildrenCheckState(twiList, 0);
	}
}

void TreeWidget::slotUnCheckSelected()
{
	QList<QTreeWidgetItem*> twiList = selectedItems();
	QTreeWidgetItem* twiCurrent = currentItem();
	if(twiCurrent != NULL && twiList.indexOf(twiCurrent) == -1) {
		twiList.append(twiCurrent);
	}
	setAllCheckState(twiList, Qt::Unchecked);	
	if(m_bCheckStateInheritanceEnabled) {
		updateParentsCheckState(twiList, 0);
		updateChildrenCheckState(twiList, 0);
	}
}

void TreeWidget::slotCheckAllChildren()
{
	QList<QTreeWidgetItem*> twiList = selectedItems();
	QTreeWidgetItem* twiCurrent = currentItem();
	if(twiCurrent != NULL && twiList.indexOf(twiCurrent) == -1) {
		twiList.append(twiCurrent);
	}
	setAllChildrenCheckState(twiList, Qt::Checked);		
	if(m_bCheckStateInheritanceEnabled) {
		updateParentsCheckState(twiList, 0);
		updateChildrenCheckState(twiList, 0);
	}
}

void TreeWidget::slotUnCheckAllChildren()
{
	QList<QTreeWidgetItem*> twiList = selectedItems();
	QTreeWidgetItem* twiCurrent = currentItem();
	if(twiCurrent != NULL && twiList.indexOf(twiCurrent) == -1) {
		twiList.append(twiCurrent);
	}
	setAllChildrenCheckState(twiList, Qt::Unchecked);	
	if(m_bCheckStateInheritanceEnabled) {
		updateParentsCheckState(twiList, 0);
		updateChildrenCheckState(twiList, 0);
	}
}

void TreeWidget::slotClearPasteItems()
{
	clearPasteItems();
}

void TreeWidget::slotCopy()
{
	if(!m_bEditingEnabled) return;
	copyItems(selectedItems());
}

void TreeWidget::slotCut()
{
	if(!m_bEditingEnabled) return;
	cutItems(selectedItems());
}

void TreeWidget::slotPasteBefore()
{
	if(!m_bEditingEnabled) return;
	pasteAtCurrentItem(TreeWidget::INSERT_BEFORE);
}

QList<QTreeWidgetItem*> TreeWidget::slotPasteAfter()
{
	QList<QTreeWidgetItem*> pastedItems;
	if(!m_bEditingEnabled) return pastedItems;
	pastedItems = pasteAtCurrentItem(TreeWidget::INSERT_AFTER);
	return pastedItems;
}

void TreeWidget::slotPasteChild()
{
	if(!m_bEditingEnabled) return;
	pasteAtCurrentItem(TreeWidget::INSERT_CHILD);
}

/*
QTreeWidgetItem* TreeWidget::duplicateItem(QTreeWidgetItem* twi)
{
	QTreeWidgetItem* twiClone = twi->clone();
	QTreeWidgetItem* newTwi = pasteItem(twiClone, twi, PASTE_COPY, INSERT_AFTER);
	return newTwi;
}*/

void TreeWidget::deleteItems(QList<QTreeWidgetItem*> twiList)
{
	clearPasteItems();

	if(twiList.size() == 0) return;

	for(int i=0; i<twiList.size(); i++)
	{
		QTreeWidgetItem *twiToDelete = twiList.at(i);
		QTreeWidgetItem *twiParent = twiToDelete->parent();
		if(twiParent != NULL) {
			int index = twiParent->indexOfChild(twiToDelete);
			if(index != -1) {
				twiParent->takeChild(index);
			}
		} else {
			int index = indexOfTopLevelItem(twiToDelete);
			if(index != -1) {
				takeTopLevelItem(index);
			}
		}
	}

	for(int i=0; i<twiList.size(); i++)
	{
		delete twiList.at(i);
	}	
}

QList<QTreeWidgetItem*>& TreeWidget::getPasteItems()
{
	return m_listItemsToPaste;
}

void TreeWidget::clearPasteItems()
{
	if(m_nPasteMode != PASTE_NONE || m_listItemsToPaste.size() > 0)
	{
		if(m_nPasteMode == PASTE_COPY) {
			//we cloned them
			for(int i=0;i<m_listItemsToPaste.size();i++) {
				qDebug() << "TreeWidget::clearPasteItems() DELETE: " << m_listItemsToPaste.at(i)->text(0);
				delete m_listItemsToPaste.at(i);
			}
		} else if(m_nPasteMode == PASTE_CUT) {
			bool enabled = setModifiedAndSaveSignalEnabled(false);
			for(int i=0;i<m_listItemsToPaste.size();i++) {
				setItemForegroundCut(m_listItemsToPaste.at(i),false);
			}
			setModifiedAndSaveSignalEnabled(enabled);
		}
		m_listItemsToPaste.clear();
		m_nPasteMode = PASTE_NONE;
		qDebug() << "TreeWidget::clearPasteItems() emit signalPasteDisabled()";
		emit signalPasteDisabled();
	}
}

int TreeWidget::getPasteMode()
{
	return m_nPasteMode;
}

void TreeWidget::copyItems(QList<QTreeWidgetItem*> twiList)
{
	qDebug() << Q_FUNC_INFO;

	clearPasteItems();

	QList<QTreeWidgetItem*> twiListSorted = toChronologialAscendingOrder(twiList);
	QList<QTreeWidgetItem*> twiListNorm = normalizeSelection(twiListSorted);

	for(int i=0; i<twiListNorm.size(); i++) {
		QTreeWidgetItem* twi = twiListNorm.at(i);
		QTreeWidgetItem* twiClone = twi->clone();
		m_listItemsToPaste.append(twiClone);
		qDebug() << "COPY: " << twi->text(0);
	}

	if(m_listItemsToPaste.size() > 0) {
		m_nPasteMode = PASTE_COPY;	
		qDebug() << "TreeWidget::copySelectedItems() emit signalPasteEnabled()";
		emit signalPasteEnabled();
	} else {
		m_nPasteMode = PASTE_NONE;	
	}
}

//static
void TreeWidget::setItemForegroundAlpha(QTreeWidgetItem* twi, int col, int alpha)
{
	QBrush brush = twi->foreground(col);
	QColor color = brush.color();
	color.setAlpha(alpha);
	brush.setColor(color);
	twi->setForeground(col,brush);
}

//static
void TreeWidget::setRowForegroundAlpha(QTreeWidgetItem* twi, int alpha)
{
	for(int col=0;col<twi->columnCount();col++) {
		setItemForegroundAlpha(twi,col,alpha);
	}
}

//static
void TreeWidget::setItemForegroundCut(QTreeWidgetItem* twi, int col, bool doit)
{
	if(doit) {
		setItemForegroundAlpha(twi,col,120);
	} else {
		setItemForegroundAlpha(twi,col,255);
	}
}

//static
void TreeWidget::setItemForegroundCut(QTreeWidgetItem* twi, bool doit)
{
	for(int col=0;col<twi->columnCount();col++) {
		setItemForegroundCut(twi,col,doit);
	}
}

void TreeWidget::cutItems(QList<QTreeWidgetItem*> twiList)
{
	qDebug() << Q_FUNC_INFO;

	clearPasteItems();

	QList<QTreeWidgetItem*> twiListSorted = toChronologialAscendingOrder(twiList);
	QList<QTreeWidgetItem*> twiListNorm = normalizeSelection(twiListSorted);

	bool enabled = setModifiedAndSaveSignalEnabled(false);
	for(int i=0; i<twiListNorm.size(); i++) {
		QTreeWidgetItem* twi = twiList.at(i);
		m_listItemsToPaste.append(twi);
		setItemForegroundCut(twi,true);
		qDebug() << "CUT: " << twi->text(0);
	}
	setModifiedAndSaveSignalEnabled(enabled);

	if(m_listItemsToPaste.size() > 0) {
		m_nPasteMode = PASTE_CUT;	
		qDebug() << "TreeWidget::cutSelectedItems() emit signalPasteEnabled()";
		emit signalPasteEnabled();
	} else {
		m_nPasteMode = PASTE_NONE;	
	}
}

QList<QTreeWidgetItem*> TreeWidget::pasteAtCurrentItem(int insertMode)
{
	qDebug() << Q_FUNC_INFO;
	QList<QTreeWidgetItem*> pastedItems;

	QTreeWidgetItem *twiPasteAt = currentItem();
	pastedItems.append( pasteAtItem(twiPasteAt, insertMode) );
	
	return pastedItems;
}

QList<QTreeWidgetItem*> TreeWidget::pasteAtItem(QTreeWidgetItem *twiPasteAt, int insertMode)
{
	qDebug() << Q_FUNC_INFO;

	QList<QTreeWidgetItem*> pastedItems;

	if(m_bUsingStandardControls) {
        ATVERIFY ( disconnect( this, &TreeWidget::itemSelectionChanged, this, &TreeWidget::slotUpdateControls ) );
	}

	for(int i=0; i<m_listItemsToPaste.size(); i++) {
		QTreeWidgetItem* twiToPaste = m_listItemsToPaste.at(i);
		QTreeWidgetItem* twiPasted = pasteItem(twiToPaste, twiPasteAt, m_nPasteMode, insertMode);
		pastedItems.append(twiPasted);
	}

	if(m_nPasteMode == PASTE_CUT) {
		clearPasteItems();
	}

	slotUpdateControls(); //notifies listener

	if(m_bUsingStandardControls) {
        ATVERIFY( connect( this, &TreeWidget::itemSelectionChanged, this, &TreeWidget::slotUpdateControls ) );
	}

	return pastedItems;
}

//private
QTreeWidgetItem* TreeWidget::pasteItem(QTreeWidgetItem *twiPasteFrom, QTreeWidgetItem *twiPasteTo, int pasteMode, int insertMode)
{
	ATASSERT(twiPasteFrom);

	qDebug() << Q_FUNC_INFO;
	qDebug() << "  twiPasteFrom: " << twiPasteFrom->text(0);
	if(twiPasteTo != NULL)
	qDebug() << "    twiPasteTo: " << twiPasteTo->text(0);
	else
	qDebug() << "    twiPasteTo: NULL -> paste at top level";
	qDebug() << "     pasteMode: " << pasteMode;
	qDebug() << "    insertMode: " << insertMode;

	ATASSERT(pasteMode == PASTE_COPY || pasteMode == PASTE_CUT);
	ATASSERT(insertMode == INSERT_AFTER || insertMode == INSERT_BEFORE || insertMode == INSERT_CHILD);

	QTreeWidgetItem* twiInserted = NULL;

	if(pasteMode == PASTE_CUT) {
		ATASSERT(twiPasteTo);
		QTreeWidgetItem *twiCutParent = twiPasteFrom->parent();
		if(hasParent(twiPasteTo, twiPasteFrom)) {
			qDebug() << twiPasteTo->text(0) << ":" << twiPasteFrom->text(0);
			QString msg = QString("Cannot move %1: The destination folder is the same as the source folder.").arg(twiPasteFrom->text(0));
			QMessageBox::warning(this, "Cannot move", msg);
		} else if(isSameItem(twiPasteFrom,twiPasteTo)) {
			//noop
		} else {
			if(twiCutParent != NULL) {
				int index = twiCutParent->indexOfChild(twiPasteFrom);
				twiInserted = twiCutParent->takeChild(index);
			} else {
				int index = indexOfTopLevelItem(twiPasteFrom);
				twiInserted = takeTopLevelItem(index);
			}
			insertItem(twiPasteTo, twiInserted, insertMode);
			emit signalAfterPasteItem(twiPasteTo, twiInserted, pasteMode, insertMode);
		}
	}
	else if(pasteMode == PASTE_COPY) {
		//twiPasteFrom could already be a clone. We still need to clone it here.
		twiInserted = twiPasteFrom->clone();
		twiInserted = insertItem(twiPasteTo, twiInserted, insertMode);
		if(twiInserted != NULL && isItemDuplicateByName(twiInserted)) {
			twiInserted->setText(0, twiInserted->text(0) + " Copy");
		}
		if(twiInserted != NULL) {
			emit signalAfterPasteItem(twiPasteTo, twiInserted, pasteMode, insertMode);
		}
	}

    return twiInserted;
}

void TreeWidget::updatePasteStatus()
{
    if(m_nPasteMode == PASTE_CUT) { clearPasteItems(); }
}

void TreeWidget::handleModifiedAndSave()
{
    if(m_bModifiedAndSaveSignalEnabled) {
        m_timerModifiedSignal.start();
        m_timerSaveSignal.start();
    }
}

//public
void TreeWidget::insertAtCurrentItem(QTreeWidgetItem* twiInsert, int insertMode)
{
	if(m_bUsingStandardControls) {
        ATVERIFY ( disconnect( this, &TreeWidget::itemSelectionChanged, this, &TreeWidget::slotUpdateControls ) );
	}

	QTreeWidgetItem *twiInsertAt = currentItem();
	insertItem(twiInsertAt, twiInsert, insertMode);

	clearSelection();
	setCurrentItem(twiInsert);
	setFocus(Qt::OtherFocusReason);

	slotUpdateControls(); //notifies listener

	if(m_bUsingStandardControls) {
        ATVERIFY( connect( this, &TreeWidget::itemSelectionChanged, this, &TreeWidget::slotUpdateControls ) );
	}
}

QTreeWidgetItem* TreeWidget::insertItem(QTreeWidgetItem *twiInsertAt, QTreeWidgetItem *twiInsert, int insertMode)
{
	qDebug() << Q_FUNC_INFO;

	ATASSERT(twiInsert); 

	if(twiInsertAt != NULL) 
	{
		if(insertMode == INSERT_AFTER) {
			qDebug() << "INSERT_AFTER twiInsertAt=" << twiInsertAt->text(0) << " twiInsert=" << twiInsert->text(0);
			QTreeWidgetItem *twiInsertAtParent = twiInsertAt->parent();
			if(twiInsertAtParent != NULL) {
				int index = twiInsertAtParent->indexOfChild(twiInsertAt);
				qDebug() << twiInsertAtParent->text(0) << "->insertChild(" << index+1 << ", " << twiInsert->text(0) << ")";
				//qDebug("%s->insertChild(%d, %s)",QString(twiInsertAtParent->text(0)),index+1,QString(twiInsert->text(0)));
				twiInsertAtParent->insertChild(index+1, twiInsert);
			} else {
				int index = indexOfTopLevelItem(twiInsertAt);
                //qDebug("insertTopLevelItem(%d, %s)",index+1,QString(twiInsert->text(0)));
				insertTopLevelItem(index+1, twiInsert);
			}
		}
		else if(insertMode == INSERT_BEFORE) {
			qDebug() << "INSERT_BEFORE twiInsertAt=" << twiInsertAt->text(0) << " twiInsert=" << twiInsert->text(0);
			QTreeWidgetItem *twiInsertAtParent = twiInsertAt->parent();
			if(twiInsertAtParent != NULL) {
				int index = twiInsertAtParent->indexOfChild(twiInsertAt);
				twiInsertAtParent->insertChild(index, twiInsert);
			} else {
				int index = indexOfTopLevelItem(twiInsertAt);
				insertTopLevelItem(index, twiInsert);
			}
		}
		else if(insertMode == INSERT_CHILD) {
			qDebug() << "INSERT_CHILD twiInsertAt=" << twiInsertAt->text(0) << " twiInsert=" << twiInsert->text(0);
			twiInsertAt->insertChild(twiInsertAt->childCount(), twiInsert);
		}
	}
	else
	{
		//insert at bottom of root
		qDebug() << "Insert at bottom of root twiInsert=" << twiInsert->text(0);
		insertTopLevelItem(topLevelItemCount(), twiInsert);
	}

	return twiInsert;
}

void TreeWidget::slotMoveLeft()
{
	if(!m_bEditingEnabled) return;
	moveItemsLeft(selectedItems());
}

void TreeWidget::slotMoveRight()
{
	if(!m_bEditingEnabled) return;
	moveItemsRight(selectedItems());
}

void TreeWidget::slotMoveUp()
{
	if(!m_bEditingEnabled) return;
	moveItemsUp(selectedItems());
}

void TreeWidget::slotMoveDown()
{
	if(!m_bEditingEnabled) return;
	moveItemsDown(selectedItems());
}

bool TreeWidget::canMoveUp(QList<QTreeWidgetItem*> twiList)
{
	if(twiList.size() == 0) return false;
	twiList = normalizeSelection(twiList);
	if(twiList.size() == 0) return false;
	for(int i=0;i<twiList.size();i++) {
		QTreeWidgetItem* twi = twiList.at(i);
		if(!canMoveUp(twi)) return false;
	}
	return true;
}

bool TreeWidget::canMoveUp(QTreeWidgetItem* item)
{
	return hasItemBefore(item);
}

bool TreeWidget::canMoveDown(QList<QTreeWidgetItem*> twiList)
{
	if(twiList.size() == 0) return false;
	twiList = normalizeSelection(twiList);
	if(twiList.size() == 0) return false;
	for(int i=0;i<twiList.size();i++) {
		QTreeWidgetItem* twi = twiList.at(i);
		if(!canMoveDown(twi)) return false;
	}
	return true;
}

bool TreeWidget::canMoveDown(QTreeWidgetItem* item)
{
	return hasItemAfter(item);
}

bool TreeWidget::canMoveLeft(QList<QTreeWidgetItem*> twiList)
{
	if(twiList.size() == 0) return false;
	twiList = normalizeSelection(twiList);
	if(twiList.size() == 0) return false;
	for(int i=0;i<twiList.size();i++) {
		QTreeWidgetItem* twi = twiList.at(i);
		if(!canMoveLeft(twi)) return false;
	}
	return true;
}

bool TreeWidget::canMoveLeft(QTreeWidgetItem* item)
{
	return item->parent() != NULL;
}

bool TreeWidget::canMoveRight(QList<QTreeWidgetItem*> twiList)
{
	if(twiList.size() == 0) return false;
	twiList = normalizeSelection(twiList);
	if(twiList.size() == 0) return false;
	for(int i=0;i<twiList.size();i++) {
		QTreeWidgetItem* twi = twiList.at(i);
		if(!canMoveRight(twi)) return false;
	}
	return true;
}

bool TreeWidget::canMoveRight(QTreeWidgetItem* item)
{
	QTreeWidgetItem* theItemBefore = itemBefore(item);
	if(theItemBefore == NULL) return false;
	if(m_bAllowItemsToBeParents) {
		return true;
	} else {
		if(isFolder(theItemBefore)) {
			return true;
		} else {
			return false;
		}
	}
}

bool TreeWidget::moveItemsUp(QList<QTreeWidgetItem*> twiList)
{
	if(twiList.size() == 0) return false;
	QList<QTreeWidgetItem*> twiListSorted = toChronologialAscendingOrder(twiList);
	QList<QTreeWidgetItem*> twiListNorm = normalizeSelection(twiListSorted);
	if(twiListNorm.size() == 0) return false;
	if(!canMoveUp(twiListNorm)) return false;

	emit signalBeforeMoveItemsUp(twiListNorm);

	if(m_bUsingStandardControls) {
        ATVERIFY ( disconnect( this, &TreeWidget::itemSelectionChanged, this, &TreeWidget::slotUpdateControls ) );
	}

	clearPasteItems();

	QTreeWidgetItem* twiCurrent = currentItem();
	clearSelection();

	//expanded ?
	QList<QTreeWidgetItem*> twiListExpanded = findAllThatAreExpanded();

	//move
	for(int i=0;i<twiListNorm.size();i++) {
		QTreeWidgetItem* twi = twiListNorm.at(i);
		internalMoveItemUp(twi);
	}

	//expand
	setExpanded(twiListExpanded);

	//select
	setFocus(Qt::OtherFocusReason);
	setCurrentItems(twiListSorted, twiCurrent);

	slotUpdateControls();

	if(m_bUsingStandardControls) {
        ATVERIFY( connect( this, &TreeWidget::itemSelectionChanged, this, &TreeWidget::slotUpdateControls ) );
	}

	emit signalAfterMoveItemsUp(twiListNorm);

	return true;
}


bool TreeWidget::moveItemUp(QTreeWidgetItem* item)
{
	QList<QTreeWidgetItem*> l;
	l.append(item);
	return moveItemsUp(l);
}

bool TreeWidget::internalMoveItemUp(QTreeWidgetItem* item)
{
	if(!canMoveUp(item)) return false;
	
	QTreeWidgetItem *parentItem = item->parent();
	if(parentItem != NULL) {
		int itemIndex = parentItem->indexOfChild(item);
		QTreeWidgetItem *itemAbove = parentItem->takeChild(itemIndex - 1);
		parentItem->insertChild(itemIndex, itemAbove);
	} else {
		int itemIndex = indexOfTopLevelItem(item);
		QTreeWidgetItem *itemAbove = takeTopLevelItem(itemIndex - 1);
		insertTopLevelItem(itemIndex, itemAbove);
	}

	return true;
}

bool TreeWidget::moveItemsDown(QList<QTreeWidgetItem*> twiList)
{
	if(twiList.size() == 0) return false;
	QList<QTreeWidgetItem*> twiListSorted = toChronologialAscendingOrder(twiList);
	QList<QTreeWidgetItem*> twiListNorm = normalizeSelection(twiListSorted);
	if(twiListNorm.size() == 0) return false;
	if(!canMoveDown(twiListNorm)) return false;

	emit signalBeforeMoveItemsDown(twiList);

	if(m_bUsingStandardControls) {
        ATVERIFY ( disconnect( this, &TreeWidget::itemSelectionChanged, this, &TreeWidget::slotUpdateControls ) );
	}

	clearPasteItems();
	QTreeWidgetItem* twiCurrent = currentItem();
	clearSelection();

	//expanded ?
	QList<QTreeWidgetItem*> twiListExpanded = findAllThatAreExpanded();

	//move
	for(int i=twiListNorm.size()-1;i>=0;i--) {
		QTreeWidgetItem* twi = twiListNorm.at(i);
		internalMoveItemDown(twi);
	}

	//expand
	setExpanded(twiListExpanded);

	//select
	setCurrentItems(twiListSorted, twiCurrent);

	slotUpdateControls();

	if(m_bUsingStandardControls) {
        ATVERIFY( connect( this, &TreeWidget::itemSelectionChanged, this, &TreeWidget::slotUpdateControls ) );
	}

	emit signalAfterMoveItemsDown(twiListNorm);

	return true;
}

bool TreeWidget::moveItemDown(QTreeWidgetItem* item)
{
	QList<QTreeWidgetItem*> l;
	l.append(item);
	return moveItemsDown(l);
}

bool TreeWidget::internalMoveItemDown(QTreeWidgetItem* item)
{
	if(!canMoveDown(item)) return false;

	QTreeWidgetItem *parentItem = item->parent();
	if(parentItem != NULL) {
		int itemIndex = parentItem->indexOfChild(item);
		QTreeWidgetItem *itemBelow = parentItem->takeChild(itemIndex + 1);
		parentItem->insertChild(itemIndex, itemBelow);
	} else {
		int itemIndex = indexOfTopLevelItem(item);
		QTreeWidgetItem *itemBelow = takeTopLevelItem(itemIndex + 1);
		insertTopLevelItem(itemIndex, itemBelow);
	}

	return true;
}

bool TreeWidget::moveItemsLeft(QList<QTreeWidgetItem*> twiList)
{
	if(twiList.size() == 0) return false;
	QList<QTreeWidgetItem*> twiListSorted = toChronologialAscendingOrder(twiList);
	QList<QTreeWidgetItem*> twiListNorm = normalizeSelection(twiListSorted);
	if(twiListNorm.size() == 0) return false;
	if(!canMoveLeft(twiListNorm)) return false;

	emit signalBeforeMoveItemsLeft(twiListNorm);

	if(m_bUsingStandardControls) {
        ATVERIFY ( disconnect( this, &TreeWidget::itemSelectionChanged, this, &TreeWidget::slotUpdateControls ) );
	}

	clearPasteItems();
	QTreeWidgetItem* twiCurrent = currentItem();
	clearSelection();

	//expanded ?
	QList<QTreeWidgetItem*> twiListExpanded = findAllThatAreExpanded();

	//move
	for(int i=twiListNorm.size()-1;i>=0;i--) {
		QTreeWidgetItem* twi = twiListNorm.at(i);
		internalMoveItemLeft(twi);
	}

	//expand
	setExpanded(twiListExpanded);

	//select
	setCurrentItems(twiListSorted, twiCurrent);

	slotUpdateControls();

	if(m_bUsingStandardControls) {
        ATVERIFY( connect( this, &TreeWidget::itemSelectionChanged, this, &TreeWidget::slotUpdateControls ) );
	}

	emit signalAfterMoveItemsLeft(twiListNorm);

	return true;
}

bool TreeWidget::moveItemLeft(QTreeWidgetItem* item) {
	QList<QTreeWidgetItem*> l;
	l.append(item);
	return moveItemsLeft(l);
}

bool TreeWidget::internalMoveItemLeft(QTreeWidgetItem* item)
{
	if(!canMoveLeft(item)) return false;
	
	int level = getItemLevel(item);

	ATASSERT(level >= 1);

	if(level > 1) 
	{
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
	}
	else //level == 1
	{
		// root
        // item1           parentItem      level 0
		// |-- item2       currentItem     level 1
		QTreeWidgetItem *parentItem = item->parent();
		int itemIndex = parentItem->indexOfChild(item);
		int parentItemIndex = indexOfTopLevelItem(parentItem);
		item = parentItem->takeChild(itemIndex);
		insertTopLevelItem(parentItemIndex+1, item);
	}

	return true;
}

bool TreeWidget::moveItemsRight(QList<QTreeWidgetItem*> twiList)
{
	if(twiList.size() == 0) return false;
	QList<QTreeWidgetItem*> twiListSorted = toChronologialAscendingOrder(twiList);
	QList<QTreeWidgetItem*> twiListNorm = normalizeSelection(twiListSorted);
	if(twiListNorm.size() == 0) return false;
	if(!canMoveRight(twiListNorm)) return false;

	emit signalBeforeMoveItemsRight(twiListNorm);

	if(m_bUsingStandardControls) {
        ATVERIFY ( disconnect( this, &TreeWidget::itemSelectionChanged, this, &TreeWidget::slotUpdateControls ) );
	}

	clearPasteItems();
	QTreeWidgetItem* twiCurrent = currentItem();
	clearSelection();

	//expanded ?
	QList<QTreeWidgetItem*> twiListExpanded = findAllThatAreExpanded();

	//Move them right
	for(int i=0;i<twiListNorm.size();i++) {
		QTreeWidgetItem* twi = twiListNorm.at(i);
		internalMoveItemRight(twi);
	}

	//expand
	setExpanded(twiListExpanded);

	//select
	setCurrentItems(twiListSorted, twiCurrent);

	slotUpdateControls();

	if(m_bUsingStandardControls) {
        ATVERIFY( connect( this, &TreeWidget::itemSelectionChanged, this, &TreeWidget::slotUpdateControls ) );
	}

	emit signalAfterMoveItemsRight(twiListNorm);

	return true;
}

bool TreeWidget::moveItemRight(QTreeWidgetItem* item) {
	QList<QTreeWidgetItem*> l;
	l.append(item);
	return moveItemsRight(l);
}

bool TreeWidget::internalMoveItemRight(QTreeWidgetItem* item)
{
	if(!canMoveRight(item)) return false;
	
	QTreeWidgetItem *beforeItem = NULL;

	QTreeWidgetItem *parentItem = item->parent();
	if(parentItem != NULL) {
		int itemIndex = parentItem->indexOfChild(item);
		if(itemIndex > 0) {
			beforeItem = parentItem->child(itemIndex-1);
			item = parentItem->takeChild(itemIndex);
		}
	} else {
		int itemIndex = indexOfTopLevelItem(item);
		if(itemIndex > 0) {
			beforeItem = topLevelItem(itemIndex-1);
			item = takeTopLevelItem(itemIndex);
		}
	}

	ATASSERT(beforeItem != NULL);

	if(beforeItem != NULL) {
		beforeItem->insertChild(beforeItem->childCount(), item);
	}

	return true;
}

bool TreeWidget::isItemDuplicateByName(QTreeWidgetItem* twi)
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
		for(int i=0;i<topLevelItemCount();i++) {
			QTreeWidgetItem *child = topLevelItem(i);
			if(child != twi && child->text(0) == twi->text(0)) {
				foundSameName = true;
				break;
			}
		}
	}
	return foundSameName;
}


bool TreeWidget::isSameItem(QTreeWidgetItem* item1, QTreeWidgetItem* item2)
{
	bool ret = false;
	QTreeWidgetItem* parent = item1->parent();
	if(parent != NULL) {
		int index1 = parent->indexOfChild(item1);
		int index2 = parent->indexOfChild(item2);
		ret = index1 == index2 ? true : false;
	} else {
		int index1 = indexOfTopLevelItem(item1);
		int index2 = indexOfTopLevelItem(item2);
		ret = index1 == index2 ? true : false;
	}
	return ret;
}

QTreeWidgetItem* TreeWidget::itemBefore(QTreeWidgetItem* item)
{
	QTreeWidgetItem* itemParent = item->parent();
	if(itemParent != NULL) {
		int index = itemParent->indexOfChild(item);
		if(index > 0) return itemParent->child(index-1);
	} else {
		int index = indexOfTopLevelItem(item);
		if(index > 0) return topLevelItem(index-1);
	}
	return NULL;
}

bool TreeWidget::hasItemBefore(QTreeWidgetItem* item)
{
	int index = 0;
	QTreeWidgetItem* itemParent = item->parent();
	if(itemParent != NULL) {
		index = itemParent->indexOfChild(item);
	} else {
		index = indexOfTopLevelItem(item);
	}

	return index > 0 ? true : false;
}

bool TreeWidget::hasItemAfter(QList<QTreeWidgetItem*> twiList)
{
	if(twiList.size() == 0) return false;
	for(int i=0;i<twiList.size();i++) {
		QTreeWidgetItem* item = twiList.at(i);
		if(!hasItemAfter(item)) return false;
	}
	return true;
}

bool TreeWidget::hasItemAfter(QTreeWidgetItem* item)
{
	int itemCount = 0;
	int index = 0;
	QTreeWidgetItem* itemParent = item->parent();
	if(itemParent != NULL) {
		index = itemParent->indexOfChild(item);
		itemCount = itemParent->childCount();
	} else {
		index = indexOfTopLevelItem(item);
		itemCount = topLevelItemCount();
	}

	return index < (itemCount-1) ? true : false;
}

QList<QTreeWidgetItem*> TreeWidget::toChronologialAscendingOrder(QList<QTreeWidgetItem*> twiList)
{
	QList<QTreeWidgetItem*> ret;
    QList<QTreeWidgetItem*> allItems = findItems(".*", Qt::MatchFlags(Qt::MatchRegularExpression | Qt::MatchRecursive), 0);
	for(int i=0;i<allItems.size();i++) {
		QTreeWidgetItem* twi = allItems.at(i);
		if(twiList.indexOf(twi) != -1) {
			ret.append(twi);
		}
	}
	return ret;
}

QList<QTreeWidgetItem*> TreeWidget::toChronologialDescendingOrder(QList<QTreeWidgetItem*> twiList)
{
	QList<QTreeWidgetItem*> ret;
    QList<QTreeWidgetItem*> allItems = findItems(".*", Qt::MatchFlags(Qt::MatchRegularExpression | Qt::MatchRecursive), 0);
	for(int i=allItems.size()-1;i>=0;i--) {
		QTreeWidgetItem* twi = allItems.at(i);
		if(twiList.indexOf(twi) != -1) {
			ret.append(twi);
		}
	}
	return ret;
}

QList<QTreeWidgetItem*> TreeWidget::findAllThatAreExpanded()
{
	QList<QTreeWidgetItem*> ret;
    QList<QTreeWidgetItem*> twiList = findItems(".*", Qt::MatchFlags(Qt::MatchRegularExpression | Qt::MatchRecursive), 0);
	for(int i=0;i<twiList.size();i++) {
		QTreeWidgetItem* twi = twiList.at(i);
		if(twi->isExpanded()) {
			ret.append(twi);
		}
	}
	return ret;
}

void TreeWidget::setExpanded(QList<QTreeWidgetItem*> twiList)
{
	for(int i=0;i<twiList.size();i++) {
		QTreeWidgetItem* twi = twiList.at(i);
		twi->setExpanded(true);
	}
}

void TreeWidget::setCurrentItems(QList<QTreeWidgetItem*> twiList, QTreeWidgetItem* twiCurrent/*=NULL*/)
{
	if(twiCurrent != NULL) {
		setCurrentItem(twiCurrent);
	}
	for(int i=0;i<twiList.size();i++) {
		QTreeWidgetItem* twi = twiList.at(i);
		twi->setSelected(true);
	}
}

//static
int TreeWidget::getItemLevel(const QTreeWidgetItem* item)
{
	int level = 0;
	QTreeWidgetItem* parentItem = item->parent();
	while(parentItem != NULL) {
		level++;
		parentItem = parentItem->parent();
	}
	return level;
}

//static
bool TreeWidget::hasParent(QTreeWidgetItem* item, QTreeWidgetItem* parent)
{
	if(parent->indexOfChild(item) != -1) {
		return true;
	}
	while(item->parent() != NULL) {
		item = item->parent();
		if(parent->indexOfChild(item) != -1) {
			return true;
		}
	}
	return false;
}

//static
QList<QTreeWidgetItem*> TreeWidget::normalizeSelection(QList<QTreeWidgetItem*> twiList)
{
	QList<QTreeWidgetItem*> ret;

	for(int i=0; i<twiList.size(); i++) {
		QTreeWidgetItem* twi = twiList.at(i);
		bool isChild = false;
		for(int j=0; j<twiList.size(); j++) {
			if( hasParent(twi,twiList.at(j)) ) {
				isChild = true;
			}
		}
		if(!isChild) {
			ret.append(twi);
		}
	}

	return ret;
}

//static
QList<QTreeWidgetItem*> TreeWidget::getChildListRecursive(QTreeWidgetItem* twiParent)
{
    QList<QTreeWidgetItem*> twiChildList;
    for(int i=0;i<twiParent->childCount();i++) {
        QTreeWidgetItem* twi = twiParent->child(i);
        twiChildList.append(twi);
        if(twi->childCount() > 0) {
            twiChildList.append(getChildListRecursive(twi));
        }
    }
    return twiChildList;
}






