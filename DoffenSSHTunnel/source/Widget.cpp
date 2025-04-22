#include "Widget.h"
#include "ComboBox.h"
#include "TreeWidget.h"
#include "TableWidget.h"
#include "pch.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{

}

bool Widget::isChildWidget(QObject *o)
{
	if(o == NULL) return false;
	while(o->parent() != NULL) {
		if(o->parent() == this) return true;
		o = o->parent();
	}
	return false;
}

void Widget::wireSignals()
{
	QList<QStackedWidget*> stackedWidgetList = findChildren<QStackedWidget*>();
	for(int i=0;i<stackedWidgetList.size();i++) {
		bool spyOnIt = true;
		QObject *o = stackedWidgetList.at(i);
		while(o->parent() != NULL) {
			if(qobject_cast<QTabWidget*>(o->parent()) != NULL) spyOnIt=false; //Ignore tab-bar
			o = o->parent();
		}
		if(spyOnIt) {
            ATVERIFY( connect( stackedWidgetList.at(i), &QStackedWidget::currentChanged, this, &Widget::slotChildWidgetModified ) );
		}
	}
	QList<QLineEdit*> lineEditList = findChildren<QLineEdit*>();
	for(int i=0;i<lineEditList.size();i++) {
		bool spyOnIt = true;
		QObject *o = lineEditList.at(i);
		while(o->parent() != NULL) {
			if(qobject_cast<TableWidget*>(o->parent()) != NULL) spyOnIt=false;
			o = o->parent();
		}
		if(spyOnIt) {
            ATVERIFY( connect( lineEditList.at(i), &QLineEdit::textChanged, this, &Widget::slotChildWidgetModified ) );
		}
	}
	QList<QRadioButton*> radioButtonList = findChildren<QRadioButton*>();
	for(int i=0;i<radioButtonList.size();i++) {
		bool spyOnIt = true;
		QObject *o = radioButtonList.at(i);
		while(o->parent() != NULL) {
			if(qobject_cast<TableWidget*>(o->parent()) != NULL) spyOnIt=false;
			o = o->parent();
		}
		if(spyOnIt) {
            ATVERIFY( connect( radioButtonList.at(i), &QRadioButton::toggled, this, &Widget::slotChildWidgetModified ) );
		}
	}
	QList<QCheckBox*> checkBoxList = findChildren<QCheckBox*>();
	for(int i=0;i<checkBoxList.size();i++) {
		bool spyOnIt = true;
		QObject *o = checkBoxList.at(i);
		while(o->parent() != NULL) {
			if(qobject_cast<TableWidget*>(o->parent()) != NULL) spyOnIt=false;
			o = o->parent();
		}
		if(spyOnIt) {
            ATVERIFY( connect( checkBoxList.at(i), &QCheckBox::toggled, this, &Widget::slotChildWidgetModified ) );
		}
	}
	QList<ComboBox*> myComboBoxList = findChildren<ComboBox*>();
	for(int i=0;i<myComboBoxList.size();i++) {
		bool spyOnIt = true;
		QObject *o = myComboBoxList.at(i);
		while(o->parent() != NULL) {
			if(qobject_cast<TableWidget*>(o->parent()) != NULL) spyOnIt=false;
			o = o->parent();
		}
		if(spyOnIt) {
            ATVERIFY( connect( myComboBoxList.at(i), &ComboBox::signalModified, this, &Widget::slotChildWidgetModified ) );
		}
	}
	QList<QPlainTextEdit*> plainTextEditList = findChildren<QPlainTextEdit*>();
	for(int i=0;i<plainTextEditList.size();i++) {
        ATVERIFY( connect( plainTextEditList.at(i), &QPlainTextEdit::textChanged, this, &Widget::slotChildWidgetModified ) );
	}
	QList<QTextEdit*> textEditList = findChildren<QTextEdit*>();
	for(int i=0;i<textEditList.size();i++) {
        ATVERIFY( connect( textEditList.at(i), &QTextEdit::textChanged, this, &Widget::slotChildWidgetModified ) );
	}
	QList<QSpinBox*> spinBoxList = findChildren<QSpinBox*>();
	for(int i=0;i<spinBoxList.size();i++) {
        ATVERIFY( connect( spinBoxList.at(i), &QSpinBox::valueChanged, this, &Widget::slotChildWidgetModified ) );
	}
	QList<QDoubleSpinBox*> doubleSpinBoxList = findChildren<QDoubleSpinBox*>();
	for(int i=0;i<doubleSpinBoxList.size();i++) {
        ATVERIFY( connect( doubleSpinBoxList.at(i), &QDoubleSpinBox::valueChanged, this, &Widget::slotChildWidgetModified ) );
	}
	QList<QDateTimeEdit*> dateTimeEditList = findChildren<QDateTimeEdit*>();
	for(int i=0;i<dateTimeEditList.size();i++) {
        ATVERIFY( connect( dateTimeEditList.at(i), &QDateTimeEdit::dateTimeChanged, this, &Widget::slotChildWidgetModified ) );
	}
	QList<QSlider*> sliderList = findChildren<QSlider*>();
	for(int i=0;i<sliderList.size();i++) {
        ATVERIFY( connect( sliderList.at(i), &QSlider::valueChanged, this, &Widget::slotChildWidgetModified ) );
	}
	QList<QDial*> dialList = findChildren<QDial*>();
	for(int i=0;i<dialList.size();i++) {
        ATVERIFY( connect( dialList.at(i), &QDial::valueChanged, this, &Widget::slotChildWidgetModified ) );
	}
	QList<QListWidget*> listWidgetList = findChildren<QListWidget*>();
	for(int i=0;i<listWidgetList.size();i++) {
        ATVERIFY( connect( listWidgetList.at(i), &QListWidget::itemChanged, this, &Widget::slotChildWidgetModified ) );
	}
    // QList<Widget*> myWidgetList = findChildren<Widget*>();
    // for(int i=0;i<myWidgetList.size();i++) {
    //     ATVERIFY( connect( myWidgetList.at(i), &Widget::signalChildWidgetModified, this, &Widget::slotChildWidgetModified ) );
    // }
	QList<TableWidget*> myTableWidgetList = findChildren<TableWidget*>();
	for(int i=0;i<myTableWidgetList.size();i++) {
        ATVERIFY( connect( myTableWidgetList.at(i), &TableWidget::signalModified, this, &Widget::slotChildWidgetModified ) );
	}
	QList<TreeWidget*> myTreeWidgetList = findChildren<TreeWidget*>();
	for(int i=0;i<myTreeWidgetList.size();i++) {
        ATVERIFY( connect( myTreeWidgetList.at(i), &TreeWidget::signalModified, this, &Widget::slotChildWidgetModified ) );
	}
}

void Widget::slotChildWidgetModified()
{
	emit signalChildWidgetModified();
}




