Changelog
==================
v0.9.58 May 23, 2025
------------------
+ New: Support for Amazon AWS Session Manager to establish tunnels using aws ssm and the Session Manager plugin with the AWS-StartPortForwardingSessionToRemoteHost document.
+ Update: Removed the radio group for selecting SSH version 1 or 2. To override the default SSH version, use the Extra Arguments field instead.
+ Update: Removed the Compression checkbox. To override the default SSH compression, use the Extra Arguments field instead.
+ Update: Improved termination of external processes, including proper cleanup of spawned child processes.
+ New: Added support for disabling the password database file feature by specifying `disabled` as the value for the -p / --pwd-file option.
+ New: Added support for naming tunnels in the More Tunnels table.
+ New: Named tunnels can now be referenced as variables using ${thename.port} and ${thename.ip}.
+ New: PopulateChildNodesWithExternalCommand automatically assigns a local port for tunnels in the "More Tunnels" table if the external script leaves it unspecified.
+ New: PopulateChildNodesWithExternalCommand preserves existing tunnels when the external script returns a matching tunnel entry, avoiding unnecessary disconnects.
+ New: PopulateChildNodesWithExternalCommand automatically expands folders that contain active tunnels.

v0.9.57 May 19, 2025
------------------
+ Fixed: Resolved an issue where file paths containing spaces could fail to process correctly.
+ Fixed: Tree item grayout state now refreshes immediately without requiring a restart.
+ New: OutputWindowAutoClose builtin custom action

v0.9.56 Apr 27, 2025
------------------
+ Fixed: Properly use QProcess::splitCommand.
+ Fixed: Removed redundant window title.
+ Fixed: Corrected SSH host font color in dark theme.

v0.9.55 Apr 24, 2025
------------------
+ Update Windows: New developement environment
    + Visual Studio 2022 Community
    + Qt 6.9.0 libraries for MSVC 2022 64-bit
    + Qt Creator 16.0.1
+ Update: App now uses Fusion style by default on first run
+ New: Dark/light theme that adapts to your system settings
+ Rewrote all signal/slot connections for Qt 6.9.0 upgrade. If anything acts up, roll back to version 0.9.54.

v0.9.54 Apr 16, 2025
------------------
+ Update: Improved command-line help descriptions for clarity and readability.
+ Update: The --data-file argument now accepts a filename without a path;
          it will be stored inside the data-dir if no path is provided.
+ New: Added --title option to override the main window title at startup.

v0.9.53 Apr 14, 2025
------------------
+ Fixed: Connect button now stays enabled if 'Populate child nodes with external command' is set
+ Windows
	+ Updated bundled plink.exe from 0.81 to 0.83
	+ Updated bundled putty.exe from 0.81 to 0.83
	+ Updated bundled winscp.exe from 6.3.3 to 6.5

v0.9.52 Apr 13, 2025
------------------
+ Fixed: Enable connect button on folder if 'Populate child nodes with external command' is set
+ Windows
	+ Updated Code Signing Cert

v0.9.51 Apr 17, 2024
------------------
+ Windows
	+ Updated bundled plink.exe from 0.78 to 0.81
	+ Updated bundled putty.exe from 0.78 to 0.81
	+ Updated bundled winscp.exe from 5.21.7 to 6.3.3
	+ Updated Code Signing Cert

v0.9.50 Mar 15, 2023
------------------
+ Windows
	+ Fixed: Compability with putty/plink v0.77,v0.78 password prompt stopped working
	+ Updated bundled plink.exe from 0.73 to 0.78
	+ Updated bundled putty.exe from 0.73 to 0.78
	+ Updated bundled winscp.exe from 5.15.9 to 5.21.7
	+ Changed: Code Signing Cert

v0.9.49 Aug 28, 2021
------------------
+ Fixed: Able to use the same port number for both -L and -R like Cygwin and Linux allows
    Thanks to Richard who suggested this in https://sourceforge.net/p/doffensshtunnel/tickets/60/
    
