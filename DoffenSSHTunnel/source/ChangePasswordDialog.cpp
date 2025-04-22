#include "pch.h"
#include "ChangePasswordDialog.h"
#include "PasswordDb.h"
#include "Images.h"

ChangePasswordDialog::ChangePasswordDialog(QWidget *parent)
: QDialog(parent)
{
	setupUi(this);
	slotLoadIcons();
    ATVERIFY( connect(buttonBox->button(QDialogButtonBox::Ok), &QAbstractButton::clicked, this, &ChangePasswordDialog::OnOK) );
    ATVERIFY( connect(buttonBox->button(QDialogButtonBox::Cancel), &QAbstractButton::clicked, this, &ChangePasswordDialog::OnCancel) );
    ATVERIFY( connect(Button_HidePassword, &QAbstractButton::toggled, this, &ChangePasswordDialog::OnHidePasswordToggled) );
    ATVERIFY( connect(EditPasswordOld, &QLineEdit::textChanged, this, &ChangePasswordDialog::OnTextChanged) );
    ATVERIFY( connect(EditPassword, &QLineEdit::textChanged, this, &ChangePasswordDialog::OnTextChanged) );
    ATVERIFY( connect(EditPasswordRepeat, &QLineEdit::textChanged, this, &ChangePasswordDialog::OnTextChanged) );
	buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

ChangePasswordDialog::~ChangePasswordDialog()
{

}

void ChangePasswordDialog::slotLoadIcons()
{
	Button_HidePassword->setIcon(Images::icon("cp_010_btn_view"));
}

void ChangePasswordDialog::OnCancel()
{
	done(0);
}


void ChangePasswordDialog::OnOK()
{
    password=EditPassword->text();
    PasswordDb *pdb = PasswordDb::getInstance();
    if(pdb->comparePassword(EditPasswordOld->text())) {
        done(1);
    } else {
        QMessageBox::warning(this, "Wrong password", "Cannot change password. Old password is wrong!");
    }
}

void ChangePasswordDialog::OnTextChanged(){
	if(EditPassword->text().isEmpty() || EditPasswordRepeat->text().isEmpty()) {
		buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
	} else if(EditPassword->text() == EditPasswordRepeat->text()) {
		buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
	} else {
		buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
	}
}

void ChangePasswordDialog::OnHidePasswordToggled(bool checked)
{
	if(checked){
		EditPasswordOld->setEchoMode(QLineEdit::Normal);
		EditPassword->setEchoMode(QLineEdit::Normal);
		EditPasswordRepeat->setEchoMode(QLineEdit::Normal);
	}
	else {
		EditPasswordOld->setEchoMode(QLineEdit::Password);
		EditPassword->setEchoMode(QLineEdit::Password);
		EditPasswordRepeat->setEchoMode(QLineEdit::Password);
	}
}
