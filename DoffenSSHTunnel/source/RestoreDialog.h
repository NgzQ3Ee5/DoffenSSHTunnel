#ifndef RESTOREDDIALOG_H
#define RESTOREDDIALOG_H

#include "pch.h"
#include "ui_RestoreDialog.h"

class TableItem
{
public:
	TableItem(int nBackupType, const QString& backupFileTimestamp, const QString& backupFilePath, const QString& infoFilePath);
	~TableItem();

	const QString& getBackupFilePath();
	const QString& getInfoFilePath();
	int getBackupType();
	const QString& getBackupFileTimestamp();
	const QString& getDateTime();
	const QString& getDescription();
	
private:
	void loadInfo();

	bool m_isInfoLoaded;
	QString m_backupFilePath;
	QString m_infoFilePath;
	int m_nBackupType;		//BACKUP_TYPE_MANUAL,BACKUP_TYPE_AUTOMATIC,BACKUP_TYPE_AUTOMATIC_SPECIAL	
	QString m_strBackupFileTimestamp;
	QString m_strDateTime;		
	QString m_strDescription;	
};

class TableModel : public QAbstractTableModel
{	
    Q_OBJECT

public:
	TableModel(QObject *parent);
	~TableModel();

	int rowCount(const QModelIndex &parent = QModelIndex()) const ;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	
	void deleteTableData();
	void setTableData(const QList<TableItem*> &tableData);
	TableItem* getTableItem(const QModelIndex &modelIndex);
	TableItem* getTableItem(int index) { return m_tableData.at(index); }
	
private:
	QList<TableItem*> m_tableData;

};


class RestoreDialogSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    RestoreDialogSortFilterProxyModel(QObject *parent = 0);
    void setBackupType(int nBackupType);
	int getBackupType();

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
	bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

private:
	int m_nBackupType;
};

class RestoreDialog : public QDialog, private Ui::RestoreDialogClass
{
    Q_OBJECT

public:
	RestoreDialog(QWidget *parent = 0);
	~RestoreDialog();

public:
	void loadData();
	QString getSelectedBackupFilePath();
	QString getSelectedBackupInfoFilePath();

protected:
	void showEvent(QShowEvent *e);
	void hideEvent(QHideEvent * event);
private:
	RestoreDialogSortFilterProxyModel *proxyModel;
	TableModel* model;
	QString m_selectedBackupDateTime;
	QString m_selectedBackupDescription;
	QString m_selectedBackupFilePath;
	QString m_selectedBackupInfoFilePath;
	QPoint m_pos;
	
private slots:
	virtual void OnCancel();
	virtual void OnOK();
	void slotFinished();
	void slotLoadData();
	void slotCurrentChanged(QModelIndex current, QModelIndex previous);
	void slotTextFilterChanged(const QString & text);
	void slotBackupTypeFilterChanged();
	void slotRowCountChanged(int,int);

signals:
	void signalLoadData();
};


#endif 