v0.9.48 Mar 18, 2020
------------------
+ Update: Keep tree node 'lock' icon colored red when reconnection timer is active
    Thanks to biece who suggested this in https://sourceforge.net/p/doffensshtunnel/tickets/59/
    
v0.9.47 Feb 5, 2020
------------------
+ Update: Auto reconnect max attempts in preferences have been increased from 99 to 9999999 
	Thanks to biece who suggested this in https://sourceforge.net/p/doffensshtunnel/tickets/58/
+ Fixed: Auto reconnect interval in seconds set in preferences is now actually used. It was previously hard-coded to 10 seconds.

v0.9.46 Jan 23, 2020
------------------
+ New: You can now configure auto reconnect in preferences. 
    The number of times to retry and the number of seconds to wait between each retry.
	Thanks to biece who suggested this in https://sourceforge.net/p/doffensshtunnel/tickets/58/
+ Windows: Updated Qt libraries from 5.12.6 to 5.14.0
	There should now be better support for High DPI screens as well as better support for Windows 150% zoom on such screens.
    
v0.9.45 Jan 20, 2020
------------------
+ Maintenance update that has new versions of 3'part sw.
+ Windows
	+ Updated bundled plink.exe from 0.72 to 0.73
	+ Updated bundled putty.exe from 0.72 to 0.73
	+ Updated bundled winscp.exe from 5.13.9 to 5.15.9
	+ Fix for wrong version of bundled msvc redistributable
	+ removed msvcp120.dll and msvcr120.dll
	+ added msvcp140.dll and vcruntime140.dll

v0.9.44 Jan 15, 2020
------------------
+ Update Windows: New developement environment
    + Moved from Win7 (vmware) to Win10 (Hyper-V)
    + Visual Studio 2017 Express Desktop
    + Qt 5.12.6 libraries for MSVC 2017 32-bit
    + Qt Creator 4.11.0

Note there is a known error in this version. If you put the program in a directory path that contains spaces then SSH Term and SSH FTP may not work.

v0.9.43 Okt 24, 2019
------------------
+ Update Windows: Make sure plink, putty, winscp is always in the executable variables list
+ Update macOS: Ensure my default executable variables always exists.
+ Update macOS: Fix missing application icon
+ Update macOS: Moved my themes dir from MacOS dir to Resources dir
+ Update macOS: Moved my .sh files from MacOS dir to Resources dir
+ Update macOS: From now on the executable is signed

Note there is a known error in this version. If you put the program in a directory path that contains spaces then SSH Term and SSH FTP may not work.

v0.9.42 Okt 20, 2019
------------------
+ New: New built-in variable $datadir that points to the directory for where data files are stored. 
+ Update Windows: From now on the executable is signed
+ Update macOS: Add quotes to execucatble var

Note there is a known error in this version. If you put the program in a directory path that contains spaces then SSH Term and SSH FTP may not work.

v0.9.41 Okt 4, 2019
------------------
+ Fix: Child ssh host list select host would reset when changing settings in parent https://sourceforge.net/p/doffensshtunnel/tickets/56/

v0.9.40 Sept 29, 2019
------------------
+ Update macOS: Support for Dark Mode
+ Update macOS: Updated from Qt 5.12.1 to 5.13.1

v0.9.39 Sept 27, 2019
------------------
+ New: If you have a child tunnel and a parent tunnel where you have added more tunnels then you can now select one of these in the child tunnel. 
  Previously, you could only select the main tunnel that was in the parent tunnel.
  Thanks to biece who suggested this in https://sourceforge.net/p/doffensshtunnel/tickets/56/
+ Update Windows: Updated bundled plink.exe from 0.70 to 0.72
+ Update Windows: Updated bundled putty.exe from 0.70 to 0.72
+ Update Windows: Updated bundled winscp.exe from 5.13.7 to 5.13.9 portable

v0.9.38 Apr 19, 2019
------------------
+ New: Support for new TRA SSH Gateway in Telenor Norway

