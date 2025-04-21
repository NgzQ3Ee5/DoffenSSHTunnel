#ifndef WIDGET_H
#define WIDGET_H

#include <QtCore>
#include <QtWidgets>
#include <QtGui>

class Widget : public QWidget
{
	Q_OBJECT

public:
	Widget(QWidget *parent = 0);
	void wireSignals();
	bool isChildWidget(QObject *o);

private slots:
	void slotChildWidgetModified();

signals:
	void signalChildWidgetModified();

};


#endif
