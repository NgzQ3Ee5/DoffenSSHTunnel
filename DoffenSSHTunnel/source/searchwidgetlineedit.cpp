#include "searchwidgetlineedit.h"
#include <QFocusEvent>
#include <QMouseEvent>
#include <QCompleter>
#include <QAbstractItemView>

SearchWidgetLineEdit::SearchWidgetLineEdit(QWidget *parent)
: ctkSearchBox(parent)
{
}

SearchWidgetLineEdit::~SearchWidgetLineEdit()
{

}

//protected override
void SearchWidgetLineEdit::keyPressEvent(QKeyEvent * e)
{
    ctkSearchBox::keyPressEvent(e);

    switch (e->key()) {
        case Qt::Key_Up:
        case Qt::Key_Down:
            slotShowCompleter();
            break;
        default:
            break;
    }

}

//public
void SearchWidgetLineEdit::slotShowCompleter()
{
    if (completer()) {
        if(!completer()->popup()->isVisible()) {
            completer()->complete();
        }
    }
}


