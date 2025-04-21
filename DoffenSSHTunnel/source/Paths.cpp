#include "Paths.h"
#include <QLibraryInfo>
#include <QLocale>
#include <QFile>
#include <QRegularExpression>
#include <QDir>

#ifndef Q_OS_WIN
#include <stdlib.h>
#endif

QString Paths::app_path;

void Paths::setAppPath(QString path) {
	app_path = path;
}

QString Paths::appPath() {
	return app_path;
}

QString Paths::themesPath() {
#ifdef THEMES_PATH
	QString path = QString(THEMES_PATH);
	if (!path.isEmpty())
		return path;
	else
		return appPath() + "/themes";
#else
    //begin v0.9.42 macos: moved my themes dir from MacOS dir to Resources dir
    #ifdef Q_OS_MACOS
        return appPath() + "/../Resources/themes"; //DoffenSSHTunnel.app/Contents/Resources/themes
    #else
        return appPath() + "/themes";
    #endif
    //end v.0.9.42
#endif
}
