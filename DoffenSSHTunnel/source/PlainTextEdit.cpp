#include "PlainTextEdit.h"

#include <QtWidgets/QWidget>
#include <QtGui/QKeyEvent>

PlainTextEdit::PlainTextEdit(QWidget *parent)
    : QPlainTextEdit(parent)
{
	m_bIgnoreReturnKey = false;
}

void PlainTextEdit::setIgnoreReturnKey(bool ignore) { m_bIgnoreReturnKey = ignore; }

bool PlainTextEdit::getIgnoreReturnKey() { return m_bIgnoreReturnKey; }

void PlainTextEdit::keyPressEvent(QKeyEvent *e)
{
    //qDebug() << "keyPressEvent() key: " << e->key();
	switch (e->key()) {
		case Qt::Key_Return:
			if(m_bIgnoreReturnKey) {
				e->ignore();
			}
			emit signalReturnKeyPressed();
			break;
		default:
			QPlainTextEdit::keyPressEvent(e);
			break;
	}
}



