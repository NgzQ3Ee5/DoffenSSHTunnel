#ifndef CUSTOMBUTTONDIALOG_H
#define CUSTOMBUTTONDIALOG_H

#include "ui_CustomButtonDialog.h"
#include "ATSkeleton.h"
class QTreeWidgetItem;


class CustomButtonDialog : public QDialog, private Ui::CustomButtonDialogClass
{
    Q_OBJECT

public:
	CustomButtonDialog(QWidget *parent, ATSkeletonWindow *pMainWindow);
	~CustomButtonDialog();
    void init(Tunnel_c* pt, QTreeWidgetItem* twi, CustomActionStruct& cas);
	QString getLabel() { return editLabel->text(); }
	QString getCommand() { return editCommand->toPlainText(); }
	QTreeWidgetItem* getTwi() { return m_pTwi; }
protected:
	void showEvent(QShowEvent * event);
	void hideEvent(QHideEvent * event);
private:
	void setVarTableData();
	void addHeaderToVarTable(const QString & strHeader);
	void addEmptyToVarTable();
	void addDataToVarTable(const QList<VariableStruct> & varList);
	int addDataToVarTable(Tunnel_c* pt, const PortForwardStruct & pfs);
	void updateCommandPreview();
private:
	ATSkeletonWindow *m_pMainWindow;
	QTreeWidgetItem* m_pTwi;
	CustomActionStruct m_cas;
	QPoint m_pos;
    Tunnel_c* m_pTunnel;

	QMenu *m_pTableVarContextMenu;
	QAction *m_pTableVarActionCopy;

private slots:
	void slotCommandTextChanged();
    void applyFilter(const QString &);
	void slotTableVarCustomContextMenuRequested(const QPoint&);
	void slotTableVarCopy();
	void slotExecute();

signals:
	void signalActionExec(const CustomActionStruct&);
};


#endif 
