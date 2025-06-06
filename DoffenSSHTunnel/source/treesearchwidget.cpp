#include "treesearchwidget.h"
#include "Images.h"
#include "ATSkeleton.h"

TreeSearchWidget::TreeSearchWidget(QWidget *parent) : QWidget(parent)
{
    m_nNextPrevIndex = -1;
    m_timerDelaySearch.setSingleShot(true);
    m_timerDelaySearch.setInterval( 500 );
}

TreeSearchWidget::~TreeSearchWidget()
{

}

void TreeSearchWidget::setup(ATSkeletonWindow *pSkeletonWindow)
{
    m_pSkeletonWindow = pSkeletonWindow;

    ATVERIFY( connect( m_pSkeletonWindow->ui.editTreeTunnelSearch, &QLineEdit::textChanged, this, &TreeSearchWidget::slotTextChanged ) );
    ATVERIFY( connect( m_pSkeletonWindow->ui.editTreeTunnelSearch, &QLineEdit::returnPressed, this, &TreeSearchWidget::slotSearchDown ) );
    ATVERIFY( connect( m_pSkeletonWindow->ui.btnTreeTunnelSearchUp, &QAbstractButton::clicked, this, &TreeSearchWidget::slotSearchUp ) );
    ATVERIFY( connect( m_pSkeletonWindow->ui.btnTreeTunnelSearchDown, &QAbstractButton::clicked, this, &TreeSearchWidget::slotSearchDown ) );
    ATVERIFY( connect( m_pSkeletonWindow->ui.btnTreeTunnelSearchHide, &QAbstractButton::clicked, this, &TreeSearchWidget::slotHide ) );
    ATVERIFY( connect( &m_timerDelaySearch, &QTimer::timeout, this, &TreeSearchWidget::slotSearch, Qt::QueuedConnection ) );
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
        QList<QTreeWidgetItem*> treeTunnelItems = m_pSkeletonWindow->ui.treeTunnels->findTunnelItemsMatching(text);
        if(m_nNextPrevIndex < 0) {
            m_nNextPrevIndex = treeTunnelItems.size()-1;
        }
        if(m_nNextPrevIndex >= treeTunnelItems.size()) {
            m_nNextPrevIndex = 0;
        }
        if(treeTunnelItems.size() > 0) {
            QTreeWidgetItem *item = treeTunnelItems[m_nNextPrevIndex];
            m_pSkeletonWindow->ui.treeTunnels->setCurrentItemWithExpand( item, false );
        }
    }
}

//protected override
void TreeSearchWidget::keyPressEvent(QKeyEvent * e)
{
    switch (e->key()) {
    case Qt::Key_Up:
        slotSearchUp();
        e->accept(); // prevents further propagation
        return;
    case Qt::Key_Down:
        slotSearchDown();
        e->accept(); // prevents further propagation
        return;
    default:
        break;
    }

    // By placing ctkSearchBox::keyPressEvent(e) after the switch and only calling it if the key wasn’t handled,
    // we ensure custom navigation logic takes precedence.
    QWidget::keyPressEvent(e);

}

void TreeSearchWidget::slotTextChanged(){
    m_nNextPrevIndex = -1;
    //m_timerDelaySearch.start();
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

