#ifndef EXPORTHOSTSDIALOG_H
#define EXPORTHOSTSDIALOG_H

#include "ui_ExportHostsDialog.h"

class ATSkeletonWindow;

class ExportHostsDialog : public QDialog, private Ui::ExportHostsDialogClass
{
    Q_OBJECT

public:
	ExportHostsDialog(QWidget *parent, ATSkeletonWindow *pMainWindow);
	~ExportHostsDialog();
	void show(QList<QTreeWidgetItem*> twiExportList);
	void setDefaultDirectoryPath(QString directoryPath=QString());
	QString defaultDirectoryPath() {return m_sDefaultDirectoryPath;}
	void setDefaultFileName(QString fileName=QString());
	QString defaultFileName() {return m_sDefaultFileName;}
protected:
	void showEvent(QShowEvent * event);
	void hideEvent(QHideEvent * event);
private:
	QString fileDialog();
	void reset();
private:
	ATSkeletonWindow *m_pMainWindow;
	QPoint m_pos;
	QString m_sDefaultDirectoryPath;
	QString m_sDefaultFileName;
	QString m_sLastSavedFilePath;

private slots:
	void slotExport();
	void slotAutoCheckToggled(bool);
};


#endif 
