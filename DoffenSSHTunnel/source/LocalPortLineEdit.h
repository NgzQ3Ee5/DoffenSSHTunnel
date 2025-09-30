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
    void signalSetNewPort(int currentPort);

protected:
    void contextMenuEvent(QContextMenuEvent *e) override {
        QMenu *menu = createStandardContextMenu();
        menu->addSeparator();

        QAction *validateAct = menu->addAction(tr("Validate Port"));
        connect(validateAct, &QAction::triggered, this, [this] {
            int port = this->text().toInt();   // returns 0 if conversion fails
            emit signalValidatePort(port);
        });

        QAction *newAct = menu->addAction(tr("Propose New Port"));
        connect(newAct, &QAction::triggered, this, [this] {
            int currentPort = this->text().toInt();   // returns 0 if conversion fails
            emit signalSetNewPort(currentPort);
        });

        menu->exec(e->globalPos());
        delete menu;
    }
};
#endif // LOCALPORTLINEEDIT_H
