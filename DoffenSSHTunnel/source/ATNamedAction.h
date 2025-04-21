#ifndef __AT_NAMED_ACTION_H__
#define __AT_NAMED_ACTION_H__

#include <QtGui/QAction>
class QObject;
class QString;

class ATNamedAction : public QAction
{
	Q_OBJECT

public:
	ATNamedAction( const QString &strName, QObject * parent );
	~ATNamedAction();

public slots:
	void slotCustomTrigger();

signals:
	void signalTriggerNamed( QString );

private:
	QString m_strName;
};

class ATIconThemeAction : public QAction
{
	Q_OBJECT

public:
	ATIconThemeAction( const QString &name, const QString &theme, QObject * parent );
	~ATIconThemeAction();
	QString name() { return m_strName; }
	QString theme() { return m_strTheme; }

public slots:
	void slotCustomTrigger();

signals:
	void signalTriggerTheme( QString );

private:
	QString m_strName;
	QString m_strTheme;
};

#endif
