
#include "ATLog.h"

#include <QtCore/QIODevice>
#include <QtCore/QFile>
#include <QtCore/QDateTime>


ATLog::ATLog()
{
    m_strFile = "";
}

ATLog::~ATLog()
{

}

void ATLog::setOutputFile( const QString &strFile )
{
    m_strFile = strFile;
}

void ATLog::clear()
{
    if ( m_strFile.length() > 0 )
    {
        QFile pOutputFile( m_strFile );
        if(pOutputFile.open( QFile::WriteOnly|QFile::Truncate )) {
            pOutputFile.close();
        }
    }
}

void ATLog::addEntry( const QString &strEntry )
{
	

    if ( m_strFile.length() > 0 )
	{
        QFile pOutputFile( m_strFile );
        if(pOutputFile.open( QFile::WriteOnly|QFile::Append )) {
            pOutputFile.write( QString("%1: %2\n")
                    .arg( QDateTime::currentDateTime().toString( "yyyy-MM-dd hh:mm:ss.zzz" ) )
                    .arg( strEntry )
                    .toUtf8() );
            pOutputFile.flush();
            pOutputFile.close();
        }
	}
/*
#ifdef WIN32
	{
		QString strDbg;
		strDbg.sprintf( "%s\n", strEntry.toUtf8().data() );
		OutputDebugStringA( strDbg.toUtf8().data() );
	}
#endif
*/
}


void ATLog::addDebugEntry( const QString &strEntry )
{
	addEntry( QString("Debug: %1").arg(strEntry) );
}

void ATLog::addWarningEntry( const QString &strEntry )
{
	addEntry( QString("Warning: %1").arg(strEntry) );
}

void ATLog::addCriticalEntry( const QString &strEntry )
{
	addEntry( QString("Critical: %1").arg(strEntry) );
}

void ATLog::addFatalEntry( const QString &strEntry )
{
	addEntry( QString("Fatal: %1").arg(strEntry) );
}

