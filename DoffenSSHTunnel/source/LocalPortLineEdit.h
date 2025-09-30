#ifndef LOCALPORTLINEEDIT_H
#define LOCALPORTLINEEDIT_H

#include <QLineEdit>
#include <QMenu>
#include <QContextMenuEvent>

class LocalPortLineEdit : public QLineEdit {
    Q_OBJECT
public:
    using QLineEdit::QLineEdit;

signals:
    void signalValidatePort(int port);

protected:
    void contextMenuEvent(QContextMenuEvent *e) override {
        QMenu *menu = createStandardContextMenu();
        menu->addSeparator();

        QAction *validateAct = menu->addAction(tr("Validate Port"));
        connect(validateAct, &QAction::triggered, this, [this] {
            int port = this->text().toInt();   // returns 0 if conversion fails
            emit signalValidatePort(port);
        });

        menu->exec(e->globalPos());
        delete menu;
    }
};
#endif // LOCALPORTLINEEDIT_H
