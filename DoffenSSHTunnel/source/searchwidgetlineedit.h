#ifndef SEARCHWIDGETLINEEDIT_H
#define SEARCHWIDGETLINEEDIT_H

#include "ctkSearchBox.h"

class SearchWidgetLineEdit : public ctkSearchBox
{
    Q_OBJECT

public:
    SearchWidgetLineEdit(QWidget *parent = 0);
    ~SearchWidgetLineEdit();

public slots:
    void slotShowCompleter();

//override
protected:
    void keyPressEvent(QKeyEvent * e);

};

#endif // SEARCHWIDGETLINEEDIT_H
