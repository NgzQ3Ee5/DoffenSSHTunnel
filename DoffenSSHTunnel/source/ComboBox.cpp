#include "ComboBox.h"
#include "pch.h"
ComboBox::ComboBox(QWidget *parent)
    : QComboBox(parent)
{
    ATVERIFY( connect( this, &ComboBox::currentIndexChanged, this, &ComboBox::slotModified ) );
    ATVERIFY( connect( this->model(), &QAbstractItemModel::dataChanged, this, &ComboBox::slotModified ) );
}




