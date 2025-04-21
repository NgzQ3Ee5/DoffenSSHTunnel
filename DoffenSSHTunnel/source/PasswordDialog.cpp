#include "PasswordDialog.h"
#include "Images.h"

PasswordDialog::PasswordDialog(QWidget *parent)
: QDialog(parent)
{
	setupUi(this);
	slotLoadIcons();
	connect(buttonBox->button(QDialogButtonBox::Ok),SIGNAL(clicked()),this,SLOT(OnOK()));
	connect(buttonBox->button(QDialogButtonBox::Cancel),SIGNAL(clicked()),this,SLOT(OnCancel()));
	connect(Button_HidePassword,SIGNAL(toggled(bool)),this,SLOT(OnHidePasswordToggled(bool)));
	connect(EditPassword,SIGNAL(textChanged(const QString&)),this,SLOT(OnTextChanged(const QString&)));
	buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

PasswordDialog::~PasswordDialog()
{

}

void PasswordDialog::setMessage(QString msg)
{
    lblMessage->setText(msg);
}

void PasswordDialog::slotLoadIcons()
{
	Button_HidePassword->setIcon(Images::icon("ep_010_btn_view"));
}

void PasswordDialog::OnCancel()
{
	done(0);
}


void PasswordDialog::OnOK()
{
	password=EditPassword->text();
	done(1);
}

void PasswordDialog::OnTextChanged(const QString& txt){
	if(txt.isEmpty())
		buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
	else
		buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}

void PasswordDialog::OnHidePasswordToggled(bool checked)
{
	if(checked){
		EditPassword->setEchoMode(QLineEdit::Normal);
	}
	else {
		EditPassword->setEchoMode(QLineEdit::Password);
	}
}
