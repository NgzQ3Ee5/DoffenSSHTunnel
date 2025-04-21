DoffenSSHTunnel
LFC <arnedokken@gmail.com>
Jan 2010

This freeware application is released under the GPL license
you can find the source at
http://doffensshtunnel.googlecode.com

IMPORTANT NOTE:
Due to a known difficulty interfacing with SSH, it is not currently possible
to automatically send the password without key registered on the server.
When SSH asks for the password, the requests is done on the TTY where you
started DoffenSSHTunnel, look out for it. It is recommended to start the program
from a Nautilus or equivalent (by double clicking on the icon) in which case
SSH will pop-up a dialog asking for the password.

To run it you need the Qt libraries, install them on debian based
systems using:
sudo apt-get install libqt4-gui

