Compile
==================

Howto increment Version Number
------------------
Applicable for new release only

  * `trunk/DoffenSSHTunnel/main.cpp`
    * `const char *APP_VERSION = "0.9.27";`
  * `trunk/build/build.properties`
    * `APP_VERSION=0.9.27`

Howto tag Version Number
------------------
```bash
cd ~/workspace/Qt/DoffenSSHTunnel/trunk
svn copy ^/trunk ^/tags/v0.9.35 -m "Tag v0.9.35"
```

Win32: Qt Creator and Visual Studio 2013 Express Desktop compiler (vs2013)
------------------

MSVC2013 makes it very easy for you to get started, just install the pre-compiled binary package and start Qt Creator, the remainder works as if by magic (once you have any version of MSVC2013 installed, even the Express edition will do).

  * Install Collabnet Subversion Client

  * Checkout source
    * Create directory `C:\QtProjects\DoffenSSHTunnel`
    * Open command prompt cmd.exe
    * `cd \QtProjects\DoffenSSHTunnel`
    * `svn checkout https://svn.code.sf.net/p/doffensshtunnel/code/trunk --username <sourceforge username>`

  * Install Microsoft Visual Studio 2017 Express Desktop compiler (vs2017)

  * Install Qt
    * Download and execute 'Qt Online Installer for Windows' http://qt-project.org/downloads 
    * Select components
      * `Qt / Qt 5.12 / msvc2017 32-bit`
      * `Qt / Tools / Qt Creator`

  * Launch Qt Creator
    * `File » Open File or Project`
    * Select `C:\QtProjects\DoffenSSHTunnel/trunk/DoffenSSHTunnel/DoffenSSHTunnel.pro`
    * Set Debug Build Directory: `C:\QtProjects\build-DoffenSSHTunnel_32bit-Debug`
    * Set Release Build Directory: `C:\QtProjects\build-DoffenSSHTunnel_32bit-Release`
    * Click button `Configure Project`
    * Select menu item: `Build » Open Build and Run Kit Selector`
    * Select `Release`
    * Compile: `Build » Build Project "DoffenSSHTunnel"`    

  * Create Distribution files
    * Install Java(TM) SE Runtime Environment http://java.com/getjava
    * Install Apache Ant http://ant.apache.org/
    * Open command prompt cmd.exe
    * `cd \QtProjects\DoffenSSHTunnel\trunk\build`
    * `build_windows.bat`
    * `cd dist`
    * Upload to SourceForge Download area
      * `DoffenSSHTunnel_win32_v0.9.27 built-in icons.zip`
      * `DoffenSSHTunnel_win32_v0.9.27.zip`
        
