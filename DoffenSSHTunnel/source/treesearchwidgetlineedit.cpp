#include "treesearchwidgetlineedit.h"
#include "Images.h"

TreeSearchWidgetLineEdit::TreeSearchWidgetLineEdit(QWidget *parent)
: ctkSearchBox(parent)
{
    //setClearIcon(QIcon(":/icons/ht_220_btn_search_clear.png"));
    //setSearchIcon(QIcon(":/icons/ht_210_btn_search.png"));
    setClearIcon(Images::icon("ht_220_btn_search_clear"));
    setSearchIcon(Images::icon("ht_210_btn_search"));
    setShowSearchIcon(true);
    //Ctrl+K captured in ATMainWindow_c::keyPressEvent
#ifndef Q_OS_MACOS
    setPlaceholderText("Cycle matches… (↑↓ or Enter, Ctrl+K)"); //Not right on Mac so just removing
#endif
    setToolTip("Jump through matching items using Enter or arrow keys. Shortcut: Ctrl+K");
}

TreeSearchWidgetLineEdit::~TreeSearchWidgetLineEdit()
{

}

void TreeSearchWidgetLineEdit::slotLoadIcons()
{

    setClearIcon(Images::icon("ht_220_btn_search_clear"));
    setSearchIcon(Images::icon("ht_210_btn_search"));

}

