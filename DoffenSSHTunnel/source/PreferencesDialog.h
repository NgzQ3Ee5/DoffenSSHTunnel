#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include "ui_PreferencesDialog.h"

class ATSkeletonWindow;

class PreferencesDialog : public QDialog, private Ui::PreferencesDialogClass
{
    Q_OBJECT

public:
	PreferencesDialog(QWidget *parent, ATSkeletonWindow *skeletonWindow);
	~PreferencesDialog();

	bool confirmOnQuitEnabled() { return checkConfirmOnQuit->isChecked(); }
	bool hidePasswordsInLogEnabled() { return checkHidePasswordsInLog->isChecked(); }
	bool autoConnectEnabled() { return checkAutoConnectEnabled->isChecked(); }
	bool autoReconnectEnabled() { return checkAutoReconnectEnabled->isChecked(); }
	bool autoClearLogEnabled() { return checkAutoClearLogEnabled->isChecked(); }
	bool autoBackupEnabled() { return checkAutoBackup->isChecked(); }
	bool autoSaveEditPaneEnabled() { return checkAutoSaveEditPane->isChecked(); }
    bool showTreeSearchEnabled() { return checkShowTreeSearch->isChecked(); }
    int autoReconnectIntervalSeconds() { return spinAutoReconnectIntervalSeconds->value(); }
    int autoReconnectMaxAttempts() { return spinAutoReconnectMaxAttempts->value(); }

	void enableEditingToggled(bool);

	void readSettings();
	void writeSettings();

protected:
	void showEvent(QShowEvent * event);

private slots:
	virtual void OnCancel();
	virtual void OnOK();

private:
	ATSkeletonWindow *m_pSkeletonWindow;

};


#endif 
