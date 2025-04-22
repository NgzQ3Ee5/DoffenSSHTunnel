#include "pch.h"
#include "CreatePasswordDialog.h"
#include "Images.h"

CreatePasswordDialog::CreatePasswordDialog(QWidget *parent)
: QDialog(parent)
{
	setupUi(this);
	slotLoadIcons();
    ATVERIFY( connect(buttonBox->button(QDialogButtonBox::Ok), &QAbstractButton::clicked, this, &CreatePasswordDialog::OnOK) );
    ATVERIFY( connect(buttonBox->button(QDialogButtonBox::Cancel), &QAbstractButton::clicked, this, &CreatePasswordDialog::OnCancel) );
    ATVERIFY( connect(Button_HidePassword, &QAbstractButton::clicked, this, &CreatePasswordDialog::OnHidePasswordToggled) );
    ATVERIFY( connect(EditPassword, &QLineEdit::textChanged, this, &CreatePasswordDialog::OnTextChanged) );
    ATVERIFY( connect(EditPasswordRepeat, &QLineEdit::textChanged, this, &CreatePasswordDialog::OnTextChanged) );
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