v0.9.37 Apr 19, 2019
------------------
+ New: Command line options
```bash
$ ./DoffenSSHTunnel --help
Usage: ./DoffenSSHTunnel [options] [datafile]
Doffen SSH Tunnel

Options:
  -h, --help                       Displays this help.
  -v, --version                    Displays version information.
  --version-number                 Displays version number.
  -d, --data-dir <data dir>        The directory where application data is
                                   stored.
  -p, --pwd-file <passwords file>  The file where passwords are stored.

Arguments:
  datafile                         Read/write your data to this INI file.
                                   Optional, e.g: ./DoffenSSHTunnel.ini
```
+ New: Column Args in Variable Settings -> Executable Applications
+ Update (macos): Updated from Qt 5.11.0 to 5.12.1
+ Fixed (ticket 55): Not prompting for password properly. Thanks to Anthony Vitale for reporting.

v0.9.35 Feb 3, 2019
------------------
+ Update (Linux): Fixed SSH Term action

v0.9.34 Jan, 2019
------------------
+ New: Linux version. Compiled on Ubuntu 16.04 64bit.
    + Packaged as an AppImage. See https://appimage.org/
    + Unlike on windows where the data files are stored in the same place as DoffenSSHTunnel.exe, the files are stored in ~ /.local/share/DoffenSSHTunnel/
+ New: new built-in variable ${name} which is the name of the host in the tree ( 'My Name' in the edit pane )
    + This was added primarily so that the name appears in the title of the terminal window on Linux and MacOS
+ Update: Fixed compiler warnings in connection with c ++ compiler update
+ Update Windows: Updated bundled winscp.exe from 5.13.2 to 5.13.7 portable


v0.9.33 Jan, 2019
------------------
+ New: MacOS version. Compiled on Mojave (10.14)
    + Unlike on windows where the data files are stored in the same place as DoffenSSHTunnel.exe, the files are stored in ~ /Library/Application Support/ DoffenSSHTunnel
 
v0.9.32 Jun 18, 2018
------------------
+ New: Support for Google Two Factor Authentication
+ Update: Updated bundled plink.exe from 0.67 to 0.70
+ Update: Updated bundled putty.exe from 0.67 to 0.70
+ Update: Updated bundled winscp.exe from 5.9.3 to 5.13.2 portable
 
v0.9.31 Jan 29, 2017
------------------
+ Update: Updated bundled plink.exe from 0.64 to 0.67
+ Update: Updated bundled putty.exe from 0.64 to 0.67
+ Update: Updated bundled winscp.exe from 4.4.0 to 5.9.3 portable

v0.9.30 Jan 19, 2017 
------------------
* New developement environment
    * NOTE! Starting with this version Windows XP is not supported
    * Moved from WinXP 32bit to Win7 64bit (vmware)
    * Visual Studio 2013 Express Desktop
    * Qt 5.5.0 libraries for VS 2013 Express 32bit
    * Qt Creator 3.4.2
* New: Folders can have custom actions
* New: Populate Hosts inside folders using an external command. I used this new feature and nodejs to get a list of Instances from Amazon AWS.
    * On the Edit tab when a folder is selected
    * Command: $node myfiles/nodejs/aws_ec2_instances.js ${ae_aws_read.uid} ${ae_aws_read.pwd}
 ${ae_aws_read.uid} is the AWS access key
 ${ae_aws_read.pwd} is the AWS secret key
    * The script uses the aws sdk to fetch all AWS Instances and then prints the result as a JSON string to stdout
* New: Set tree foreground and background color per host (click My Name in the edit pane)
* Update: Added icons in search toolbar completer
* Update: Updated bundled plink.exe from 0.63 to 0.64
* Update: Updated bundled putty.exe from 0.63 to 0.64
* Update: Moved the Clear Log button. Now click the Log... title above the log window. 
* Update: Updated icons documentation
* Update: No longer show search completer on focus. Show on Ctrl+F and Key_Up/Key_Down
* Update: Increased max number of hosts from 999 to 999999. It is absolutely ridiculous high. It's just to have no limit.
* 5000 is the practical limit really considering responsiveness in the GUI
* Update: Improved '...' button next to a variable in Settings -> Passwords and Variables -> Executable Applications
    * When clicking the '...' button next to a variable
        * If variable value is filled in _and_ it is an existing directory: 
        * show directory select dialog
        * in any other case show file select dialog
    * Resolve variables
