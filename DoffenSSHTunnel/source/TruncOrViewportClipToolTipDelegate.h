#ifndef TRUNCORVIEWPORTCLIPTOOLTIPDELEGATE_H
#define TRUNCORVIEWPORTCLIPTOOLTIPDELEGATE_H

#include <QtWidgets/QStyledItemDelegate>
#include <QtWidgets/QToolTip>
#include <QHelpEvent>

class TruncOrViewportClipToolTipDelegate: public QStyledItemDelegate {
public:
    using QStyledItemDelegate::QStyledItemDelegate;
    bool helpEvent(QHelpEvent *e, QAbstractItemView *view, const QStyleOptionViewItem &option, const QModelIndex &idx) override {
        if (!e || !view || !idx.isValid())
            return false;

        QStyleOptionViewItem opt(option);
        initStyleOption(&opt, idx);
        opt.widget = view;

        // Rects in viewport coordinates
        const QRect cellRect = view->visualRect(idx);
        const QRect viewportRect = view->viewport()->rect();

        // The exact rect Qt uses for the *text* inside the cell
        const QRect textRect = view->style()->subElementRect(
            QStyle::SE_ItemViewItemText, &opt, view);

        // 1) Is the *item* itself too small (classic truncation)?
        const QSize ideal = QStyledItemDelegate::sizeHint(opt, idx);
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
                    textRect, opt.displayAlignment | Qt::TextWordWrap, opt.text);
                // If textRect is partly out of the viewport, its visible height shrinks
                clippedByViewport = (visibleTextRect.height() < needed.height());
            } else {
                // Single-line: compare full text width vs. visible width (after scroll)
                const int fullTextWidth = fm.horizontalAdvance(opt.text);
                clippedByViewport = (visibleTextRect.width() < fullTextWidth);
            }
        }

        const bool shouldShow = clippedByCell || clippedByViewport;

        if (shouldShow) {
            QString tip = idx.data(Qt::ToolTipRole).toString();
            if (tip.isEmpty()) tip = idx.data(Qt::DisplayRole).toString();
            if (!tip.isEmpty()) {
                QToolTip::showText(e->globalPos(), tip, view->viewport());
                return true; // handled, no flicker
            }
        }

        QToolTip::hideText();
        return true; // consume so there’s no brief flash from default handling
    }
};

#endif // TRUNCORVIEWPORTCLIPTOOLTIPDELEGATE_H