MacOS Mojave: Qt Creator and XCode compiler
------------------

  * Install Xcode 10.1
 
  * Install Xcode Command Line Tools
    * Go to `Xcode » Preferences » Downloads » Command Line Tools » Install`
  
  * Install Xcode Additional Tools
    * Go to `Xcode » Open Developer Tool » More Developer Tools...`
    * In the web site that appears you need to search for `Additional Tools for Xcode 10.1` and download the dmg file with the same name. It will contain the Icon Composer app previously distributed with XCode. 

  * Checkout source
    * Open Terminal
    * Create directory `/Users/<your_user_name>/workspace/Qt/DoffenSSHTunnel`
      * `mkdir -p ~/workspace/Qt/DoffenSSHTunnel`
    * `cd ~/workspace/Qt/DoffenSSHTunnel`
    * `svn checkout https://svn.code.sf.net/p/doffensshtunnel/code/trunk --username <sourceforge username>`

  * Install Qt 5.13.1
    * Download and execute 'Qt Online Installer for Mac' http://qt-project.org/downloads 
    * Install in directory ``/Users/<your_user_name>/Qt`
    * Select components
      * `Qt / Qt <version> / clang 64-bit`
      * `Qt / Tools / Qt Creator`

   * Launch Qt Creator
    * Launch Spotlight (Command+Spacebar) and search for `Qt Creator`
    * `File » Open File or Project`
    * Select DoffenSSHTunnel.pro 
     * `~/workspace/Qt/DoffenSSHTunnel/trunk/DoffenSSHTunnel/DoffenSSHTunnel.pro`
    * Click button `Configure Project`
    * Select menu item: `Build » Open Build and Run Kit Selector`
    * Select `Release`
    * Compile: `Build » Build Project "DoffenSSHTunnel"`
        
  * Create Distribution files
    * `cd /tmp`
    * `~/workspace/Qt/DoffenSSHTunnel/trunk/build/build_mac.sh`
    * Zip file found in
     * `/tmp/build-DoffenSSHTunnel-Release/dist`

  * Install
    * Unzip `DoffenSSHTunnel_MacOS_<version>.zip`
    * Drag `DoffenSSHTunnel` to `Applications` in Finder

  * Data file locations
    * Directory: `/Users/<your username>/Library/Application\ Support/DoffenSSHTunnel`
      * `cd ~/Library/Application\ Support/DoffenSSHTunnel`
      * `ls -l`
      * `DoffenSSHTunnel.ini`
      * `DoffenSSHTunnel.pwd`
      * `DoffenSSHTunnelApp.ini`
      * `DoffenSSHTunnelBackup`


Develop on Linux Mint 19.1
------------------
**Install core dependencies**
```bash
sudo apt-get update && \
    sudo apt-get -y install -y \
    g++ git subversion automake build-essential bash libxcb1-dev libicu-dev \
    file libssl-dev linux-headers-generic curl wget libxrender-dev \
    libpng-dev libjpeg-turbo8 libjpeg-turbo8-dev libicu-dev \
    libgles2-mesa libgles2-mesa-dev libfreetype6-dev libsqlite3-dev \
    libfontconfig1-dev libogg-dev libvorbis-dev bzip2 gperf \
    libgl1-mesa-dev bison ruby flex vim
```

**Install Qt Creator**
```
cd /tmp \
    && curl -L http://download.qt.io/official_releases/qt/5.12/5.12.0/qt-opensource-linux-x64-5.12.0.run \
    -o qt-opensource-linux.run \
    && chmod u+x qt-opensource-linux.run \
    && ./qt-opensource-linux.run
```

**Checkout source**
```bash
mkdir -p ~/workspace/Qt/DoffenSSHTunnel
```
```bash
cd ~/workspace/Qt/DoffenSSHTunnel
```
```
svn checkout https://svn.code.sf.net/p/doffensshtunnel/code/trunk
```

Compile Linux release 
------------------
This I did on Linux Mint 19.1 (Ubuntu 18.04)

**Checkout source**
See above

**Install Docker**
https://docs.docker.com/install/linux/docker-ce/ubuntu/
```bash
sudo apt-get install \
    apt-transport-https \
    ca-certificates \
    gnupg-agent \
    software-properties-common
```
```bash
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo apt-key add -
```
```bash
sudo add-apt-repository "deb [arch=amd64] https://download.docker.com/linux/ubuntu bionic stable"
```
```bash
sudo apt-get update
```
```bash
sudo apt-get install docker-ce
```
```bash
sudo usermod -aG docker user
```
```bash
sudo curl -L "https://github.com/docker/compose/releases/download/1.23.2/docker-compose-$(uname -s)-$(uname -m)" \
    -o /usr/local/bin/docker-compose
```
```bash
sudo chmod +x /usr/local/bin/docker-compose
```
Now restart
```bash
sudo shutdown -r now
```

**Download Docker build image from dockerhub**

``` bash
docker pull doffen/ubuntu1604_doffen
```

**Create Linux Distribution files (using the docker image)**

``` bash
~/workspace/Qt/DoffenSSHTunnel/trunk/build/build_ubuntu.sh
```
Distribution file (AppImage bundle) found in 
```
/tmp/buildout/build-DoffenSSHTunnel-Release/dist
```
E.g: DoffenSSHTunnel-v0.9.34-x86_64.AppImage
Upload the file to SourceForge


