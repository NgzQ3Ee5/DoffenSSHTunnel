#ifndef TREEWIDGET_H
#define TREEWIDGET_H

#include <QtWidgets/QTreeWidget>
#include <QtCore/QList>
#include <QtCore/QTimer>

class QTreeWidget;
class QTreeWidgetItem;
class QModelIndex;
class QMenu;
class QAction;
class QKeyEvent;
class QFocusEvent;

class TreeWidget : public QTreeWidget
{
	Q_OBJECT

public:
	TreeWidget(QWidget *parent = 0);
	~TreeWidget();

	void setupStandardControls();
    void setAllowItemsToBeParents(bool allow);
    bool allowItemsToBeParents();

	void setupCheckControls();

	bool isFolder(const QModelIndex& index);
	static bool isFolder(QTreeWidgetItem *twi);
	void setIsFolder(QTreeWidgetItem *twi, bool isFolder=true);

	//autoset check state inheritance. triggers slotUpdateCheckStateInheritance(..)
	void setCheckStateInheritanceEnabled(bool enable);
    bool checkStateInheritanceEnabled();

	//called by slotUpdateCheckStateInheritance(..)
	void updateParentsCheckState(QTreeWidgetItem* twi, int col);
	void updateParentCheckState(QTreeWidgetItem* twi, int col);
	void updateChildrenCheckState(QTreeWidgetItem* twi, int col);

	//called by context-menu check/uncheck selected
	void updateParentsCheckState(QList<QTreeWidgetItem*> twiList, int col); 
	void updateChildrenCheckState(QList<QTreeWidgetItem*> twiList, int col); 

	void clearPasteItems();

	//updatecontrols
	//QTreeWidgetItem* duplicateItem(QTreeWidgetItem* twi);
	//updatecontrols
	void deleteItems(QList<QTreeWidgetItem*> twiList);
	void copyItems(QList<QTreeWidgetItem*> twiList);
	void cutItems(QList<QTreeWidgetItem*> twiList);
	//updatecontrols
	QList<QTreeWidgetItem*> pasteAtCurrentItem(int insertMode);
	//updatecontrols
	void insertAtCurrentItem(QTreeWidgetItem* twiInsert, int insertMode);

	//moving
	bool canMoveUp(QList<QTreeWidgetItem*> twiList);
	bool canMoveUp(QTreeWidgetItem* item);
	bool canMoveDown(QList<QTreeWidgetItem*> twiList);
	bool canMoveDown(QTreeWidgetItem* item);
	bool canMoveLeft(QList<QTreeWidgetItem*> twiList);
	bool canMoveLeft(QTreeWidgetItem* item);
	bool canMoveRight(QList<QTreeWidgetItem*> twiList);
	bool canMoveRight(QTreeWidgetItem* item);
	bool moveItemsUp(QList<QTreeWidgetItem*> twiList);
	bool moveItemUp(QTreeWidgetItem* twi);
	bool moveItemsDown(QList<QTreeWidgetItem*> twiList);
	bool moveItemDown(QTreeWidgetItem* twi);
	bool moveItemsLeft(QList<QTreeWidgetItem*> twiList);
	bool moveItemLeft(QTreeWidgetItem* twi);
	bool moveItemsRight(QList<QTreeWidgetItem*> twiList);
	bool moveItemRight(QTreeWidgetItem* twi);

	QList<QTreeWidgetItem*>& getPasteItems() ;
	int getPasteMode();
	//updatecontrols
	QList<QTreeWidgetItem*> pasteAtItem(QTreeWidgetItem *twiPasteAt, int insertMode);
	QTreeWidgetItem* insertItem(QTreeWidgetItem *twiInsertAt, QTreeWidgetItem *twiInsert, int insertMode);
	bool isItemDuplicateByName(QTreeWidgetItem* twi);
    enum { INSERT_AFTER, INSERT_BEFORE, INSERT_CHILD, PASTE_NONE, PASTE_COPY, PASTE_CUT };
	static int getItemLevel(const QTreeWidgetItem* item);
	bool isSameItem(QTreeWidgetItem* item1, QTreeWidgetItem* item2);
	QTreeWidgetItem* itemBefore(QTreeWidgetItem* item);
	bool hasItemBefore(QTreeWidgetItem* item);
	bool hasItemAfter(QTreeWidgetItem* item);
	bool hasItemAfter(QList<QTreeWidgetItem*> twiList);
	QList<QTreeWidgetItem*> toChronologialAscendingOrder(QList<QTreeWidgetItem*> twiList);
	QList<QTreeWidgetItem*> toChronologialDescendingOrder(QList<QTreeWidgetItem*> twiList);
	QList<QTreeWidgetItem*> findAllThatAreExpanded();
	void setExpanded(QList<QTreeWidgetItem*> twiList);
	void setCurrentItems(QList<QTreeWidgetItem*> twiList, QTreeWidgetItem* twiCurrent=NULL);
	static void setItemForegroundAlpha(QTreeWidgetItem* twi, int col, int alpha);
	static void setRowForegroundAlpha(QTreeWidgetItem* twi, int alpha);
	static void setItemForegroundCut(QTreeWidgetItem* twi, int col, bool doit);
	static void setItemForegroundCut(QTreeWidgetItem* twi, bool doit);
	static bool hasParent(QTreeWidgetItem* item, QTreeWidgetItem* parent);
	static QList<QTreeWidgetItem*> normalizeSelection(QList<QTreeWidgetItem*> twiList);
    static QList<QTreeWidgetItem*> getChildListRecursive(QTreeWidgetItem* twiParent);
    static void collapseRecursively(QTreeWidgetItem* item);
    static void expandRecursively(QTreeWidgetItem* item);
	//check states
	bool hasAllCheckState(QList<QTreeWidgetItem*> twiList, Qt::CheckState checkCheckState);
	bool hasCheckState(QTreeWidgetItem* twi, Qt::CheckState checkCheckState);
	bool hasAllChildrenCheckState(QList<QTreeWidgetItem*> twiList, Qt::CheckState checkCheckState);
	bool hasAllChildrenCheckState(QTreeWidgetItem* twi, Qt::CheckState checkCheckState);
	
