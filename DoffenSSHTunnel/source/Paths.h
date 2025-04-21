
#ifndef _PATHS_H_
#define _PATHS_H_

#include <QString>

class Paths {

public:

	static void setAppPath(QString path);
	static QString appPath();
	static QString themesPath();

private:
	static QString app_path;
};

#endif
