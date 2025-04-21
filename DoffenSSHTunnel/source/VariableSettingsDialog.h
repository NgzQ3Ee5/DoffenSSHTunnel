#ifndef VARIABLESETTINGSDIALOG_H
#define VARIABLESETTINGSDIALOG_H

#include "ui_VariableSettingsDialog.h"

class ATSkeletonWindow;
struct VariableStruct;
struct VariableStatStruct;

class VariableSettingsDialog : public QDialog, private Ui::VariableSettingsDialogClass
{
    Q_OBJECT

public:
	VariableSettingsDialog(ATSkeletonWindow *parent);
	~VariableSettingsDialog();
	void setVariableStatistics(QMap<QUuid,VariableStatStruct> statMap); //QUuid: VariableStruct.uUid , VariableStatStruct: The stat values
	void setPasswordVariables(QList<VariableStruct> varList); //Stored in password file DoffenSSHTunnel.pwd
	void setExecutableVariables(QList<VariableStruct> varList); //Stored in app settings DoffenSSHTunnelApp.ini
	void setUserDefinedVariables(QList<VariableStruct> varList); //Stored in hosts settings DoffenSSHTunnel.ini
    QList<QPair<VariableStruct,VariableStruct> > getPasswordVariables();
    QList<QPair<VariableStruct,VariableStruct> > getExecutableVariables();
    QList<QPair<VariableStruct,VariableStruct> > getUserDefinedVariables();
	bool varNameExists(const QString& text);
	void enableEditingToggled(bool);
    QString getMasterPassword();
    void clearMasterPassword();
    void clearAllData();
protected:
	void showEvent(QShowEvent * event);
	void hideEvent(QHideEvent * event);
private:
	void updateStatistics();
	QUuid getPwdUuid(int row);
	QUuid getExecutableUuid(int row);
	QUuid getUserDefinedUuid(int row);
	bool isDuplicateVarName(QTableWidgetItem *currTwi, const QString& text);
	bool isDuplicateVarName(QTableWidget *tw, QTableWidgetItem *currTwi, const QString& text, int col);
	QString ensureIsValidVarName(QTableWidgetItem *currTwi, const QString& text);
    void setReadOnly(QTableWidgetItem *item, bool readOnly);
    QString replaceExecutableVarsFile(const QString &str);
    QString replaceUserDefVars(const QString &str);
private:
	ATSkeletonWindow *m_pSkeletonWindow;
	QPoint m_pos;
	bool m_bEditingEnabled;
    QString m_masterPassword;
	QMap<QUuid,VariableStatStruct> m_statMap; //QUuid: VariableStruct.uUid , VariableStatStruct: The stat values
    enum {EXECUTABLES_COL_INUSE=0, EXECUTABLES_COL_NAME=1, EXECUTABLES_COL_VALUE=2, EXECUTABLES_COL_ARGS=3, EXECUTABLES_COL_BUTTON=4, EXECUTABLES_COL_DESCRIPTION=5};
	enum {USERDEF_COL_INUSE=0, USERDEF_COL_NAME=1, USERDEF_COL_VALUE=2, USERDEF_COL_BUTTON=3, USERDEF_COL_DESCRIPTION=4};
	enum {QT_USERROLE_ORIGVAR=Qt::UserRole, QT_USERROLE_VARNAME=Qt::UserRole+1, QT_USERROLE_VARUUID=Qt::UserRole+2};
	enum {PWD_COL_INUSE=0, PWD_COL_NAME=1, PWD_COL_LOGIN=2, PWD_COL_PWD=3, PWD_COL_BUTTON=4};
    enum {TAB_PASSWORDS=0, TAB_EXECUTABLES=1, TAB_USERDEF=2};
public slots:
	void slotLoadIcons();
private slots:
    //dialog
    void slotAccepted();
    void slotRejected();
    //tab
    void slotTabChanged(int index);
	//passwords
	void slotPwdItemChanged(QTableWidgetItem *item);
	void slotPwdAdd();
	void slotPwdDelete();
	void slotPwdShowPasswords(bool show);
	void slotPwdShowPassword(bool show, int row, int col);
	void slotPwdUpdateControls();
	void slotPwdUpdateTableWidgets();
    void slotPwdEnableEditView(bool enable);
	//signales from QTableWidget
	void slotExecItemChanged(QTableWidgetItem *item);
	void slotUserItemChanged(QTableWidgetItem *item);
	//my signals from TableWidget
	void slotExecBrowse(int row, int column);
	void slotExecAdd();
	void slotExecDelete();
	void slotExecMoveUp();
	void slotExecMoveDown();
	void slotExecUpdateControls();
	void slotExecUpdateBrowseButtons();
	void slotUserBrowse(int row, int column);
	void slotUserAdd();
	void slotUserDelete();
	void slotUserMoveUp();
	void slotUserMoveDown();
	void slotUserUpdateControls();
	void slotUserUpdateBrowseButtons();
};


#endif 
