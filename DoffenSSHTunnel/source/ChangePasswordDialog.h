#ifndef CHANGEPASSWORDDIALOG_H
#define CHANGEPASSWORDDIALOG_H

#include "ui_ChangePasswordDialog.h"

class ChangePasswordDialog : public QDialog, private Ui::ChangePasswordDialogClass
{
    Q_OBJECT

public:
	ChangePasswordDialog(QWidget *parent = 0);
	~ChangePasswordDialog();
	QString password;
public slots:
	void slotLoadIcons();
private slots:
	virtual void OnCancel();
	virtual void OnOK();
	virtual void OnHidePasswordToggled(bool state);
	virtual void OnTextChanged();
};


#endif 
