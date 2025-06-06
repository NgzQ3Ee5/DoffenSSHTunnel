#ifndef SEARCHWIDGET_H
#define SEARCHWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QModelIndex>
#include <QMap>

    class SearchWidgetLineEdit;
class QCompleter;
class QStandardItemModel;
class QTimer;
class ATSkeletonWindow;

class SearchWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SearchWidget(QWidget *parent = 0);
    ~SearchWidget();
    void setup(ATSkeletonWindow *pMainWindow);

    void focus();
    void showCompleter();

private:
    SearchWidgetLineEdit *m_pSearchBox;
    QCompleter *m_pSearchBoxCompleter;
    QStandardItemModel *m_pSearchBoxCompleterModel;
    ATSkeletonWindow *m_pSkeletonWindow;
    QTimer *m_pTimerDelayIndex;
    QTimer *m_pTimerDelayUpdateCompleterIcons;
    QMap <QUuid, QModelIndex> *m_pMapUuuidToModelIndex;

public slots:
    void slotIndex(); //delayes indexing using m_pTimerDelayIndex -> slotDelayIndex()
    void slotLoadIcons();
    void slotUpdateCompleterIcons();

private slots:
    void slotDelayIndex(); //use slotIndex() for delayed execution
    void slotDelayUpdateCompleterIcons(); //use slotUpdateCompleterIcons() for delayed execution
    void textChanged(const QString & text); //called when text box string changes
    void slotReturnPressed();
    void slotCompleterActivated(const QModelIndex &);


};

#endif // SEARCHWIDGET_H
