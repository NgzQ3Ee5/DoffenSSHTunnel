#ifndef MORETUNNELSEDITWIDGET_H
#define MORETUNNELSEDITWIDGET_H

#include "pch.h"
#include "Widget.h"
#include "TableWidget.h"
#include "TruncOrViewportClipToolTipDelegate.h"
#include "LocalPortLineEdit.h"

#include <QtCore>
#include <QtWidgets>
#include <QtGui>

class ATSkeletonWindow;
class TableWidgetComboBox;
class TableWidgetLineEdit;
class TableWidgetCheckBox;
class TableWidget;
class MoreTunnelsTableWidget;
struct PortForwardStruct;

class MoreTunnelsEditWidget : public Widget
{
	Q_OBJECT

#define PORTFORWARD_TYPE_LOCAL ("Local")
#define PORTFORWARD_TYPE_REMOTE ("Remote")
#define PORTFORWARD_TYPE_DYNAMIC ("Dynamic")

public:
	MoreTunnelsEditWidget(QWidget *parent = 0);
	void setup(ATSkeletonWindow *pMainWindow);
	void setData(QList<PortForwardStruct> &portForwardStructList);
	QList<PortForwardStruct> getData();
    enum { COL_ENABLED, COL_NAME, COL_TYPE, COL_LOCALIP, COL_LOCALPORT, COL_DESTHOST, COL_DESTPORT, COL_DESCRIPTION };
    enum {QT_USERROLE_ORIG=Qt::UserRole, QT_USERROLE_NAME=Qt::UserRole+1, QT_USERROLE_UUID=Qt::UserRole+2};

private: 
	void setRowData(int row, PortForwardStruct& pfs);
	void updateRowStyle(int row);
	PortForwardStruct getOrigStruct(int row);
	void setOrigStruct(int row, PortForwardStruct pfs);
	QString ensureIsValidVarName(QTableWidgetItem *currTwi, const QString& text);

private:
	ATSkeletonWindow *m_pSkeletonWindow;
    MoreTunnelsTableWidget *m_pTable;
	QToolButton *m_pBtnAdd;
	QToolButton *m_pBtnDelete;
	QToolButton *m_pBtnMoveUp;
	QToolButton *m_pBtnMoveDown;
	QToolButton *m_pBtnMoreTunnelsMore;

public slots:
	void slotUpdateControls();

private slots:
	void slotItemChanged(QTableWidgetItem*);
	void slotAdd();
	void slotDelete();
	void slotMoveUp();
	void slotMoveDown();
	void slotMoreToggled(bool);
	void slotModified() { emit signalModified(); }
    void slotValidatePort(int port, int row, int column);
    void slotSetNewPort(int currentPort, int row, int column);

signals:
	void signalModified();
};

