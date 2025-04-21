#ifndef PASSWORDDIALOG_H
#define PASSWORDDIALOG_H

#include "pch.h"
#include "ui_PasswordDialog.h"

class PasswordDialog : public QDialog, private Ui::PasswordDialogClass
{
    Q_OBJECT

public:
	PasswordDialog(QWidget *parent = 0);
	~PasswordDialog();
    void setMessage(QString msg);
	QString password;
public slots:
	void slotLoadIcons();
private slots:
	virtual void OnCancel();
	virtual void OnOK();
	virtual void OnHidePasswordToggled(bool state);
	virtual void OnTextChanged(const QString&);
};


#endif 
