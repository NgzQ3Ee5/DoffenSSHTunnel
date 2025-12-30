#include "PreferencesDialog.h"
#include "ATSkeleton.h"

PreferencesDialog::PreferencesDialog(QWidget *parent, ATSkeletonWindow *skeletonWindow)
: QDialog(parent)
{
	setupUi(this);
	m_pSkeletonWindow = skeletonWindow;
    connect(buttonBox->button(QDialogButtonBox::Ok),     &QAbstractButton::clicked, this, &PreferencesDialog::OnOK);
    connect(buttonBox->button(QDialogButtonBox::Cancel), &QAbstractButton::clicked, this, &PreferencesDialog::OnCancel);
	readSettings();
}

PreferencesDialog::~PreferencesDialog()
{

}

void PreferencesDialog::showEvent(QShowEvent * event)
{
    Q_UNUSED(event)
}

void PreferencesDialog::enableEditingToggled(bool)
{
    qDebug( "%s", Q_FUNC_INFO );
	
}

void PreferencesDialog::readSettings()
{
    qDebug( "%s", Q_FUNC_INFO );

	QSettings settings( g_strAppIniFile, QSettings::IniFormat);

	//+++ General
	
	bool bHidePasswordsInLog = settings.value( "hide_passwords_in_log", true ).toBool();
	checkHidePasswordsInLog->setChecked( bHidePasswordsInLog );

	bool bAutoConnectEnabled = settings.value( "auto_connect_enabled", false ).toBool();
	checkAutoConnectEnabled->setChecked( bAutoConnectEnabled );

	bool bAutoReconnectEnabled = settings.value( "auto_reconnect_enabled", false ).toBool();
	checkAutoReconnectEnabled->setChecked( bAutoReconnectEnabled );

    int iAutoReconnectIntervalSeconds = settings.value( "auto_reconnect_interval_seconds", 5 ).toInt();
    spinAutoReconnectIntervalSeconds->setValue(iAutoReconnectIntervalSeconds);

    int iAutoReconnectMaxAttempts = settings.value( "auto_reconnect_max_attempts", 2 ).toInt();
    spinAutoReconnectMaxAttempts->setValue(iAutoReconnectMaxAttempts);

    int iPortRangeStart = settings.value( "portrange_start", 30000 ).toInt();
    spinPortRangeStart->setValue(iPortRangeStart);

	bool bAutoClearLogEnabled = settings.value( "auto_clearlog_enabled", true ).toBool();
	checkAutoClearLogEnabled->setChecked( bAutoClearLogEnabled );

    bool bWrapLogLinesEnabled = settings.value( "wrap_loglines_enabled", false ).toBool();
    checkWrapLogLinesEnabled->setChecked( bWrapLogLinesEnabled );

	bool bConfirmOnQuit = settings.value( "ConfirmOnQuit", true ).toBool();
	checkConfirmOnQuit->setChecked( bConfirmOnQuit );

	bool bAutoBackupEnabled = settings.value( "auto_backup_enabled", true ).toBool();
	checkAutoBackup->setChecked( bAutoBackupEnabled );

	bool bAutoSaveEditPane = settings.value( "autosave_editpane", true ).toBool();
	checkAutoSaveEditPane->setChecked( bAutoSaveEditPane );

    bool bShowTreeSearch = settings.value( "show_tree_search", false ).toBool();
    checkShowTreeSearch->setChecked( bShowTreeSearch );

	//--- General

    //+++ In settings file only

    m_nMobaXTermNewTabDelaySeconds = settings.value("mobaxterm_newtab_delay_seconds", 1).toInt();
    if(m_nMobaXTermNewTabDelaySeconds < 1) {
        m_nMobaXTermNewTabDelaySeconds = 1;
    }
    m_nMobaXTermStartupDelaySeconds = settings.value("mobaxterm_startup_delay_seconds", 3).toInt();
    if(m_nMobaXTermStartupDelaySeconds < 2) {
        m_nMobaXTermStartupDelaySeconds = 2;
    }

    //--- In settings file only
}

void PreferencesDialog::writeSettings()
{
    qDebug( "%s", Q_FUNC_INFO );

	QSettings settings( g_strAppIniFile, QSettings::IniFormat);

	settings.setValue( "ConfirmOnQuit", checkConfirmOnQuit->isChecked() );
	settings.setValue( "hide_passwords_in_log", checkHidePasswordsInLog->isChecked() );
	settings.setValue( "auto_connect_enabled", checkAutoConnectEnabled->isChecked() );
	settings.setValue( "auto_reconnect_enabled", checkAutoReconnectEnabled->isChecked() );
	settings.setValue( "auto_clearlog_enabled", checkAutoClearLogEnabled->isChecked() );
    settings.setValue( "wrap_loglines_enabled", checkWrapLogLinesEnabled->isChecked() );
	settings.setValue( "auto_backup_enabled", checkAutoBackup->isChecked() );
	settings.setValue( "autosave_editpane", checkAutoSaveEditPane->isChecked() );
    settings.setValue( "show_tree_search", checkShowTreeSearch->isChecked() );
    settings.setValue( "auto_reconnect_interval_seconds", spinAutoReconnectIntervalSeconds->value() );
    settings.setValue( "auto_reconnect_max_attempts", spinAutoReconnectMaxAttempts->value() );
    settings.setValue( "portrange_start", spinPortRangeStart->value() );
    settings.setValue( "mobaxterm_newtab_delay_seconds", m_nMobaXTermNewTabDelaySeconds);
    settings.setValue( "mobaxterm_startup_delay_seconds", m_nMobaXTermStartupDelaySeconds);
	settings.sync();
}

void PreferencesDialog::OnCancel()
{
	done(0);
}


void PreferencesDialog::OnOK()
{
	writeSettings();
	done(1);
}
