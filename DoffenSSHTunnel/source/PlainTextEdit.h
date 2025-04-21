#ifndef PLAINTEXTEDIT_H
#define PLAINTEXTEDIT_H

#include <QtWidgets/QPlainTextEdit>
class QWidget;
class QKeyEvent;

class PlainTextEdit : public QPlainTextEdit
{
	Q_OBJECT

private:
	bool m_bIgnoreReturnKey;

public:
	PlainTextEdit(QWidget *parent = 0);
    void setIgnoreReturnKey(bool ignore=true);
    bool getIgnoreReturnKey();

protected:
	void keyPressEvent(QKeyEvent *e);

signals:
	void signalReturnKeyPressed();
};


#endif