* Fixed: Search toolbar could cause app to crash
* Fixed: Search toolbar popup completer too eager
* Fixed: Password variable usage counter not incremented when referring to ${varname.uid}
* Fixed: Did not ask for save when turning off 'autosave' in options
* Fixed: A little too eager to autosave edit pane
* Fixed: Newline sometimes disappeared in log

v0.9.29 May 28, 2015
------------------
+ New: Search toolbar to find hosts in the tree (Ctrl+F). This bar is allways visible.
+ New: Another search box below the tree that are invisible by default (Ctrl+K).
+ New: Settings -> Preferences option to show the search box below the tree by default.

v0.9.28 Dec 5, 2014
------------------
+ Fixed:  Displaying the wrong custom icons name
+ Update: Updated bundled plink.exe to v0.63
+ Update: Updated bundled putty.exe to v0.63
+ Update: Updated bundled winscp.exe to v4.40
+ Update: Added handdrawn icons Settings -> Icons -> Hand Drawn 1
+ Update: Updated URL shown in Help -> About: https//sourceforge.net/p/doffensshtunnel

v0.9.27 Nov 30, 2014
------------------
+ Fixed: If you typed the wrong password when you start the app then executable variables would reset to default 
+ Update: Rewritten parts of the code to be platform independent 
+ Update: Updated from Qt 4.8.4 to Qt 5.2.0

v0.9.26 May 2, 2013
------------------
+ New: Added option to specify Local IP (listening-IP) in tunnels (suggests localhost by default)
+ Update: Added tooltip that explains the Extra Arguments input box in the Edit pane.

v0.9.25 January 23, 2013
------------------
+ Update: Updated Qt libraries from 4.8.1 to 4.8.4
+ Update: Updated the bundled winscp.exe from 4.3.7 to 4.3.9
+ Update: Default disabled Preferences -> 'Enable auto reconnect tunnels'
+ Update: Menu item Settings -> 'Password and Variables' is allways available even when editing is turned off (unchecked File -> 'Editing Enbaled').

v0.9.24 September 14, 2012
------------------
+ Fixed: Cut/Paste custom actions not working properly 
+ Fixed: Backing up when changing icon theme 
+ Fixed: New SSH Host sets the custom action command as the name 

v0.9.23 June 4, 2012
------------------
+ Update: Icons themes can be installed inside the icons folder ./DoffeSSHTunnel/themes/icons/My Icons
+ Update: Updated bundled putty.exe to latest release (v0.62). This is the modified putty from http://jakub.kotrla.net/putty/ that stores session data i files.
+ Update: Updated bundled plink.exe to latest release (v0.62)
+ Update: Updated bundled winscp.exe to latest release (v4.3.7)

v0.9.22 June 3, 2012
------------------
+ New: Support for multiple icon themes. See IconThemes

v0.9.21 May 25, 2012
------------------
+ New: More Tunnels in the Edit pane. 
+ New: Use your own icons. Create folder themes/icons/. An icon found in that folder overrides the built in icon. Download built in icons from here: http://code.google.com/p/doffensshtunnel/source/browse/trunk/res
+ Update: Lots of UI improvements
+ Update: Simplified mgmt of Custom Actions. Edit directly in the Connect pane.
+ Update: Moved Options to Menu -> Settings -> Preferences...
+ Update: Moved Passwords to Menu -> Settings -> Passwords & Variables...

v0.9.20 
------------------
+ Fixed: User defined variables disappeared

v0.9.19
------------------
+ Fixed: Restore restored wrong backup - http://http://code.google.com/p/doffensshtunnel/issues/detail?id=40

v0.9.18
------------------
+ New: Import / Export - http://code.google.com/p/doffensshtunnel/issues/detail?id=38
+ New: Detect local port already in use when connecting - http://code.google.com/p/doffensshtunnel/issues/detail?id=26
+ New: Propose tunnel local port - http://code.google.com/p/doffensshtunnel/issues/detail?id=27
+ New: Propose tunnel remote port 22 - http://code.google.com/p/doffensshtunnel/issues/detail?id=39
+ New: Restore dialog: filter on manual and auto backup. - http://code.google.com/p/doffensshtunnel/issues/detail?id=32

