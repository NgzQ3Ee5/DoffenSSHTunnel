#ifndef TRUNCORVIEWPORTCLIPTOOLTIPDELEGATE_H
#define TRUNCORVIEWPORTCLIPTOOLTIPDELEGATE_H

#include <QtWidgets/QStyledItemDelegate>
#include <QtWidgets/QToolTip>
#include <QHelpEvent>

class TruncOrViewportClipToolTipDelegate: public QStyledItemDelegate {
public:
    using QStyledItemDelegate::QStyledItemDelegate;
    bool helpEvent(QHelpEvent *e, QAbstractItemView *view,
                                                 const QStyleOptionViewItem &option,
                                                 const QModelIndex &index) override {
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

    QString getDisplayText(const QModelIndex &index) {
        QString displayText = index.data(Qt::ToolTipRole).toString().trimmed();
        if(displayText.isEmpty()) {
            displayText = index.data(Qt::DisplayRole).toString().trimmed();
        }
        return displayText;
    }
};

#endif // TRUNCORVIEWPORTCLIPTOOLTIPDELEGATE_H
