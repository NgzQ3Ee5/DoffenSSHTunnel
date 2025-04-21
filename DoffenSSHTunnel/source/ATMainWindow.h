#ifndef AT_MAINWINDOW_C
#define AT_MAINWINDOW_C

#include <QtWidgets/QMainWindow>
#include "searchwidget.h"

class QAction;
class ATSkeletonWindow;
class RestoreDialog;
class VariableSettingsDialog;
class PreferencesDialog;
class QWidget;
class QCloseEvent;
class QString;
class QObject;
class QMenu;
class QEvent;

class ATMainWindow_c : public QMainWindow
{
	Q_OBJECT

public:
	ATMainWindow_c( QWidget *vpParent = NULL );
	~ATMainWindow_c();

	bool InitMenusAndActions();
	void setDetectSaveTunnel(bool enable) { m_bDoDetectSaveTunnel = enable; }
	PreferencesDialog* preferences() { return preferencesDialog; }

signals:

public slots:
	void slotEnableEditingToggled(bool);
	void slotChangeMasterPassword( );
	void slotChangeStyle( QString );
	void slotChangeIcons( QString );
	void slotShowAbout();
	void slotLoadIcons();
	void slotShowConfiguredLocalPorts();
	void slotVariablesSetup();
	void slotPreferencesSetup();
	void slotClearAllTunnelLogs();
	void slotSetTrayIcon(int iIndex);
	void slotTentativeQuit();
	void slotRestore();
	void slotTabEditHideButton();

protected slots:
	void slotRestoreDialogAccepted();
	void slotVariableSettingsDialogAccepted();
	void slotPreferencesDialogAccepted();

//override
protected:
	void closeEvent ( QCloseEvent * event );
	bool eventFilter(QObject *obj, QEvent *ev);
    void keyPressEvent(QKeyEvent *e);

private:
	ATSkeletonWindow *m_pSkeletonWindow;
	QString m_strStyle; 
	QString m_strIconTheme;

	QMenu *m_pIconsMenu;

	RestoreDialog *restoreDialog;
	VariableSettingsDialog *variableSettingsDialog;
	PreferencesDialog *preferencesDialog;

	QAction *m_pActionEnableEditing;
	QMenu *m_pMenuExportTunnels;
	QAction *m_pActionExportSelectedTunnels;
	QAction *m_pActionExportAllTunnels;
	QMenu *m_pMenuImportTunnels;
	QAction *m_pActionImportTunnelsAfter;
	QAction *m_pActionImportTunnelsChild;
	QAction *m_pActionBackupTunnelSettings;
	QAction *m_pActionRestoreTunnelSettings;
	QAction *m_pActionChangeMasterPassword;
	QAction *m_pActionSetupVariables;

	bool m_bDoDetectSaveTunnel;

    SearchWidget *m_pSearchWidget;


    void readWindowSettings();
    void writeWindowSettings();
    void writeSettingsExecutableVariables();

	friend class ATSkeletonWindow;
};

#endif