class MoreTunnelsItemDelegate : public TruncOrViewportClipToolTipDelegate
{
	Q_OBJECT

private slots:
	void slotModified() { emit signalModified(); }
    void slotValidatePort(int port) {
        auto *ed = qobject_cast<LocalPortLineEdit*>(sender());
        if (!ed) return;
        QPersistentModelIndex pidx = ed->property("editorIndex").value<QPersistentModelIndex>();
        if (pidx.isValid()) {
            emit signalValidatePort(port, pidx.row(), pidx.column());
        }
    }
    void slotSetNewPort(int currentPort) {
        auto *ed = qobject_cast<LocalPortLineEdit*>(sender());
        if (!ed) return;
        QPersistentModelIndex pidx = ed->property("editorIndex").value<QPersistentModelIndex>();
        if (pidx.isValid()) {
            emit signalSetNewPort(currentPort, pidx.row(), pidx.column());
        }
    }

signals:
	void signalModified();
    void signalValidatePort(int port, int row, int column);
    void signalSetNewPort(int currentPort, int row, int column);

public:
    MoreTunnelsItemDelegate(QObject* parent) : TruncOrViewportClipToolTipDelegate(parent) { }
	
	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem & option, const QModelIndex & index) const
	{
		if(index.column() == MoreTunnelsEditWidget::COL_NAME) {
			QLineEdit *le = new QLineEdit(parent);
			le->setFrame(false);
            ATVERIFY( connect( le,	&QLineEdit::textEdited, this, &MoreTunnelsItemDelegate::slotModified ) );
			return le;
		} 
		if(index.column() == MoreTunnelsEditWidget::COL_TYPE) {
			QComboBox *cb = new QComboBox(parent);
			cb->setFrame(false);
			cb->addItem(PORTFORWARD_TYPE_LOCAL);
			cb->addItem(PORTFORWARD_TYPE_REMOTE);
			cb->addItem(PORTFORWARD_TYPE_DYNAMIC);
            ATVERIFY( connect( cb,	&QComboBox::currentIndexChanged, this, &MoreTunnelsItemDelegate::slotModified ) );
			return cb;
		} 
		if(index.column() == MoreTunnelsEditWidget::COL_LOCALIP) {
			QLineEdit *le = new QLineEdit(parent);
            ATVERIFY( connect( le,	&QLineEdit::textEdited, this, &MoreTunnelsItemDelegate::slotModified ) );
			le->setFrame(false);
			return le;
		} 
		if(index.column() == MoreTunnelsEditWidget::COL_LOCALPORT) {
            LocalPortLineEdit *le = new LocalPortLineEdit(parent);
            le->setProperty("editorIndex", QVariant::fromValue(QPersistentModelIndex(index))); // Store the index inside the editor
			QIntValidator *v = new QIntValidator(0,65535,parent);
			le->setValidator(v);
            ATVERIFY( connect( le, &QLineEdit::textEdited, this, &MoreTunnelsItemDelegate::slotModified ) );
            ATVERIFY( connect( le, &LocalPortLineEdit::signalValidatePort, this, &MoreTunnelsItemDelegate::slotValidatePort ) );
            ATVERIFY( connect( le, &LocalPortLineEdit::signalSetNewPort, this, &MoreTunnelsItemDelegate::slotSetNewPort ) );
			le->setFrame(false);
			return le;
		} 
		if(index.column() == MoreTunnelsEditWidget::COL_DESTHOST) {
			QLineEdit *le = new QLineEdit(parent);
            ATVERIFY( connect( le,	&QLineEdit::textEdited, this, &MoreTunnelsItemDelegate::slotModified ) );
			le->setFrame(false);
			return le;
		} 
		if(index.column() == MoreTunnelsEditWidget::COL_DESTPORT) {
			QLineEdit *le = new QLineEdit(parent);
			QIntValidator *v = new QIntValidator(0,65535,parent);
			le->setValidator(v);
            ATVERIFY( connect( le,	&QLineEdit::textEdited, this, &MoreTunnelsItemDelegate::slotModified ) );
			le->setFrame(false);
			return le;
		} 
		if(index.column() == MoreTunnelsEditWidget::COL_DESCRIPTION) {
			QLineEdit *le = new QLineEdit(parent);
            ATVERIFY( connect( le,	&QLineEdit::textEdited, this, &MoreTunnelsItemDelegate::slotModified ) );
			le->setFrame(false);
			return le;
		}
		return QStyledItemDelegate::createEditor(parent, option, index);
	}

	void setEditorData(QWidget *editor, const QModelIndex &index) const
	{
		switch(index.column()) {
			case MoreTunnelsEditWidget::COL_TYPE: {
				QString value = index.model()->data(index, Qt::EditRole).toString();
				QComboBox *cb = static_cast<QComboBox*>(editor);
				bool blocked = cb->blockSignals(true);
				if(value == PORTFORWARD_TYPE_LOCAL) cb->setCurrentIndex(0);
				else if(value == PORTFORWARD_TYPE_REMOTE) cb->setCurrentIndex(1);
				else if(value == PORTFORWARD_TYPE_DYNAMIC) cb->setCurrentIndex(2);
				else cb->setCurrentIndex(0);
				cb->blockSignals(blocked);
				break; 
			}
			case MoreTunnelsEditWidget::COL_NAME: 
			case MoreTunnelsEditWidget::COL_LOCALIP:
			case MoreTunnelsEditWidget::COL_LOCALPORT: 
			case MoreTunnelsEditWidget::COL_DESTHOST:
			case MoreTunnelsEditWidget::COL_DESTPORT:
			case MoreTunnelsEditWidget::COL_DESCRIPTION: {
				QString value = index.model()->data(index, Qt::EditRole).toString();
				QLineEdit *le = static_cast<QLineEdit*>(editor);
				le->setText(value);
				break;
			}
			default: QStyledItemDelegate::setEditorData(editor, index);
		}
	}

	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
	{
		switch(index.column()) {
			case MoreTunnelsEditWidget::COL_TYPE: {
				QComboBox *cb = static_cast<QComboBox*>(editor);
				QString value = cb->currentText();
				model->setData(index, value, Qt::EditRole);
				break; 
			}
			case MoreTunnelsEditWidget::COL_NAME: 
			case MoreTunnelsEditWidget::COL_LOCALIP: 
			case MoreTunnelsEditWidget::COL_LOCALPORT: 
			case MoreTunnelsEditWidget::COL_DESTHOST:
			case MoreTunnelsEditWidget::COL_DESTPORT:
			case MoreTunnelsEditWidget::COL_DESCRIPTION: {
				QLineEdit *le = static_cast<QLineEdit*>(editor);
				QString value = le->text();
				model->setData(index, value, Qt::EditRole);
				break;
			}
			default: QStyledItemDelegate::setModelData(editor, model, index);
		}
	}

	void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const
    {
        QStyleOptionViewItem viewItemOption(option);
        if (index.column() == MoreTunnelsEditWidget::COL_ENABLED) {
            const int textMargin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin) + 1;
            QRect newRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
                                                QSize(option.decorationSize.width() + 5,option.decorationSize.height()),
                                                QRect(option.rect.x() + textMargin, option.rect.y(),
                                                      option.rect.width() - (2 * textMargin), option.rect.height()));
            viewItemOption.rect = newRect;
        }
        QStyledItemDelegate::paint(painter, viewItemOption, index);
    }

	virtual bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option,
                             const QModelIndex &index)
    {
        Q_ASSERT(event);
        Q_ASSERT(model);
 
        // make sure that the item is checkable
        Qt::ItemFlags flags = model->flags(index);
        if (!(flags & Qt::ItemIsUserCheckable) || !(flags & Qt::ItemIsEnabled))
            return false;
        // make sure that we have a check state
        QVariant value = index.data(Qt::CheckStateRole);
        if (!value.isValid())
            return false;
        // make sure that we have the right event type
        if (event->type() == QEvent::MouseButtonRelease) {
            const int textMargin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin) + 1;
            QRect checkRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
                                                  option.decorationSize,
                                                  QRect(option.rect.x() + (2 * textMargin), option.rect.y(),
                                                        option.rect.width() - (2 * textMargin),
                                                        option.rect.height()));
            if (!checkRect.contains(static_cast<QMouseEvent*>(event)->pos()))
                return false;
        } else if (event->type() == QEvent::KeyPress) {
            if (static_cast<QKeyEvent*>(event)->key() != Qt::Key_Space&& static_cast<QKeyEvent*>(event)->key() != Qt::Key_Select)
                return false;
        } else {
            return false;
        }
        Qt::CheckState state = (static_cast<Qt::CheckState>(value.toInt()) == Qt::Checked
                                ? Qt::Unchecked : Qt::Checked);
        return model->setData(index, state, Qt::CheckStateRole);
    }

};

