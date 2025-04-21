#include "SSHHostDialog.h"
#include <QtWidgets/QPushButton>

SSHHostDialog::SSHHostDialog(QWidget *parent, const QString& host)
: QDialog(parent)
{
	setupUi(this);
	connect(buttonBox->button(QDialogButtonBox::Ok),SIGNAL(clicked()),this,SLOT(OnOK()));
	connect(buttonBox->button(QDialogButtonBox::Cancel),SIGNAL(clicked()),this,SLOT(OnCancel()));

	sshHost = host;
	editSSHHost->setPlainText(host);
}

SSHHostDialog::~SSHHostDialog()
{

}

void SSHHostDialog::OnCancel()
{
	done(0);
}


void SSHHostDialog::OnOK()
{
	sshHost=editSSHHost->toPlainText();
	done(1);
}
