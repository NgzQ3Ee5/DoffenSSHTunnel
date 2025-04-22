#include "TableWidget.h"
#include "pch.h"
#include <QtCore/QDebug>
#include <QtGui/QKeyEvent>

TableWidgetCheckBox::TableWidgetCheckBox(QWidget *parent, int row, int col)
    : QCheckBox(parent)
{
	m_rowIndex = row;
	m_colIndex = col;
    installEventFilter((QObject *)this);
    ATVERIFY( connect( this, &TableWidgetCheckBox::checkStateChanged, this, &TableWidgetCheckBox::slotStateChanged ) );
}

void TableWidgetCheckBox::setRowIndex(int index) { m_rowIndex = index; }

int TableWidgetCheckBox::rowIndex() { return m_rowIndex; }

void TableWidgetCheckBox::setColIndex(int index) { m_colIndex = index; }

int TableWidgetCheckBox::colIndex() { return m_colIndex; }

void TableWidgetCheckBox::setEnabled(bool enabled) { QCheckBox::setEnabled(enabled); }

void TableWidgetCheckBox::focusInEvent(QFocusEvent * e)
{
    QCheckBox::focusInEvent(e);
	emit setCurrentCell(m_rowIndex, m_colIndex);
}

void TableWidgetCheckBox::keyPressEvent(QKeyEvent *e)
{
	qDebug() << "TableWidgetCheckBox::keyPressEvent() key: " << e->key();
	switch (e->key()) {
		case Qt::Key_N:
			if(e->modifiers() == Qt::ControlModifier) {
				e->ignore();
			}
			break;
		case Qt::Key_Up:
			if(e->modifiers() == Qt::ControlModifier) {
				e->ignore();
			}
			break;
		case Qt::Key_Down:
			if(e->modifiers() == Qt::ControlModifier) {
				e->ignore();
			}
			break;
		case Qt::Key_Delete:
			e->ignore();
			break;
		default:
			break;
	}

	if(e->isAccepted()) {
		QCheckBox::keyPressEvent(e);
	}
}

void TableWidgetCheckBox::slotStateChanged(int state)
{
	emit signalStateChanged(state, m_rowIndex, m_colIndex);
}


TableWidgetComboBox::TableWidgetComboBox(QWidget *parent, int row, int col)
    : QComboBox(parent)
{
	m_rowIndex = row;
	m_colIndex = col;
	installEventFilter((QObject *)this);
    ATVERIFY( connect( this, &TableWidgetComboBox::currentIndexChanged, this, &TableWidgetComboBox::slotCurrentIndexChanged ) );
}

void TableWidgetComboBox::setRowIndex(int index) { m_rowIndex = index; }

int TableWidgetComboBox::rowIndex() { return m_rowIndex; }

void TableWidgetComboBox::setColIndex(int index) { m_colIndex = index; }

int TableWidgetComboBox::colIndex() { return m_colIndex; }

void TableWidgetComboBox::setEnabled(bool enabled) { QComboBox::setEnabled(enabled); }

void TableWidgetComboBox::focusInEvent(QFocusEvent * e)
{
    QComboBox::focusInEvent(e);
	emit setCurrentCell(m_rowIndex, m_colIndex);
}

void TableWidgetComboBox::keyPressEvent(QKeyEvent *e)
{
	qDebug() << "TableWidgetComboBox::keyPressEvent() key: " << e->key();
	switch (e->key()) {
		case Qt::Key_N:
			if(e->modifiers() == Qt::ControlModifier) {
				e->ignore();
			}
			break;
		case Qt::Key_Up:
			if(e->modifiers() == Qt::ControlModifier) {
				e->ignore();
			}
			break;
		case Qt::Key_Down:
			if(e->modifiers() == Qt::ControlModifier) {
				e->ignore();
			}
			break;
		case Qt::Key_Delete:
			e->ignore();
			break;
		default:
			break;
	}

	if(e->isAccepted()) {
		QComboBox::keyPressEvent(e);
	}
}

void TableWidgetComboBox::slotCurrentIndexChanged(int index)
{
	emit signalCurrentIndexChanged(index, m_rowIndex, m_colIndex);
}

TableWidgetLineEdit::TableWidgetLineEdit(QWidget *parent, int row, int col)
    : QLineEdit(parent)
{
	m_rowIndex = row;
	m_colIndex = col;
	installEventFilter((QObject *)this);
}

void TableWidgetLineEdit::setRowIndex(int index) { m_rowIndex = index; }

int TableWidgetLineEdit::rowIndex() { return m_rowIndex; }

void TableWidgetLineEdit::setColIndex(int index) { m_colIndex = index; }

int TableWidgetLineEdit::colIndex() { return m_colIndex; }

void TableWidgetLineEdit::focusInEvent(QFocusEvent * e)
{
    QLineEdit::focusInEvent(e);
	emit setCurrentCell(m_rowIndex, m_colIndex);
}

