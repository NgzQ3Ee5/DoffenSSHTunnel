
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

bool TunnelTreeWidgetItemDelegate::helpEvent(QHelpEvent *e, QAbstractItemView *view,
                                             const QStyleOptionViewItem &option,
                                             const QModelIndex &index) {
    if (!e || !view || !index.isValid())
        return false;

    QString displayText = getDisplayText(index);

    QStyleOptionViewItem opt(option);
    initStyleOption(&opt, index);
    opt.widget = view;
    opt.text = displayText;

    // Rects in viewport coordinates
    const QRect cellRect = view->visualRect(index);
    const QRect viewportRect = view->viewport()->rect();

    // The exact rect Qt uses for the *text* inside the cell
    const QRect textRect = view->style()->subElementRect(
        QStyle::SE_ItemViewItemText, &opt, view);

    // 1) Is the *item* itself too small (classic truncation)?
    const QSize ideal = QStyledItemDelegate::sizeHint(opt, index);
    const bool clippedByCell = (ideal.width() > cellRect.width()) ||
                               (ideal.height() > cellRect.height());

    // 2) Is the text area partially outside the viewport?
    // If so, compare the visible width with the full text width.
    const QRect visibleTextRect = textRect.intersected(viewportRect);

    bool clippedByViewport = false;
    {
        const QFontMetrics fm(opt.font);
        if (opt.features.testFlag(QStyleOptionViewItem::WrapText)) {
            // Vertical clipping (multi-line wrap) against viewport
            const QRect needed = fm.boundingRect(
                textRect, opt.displayAlignment | Qt::TextWordWrap, displayText);
            // If textRect is partly out of the viewport, its visible height shrinks
            clippedByViewport = (visibleTextRect.height() < needed.height());
        } else {
            // Single-line: compare full text width vs. visible width (after scroll)
            const int fullTextWidth = fm.horizontalAdvance(displayText);
            clippedByViewport = (visibleTextRect.width() < fullTextWidth);
        }
    }

    const bool shouldShow = clippedByCell || clippedByViewport;

    if (shouldShow) {
        if (!displayText.isEmpty()) {
            QToolTip::showText(e->globalPos(), displayText, view->viewport());
            return true; // handled, no flicker
        }
    }

    QToolTip::hideText();
    return true; // consume so there’s no brief flash from default handling
}

QString TunnelTreeWidgetItemDelegate::getDisplayText(const QModelIndex &index) {
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

    return displayText;
}

