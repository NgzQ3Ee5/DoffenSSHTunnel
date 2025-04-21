#ifndef TUNNELTREEWIDGET_H
#define TUNNELTREEWIDGET_H

#include "TreeWidget.h"

#include <QtCore>
#include <QtWidgets>
#include <QtGui>

class TunnelTreeWidget : public TreeWidget
{
	Q_OBJECT

public:
	TunnelTreeWidget(QWidget *parent = 0);

protected:
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);

private:
	QTreeWidgetItem *m_pDragItem;

signals:
	void signalDragMoveStarted(QTreeWidgetItem*);
	void signalDragMoveFinished(QTreeWidgetItem*);
};


#endif