void TableWidgetLineEdit::keyPressEvent(QKeyEvent *e)
{
	qDebug() << "TableWidgetLineEdit::keyPressEvent() key: " << e->key();
	switch (e->key()) {
		case Qt::Key_N:
			if(e->modifiers() == Qt::ControlModifier) {
				e->ignore();
			}
			break;
		case Qt::Key_Up:
			if(e->modifiers() == Qt::ControlModifier) {
				e->ignore();
			}
			break;
		case Qt::Key_Down:
			if(e->modifiers() == Qt::ControlModifier) {
				e->ignore();
			}
			break;
		case Qt::Key_Delete:
			e->ignore();
			break;
		default:
			break;
	}

	if(e->isAccepted()) {
		QLineEdit::keyPressEvent(e);
	}
}


TableWidgetPushButton::TableWidgetPushButton(QWidget *parent, int row, int col)
    : QPushButton(parent)
{
	m_rowIndex = row;
	m_colIndex = col;
    ATVERIFY( connect( this, &TableWidgetPushButton::clicked, this, &TableWidgetPushButton::slotClicked ) );
}

void TableWidgetPushButton::setRowIndex(int index) { m_rowIndex = index; }

int TableWidgetPushButton::rowIndex() { return m_rowIndex; }

void TableWidgetPushButton::setColIndex(int index) { m_colIndex = index; }

int TableWidgetPushButton::colIndex() { return m_colIndex; }

void TableWidgetPushButton::setEnabled(bool enabled) { QPushButton::setEnabled(enabled); }

void TableWidgetPushButton::slotClicked()
{ 
    emit signalClicked(m_rowIndex, m_colIndex);
}



TableWidgetToolButton::TableWidgetToolButton(QWidget *parent, int row, int col)
    : QToolButton(parent)
{
	m_rowIndex = row;
	m_colIndex = col;
    ATVERIFY( connect( this, &TableWidgetToolButton::clicked, this, &TableWidgetToolButton::slotClicked ) );
    ATVERIFY( connect( this, &TableWidgetToolButton::toggled, this, &TableWidgetToolButton::slotToggled ) );
}

void TableWidgetToolButton::setRowIndex(int index) { m_rowIndex = index; }

int TableWidgetToolButton::rowIndex() { return m_rowIndex; }

void TableWidgetToolButton::setColIndex(int index) { m_colIndex = index; }

int TableWidgetToolButton::colIndex() { return m_colIndex; }

void TableWidgetToolButton::setEnabled(bool enabled) { QToolButton::setEnabled(enabled); }

void TableWidgetToolButton::slotClicked()
{ 
    emit signalClicked(m_rowIndex, m_colIndex);
}

void TableWidgetToolButton::slotToggled(bool checked)
{ 
	emit signalToggled(checked, m_rowIndex, m_colIndex); 
}

TableWidget::TableWidget(QWidget *parent)
    : QTableWidget(parent)
{
	m_bModifiedAndSaveSignalEnabled = false;
	m_timerModifiedSignal.setSingleShot(true);
	m_timerModifiedSignal.setInterval(100);
	m_timerSaveSignal.setSingleShot(true);
	m_timerSaveSignal.setInterval(500);

    ATVERIFY( connect( this,					&TableWidget::itemChanged,	this, &TableWidget::slotHandleModifiedAndSave ) );
    ATVERIFY( connect( &m_timerModifiedSignal,  &QTimer::timeout,           this, &TableWidget::slotModifiedTimerTimeout, Qt::QueuedConnection ) );
    ATVERIFY( connect( &m_timerSaveSignal,		&QTimer::timeout,           this, &TableWidget::slotSaveTimerTimeout, Qt::QueuedConnection ) );
}

void TableWidget::keyPressEvent(QKeyEvent *e)
{
	qDebug() << "TableWidget::keyPressEvent() key: " << e->key();
	switch (e->key()) {
		case Qt::Key_N:
			if(e->modifiers() == Qt::ControlModifier) {
				e->ignore();
				emit signalKeyCtrlNPressed();
			}
			break;
		case Qt::Key_Up:
			if(e->modifiers() == Qt::ControlModifier) {
				e->ignore();
				emit signalKeyCtrlUpPressed();
			}
			break;
		case Qt::Key_Down:
			if(e->modifiers() == Qt::ControlModifier) {
				e->ignore();
				emit signalKeyCtrlDownPressed();
			}
			break;
		case Qt::Key_Delete:
			e->ignore();
			emit signalKeyDeletePressed();
			break;
		default:
			break;
	}

	if(e->isAccepted()) {
		QTableWidget::keyPressEvent(e);
	}
}

void TableWidget::rowsAboutToBeRemoved(const QModelIndex &i, int s, int e) {
    slotHandleModifiedAndSave();
    QTableWidget::rowsAboutToBeRemoved(i,s,e);
}