class MoreTunnelsTableWidget : public TableWidget
{
	Q_OBJECT

private:
	MoreTunnelsItemDelegate* m_pItemDelegate;

public:
    MoreTunnelsTableWidget(QWidget *parent = 0) : TableWidget(parent) {
		m_pItemDelegate = new MoreTunnelsItemDelegate(this);
		setItemDelegate(m_pItemDelegate);
        ATVERIFY( connect( m_pItemDelegate,	&MoreTunnelsItemDelegate::signalModified, this, &MoreTunnelsTableWidget::slotModified ) );
        ATVERIFY( connect( m_pItemDelegate,	&MoreTunnelsItemDelegate::signalValidatePort, this, &MoreTunnelsTableWidget::slotValidatePort ) );
        ATVERIFY( connect( m_pItemDelegate,	&MoreTunnelsItemDelegate::signalSetNewPort, this, &MoreTunnelsTableWidget::slotSetNewPort ) );
	}

private slots:
	void slotModified() { emit signalModified(); }
    void slotValidatePort(int port, int row, int column) { emit signalValidatePort(port, row, column); }
    void slotSetNewPort(int currentPort, int row, int column) { emit signalSetNewPort(currentPort, row, column); }

signals:
	void signalModified();
    void signalValidatePort(int port, int row, int column);
    void signalSetNewPort(int currentPort, int row, int column);
};


#endif
