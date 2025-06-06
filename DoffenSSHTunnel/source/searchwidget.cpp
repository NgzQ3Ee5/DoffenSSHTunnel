#include "searchwidget.h"

#include <QCompleter>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QTimer>
#include <QtWidgets/QHBoxLayout>
#include "searchwidgetlineedit.h"
#include "Images.h"
#include "ATSkeleton.h"
#include "Utils.h"


class CustomFilterModel : public QSortFilterProxyModel {
public:
    CustomFilterModel(QObject *parent = nullptr) : QSortFilterProxyModel(parent) {}
    void setMyFilterText(QString text) {
        myFilterText = text.trimmed();
    }

private:
    QString myFilterText;

protected:
    // Called for each row in my QCompleter data model (my m_pSearchBoxCompleterModel)
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const override {
        QModelIndex index = sourceModel()->index(sourceRow, filterKeyColumn(), sourceParent);
        QString text = index.data(Qt::DisplayRole).toString();
        if(myFilterText.isEmpty()) {
            return true;
        }
        return MatchUtils::matchesAllWords(text, myFilterText);
    }
};

class CustomCompleter : public QCompleter {
public:
    CustomCompleter(QObject *parent = nullptr) : QCompleter(parent) {}
    QStringList splitPath(const QString &path) const {
        //Q_UNUSED(path)
        return QStringList({""}); // Disables the QCompleter's own filtering
    }
};

SearchWidget::SearchWidget(QWidget *parent) : QWidget(parent)
{
    m_pMapUuuidToModelIndex = new QMap<QUuid, QModelIndex>();

    m_pTimerDelayIndex = new QTimer(this);
    m_pTimerDelayIndex->setSingleShot(true);
    m_pTimerDelayIndex->setInterval(500);

    m_pTimerDelayUpdateCompleterIcons = new QTimer(this);
    m_pTimerDelayUpdateCompleterIcons->setSingleShot(true);
    m_pTimerDelayUpdateCompleterIcons->setInterval(500);

    setObjectName(QStringLiteral("searchWidget"));
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setSpacing(0);
    layout->setObjectName(QStringLiteral("searchWidgetLayout"));
    layout->setContentsMargins(0, 0, 0, 0);

    m_pSearchBoxCompleterModel = new QStandardItemModel(this);
    m_pSearchBox = new SearchWidgetLineEdit(this);
    m_pSearchBoxCompleter = new CustomCompleter(this); //WARN Do not set m_pSearchBox as parent (m_pSearchBox->setCompleter(0) will then delete our completer)
    auto *proxy = new CustomFilterModel(m_pSearchBoxCompleter);
    proxy->setSourceModel(m_pSearchBoxCompleterModel); // your QStringListModel or similar
    proxy->setFilterKeyColumn(0); // if needed
    m_pSearchBoxCompleter->setModel(proxy);

    m_pSearchBoxCompleter->setCompletionColumn(0);
    m_pSearchBoxCompleter->setMaxVisibleItems(30);

//Ctrl+F captured in ATMainWindow_c::keyPressEvent
#ifndef Q_OS_MACOS
    m_pSearchBox->setPlaceholderText("Type to locate (Ctrl+F)"); //Not right on Mac so just removing
#endif
    m_pSearchBox->setPlaceholderText("");
    m_pSearchBox->setClearIcon(Images::icon("tb_020_btn_search_clear"));
    m_pSearchBox->setSearchIcon(Images::icon("tb_010_btn_search"));
    m_pSearchBox->setShowSearchIcon(true);
    m_pSearchBox->setCompleter(m_pSearchBoxCompleter);

    layout->addWidget(m_pSearchBox);

    ATVERIFY( connect( m_pTimerDelayIndex, &QTimer::timeout, this, &SearchWidget::slotDelayIndex ) );
    ATVERIFY( connect( m_pTimerDelayUpdateCompleterIcons, &QTimer::timeout, this, &SearchWidget::slotDelayUpdateCompleterIcons ) );
    ATVERIFY( connect( m_pSearchBox, &QLineEdit::textChanged, this, &SearchWidget::textChanged ) );
    ATVERIFY(connect(m_pSearchBoxCompleter,
                     static_cast<void (QCompleter::*)(const QModelIndex &)>(&QCompleter::activated),
                     this,
                     &SearchWidget::slotCompleterActivated));

}

SearchWidget::~SearchWidget()
{
}

//public
void SearchWidget::setup(ATSkeletonWindow *pSkeletonWindow)
{
    m_pSkeletonWindow = pSkeletonWindow;
}

//public
void SearchWidget::slotLoadIcons()
{
    qDebug() << "SearchWidget::slotLoadIcons()";
    m_pSearchBox->setClearIcon(Images::icon("tb_020_btn_search_clear"));
    m_pSearchBox->setSearchIcon(Images::icon("tb_010_btn_search"));
}

//public
void SearchWidget::slotIndex()
{
    qDebug() << "SearchWidget::slotIndex()";
    m_pTimerDelayIndex->start();
}

