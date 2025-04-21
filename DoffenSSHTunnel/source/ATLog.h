
#ifndef __AT_LOG_H__
#define __AT_LOG_H__

#include <QtCore/QString>

class ATLog
{
private:
    QString m_strFile;
public:
	ATLog();
	~ATLog();

	void setOutputFile( const QString &strFile );
    void clear();
	void addEntry( const QString &strEntry );
	void addDebugEntry( const QString &strEntry );
	void addWarningEntry( const QString &strEntry );
	void addCriticalEntry( const QString &strEntry );
	void addFatalEntry( const QString &strEntry );
};

#endif

