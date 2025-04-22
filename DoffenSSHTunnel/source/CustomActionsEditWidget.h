#ifndef CUSTOMACTIONSEDITWIDGET_H
#define CUSTOMACTIONSEDITWIDGET_H


#include "TreeWidget.h"

class Tunnel_c;
class ATSkeletonWindow;
class CustomButtonDialog;
struct CustomActionStruct;

class CustomActionsEditWidget : public QWidget
{
	Q_OBJECT

public:
	CustomActionsEditWidget(QWidget *parent = 0);
	void setup(ATSkeletonWindow *pMainWindow);
	void setData(Tunnel_c *pTunnel);
	QList<CustomActionStruct> getData();

private:
	QUuid getUUuid(QTreeWidgetItem * twi);
	void setUUuid(QTreeWidgetItem * twi, const QUuid &uuid);

public slots:
	void slotLoadIcons();
private slots:
	void slotAddItem(int insertMode = TreeWidget::INSERT_AFTER);
	void slotCreatedNewFolder(QTreeWidgetItem* twi);
	void slotAfterPasteItem(QTreeWidgetItem *twiPasteTo, QTreeWidgetItem *twiInserted, int pasteMode, int insertMode);
	void slotEditItem();
	void slotUpdateControls();
	void slotEditDialogAccepted();
    void slotExecuteCurrentItem();
	void slotExecute(const CustomActionStruct& cas);
	void slotBeforeShowingContextMenu();
	void slotSave();

	void slotItemCollapsed(QTreeWidgetItem * item);
	void slotItemExpanded(QTreeWidgetItem * item);

public: 
    enum { COL_LABEL, COL_COMMAND };

private:
	ATSkeletonWindow *m_pSkeletonWindow;
	Tunnel_c *m_pTunnel;
	QAction *m_pActionExecute;
	CustomButtonDialog *m_pDlgEdit;

signals:
	void signalActionExec(const CustomActionStruct&);
};



#endif
