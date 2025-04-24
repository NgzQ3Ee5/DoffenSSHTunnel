Install Linux
==================

Download DoffenSSHTunnel*.AppImage
https://sourceforge.net/projects/doffensshtunnel/files/Linux/

```
chmod u+x DoffenSSHTunnel*.AppImage
./DoffenSSHTunnel*.AppImage
```

If you want to use SSH Term and SSH FTP then you also need to install Terminator and FileZilla
https://gnometerminator.blogspot.com/p/introduction.html
https://filezilla-project.org/download.php?platform=linux

**Optional**
Learn more about AppImage. https://itsfoss.com/use-appimage-linux/

If you want to use a terminal other than Terminator then you can configure this by going to
Settings -> Passwords and Variables -> Executable Applications

You then edit the variable called ssh. Change the value shown in column # 2 (Executable File).

**ssh variable alternatives**

gnome-terminal
```
/usr/bin/gnome-terminal --title "${name} - ${rhost}" -- /usr/bin/ssh
```
konsole (KDE)
```
/usr/bin/konsole --title "${name} - ${rhost}" -e /usr/bin/ssh
```
xfce4-terminal
```
/usr/bin/xfce4-terminal --title "${name} - ${rhost}" --execute /usr/bin/ssh
```
terminator (default)
```
/usr/bin/terminator --title "${name} - ${rhost}" --execute /usr/bin/ssh
```

If none of these suits you then just figure out how to start your desired terminal from the command line. You can test this in the following way.
Test to start a new gnome terminal window
```
  /usr/bin/gnome-terminal --title "My Title" - bash
```

**All default variable values**
Here are the variables that are set as default the first time you start the program.

Variable Name | Executable File | Description
---------- | ---------- | ------
firefox | ```/usr/bin/firefox``` | Firefox
safari | ${firefox} | Alias
chrome | ${firefox} | Alias
iexplore | ${firefox} | Alias
ssh | ```/usr/bin/terminator --title "${name} - ${rhost}" --execute /usr/bin/ssh``` | SSH Term
putty | ${ssh} | Alias
filezilla | ```/usr/bin/filezilla -l interactive``` | FileZilla
winscp | ${filezilla} | Alias