v0.9.17 May 3, 2012
------------------
+ New: Get help while creating custom actions - http://code.google.com/p/doffensshtunnel/issues/detail?id=37
+ New: Support for user defined variables - http://code.google.com/p/doffensshtunnel/issues/detail?id=34
+ New: Unlimited custom actions (the buttons are gone) - http://code.google.com/p/doffensshtunnel/issues/detail?id=31
+ Update: UI improvements - http://code.google.com/p/doffensshtunnel/issues/detail?id=36
+ Update: Automatically save changes - http://code.google.com/p/doffensshtunnel/issues/detail?id=35
+ Update: Ask for username and password if not stored - http://code.google.com/p/doffensshtunnel/issues/detail?id=33 

v0.9.16 April 16, 2012
------------------
+ New: Sortable columns in Tools->View configured local ports - http://code.google.com/p/doffensshtunnel/issues/detail?id=18
+ New: Drag&Drop in tree tunnel - http://code.google.com/p/doffensshtunnel/issues/detail?id=17
+ New: Checkbox in the Options pane 'Enable drag and drop in tunnel tree' - http://code.google.com/p/doffensshtunnel/issues/detail?id=16
+ New: File -> Editing enabled CTRL+E - http://code.google.com/p/doffensshtunnel/issues/detail?id=15
+ Fixed: Child tunnel host not updated on folder move,cut,copy,paste,duplicate - http://code.google.com/p/doffensshtunnel/issues/detail?id=23
+ Update: DoffenSSHTunnel.ini change variable names - http://code.google.com/p/doffensshtunnel/issues/detail?id=30
+ Update: Remove unused data from DoffenSSHTunnel.ini - http://code.google.com/p/doffensshtunnel/issues/detail?id=29
+ Update: Link libraries dynamically - http://code.google.com/p/doffensshtunnel/issues/detail?id=25
+ Update: Update to QT 4.8.0 - http://code.google.com/p/doffensshtunnel/issues/detail?id=24
+ Update: Enable custom buttons even when a host is not connected. - http://code.google.com/p/doffensshtunnel/issues/detail?id=20
+ Update: Do not require a tunnel to be defined. - http://code.google.com/p/doffensshtunnel/issues/detail?id=19


v0.9.15 April 10, 2012
------------------
+ New: Added support for backup/restore. Automatic backup enabled by default. Creates folder 'DoffenSSHTunnelBackup' in same dir as ini file
+ New: checkbox in the Options pane 'Auto backup enabled'. Enabled by default
+ New: checkbox in the Options pane 'Ask for description on manual backup'. Enabled by default
+ New: 'File' menu item 'Backup now'. Hotkey Ctrl+B
+ New: 'File' menu item 'Restore a backup'. Hotkey Ctrl+R
+ New: Folder icon shows whether child tunnels are connected
+ New: Dobleclick or Enter on folder disconnects all child tunnels
+ New: Tools -> Disconnect all tunnels
+ New: Show Count in 'Tools'->'View configured local ports'.

v0.9.14 Mars 29, 2012
------------------
+ New: Added support for folders.

v0.9.13 February 10, 2012
------------------
+ New: Added new button 'Clear Log' in the Connect pane.
+ New: Added new checkbox in the Options pane 'Clear log automatically' (default enabled). Clear the log window before trying to connect.

v0.9.12 May 24, 2011
------------------
+ New: Added new custom command variable in Edit pane: $kfile . Note that you must quote it if the key file path contains spaces: "$kfile"

v0.9.11 September 16, 2010
------------------
+ New: Context menu item 'Clear log' in tunnel tree. Right click tunnel in tree and select Clear log. Clears the log on the Connect tab.
+ New: Menu item 'Clear all logs' in the Tools menu.

