// Precompiled header

#ifndef __DOFFENSSHTUNNEL_PCH__
#define __DOFFENSSHTUNNEL_PCH__

#ifdef WIN32
    // WINDOWS
    #define IF_WIN32(_X) _X
    #define IF_NWIN32(_X)
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
#else
    // LINUX
    #define IF_NWIN32(_X) _X
    #define IF_WIN32(_X)
#endif

//#include <QtCore>
//#include <QtWidgets>
//#include <QtGui>


#include <assert.h>

// For the about box
extern const char *APP_VERSION;
extern const char *APP_NICE_NAME;

// For the registry
extern const char *SETTINGS_COMPANY;
extern const char *SETTINGS_APP;

#ifdef _DEBUG
#define ATASSERT(X) assert(X)
#define ATVERIFY(X) assert(X)
#else // _DEBUG
#define ATASSERT (void)
#define ATVERIFY(X) (X)
#endif // _DEBUG

#ifndef WIN32
	#define sprintf_s sprintf
#endif

class ATLog;
extern ATLog atlog;
extern const char *argv0;
#include <QtCore/QString>
extern QString g_strAppDirectoryPath;
extern QString g_strDataDirectoryPath;
extern QString g_strAppIniFile;
extern QString g_strTunnelIniFile;
extern QString g_strPwdFile;
extern QString g_strWindowTitle;

#endif //__DOFFENSSHTUNNEL_PCH__

