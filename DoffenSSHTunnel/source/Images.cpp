#include "Images.h"
#include "Paths.h"

QPixmap Images::icon(QString name, int size, bool png) {
    qDebug( "[start] %s", Q_FUNC_INFO );

	QString theme = instance().m_theme;

	QString icon_name = name;
	if(png && !icon_name.endsWith(".png")) icon_name = icon_name+".png";

    qDebug( "Images::icon: theme: '%s'", qPrintable(theme));
    qDebug( "Images::icon: name: '%s'", qPrintable(icon_name));

	QPixmap p;

	if(theme != ICON_THEME_DEFAULT) {
		if(theme == ICON_THEME_CUSTOM) theme = "";
		QString file = Paths::themesPath() + "/icons/" + theme + "/" + icon_name;
        qDebug( "Images::icon: file '%s'", qPrintable(file));
		p = QPixmap(file);
		if(p.isNull()) {
			file = Paths::themesPath() + "/icons" + theme + "/" + icon_name;
            qDebug( "Images::icon: file '%s'", qPrintable(file));
			p = QPixmap(file);
		}
	}

	if (p.isNull()) {
        qDebug( "Images::icon: file ':/icons/%s'", qPrintable(icon_name));
		p = QPixmap(":/icons/" + icon_name);
	}
	if(!p.isNull()) {
		if (size!=-1) {
            qDebug( "Images::icon: resize '%d'", size);
			p = resize(&p,size);
		}
	} else {
		qWarning("Images::icon: icon '%s' not found", name.toUtf8().data());
	}

    qDebug( "[end  ] %s", Q_FUNC_INFO );
	return p;
}

QStringList Images::iconThemes()
{
    qDebug( "[start] %s", Q_FUNC_INFO );

	QStringList themes;
	QDir themesDir(Paths::themesPath());
    qDebug( "themesPath: %s", qPrintable(themesDir.absolutePath()));
	if(themesDir.exists()) {
		QStringList directories = themesDir.entryList(QStringList("icons*"), QDir::Dirs | QDir::NoSymLinks, QDir::Name );
		for(int i=0;i<directories.count();i++) {
			QString dirname = directories.at(i);
			if(dirname.length() > 5) {
				//C:/DoffenSSHTunnel/themes/icons My Theme/
				themes.append(dirname.right(dirname.length()-5)); //icons My Theme -> ' My Theme'
			} else {
				//C:/DoffenSSHTunnel/themes/icons/
				//Ignore if there are no *.png files inside
				QDir iconsDir(Paths::themesPath()+"/"+dirname);
				QStringList files = iconsDir.entryList(QStringList("*.png"), QDir::Files | QDir::NoSymLinks, QDir::Name );
				if(files.count() > 0) {
					themes.append(ICON_THEME_CUSTOM); //the icons dir
				}
				//C:/DoffenSSHTunnel/themes/icons/My Icons 1, My Icons 2,..
				QStringList dirs = iconsDir.entryList(QStringList(), QDir::AllDirs | QDir::NoDotAndDotDot | QDir::NoSymLinks, QDir::Name );
				for(int j=0;j<dirs.count();j++) {
					dirname = dirs.at(j);
					themes.append(dirname); //the icons dir
				}
			}
		}
    } else {
        qDebug( "themesPath not found: %s", qPrintable(themesDir.absolutePath()));
    }
    qDebug( "[end  ] %s", Q_FUNC_INFO );
	return themes;
}


QPixmap Images::resize(QPixmap *p, int size) {
	return QPixmap::fromImage( (*p).toImage().scaled(size,size,Qt::IgnoreAspectRatio,Qt::SmoothTransformation) );
}
