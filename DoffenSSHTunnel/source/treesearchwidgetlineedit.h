#ifndef TREESEARCHWIDGETLINEEDIT_H
#define TREESEARCHWIDGETLINEEDIT_H

#include "ctkSearchBox.h"

class TreeSearchWidgetLineEdit : public ctkSearchBox
{
public:
    TreeSearchWidgetLineEdit(QWidget *parent = 0);
    ~TreeSearchWidgetLineEdit();

public slots:
    void slotLoadIcons();
};

#endif // TREESEARCHWIDGETLINEEDIT_H
