#include "treesearchwidget.h"
#include "Images.h"
#include "ATSkeleton.h"

TreeSearchWidget::TreeSearchWidget(QWidget *parent) : QWidget(parent)
{
    m_nNextPrevIndex = 0;
    m_timerDelaySearch.setSingleShot(true);
    m_timerDelaySearch.setInterval( 500 );
}

TreeSearchWidget::~TreeSearchWidget()
{

}

void TreeSearchWidget::setup(ATSkeletonWindow *pSkeletonWindow)
{
    m_pSkeletonWindow = pSkeletonWindow;

    ATVERIFY( connect( m_pSkeletonWindow->ui.editTreeTunnelSearch, SIGNAL( textChanged(const QString&) ), this, SLOT( slotTextChanged() ) ) );
    ATVERIFY( connect( m_pSkeletonWindow->ui.editTreeTunnelSearch, SIGNAL( returnPressed() ), this, SLOT( slotSearchDown() ) ) );
    ATVERIFY( connect( m_pSkeletonWindow->ui.btnTreeTunnelSearchUp, SIGNAL( clicked() ), this, SLOT( slotSearchUp() ) ) );
    ATVERIFY( connect( m_pSkeletonWindow->ui.btnTreeTunnelSearchDown, SIGNAL( clicked() ), this, SLOT( slotSearchDown() ) ) );
    ATVERIFY( connect( m_pSkeletonWindow->ui.btnTreeTunnelSearchHide, SIGNAL( clicked() ), this, SLOT( slotHide() ) ) );
    ATVERIFY( connect( &m_timerDelaySearch, SIGNAL( timeout() ), this, SLOT( slotSearch() ), Qt::QueuedConnection ) );
}

void TreeSearchWidget::slotLoadIcons()
{
    m_pSkeletonWindow->ui.btnTreeTunnelSearchUp->setIcon(Images::icon("ht_190_btn_search_up"));
    m_pSkeletonWindow->ui.btnTreeTunnelSearchDown->setIcon(Images::icon("ht_200_btn_search_down"));
    m_pSkeletonWindow->ui.editTreeTunnelSearch->slotLoadIcons();
}

void TreeSearchWidget::slotSearch() {
    QString text = m_pSkeletonWindow->ui.editTreeTunnelSearch->text();
    if(text.isEmpty()) {

    } else {
        QList<QTreeWidgetItem*> treeTunnelItems = m_pSkeletonWindow->ui.treeTunnels->findItems(text, Qt::MatchContains | Qt::MatchRecursive, 0);
        if(m_nNextPrevIndex < 0) {
            m_nNextPrevIndex = treeTunnelItems.size()-1;
        }
        if(m_nNextPrevIndex >= treeTunnelItems.size()) {
            m_nNextPrevIndex = 0;
        }
        if(treeTunnelItems.size() > 0) {
            QTreeWidgetItem *item = treeTunnelItems[m_nNextPrevIndex];
            m_pSkeletonWindow->ui.treeTunnels->setCurrentItem( item );
        }
    }
}

void TreeSearchWidget::slotTextChanged(){
    m_nNextPrevIndex = 0;
    m_timerDelaySearch.start();
}

void TreeSearchWidget::slotSearchUp(){
    m_nNextPrevIndex--;
    slotSearch();
}

void TreeSearchWidget::slotSearchDown(){
    m_nNextPrevIndex++;
    slotSearch();
}

void TreeSearchWidget::slotHide(){
    m_pSkeletonWindow->ui.editTreeTunnelSearch->setText("");
    setVisible(false);
}

void TreeSearchWidget::slotShow(){
    setVisible(true);
    m_pSkeletonWindow->ui.editTreeTunnelSearch->setFocus();
    m_pSkeletonWindow->ui.editTreeTunnelSearch->selectAll();
}

