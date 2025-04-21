#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H

#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QTableWidget>
#include <QtCore/QTimer>

class TableWidgetCheckBox : public QCheckBox
{
	Q_OBJECT

public:
	TableWidgetCheckBox(QWidget *parent, int row, int col);
    void setRowIndex(int index);
    int rowIndex();
    void setColIndex(int index);
    int colIndex();
    void setEnabled(bool enabled);

private:
	int m_rowIndex;
	int m_colIndex;

protected:
	//override
	void focusInEvent(QFocusEvent * e);
	void keyPressEvent(QKeyEvent *e);

private slots:
	void slotStateChanged(int);

signals:
	void setCurrentCell(int rowIndex, int colIndex);
	void signalStateChanged(int state, int rowIndex, int colIndex);
};

class TableWidgetComboBox : public QComboBox
{
	Q_OBJECT

public:
	TableWidgetComboBox(QWidget *parent, int row, int col);
    void setRowIndex(int index);
    int rowIndex();
    void setColIndex(int index);
    int colIndex();
    void setEnabled(bool enabled);

private:
	int m_rowIndex;
	int m_colIndex;

protected:
	//override
	void focusInEvent(QFocusEvent * e);
	void keyPressEvent(QKeyEvent *e);

private slots:
	void slotCurrentIndexChanged(int);

signals:
	void setCurrentCell(int rowIndex, int colIndex);
	void signalCurrentIndexChanged(int,int,int);
};

class TableWidgetLineEdit : public QLineEdit
{
	Q_OBJECT

public:
	TableWidgetLineEdit(QWidget *parent, int row, int col);
    void setRowIndex(int index);
    int rowIndex();
    void setColIndex(int index);
    int colIndex();

private:
	int m_rowIndex;
	int m_colIndex;

protected:
	//override
	void focusInEvent(QFocusEvent * e);
	void keyPressEvent(QKeyEvent *e);

signals:
	void setCurrentCell(int rowIndex, int colIndex);
};

class TableWidgetPushButton : public QPushButton
{
	Q_OBJECT

public:
	TableWidgetPushButton(QWidget *parent, int row, int col);
    void setRowIndex(int index);
    int rowIndex();
    void setColIndex(int index);
    int colIndex();
    void setEnabled(bool enabled);

private:
	int m_rowIndex;
	int m_colIndex;

private slots:
	void slotClicked();

signals:
	void signalClicked(int rowIndex, int colIndex);
};

class TableWidgetToolButton : public QToolButton
{
	Q_OBJECT

public:
	TableWidgetToolButton(QWidget *parent, int row, int col);
    void setRowIndex(int index);
    int rowIndex();
    void setColIndex(int index);
    int colIndex();
    void setEnabled(bool enabled);

private:
	int m_rowIndex;
	int m_colIndex;

private slots:
	void slotClicked();
	void slotToggled(bool checked);

signals:
	void signalClicked(int rowIndex, int colIndex);
	void signalToggled(bool checked, int rowIndex, int colIndex);
};

class TableWidget : public QTableWidget
{
	Q_OBJECT

public:
	TableWidget(QWidget *parent = 0);
	void moveCurrentRowUp();
	void moveSelectedRowUp();
	void moveSelectedRowDown();
	int addRowBelowSelected();
	int getSelectedRowIndex();
	int getSelectedColumnIndex();
	bool isFirstRow(int rowIndex);
	bool isLastRow(int rowIndex);
    bool setModifiedAndSaveSignalEnabled(bool enabled);

//override
protected:
	void keyPressEvent(QKeyEvent *e);
    void rowsAboutToBeRemoved ( const QModelIndex & i, int s, int e );
    void rowsInserted ( const QModelIndex & i, int s, int e );
    void dataChanged ( const QModelIndex & l, const QModelIndex & r );

private:
	QList<QTableWidgetItem*> takeRow(int row);
	void setRow(int row, const QList<QTableWidgetItem*>& rowItems);
	bool m_bModifiedAndSaveSignalEnabled; //setModifiedAndSaveSignalEnabled(bool), used by private handleModified()
	QTimer m_timerModifiedSignal;
	QTimer m_timerSaveSignal;		

private slots:
    void slotHandleModifiedAndSave();

	//TABLE AND OR DATA IN TABLE MODIFIED  
	// handleModifiedAndSave()
	//	-> IF(m_bModifiedAndSaveSignalEnabled) THEN m_timerModifiedSignal.start() <--- 50 millis
	//		-> slotModifiedTimerTimeout()
    void slotModifiedTimerTimeout();

	//TABLE AND OR DATA IN TABLE MODIFIED  
	// handleModifiedAndSave()
	//	-> IF(m_bModifiedAndSaveSignalEnabled) THEN m_timerSaveSignal.start() <--- more millis (minimum 200)
	//		-> slotSaveTimerTimeout()
    void slotSaveTimerTimeout();

public slots:
    void slotSetCurrentCell(int row,int col);
signals:
	void signalKeyCtrlNPressed();
	void signalKeyDeletePressed();
	void signalKeyCtrlUpPressed();
	void signalKeyCtrlDownPressed();
	void signalModified();
	void signalSave();
};


#endif
