#include "ComboBox.h"
#include "pch.h"
ComboBox::ComboBox(QWidget *parent)
    : QComboBox(parent)
{
	ATVERIFY( connect( this, SIGNAL( currentIndexChanged(int) ), this, SLOT( slotModified() ) ) );
	ATVERIFY( connect( this->model(), SIGNAL( dataChanged (const QModelIndex&, const QModelIndex&) ), this, SLOT( slotModified() ) ) );
}




