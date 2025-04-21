
#include "TunnelTreeWidget.h"

TunnelTreeWidget::TunnelTreeWidget(QWidget *parent)
    : TreeWidget(parent)
{
	m_pDragItem = NULL;
}

void TunnelTreeWidget::dragEnterEvent(QDragEnterEvent *event)
{
	/*
	m_pDragItem = currentItem();
	qDebug() << Q_FUNC_INFO << m_pDragItem->text(0);
	emit signalDragMoveStarted(m_pDragItem);*/
    QTreeWidget::dragEnterEvent(event);
}
/*
void TunnelTreeWidget::dropEvent(QDropEvent *event) 
{
	qDebug() << Q_FUNC_INFO << m_pDragItem->text(0);
	QTreeWidget::dropEvent(event);
	emit signalDragMoveFinished(m_pDragItem);
	m_pDragItem = NULL;
}
*/

void TunnelTreeWidget::dropEvent(QDropEvent *event) 
{
	QTreeWidgetItem *currItem = currentItem();
	if(currItem != NULL && event->source() == this) {
		qDebug() << Q_FUNC_INFO << currItem->text(0);
		emit signalDragMoveStarted(currItem);
	}
	TreeWidget::dropEvent(event);
	if(currItem != NULL && event->source() == this) {
		emit signalDragMoveFinished(currItem);
	}
}