	void setAllCheckState(QList<QTreeWidgetItem*> twiList, Qt::CheckState checkState);
	void setAllChildrenCheckState(QList<QTreeWidgetItem*> twiList, Qt::CheckState checkState);
	void setAllChildrenCheckState(QTreeWidgetItem* twi, Qt::CheckState checkState);

    QMenu* contextMenu();					//created by constructor
    QAction* actionEdit();						//created by setupStandardControls()
    QAction* actionReanme();				//created by setupStandardControls()
    QAction* actionDuplicate();		//created by setupStandardControls()
    QAction* actionInsertFolderAfter();		//created by setupStandardControls()
    QAction* actionInsertFolderBefore();		//created by setupStandardControls()
    QAction* actionInsertFolderChild();	//created by setupStandardControls()
    QAction* actionInsertAfter();		//created by setupStandardControls()
    QAction* actionInsertBefore();		//created by setupStandardControls()
    QAction* actionInsertChild();	//created by setupStandardControls()
    QAction* actionCut();					//created by setupStandardControls()
    QAction* actionCopy();						//created by setupStandardControls()
    QAction* actionPasteAfter();			//created by setupStandardControls()
    QAction* actionPasteBefore();		//created by setupStandardControls()
    QAction* actionPasteChild();				//created by setupStandardControls()
    QAction* actionDelete();					//created by setupStandardControls()

    bool setModifiedAndSaveSignalEnabled(bool enabled);

//override
public:
    void clear();

//override
protected:
	void keyPressEvent(QKeyEvent *e);
    void rowsRemoved ( const QModelIndex & i, int s, int e );
    void rowsAboutToBeRemoved ( const QModelIndex & i, int s, int e );
    void rowsInserted ( const QModelIndex & i, int s, int e );
    void dataChanged ( const QModelIndex & l, const QModelIndex & r );
    void focusOutEvent ( QFocusEvent * e );

	void dragMoveEvent(QDragMoveEvent* event);
	void dropEvent(QDropEvent* event);

private:
	QTreeWidgetItem* pasteItem(QTreeWidgetItem *twiPasteFrom, QTreeWidgetItem *twiPasteTo, int pasteMode, int insertMode);
    void updatePasteStatus();
    void handleModifiedAndSave();
	void updateStandardControls();
	bool internalMoveItemUp(QTreeWidgetItem* twi);
	bool internalMoveItemDown(QTreeWidgetItem* twi);
	bool internalMoveItemLeft(QTreeWidgetItem* twi);
	bool internalMoveItemRight(QTreeWidgetItem* twi);
	QTreeWidgetItem* createNewFolder(int insertMode);

private:
	QList<QTreeWidgetItem*> m_listItemsToPaste;
	int m_nPasteMode; //Copy or Cut

	bool m_bUsingCheckControls;			  //setupCheckControls()
	bool m_bCheckStateInheritanceEnabled; //enableCheckStateInheritance();

	bool m_bUsingStandardControls;		  //setupStandardControls()
	bool m_bAllowItemsToBeParents;		  //default false
 
	bool m_bEditingEnabled;				  //slotEnableEditing(bool)

	bool m_bModifiedAndSaveSignalEnabled; //setModifiedAndSaveSignalEnabled(bool), used by private handleModified()
	
	QTimer m_timerModifiedSignal;
	QTimer m_timerSaveSignal;			  