//private
void SearchWidget::slotDelayIndex()
{
    //QMessageBox::information(this, "SearchWidget", "slotDelayIndex");
    qDebug() << "SearchWidget::slotDelayIndex()";
    m_pSearchBoxCompleterModel->clear();
    m_pMapUuuidToModelIndex->clear();

    QList<QTreeWidgetItem*> treeTunnelItems = m_pSkeletonWindow->ui.treeTunnels->findItems(".*", Qt::MatchFlags(Qt::MatchRegularExpression | Qt::MatchRecursive), 0);
    for(int i=0;i<treeTunnelItems.size();i++) {
        QTreeWidgetItem *twi = treeTunnelItems[i];
        Tunnel_c *pt = ATSkeletonWindow::getTunnel(twi);
        if(pt == NULL) continue;

        QString text = QString("/ %0").arg(twi->text(0));

        QTreeWidgetItem* parentTwi = twi->parent();
        while(parentTwi != NULL) {
            QString parentText = parentTwi->text(0);
            Tunnel_c *parentPt = ATSkeletonWindow::getTunnel(parentTwi);
            if(parentPt && !parentPt->strDescription.trimmed().isEmpty()) {
                parentText = QString("%0 - %1").arg(parentText).arg(parentPt->strDescription.trimmed());
            }
            text = QString("/ %0 %1").arg(parentText).arg(text);
            parentTwi = parentTwi->parent();
        }

        if(!pt->strDescription.trimmed().isEmpty()) {
            text = QString("%0 - %1").arg(text).arg(pt->strDescription.trimmed());
        }

        QStandardItem *item = new QStandardItem(text);
        item->setIcon(twi->icon(0));

        item->setData(pt->uUid, Qt::UserRole);
        m_pSearchBoxCompleterModel->appendRow(item);
        m_pMapUuuidToModelIndex->insert(pt->uUid, m_pSearchBoxCompleterModel->indexFromItem(item));
    }
}

//public
void SearchWidget::slotUpdateCompleterIcons()
{
    qDebug() << "SearchWidget::slotUpdateCompleterIcons()";
    m_pTimerDelayUpdateCompleterIcons->start();
}

//private
void SearchWidget::slotDelayUpdateCompleterIcons()
{
    qDebug() << "SearchWidget::slotDelayUpdateCompleterIcons()";

    /*
    QModelIndex currentIndex;
    #ifndef QT_NO_LISTVIEW
        if(QListView *listView = qobject_cast<QListView *>(m_pSearchBoxCompleter->popup())) {
            currentIndex = listView->currentIndex();
        }
    #endif
    */

    QList<QTreeWidgetItem*> treeTunnelItems = m_pSkeletonWindow->ui.treeTunnels->findItems(".*", Qt::MatchFlags(Qt::MatchRegularExpression | Qt::MatchRecursive), 0);
    for(int i=0;i<treeTunnelItems.size();i++) {
        QTreeWidgetItem *twi = treeTunnelItems[i];
        Tunnel_c *pt = ATSkeletonWindow::getTunnel(twi);
        ATASSERT(pt);

        if(m_pMapUuuidToModelIndex->contains(pt->uUid)) {
            QModelIndex idx = m_pMapUuuidToModelIndex->value(pt->uUid);
            QStandardItem *item = m_pSearchBoxCompleterModel->itemFromIndex(idx);
            if(item != NULL) {
                QUuid uUid = item->data(Qt::UserRole).value<QUuid>();
                if(uUid == pt->uUid) {
                    item->setIcon(twi->icon(0));
                }
            }
        }
    }

    /*
    #ifndef QT_NO_LISTVIEW
        if(QListView *listView = qobject_cast<QListView *>(m_pSearchBoxCompleter->popup())) {
            if(currentIndex.isValid() && !listView->currentIndex().isValid()) {
                listView->setCurrentIndex(currentIndex);
            }
        }
    #endif
    */
}

//private
void SearchWidget::textChanged(const QString &text)
{
    if(text.isEmpty()) {
        //reset completer
        m_pSearchBoxCompleter->setCompletionPrefix("");
    }
    CustomFilterModel *proxy = static_cast<CustomFilterModel *>(m_pSearchBoxCompleter->model());
    // I have my own custom filter text member
    proxy->setMyFilterText(m_pSearchBoxCompleter->completionPrefix());
    // Calling this slot with dummy empty string to trigger my proxy to update it's filtered contents (it will call my custom filterAcceptsRow)
    proxy->setFilterFixedString("");
}


//private
void SearchWidget::slotCompleterActivated(const QModelIndex & idx)
{
    QVariant v = m_pSearchBoxCompleter->completionModel()->data(idx, Qt::UserRole);
    if(!v.isNull()) {
        QUuid uuid = v.value<QUuid>();
        QList<QTreeWidgetItem*> treeTunnelItems = m_pSkeletonWindow->ui.treeTunnels->findItems(".*", Qt::MatchFlags(Qt::MatchRegularExpression | Qt::MatchRecursive), 0);
        for(int i=0;i<treeTunnelItems.size();i++) {
            QTreeWidgetItem *twi = treeTunnelItems[i];
            Tunnel_c *pt = ATSkeletonWindow::getTunnel(twi);
            ATASSERT(pt);
            if(pt->uUid == uuid) {
                m_pSkeletonWindow->ui.treeTunnels->setCurrentItem(twi);
                m_pSkeletonWindow->ui.treeTunnels->setFocus();
                break;
            }
        }
    }
}

//public
void SearchWidget::focus()
{
    m_pSearchBox->setFocus();
    m_pSearchBox->selectAll();
}

//public
void SearchWidget::showCompleter()
{
    m_pSearchBox->slotShowCompleter();
}



