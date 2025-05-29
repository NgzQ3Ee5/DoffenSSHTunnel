Compile
==================

Howto increment Version Number
------------------
Applicable for new release only

  * `trunk/DoffenSSHTunnel/main.cpp`
    * `const char *APP_VERSION = "0.9.59";`
  * `trunk/build/build.properties`
    * `APP_VERSION=0.9.59`


Qt Creator and Visual Studio Community (vs2022)
------------------

  * Install Git Client

  * Checkout source
    * Create directory `C:\QtProjects`
    * Open command prompt cmd.exe
    * `cd \QtProjects`
    * `git clone git@github.com:NgzQ3Ee5/DoffenSSHTunnel.git`

  * Install Microsoft Visual Studio Community (vs2022)
    * Choose Desktop development with C++

  * Install Qt
    * Download and execute 'Qt Online Installer for Windows' https://www.qt.io/download-dev
    * Select components
      * `Qt / Qt 6.9.0 / MSVC 2022 64-bit`
      * `Qt Creator / Qt Creator 16.0.1`

  * Launch Qt Creator
    * `File » Open File or Project`
    * Select `C:\QtProjects\DoffenSSHTunnel/trunk/DoffenSSHTunnel/DoffenSSHTunnel.pro`
    * Set Debug Build Directory: `C:\QtProjects\build-DoffenSSHTunnel_64bit-Debug`
    * Set Release Build Directory: `C:\QtProjects\build-DoffenSSHTunnel_64bit-Release`
    * Click button `Configure Project`
    * Select menu item: `Build » Open Build and Run Kit Selector`
    * Select `Release`
    * Compile: `Build » Build Project "DoffenSSHTunnel"`    

  * Create distribution file
    * Execute: C:\QtProjects\DoffenSSHTunnel\build\build_windows.bat
	* File created: C:\QtProjects\build-DoffenSSHTunnel_64bit-Release\dist\DoffenSSHTunnel-v0.9.58-Win64.zip

  * Create Release in Github
    * https://github.com/NgzQ3Ee5/DoffenSSHTunnel/releases
	* Click 'Draft a new release'
	* Create new tag: v0.9.59
    * Release title: v0.9.59
    * Description: Copy/paste the changelog
    * Attach file: C:\QtProjects\build-DoffenSSHTunnel_64bit-Release\dist\DoffenSSHTunnel-v0.9.58-Win64.zip
	* Click 'Publish release'
        
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


