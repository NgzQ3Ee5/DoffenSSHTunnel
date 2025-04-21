#ifndef IMPORTHOSTSDIALOG_H
#define IMPORTHOSTSDIALOG_H

#include "ui_ImportHostsDialog.h"

class ATSkeletonWindow;
class Tunnel_c;
struct VariableStruct;

class ImportHostsDialog : public QDialog, private Ui::ImportHostsDialogClass
{
    Q_OBJECT

public:
	ImportHostsDialog(QWidget *parent, ATSkeletonWindow *pMainWindow);
	~ImportHostsDialog();
	void show(QString filePath, QTreeWidgetItem *twiImportAt, int importMode);
	void setDefaultDirectoryPath(QString directoryPath=QString());
	QString defaultDirectoryPath() {return m_sDefaultDirectoryPath;}
	void setDefaultFileName(QString fileName=QString());
	QString defaultFileName() {return m_sDefaultFileName;}
	QString fileDialog();
protected:
	void showEvent(QShowEvent * event);
	void hideEvent(QHideEvent * event);
private:
	QList<Tunnel_c*> getCheckedTunnels();
	void reset();
private:
	ATSkeletonWindow *m_pMainWindow;
	QPoint m_pos;
	QString m_sDefaultDirectoryPath;
	QString m_sDefaultFileName;
	QTreeWidgetItem *m_pTwiImportAt;
	int m_nImportMode;
	QString m_sFilePath;
	QList<VariableStruct> m_listExecutableVariables;
	QList<VariableStruct> m_listUserDefinedVariables;

private slots:
	void slotImport();
	void slotAutoCheckToggled(bool);
};


#endif 
