#include "TableView.h"
#include "TruncOrViewportClipToolTipDelegate.h"

TableView::TableView(QWidget *parent)
    : QTableView(parent)
{
    this->setItemDelegate(new TruncOrViewportClipToolTipDelegate(this));
}




