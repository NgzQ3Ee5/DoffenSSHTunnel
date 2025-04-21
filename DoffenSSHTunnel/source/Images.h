/*  smplayer, GUI front-end for mplayer.
    Copyright (C) 2006-2012 Ricardo Villalba <rvm@users.sourceforge.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _IMAGES_H_
#define _IMAGES_H_

#include <QtCore>
#include <QtWidgets>
#include <QtGui>

#define ICON_THEME_DEFAULT ("__DEFAULT__")
#define ICON_THEME_CUSTOM ("__CUSTOM__")

/* Warning: don't use this until global->preferences is created! */
class Images
{

public:
	static Images& instance() {
        static Images _instance; // create static instance of our class
        return _instance;   // return it
    }
	static QPixmap icon(QString name, int size=-1, bool png = true);
	static QPixmap resize(QPixmap *p, int size=20);
	static void setIconTheme(QString theme) { instance().m_theme = theme; }
	static QString iconTheme() { return instance().m_theme; }
	static QStringList iconThemes();

private:
	Images() { m_theme = QString(); }	// hide constructor
    ~Images() {}	// hide destructor
    Images(const Images &); // hide copy constructor
    Images& operator=(const Images &); // hide assign op
	QString m_theme;
};

#endif