void TableWidget::rowsInserted(const QModelIndex &i, int s, int e) {
    slotHandleModifiedAndSave();
    QTableWidget::rowsInserted(i,s,e);
}

void TableWidget::dataChanged(const QModelIndex &l, const QModelIndex &r) {
    //slotHandleModifiedAndSave();
    QTableWidget::dataChanged(l,r);
}


void TableWidget::moveSelectedRowUp()
{
    int sourceRow = getSelectedRowIndex();
    if(sourceRow < 1) return;
    int col = getSelectedColumnIndex();
    int destRow = sourceRow - 1;

	// take whole rows
	QList<QTableWidgetItem*> sourceItems = takeRow(sourceRow);
	QList<QTableWidgetItem*> destItems = takeRow(destRow);

	// set back in reverse order
	setRow(sourceRow, destItems);
	setRow(destRow, sourceItems);
	setCurrentCell(destRow,col);
}

void TableWidget::moveCurrentRowUp()
{
	int sourceRow = currentRow();
	if(sourceRow < 1) return;
	int col = currentColumn();
	int destRow = sourceRow - 1;

	// take whole rows
	QList<QTableWidgetItem*> sourceItems = takeRow(sourceRow);
	QList<QTableWidgetItem*> destItems = takeRow(destRow);

	// set back in reverse order
	setRow(sourceRow, destItems);
	setRow(destRow, sourceItems);
	setCurrentCell(destRow,col);
}

void TableWidget::moveSelectedRowDown()
{
	int sourceRow = getSelectedRowIndex();
	int nrowCount = rowCount();
	if(sourceRow >= (nrowCount-1)) return;	
	int col = getSelectedColumnIndex();
	int destRow = sourceRow + 1;

	// take whole rows
	QList<QTableWidgetItem*> sourceItems = takeRow(sourceRow);
	QList<QTableWidgetItem*> destItems = takeRow(destRow);

	// set back in reverse order
	setRow(sourceRow, destItems);
	setRow(destRow, sourceItems);
	setCurrentCell(destRow,col);
}

int TableWidget::addRowBelowSelected()
{
	int row = getSelectedRowIndex() + 1; //add below selected
	if(selectedItems().size() == 0) {
		//nothing selected, add at bottom
		row = rowCount();
	}

	int col = getSelectedColumnIndex();
	if(col == -1) col = 0;

	insertRow(row);
	for (int col = 0; col < columnCount(); col++)
	{
		QTableWidgetItem *item = new QTableWidgetItem();
		item->setText("");
		setItem(row,col,item);
	}

	resizeRowToContents(row);
	setCurrentCell(row,col);
	setFocus(Qt::OtherFocusReason);

	return row;
}

int TableWidget::getSelectedRowIndex()
{
	int rowIndex = -1;
	QList<QTableWidgetItem *> selectedItemsList = selectedItems();
	if(selectedItemsList.size() > 0) {
		rowIndex = selectedItemsList.at(0)->row();
	}
	return rowIndex;
}

int TableWidget::getSelectedColumnIndex()
{
	int colIndex = -1;
	QList<QTableWidgetItem *> selectedItemsList = selectedItems();
	if(selectedItemsList.size() > 0) {
		colIndex = selectedItemsList.at(0)->column();
	}
	return colIndex;
}

bool TableWidget::isFirstRow(int rowIndex)
{
	if(rowIndex == 0) {
		return true;
	} else {
		return false;
	}
}

bool TableWidget::isLastRow(int rowIndex)
{
	if(rowIndex < (rowCount()-1)) {
		return false;
	} else {
		return true;
	}
}

bool TableWidget::setModifiedAndSaveSignalEnabled(bool enabled) {
    bool previous = m_bModifiedAndSaveSignalEnabled;
    m_bModifiedAndSaveSignalEnabled = enabled;
    if(!enabled) { m_timerModifiedSignal.stop(); m_timerSaveSignal.stop(); }
    return previous;
}

// takes and returns the whole row
QList<QTableWidgetItem*> TableWidget::takeRow(int row)
{
    QList<QTableWidgetItem*> rowItems;
    for (int col = 0; col < columnCount(); ++col)
    {
        rowItems << takeItem(row, col);
    }
    return rowItems;
}

// sets the whole row
void TableWidget::setRow(int row, const QList<QTableWidgetItem*>& rowItems)
{
	for (int col = 0; col < columnCount(); ++col)
	{
		setItem(row, col, rowItems.at(col));
	}
}

void TableWidget::slotHandleModifiedAndSave() {
    if(m_bModifiedAndSaveSignalEnabled) {
        m_timerModifiedSignal.start();
        m_timerSaveSignal.start();
    }
}

void TableWidget::slotModifiedTimerTimeout() {
    emit signalModified();
}

void TableWidget::slotSaveTimerTimeout() {
    emit signalSave();
}

void TableWidget::slotSetCurrentCell(int row, int col) { setCurrentCell(row,col); }