v0.9.10 August 27, 2010
------------------
+ New: Support for challenge response password prompt. A password dialog appears when the server says %enter challenge response%
+ New: Support for personal code prompt. A password dialog appears when the server says %enter personal code%

v0.9.9 
------------------
+ Fixed: Added missing files to the QT project file ATSkeleton.pri

v0.9.8 May 20, 2010
------------------
+ New: When connecting a child tunnel, automatically connect parent tunnels if they are disconnected.
+ New: Intentionally disconnecting parent tunnel disconnects all child tunnels.
+ New: Checkbox in Options tab to globally turn on/off the auto connect feature. Overrides tunnel setting in the Edit tab.
+ New: Checkbox in Options tab to globally turn on/off the auto reconnect feature. Overrides tunnel setting in the Edit tab.
+ New: Support for several remote  hosts in the tunnel. On the Edit tab, Click the [...] button next to the Remote Host
    textbox to open the edit dialog. Insert multiple hosts by pressing the enter key. One host per line. The textbox on
    the Edit tab turns into a combobox where you select the one to be active.
+ New: Write password to STDIN instead of -pw parameter to plink.exe
+ New: Support for password protected private key. Refer to password in password manager with variable `${<name>.pwd}`
+ New: Save expanded/collapsed state in the tunnel tree
+ New: Tools -> View configured local ports. Show distinct list of local ports in ascending order.
+ Fixed: Duplicate tunnel and copy/paste caused memory leaks since the QTreeWidgetItemiterator traversed the whole tree and created copies of too many internal tunnel objects.

v0.9.7 May 8, 2010
------------------
+ New: Support for several SSH hosts in the tunnel. On the Edit tab, Click the [...] button next to the SSH Host
    textbox to open the edit dialog. Insert multiple hosts by pressing the enter key. One host per line. The textbox on
    the Edit tab turns into a combobox where you select the one to be active. This feature will be utilized by
    autoconnect in a future version. 

v0.9.6 May 6, 2010
------------------
+ New: Origanizing the tunnels in a treview 
+ New: Right click context menu in treeview with copy/paste etc...
+ Fixed: Duplicate tunnel did not copy the custom button labels

v0.9.5 Mars 26, 2010
------------------
+ New: Customizable label on the 'SSH Term' and 'SSH FTP' buttons
  + On the Edit tab, click the [...] button next to the Cmd text box
+ Change: INI file 
  + renamed property SSHTermCmd=.. to CustomButton1Cmd=...
  + renamed property SSHFTPCmd=.. to CustomButton1Cmd=...
  + added property CustomButton1Label
  + added property CustomButton2Label
+ Fixed: Update info in about dialog

v0.9.4 February 5, 2010
------------------
+ Minor improvements

v0.9.3 January 25, 2010
------------------
+ New: Option to specify the amount of spaces used to indent the tunnel name in the tree view when clicking left/right
+ New: Option turn on/off the masking of passwords in the log
+ Fixed: nasty bug when moving tunnel down and edit edited the wrong tunnel

v0.9.2
------------------
+ New: Added password manager. 
  + DB file DoffenSSHTunnel.pwd craeted on app start if not exists. Encrypted with AES 256. 
  + User prompted to create a master password for the db file if not found
  + User must enter master password on app start to decrypt db file
  + Password fields: name, login, password
  + Refer to username and passwords with variables `${<name>.uid}` and `${<name>.pwd}`
  + Added combobox in edit page to select username and password from password manager
  + To change master password select File->Change master password
+ New: Hiding passwords in log window
+ New: Store window splitter pos in ini file
+ New: Added cancel button to edit page

v0.9.1
------------------
+ New: Saving app version number in ini file
+ Change: renamed tunnel group in ini file from [TunnelX] to [Tunnel_XXX]
+ Fixed: crash when duplicating tunnel and then moving it up/down/left/right
+ Fixed: delete tunnel did not remove tunnel from ini file

v0.9.0
------------------
+ Port of ezTunnel SSH to Doffen SSH Tunnel
+ Added ability to move tunnel up/down/left/right in the tree view
+ Minor bug fixes
