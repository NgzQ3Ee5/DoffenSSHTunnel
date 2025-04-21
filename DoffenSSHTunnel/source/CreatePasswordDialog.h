#ifndef CREATEPASSWORDDIALOG_H
#define CREATEPASSWORDDIALOG_H

#include "ui_CreatePasswordDialog.h"

class CreatePasswordDialog : public QDialog, private Ui::CreatePasswordDialogClass
{
    Q_OBJECT

public:
	CreatePasswordDialog(QWidget *parent = 0);
	~CreatePasswordDialog();
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
