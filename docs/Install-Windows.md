Upgrade on Windows
==================
Read [Upgrade-Windows] if you already have a previous release

Install on Windows
==================

Download, unzip and run the exe

https://sourceforge.net/projects/doffensshtunnel/files/Windows/


3'party files
------------------

The zip file already includes these 3'party files but if you want you can download and save/replace them in the same folder as DoffenSSHTunnel.exe

  * [plink.exe](http://the.earth.li/~sgtatham/putty/latest/x86/plink.exe)
  * [putty.exe](http://the.earth.li/~sgtatham/putty/latest/x86/putty.exe)
  * [winscp.exe](https://sourceforge.net/projects/winscp/files/WinSCP/) unzip and rename the file to winscp.exe

If DoffenSSHTunnel.exe fails to start, install Microsoft Visual C++ 2017 Redistributable Package (x86)

  * [vcredist_x86.exe](https://aka.ms/vs/16/release/vc_redist.x86.exe)



Starting
------------------

Double click DoffenSSHTunnel.exe. You will be asked to create a master key for the password database the first time. Passwords are stored in DoffenSSHTunnel.pwd. The file is encrypted using AES 256.

Tunnels are stored in DoffenSSHTunnel.ini.

plink.exe is used to setup the ssh tunnels. 

putty.exe is launced when you click on the SSH Term button. winscp.exe is launced when you click the SSH FTP button.

Alternative PuTTY for win32 storing configuration into file
------------------

The original PuTTY stores all configuration into registry (`HKEY_CURRENT_USER\Software\SimonTatham\PuTTY`) - you cannot store/load it from file on the harddrive or other removable devices. There is a workaround with bat/reg files but that isn't very nice. 

Here you find a rewritten PuTTY which handle storing/loading configuration in files.

[Alternative PuTTY homepage](http://jakub.kotrla.net/putty/) | [putty.exe](http://jakub.kotrla.net/putty/putty.exe)

Note! The rewritten plink found on that page does not work