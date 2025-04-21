#ifndef COMBOBOX_H
#define COMBOBOX_H

#include <QtCore>
#include <QtWidgets>
#include <QtGui>

class ComboBox : public QComboBox
{
	Q_OBJECT

public:
	ComboBox(QWidget *parent = 0);

private slots:
	void slotModified() { emit signalModified(); }

signals:
	void signalModified();

};


#endif
