#include "searchwidget.h"

#include <QCompleter>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QTimer>
#include <QtWidgets/QHBoxLayout>
#include "searchwidgetlineedit.h"
#include "Images.h"
#include "ATSkeleton.h"

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
    m_pSearchBoxCompleter = new QCompleter(this); //WARN Do not set m_pSearchBox as parent (m_pSearchBox->setCompleter(0) will then delete our completer)

    m_pSearchBoxCompleter->setCompletionColumn(0);
    m_pSearchBoxCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    m_pSearchBoxCompleter->setFilterMode(Qt::MatchContains);
    //m_pSearchBoxCompleter->setCompletionRole(Qt::DisplayRole);
    //m_pSearchBoxCompleter->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    m_pSearchBoxCompleter->setMaxVisibleItems(30);
    m_pSearchBoxCompleter->setModel(m_pSearchBoxCompleterModel);

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

    ATVERIFY( connect( m_pTimerDelayIndex, SIGNAL( timeout() ), this, SLOT( slotDelayIndex() ) ) );
    ATVERIFY( connect( m_pTimerDelayUpdateCompleterIcons, SIGNAL( timeout() ), this, SLOT( slotDelayUpdateCompleterIcons() ) ) );
    ATVERIFY( connect( m_pSearchBox, SIGNAL( textChanged(QString) ), this, SLOT( textChanged(QString) ) ) );
    ATVERIFY( connect( m_pSearchBoxCompleter, SIGNAL(activated(const QModelIndex &)), this, SLOT(slotCompleterActivated(const QModelIndex &) ) ) );
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
            text = QString("/ %0 %1").arg(parentTwi->text(0)).arg(text);
            parentTwi = parentTwi->parent();
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



