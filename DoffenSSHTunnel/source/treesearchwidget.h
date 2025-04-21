#ifndef TREESEARCHWIDGET_H
#define TREESEARCHWIDGET_H

#include <QWidget>
#include <QTimer>

class ATSkeletonWindow;

class TreeSearchWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TreeSearchWidget(QWidget *parent = 0);
    ~TreeSearchWidget();
    void setup(ATSkeletonWindow *pMainWindow);

private:
    ATSkeletonWindow *m_pSkeletonWindow;
    QTimer m_timerDelaySearch;
    int m_nNextPrevIndex;

signals:

public slots:
    void slotLoadIcons();
    void slotTextChanged();
    void slotSearch();
    void slotSearchUp();
    void slotSearchDown();
    void slotHide();
    void slotShow();
};

#endif // TREESEARCHWIDGET_H
