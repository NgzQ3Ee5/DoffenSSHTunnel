#!/bin/bash
set -e

if ! [ -x "$(which qmake)" ] ; then
    echo "Could not execute qmake" >&2
    exit 1
fi

if ! [ -x "$(which linuxdeployqt)" ] ; then
    echo "Could not execute linuxdeployqt" >&2
    exit 1
fi

if ! [ -x "$(which appimagetool)" ] ; then
    echo "Could not execute appimagetool" >&2
    exit 1
fi

if ! [ -x "$(which make)" ] ; then
    echo "Could not execute make" >&2
    exit 1
fi

#svn checkout https://svn.code.sf.net/p/doffensshtunnel/code/trunk

CUR_DIR=$PWD
SCRIPT_DIR=$( cd ${0%/*} && pwd -P )
DOFFEN_DIR=$( cd "$SCRIPT_DIR/../../" && pwd -P) # DoffenSSHTunnel/trunk directory

source "$DOFFEN_DIR/build/build.properties"
APP_VERSION=${APP_VERSION//[$'\t\r\n']} && APP_VERSION=${APP_VERSION%%*( )}

DIST_NAME="DoffenSSHTunnel"
VERSION="v$APP_VERSION"
DIST_FULLNAME="$DIST_NAME-Ubuntu-$VERSION"
SRC_DIR="$DOFFEN_DIR/DoffenSSHTunnel/source"
BULD_DIR="$CUR_DIR/build-DoffenSSHTunnel-Release"
DIST_DIR="$BULD_DIR/dist"
APP_DIR="$DIST_DIR/$DIST_NAME.AppDir"

if [ -d $BULD_DIR ]; then rm -fr $BULD_DIR; fi
mkdir -p $BULD_DIR

if [ -d $DIST_DIR ]; then rm -fr $DIST_DIR; fi
mkdir -p $DIST_DIR

#Current dir is /buildout

#Compile sshpass
mkdir -p "$BULD_DIR/sshpass"
cp -R "$DOFFEN_DIR/deps/sshpass_v1.05_mymod/source" "$BULD_DIR/sshpass"
cd "$BULD_DIR/sshpass"
./source/configure
make

#Compile Doffen
cd $BULD_DIR
qmake "$DOFFEN_DIR/DoffenSSHTunnel/DoffenSSHTunnel.pro"
make

#/buildout/build-DoffenSSHTunnel-Release/dist/DoffenSSHTunnel.AppDir/usr/bin
mkdir -p "$APP_DIR/usr/bin"
cp "$BULD_DIR/source/DoffenSSHTunnel" "$APP_DIR/usr/bin/$DIST_NAME"
cp "$BULD_DIR/sshpass/sshpass" "$APP_DIR/usr/bin/sshpass"
cp "$DOFFEN_DIR/deps/linux_scripts/exec1.sh" "$APP_DIR/usr/bin/exec1.sh"
cp "$DOFFEN_DIR/deps/linux_scripts/exec2.sh" "$APP_DIR/usr/bin/exec2.sh"
chmod ug+rx "$APP_DIR/usr/bin"*

#/buildout/build-DoffenSSHTunnel-Release/dist/DoffenSSHTunnel.AppDir/usr/bin/themes
cp -R "$DOFFEN_DIR/deps/themes" "$APP_DIR/usr/bin"

#/buildout/build-DoffenSSHTunnel-Release/dist/DoffenSSHTunnel.AppDir/usr/share/applications
mkdir -p "$APP_DIR/usr/share/applications"
cp "$SRC_DIR/res/linux/DoffenSSHTunnel.desktop" "$APP_DIR/usr/share/applications/$DIST_NAME.desktop"

#/buildout/build-DoffenSSHTunnel-Release/dist/DoffenSSHTunnel.AppDir/usr/share/icons/hicolor/128x128/apps
mkdir -p "$APP_DIR/usr/share/icons/hicolor/128x128/apps"
cp "$SRC_DIR/res/linux/DoffenSSHTunnel.png" "$APP_DIR/usr/share/icons/hicolor/128x128/apps/$DIST_NAME.png"

#LinuxDeployQt /buildout/build-DoffenSSHTunnel-Release/dist/DoffenSSHTunnel.AppDir/usr/share/applications/DoffenSSHTunnel.desktop
linuxdeployqt "$APP_DIR/usr/share/applications/$DIST_NAME.desktop" -verbose=1 -bundle-non-qt-libs

#AppImageTool
cd $DIST_DIR #/buildout/build-DoffenSSHTunnel-Release/dist
export VERSION="$VERSION"
appimagetool -v -n "$APP_DIR" #/buildout/build-DoffenSSHTunnel-Release/dist/DoffenSSHTunnel.AppDir









