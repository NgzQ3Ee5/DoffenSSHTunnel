FROM ubuntu:16.04
MAINTAINER Doffen

# Run container:
# cd ~/workspace/Qt/DoffenSSHTunnel/trunk/build/docker-ubuntu1604
# docker run -v ${PWD}:/hostvolume:ro --rm -it docker-ubuntu1604_doffen /buildin/build/docker-ubuntu1604/build.sh

# Install core dependencies

RUN apt-get update && \
    apt-get -y install -y \
    git subversion automake build-essential bash libxcb1-dev libicu-dev \
    file libssl-dev linux-headers-generic curl git libxrender-dev \
    libpng3 libpng-dev libjpeg-turbo8 libjpeg-turbo8-dev libicu55 libicu-dev \
    libgles2-mesa libgles2-mesa-dev libfreetype6-dev libsqlite3-dev \
    libgstreamer0.10-0 libgstreamer0.10-dev libfontconfig1-dev \
    libogg-dev libvorbis-dev bzip2 gperf bison ruby flex vim \
    g++=4:5.3.1-1ubuntu1 \
 && rm -rf /var/lib/apt/lists/*

# Choose Qt version

ENV QT_VERSION_MAJOR 5.12
ENV QT_VERSION 5.12.0

# Compile and install Qt Base

ENV QT_DIST /usr/local/Qt-"$QT_VERSION"
ENV QT_BASE_SRC https://download.qt.io/official_releases/qt/"$QT_VERSION_MAJOR"/"$QT_VERSION"/submodules/qtbase-everywhere-src-"$QT_VERSION".tar.xz
ENV QT_BASE_DIR qtbase-everywhere-src-"$QT_VERSION"

RUN cd /tmp && rm -fr $QT_BASE_DIR \
    && curl -sSL $QT_BASE_SRC | tar xJ \
    && cd $QT_BASE_DIR \
    && bash ./configure --help \
    && bash ./configure -opensource -confirm-license -silent -fontconfig \
       -no-compile-examples -nomake examples \
    && make \
    && make install \
    && make clean

ENV PATH=/usr/local/Qt-"$QT_VERSION"/bin:$PATH

# Install LinuxDeployQt

RUN cd /tmp \
    && curl -sSL -o linuxdeployqt.AppImage https://github.com/probonopd/linuxdeployqt/releases/download/6/linuxdeployqt-6-x86_64.AppImage \
    && chmod u+x linuxdeployqt.AppImage \
    && ./linuxdeployqt.AppImage --appimage-extract \
    && chmod +rx squashfs-root \
    && find ./squashfs-root -type d -exec chmod +rx {} \; \
    && mv squashfs-root /usr/local/bin/linuxdeployqt.AppImage \
    && ln -s /usr/local/bin/linuxdeployqt.AppImage/AppRun /usr/local/bin/linuxdeployqt \
    && rm linuxdeployqt.AppImage

# Install AppImageKit

RUN cd /tmp \
    && curl -sSL -o appimagetool.AppImage https://github.com/AppImage/AppImageKit/releases/download/11/appimagetool-x86_64.AppImage \
    && chmod u+x appimagetool.AppImage \
    && ./appimagetool.AppImage --appimage-extract \
    && chmod +rx squashfs-root \
    && find ./squashfs-root -type d -exec chmod +rx {} \; \
    && mv squashfs-root /usr/local/bin/appimagetool.AppImage \
    && ln -s /usr/local/bin/appimagetool.AppImage/AppRun /usr/local/bin/appimagetool \
    && rm appimagetool.AppImage

ADD build.sh /build.sh
CMD ["bash", "/build.sh"]