	QMenu *m_pContextMenu;
	//setupCheckControls()
	QAction *m_pCheckSelected;
	QAction *m_pUnCheckSelected;
	QAction *m_pCheckAllChildren;
	QAction *m_pUnCheckAllChildren;
	//setupStandardControls()
	QAction *m_pActionEdit;
	QAction *m_pActionRename;
	QAction *m_pActionDuplicate;
	QAction *m_pActionInsertFolderAfter;
	QAction *m_pActionInsertFolderBefore;
	QAction *m_pActionInsertFolderChild;
	QAction *m_pActionInsertAfter;
	QAction *m_pActionInsertBefore;
	QAction *m_pActionInsertChild;
	QAction *m_pActionCut;
	QAction *m_pActionCopy;
	QAction *m_pActionPasteAfter;
	QAction *m_pActionPasteBefore;
	QAction *m_pPasteChild;
	QAction *m_pActionDelete;

public slots:
	//Called IF m_bUsingCheckControls
	void slotCheckSelected();
	void slotUnCheckSelected();
	void slotCheckAllChildren();
	void slotUnCheckAllChildren();
	//Called IF m_bUsingStandardControls
	void slotInsertFolderAfter();
	void slotInsertFolderBefore();
	void slotInsertFolderChild();
	void slotInsertAfter();
	void slotInsertBefore();
	void slotInsertChild();
	QList<QTreeWidgetItem*> slotDuplicate();
	void slotDelete();
	void slotRename();
	void slotCopy();
	void slotCut();
	void slotPasteBefore();
	QList<QTreeWidgetItem*> slotPasteAfter();
	void slotPasteChild();
	void slotClearPasteItems();
	void slotMoveLeft();
	void slotMoveRight();
	void slotMoveUp();
	void slotMoveDown();

	//connected to signalKeySpacePressed() Qt::QueuedConnection
    void slotUpdateCheckStateInheritanceCurrentItem();
	//called by slotItemClicked(...) if m_bCheckStateInheritanceEnabled
	void slotUpdateCheckStateInheritance(QTreeWidgetItem* twi, int col); 

	void slotEnableEditing(bool enable);

	//TREE AND OR DATA IN TREE MODIFIED  
	// handleModifiedAndSave()
	//	-> IF(m_bModifiedAndSaveSignalEnabled) THEN m_timerModifiedSignal.start() <--- 50 millis
	//		-> slotModifiedTimerTimeout()
    void slotModifiedTimerTimeout();

	//TREE AND OR DATA IN TREE MODIFIED  
	// handleModifiedAndSave()
	//	-> IF(m_bModifiedAndSaveSignalEnabled) THEN m_timerSaveSignal.start() <--- more millis (minimum 200)
	//		-> slotSaveTimerTimeout()
    void slotSaveTimerTimeout();
	

private slots:
	void slotEdit();
	void slotUpdateControls();
	void slotCustomContextMenuRequested(const QPoint& pos);
	void slotItemClicked( QTreeWidgetItem* twi, int col ); //chechStateInheritance
    void slotItemChanged(QTreeWidgetItem*,int);

signals:
	void signalModified();
	void signalSave();
	//Called IF m_bUsingCheckControls OR m_bUsingStandardControls
	void signalBeforeShowingContextMenu();
	//changing
	void signalAddItem(int insertMode);
	void signalEditItem();
	void signalUpdateControls();
	void signalPasteDisabled();
	void signalPasteEnabled();
	void signalCreatedNewFolder(QTreeWidgetItem* twi);
	void signalAfterPasteItem(QTreeWidgetItem *twiPasteTo, QTreeWidgetItem *twiInserted, int pasteMode, int insertMode);
	//key events
	void signalKeyCtrlUpPressed();
	void signalKeyCtrlDownPressed();
	void signalKeyCtrlLeftPressed();
	void signalKeyCtrlRightPressed();
	void signalKeyDeletePressed();
	void signalKeySpacePressed();
	void signalKeyEscapePressed();
	void signalKeyCtrlCPressed();
	void signalKeyCtrlXPressed();
	void signalKeyCtrlVPressed();
	void signalKeyCtrlNPressed();
	//moving
	void signalBeforeMoveItemsUp(QList<QTreeWidgetItem*>);
	void signalAfterMoveItemsUp(QList<QTreeWidgetItem*>);
	void signalBeforeMoveItemsDown(QList<QTreeWidgetItem*>);
	void signalAfterMoveItemsDown(QList<QTreeWidgetItem*>);
	void signalBeforeMoveItemsLeft(QList<QTreeWidgetItem*>);
	void signalAfterMoveItemsLeft(QList<QTreeWidgetItem*>);
	void signalBeforeMoveItemsRight(QList<QTreeWidgetItem*>);
	void signalAfterMoveItemsRight(QList<QTreeWidgetItem*>);
};


#endif
