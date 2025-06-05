#ifndef SEARCHWIDGET_H
#define SEARCHWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QModelIndex>
#include <QMap>
#include <QSortFilterProxyModel>
#include <QCompleter>

class SearchWidgetLineEdit;
class QCompleter;
class QStandardItemModel;
class QTimer;
class ATSkeletonWindow;
class AndFilterProxyModel;
class AndCompleter;

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
    AndCompleter *m_pSearchBoxCompleter;
    QStandardItemModel *m_pSearchBoxCompleterModel;
    AndFilterProxyModel* m_pSearchBoxProxyModel = nullptr;
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
   void slotCompleterActivated(const QModelIndex &);


};

class AndFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit AndFilterProxyModel(QObject* parent = nullptr)
        : QSortFilterProxyModel(parent) {}

    void setFilterWords(const QStringList& words)
    {
        m_words = words;
        invalidateFilter();
    }

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& parent) const override
    {
        if (m_words.isEmpty())
            return false;

        const QModelIndex index = sourceModel()->index(sourceRow, filterKeyColumn(), parent);
        const QString value = sourceModel()->data(index).toString();

        for (const QString& word : m_words)
        {
            if (!value.contains(word, Qt::CaseInsensitive))
                return false;
        }
        return true;
    }

private:
    QStringList m_words;
};

class AndCompleter : public QCompleter
{
    Q_OBJECT
public:
    using QCompleter::QCompleter;

protected:
    QStringList splitPath(const QString &input) const override
    {
        auto proxy = qobject_cast<AndFilterProxyModel*>(this->model());
        if (proxy)
        {
            QStringList words = input.simplified().split(' ', Qt::SkipEmptyParts);
            proxy->setFilterWords(words); // modifies the model
        }
        return QStringList{""}; // disables QCompleter's own filtering
    }
};

#endif // SEARCHWIDGET_H
