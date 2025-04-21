#include "pch.h"
#include "CreatePasswordDialog.h"
#include "Images.h"

CreatePasswordDialog::CreatePasswordDialog(QWidget *parent)
: QDialog(parent)
{
	setupUi(this);
	slotLoadIcons();
	connect(buttonBox->button(QDialogButtonBox::Ok),SIGNAL(clicked()),this,SLOT(OnOK()));
	connect(buttonBox->button(QDialogButtonBox::Cancel),SIGNAL(clicked()),this,SLOT(OnCancel()));
	connect(Button_HidePassword,SIGNAL(toggled(bool)),this,SLOT(OnHidePasswordToggled(bool)));
	connect(EditPassword,SIGNAL(textChanged(const QString&)),this,SLOT(OnTextChanged(const QString&)));
	connect(EditPasswordRepeat,SIGNAL(textChanged(const QString&)),this,SLOT(OnTextChanged(const QString&)));
	buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

CreatePasswordDialog::~CreatePasswordDialog()
{

}

void CreatePasswordDialog::slotLoadIcons()
{
	Button_HidePassword->setIcon(Images::icon("sd_010_btn_view"));
}

void CreatePasswordDialog::OnCancel()
{
	done(0);
}


void CreatePasswordDialog::OnOK()
{
	password=EditPassword->text();
	done(1);
}

void CreatePasswordDialog::OnTextChanged(const QString& txt){
	if(txt.isEmpty()) {
		buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
	} else if(EditPassword->text() == EditPasswordRepeat->text()) {
		buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
	} else {
		buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
	}
}

void CreatePasswordDialog::OnHidePasswordToggled(bool checked)
{
	if(checked){
		EditPassword->setEchoMode(QLineEdit::Normal);
		EditPasswordRepeat->setEchoMode(QLineEdit::Normal);
	}
	else {
		EditPassword->setEchoMode(QLineEdit::Password);
		EditPasswordRepeat->setEchoMode(QLineEdit::Password);
	}
}