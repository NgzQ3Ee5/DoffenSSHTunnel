
#include "ATNamedAction.h"
#include "pch.h"
#include <QtCore/QObject>
#include <QtCore/QString>

ATNamedAction::ATNamedAction( const QString &strName, QObject * parent )
	: QAction( strName, parent )
{
    m_strName = strName;

    bool bRet = connect(this, &ATNamedAction::triggered, this, &ATNamedAction::slotCustomTrigger);
    ATASSERT( bRet );
}

ATNamedAction::~ATNamedAction()
{
}

void ATNamedAction::slotCustomTrigger()
{
    qDebug(Q_FUNC_INFO);

	emit signalTriggerNamed( m_strName );
}


ATIconThemeAction::ATIconThemeAction( const QString &strName, const QString &strTheme, QObject * parent )
	: QAction( strName, parent )
{
	m_strName = strName;
	m_strTheme = strTheme;
    bool bRet = connect(this,  &ATIconThemeAction::triggered, this,  &ATIconThemeAction::slotCustomTrigger);
	ATASSERT( bRet );
}

ATIconThemeAction::~ATIconThemeAction()
{
}

void ATIconThemeAction::slotCustomTrigger()
{
    qDebug(Q_FUNC_INFO);
	emit signalTriggerTheme( m_strTheme );
}


