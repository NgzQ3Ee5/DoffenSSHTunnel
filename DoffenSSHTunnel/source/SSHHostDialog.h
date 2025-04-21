#ifndef SSHHOSTDIALOG_H
#define SSHHOSTDIALOG_H

#include "ui_SSHHostDialog.h"

class SSHHostDialog : public QDialog, private Ui::SSHHostDialogClass
{
    Q_OBJECT

public:
	SSHHostDialog(QWidget *parent, const QString& host);
	~SSHHostDialog();
	QString sshHost;

private slots:
	virtual void OnCancel();
	virtual void OnOK();
};


#endif 
