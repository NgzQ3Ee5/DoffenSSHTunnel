#!/bin/bash

# https://github.com/hadim/docker-qt/blob/master/Dockerfile


function pathmunge () {
    if ! echo "$PATH" | /bin/grep -q "$1" ; then
       if [ "$2" = "after" ] ; then
          PATH=$PATH:$1
       else
          PATH=$1:$PATH
       fi
    fi
    if ! /bin/grep -q "$1" ~/.profile ; then
       if [ "$2" = "after" ] ; then
          echo "PATH=\$PATH:$1" >> ~/.profile
       else
          echo "PATH=$1:\$PATH" >> ~/.profile
       fi
    fi
}

# Install core dependencies

apt-get -y install -y \
    git subversion automake build-essential bash libxcb1-dev libicu-dev \
    libssl-dev linux-headers-generic curl git libxrender-dev \
    libpng3 libpng-dev libjpeg-turbo8 libjpeg-turbo8-dev libicu55 libicu-dev \
    libgles2-mesa libgles2-mesa-dev libfreetype6-dev libsqlite3-dev \
    libgstreamer0.10-0 libgstreamer0.10-dev libfontconfig1-dev \
    libogg-dev libvorbis-dev bzip2 gperf bison ruby flex vim \
    openjdk-8-jdk ant \
    g++=4:5.3.1-1ubuntu1

pathmunge /usr/lib/gcc/x86_64-linux-gnu/5.4.0

# Compile and install cmake

CMAKE_VERSION_MAJOR=3.13
CMAKE_VERSION=3.13.3
CMAKE_BASE_SRC=http://www.cmake.org/files/v"$CMAKE_VERSION_MAJOR"/cmake-"$CMAKE_VERSION".tar.gz
CMAKE_BASE_DIR=cmake-"$CMAKE_VERSION"

cd /tmp \
    && rm -fr $CMAKE_BASE_DIR \
    && curl -sSL $CMAKE_BASE_SRC | tar zx \
    && cd $CMAKE_BASE_DIR \
    && bash ./configure \
    && make install

# Choose Qt version

QT_VERSION_MAJOR=5.9
QT_VERSION=5.9.7

# Compile and install Qt Base

QT_DIST=/usr/local/Qt-"$QT_VERSION"
QT_BASE_SRC=https://download.qt.io/official_releases/qt/"$QT_VERSION_MAJOR"/"$QT_VERSION"/submodules/qtbase-opensource-src-"$QT_VERSION".tar.xz
QT_BASE_DIR=qtbase-opensource-src-"$QT_VERSION"

cd /tmp && rm -fr $QT_BASE_DIR \
    && curl -sSL $QT_BASE_SRC | tar xJ \
    && cd $QT_BASE_DIR \
    && bash ./configure --help \
    && bash ./configure -opensource -confirm-license -silent -fontconfig -no-qml-debug \
       -no-compile-examples -nomake examples \
    && make \
    && make install

pathmunge $QT_DIST/bin

# Compile and install patchelf

PATCHELF_VERSION=0.9
PATCHELF_BASE_SRC=https://nixos.org/releases/patchelf/patchelf-"$PATCHELF_VERSION"/patchelf-"$PATCHELF_VERSION".tar.bz2
PATCHELF_BASE_DIR=patchelf-"$PATCHELF_VERSION"

cd /tmp \
    && rm -fr $PATCHELF_BASE_DIR \
    && curl -sSL $PATCHELF_BASE_SRC | tar xj \
    && cd $PATCHELF_BASE_DIR \
    && bash ./configure \
    && make install



# Compile and install LinuxDeployQt

LINUXDEPLOYQT_VERSION=5
LINUXDEPLOYQT_BASE_SRC=https://github.com/probonopd/linuxdeployqt/archive/"$LINUXDEPLOYQT_VERSION".tar.gz
LINUXDEPLOYQT_BASE_DIR=linuxdeployqt-"$LINUXDEPLOYQT_VERSION"

cd /tmp \
    && rm -fr $LINUXDEPLOYQT_BASE_DIR \
    && curl -sSL -o $LINUXDEPLOYQT_BASE_DIR.tar.gz $LINUXDEPLOYQT_BASE_SRC \
    && tar -zxf $LINUXDEPLOYQT_BASE_DIR.tar.gz \
    && cd $LINUXDEPLOYQT_BASE_DIR \
    && qmake \
    && make \
    && make install

# Compile and install AppImageKit

APPIMAGEKIT_VERSION=11
APPIMAGEKIT_BASE_DIR=AppImageKit

cd /tmp \
    && rm -fr $APPIMAGEKIT_BASE_DIR \
    && git clone --single-branch --recursive https://github.com/AppImage/AppImageKit \
    && cd $APPIMAGEKIT_BASE_DIR \
    && git checkout tags/$APPIMAGEKIT_VERSION \
    && bash -ex install-build-deps.sh \
    && bash -ex build.sh \
    && cd build \
    && make install

