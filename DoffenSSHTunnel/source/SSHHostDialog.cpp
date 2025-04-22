#include "SSHHostDialog.h"
#include <QtWidgets/QPushButton>

SSHHostDialog::SSHHostDialog(QWidget *parent, const QString& host)
: QDialog(parent)
{
	setupUi(this);
    connect(buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &SSHHostDialog::OnOK);
    connect(buttonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, this, &SSHHostDialog::OnCancel);

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
