
#include "TunnelTreeWidget.h"
#include "ATSkeleton.h"
#include "Utils.h"

TunnelTreeWidget::TunnelTreeWidget(QWidget *parent)
    : TreeWidget(parent)
{
	m_pDragItem = NULL;
    setItemDelegate(new TunnelTreeWidgetItemDelegate(this));
}

QList<QTreeWidgetItem*> TunnelTreeWidget::findTunnelItemsMatching(const QString& searchText)
{
    QList<QTreeWidgetItem*> matchingItems;

    QList<QTreeWidgetItem*> treeTunnelItems = this->findItems(".*", Qt::MatchFlags(Qt::MatchRegularExpression | Qt::MatchRecursive), 0);
    for(int i=0;i<treeTunnelItems.size();i++) {
        QTreeWidgetItem *twi = treeTunnelItems[i];
        QString testStr = twi->text(0);

        Tunnel_c *tunnel = ATSkeletonWindow::getTunnel(twi);
        if(tunnel && !tunnel->strDescription.trimmed().isEmpty()) {
            testStr = QString("%0 - %1").arg(testStr, tunnel->strDescription.trimmed());
        }

        bool match = MatchUtils::matchesAllWords(testStr, searchText);
        if (match) {
            matchingItems.append(twi);
        }
    }

    return matchingItems;
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


TunnelTreeWidgetItemDelegate::TunnelTreeWidgetItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void TunnelTreeWidgetItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const
{
    painter->save();

    //QString displayText = index.data(Qt::DisplayRole).toString().remove(SanitizationUtils::unsafeCharsRegex()).trimmed();
    QString displayText = index.data(Qt::DisplayRole).toString().trimmed();

    QVariant v = index.data(Qt::UserRole);
    if(!v.isNull() && v.isValid()) {
        Tunnel_c* pt = (Tunnel_c*)v.value<qulonglong>();
        //QString strDescription = pt->strDescription.remove(SanitizationUtils::unsafeCharsRegex()).trimmed();
        QString strDescription = pt->strDescription.trimmed();
        if(!strDescription.isEmpty()) {
            displayText += " - " + strDescription;
        }
    }

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);
    opt.text = displayText;

    const QWidget *widget = option.widget;
    QStyle *style = widget ? widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, widget);

    painter->restore();
}

QSize TunnelTreeWidgetItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                               const QModelIndex &index) const
{
    // Let base implementation handle it
    return QStyledItemDelegate::sizeHint(option, index);
}

