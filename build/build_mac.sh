#!/bin/bash
set -e

#ant -f build_qt5110_xcode101_osx_mojave.xml $@

if ! [ -x "$(which qmake)" ] ; then
    echo "Could not execute qmake. Plase add the Qt bin directory to PATH" >&2
    exit 1
fi

if ! [ -x "$(which macdeployqt)" ] ; then
    echo "Could not execute macdeployqt. Plase add the Qt bin directory to PATH" >&2
    exit 1
fi

if ! [ -x "$(which make)" ] ; then
    echo "Could not execute make." >&2
    exit 1
fi

CUR_DIR=$PWD
SCRIPT_DIR=$( cd ${0%/*} && pwd -P )

source "$SCRIPT_DIR/build.properties"
APP_VERSION=${APP_VERSION//[$'\t\r\n']} && APP_VERSION=${APP_VERSION%%*( )}

DIST_NAME="DoffenSSHTunnel"
VERSION="v$APP_VERSION"
DIST_FULLNAME="$DIST_NAME-$VERSION-MacOS"
DEPS_DIR="$SCRIPT_DIR/../deps"
BULD_DIR="$CUR_DIR/build-DoffenSSHTunnel-Release"
DIST_DIR="$BULD_DIR/dist"
DIST_DIR_APP="$DIST_DIR/$DIST_FULLNAME/$DIST_NAME.app"

if [ -d $BULD_DIR ]; then rm -fr $BULD_DIR; fi
mkdir $BULD_DIR

if [ -d $DIST_DIR ]; then rm -fr $DIST_DIR; fi
mkdir $DIST_DIR

cd $BULD_DIR
qmake "$SCRIPT_DIR/../DoffenSSHTunnel/DoffenSSHTunnel.pro"
make

mkdir "$DIST_DIR/$DIST_FULLNAME"
cp -R "$BULD_DIR/source/$DIST_NAME.app" "$DIST_DIR/$DIST_FULLNAME"
chmod ug+rx "$DIST_DIR/$DIST_FULLNAME/$DIST_NAME.app/Contents/MacOS/"*
chmod ug+rx "$DIST_DIR/$DIST_FULLNAME/$DIST_NAME.app/Contents/Resources/"*.sh
macdeployqt "$DIST_DIR/$DIST_FULLNAME/$DIST_NAME.app" -verbose=1 -codesign="Developer ID Application: Lars-Flemming Clausen"
cd "$DIST_DIR/$DIST_FULLNAME"
zip -yr "$DIST_DIR/$DIST_FULLNAME.zip" "$DIST_NAME.app"
echo "$DIST_DIR/$DIST_FULLNAME.zip" > "$BULD_DIR/distfile.txt"
echo "FINISHED $DIST_DIR/$DIST_FULLNAME.zip"
