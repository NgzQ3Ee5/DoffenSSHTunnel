#ifndef TUNNELTREEWIDGET_H
#define TUNNELTREEWIDGET_H

#include "TreeWidget.h"

#include <QtCore>
#include <QtWidgets>
#include <QtGui>
#include <QStyledItemDelegate>
#include <QPainter>

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


class TunnelTreeWidgetItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit TunnelTreeWidgetItemDelegate(QObject *parent = nullptr);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    // Optionally override sizeHint if needed
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const override;
};

#endif
