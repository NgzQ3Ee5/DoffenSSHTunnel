Variables
==================

Introduction
------------------

Variables that can be used in your action commands.

Tip!
> Right click any custom action and select 'Edit Wizard'. You can then easily discover and pick variables.

System Variables
------------------
*Variable*  | *Description*
-------------------|------------|--------------
$appdir | The directory that contains the application executable ||
$datadir | The directory that contains the application data ||

Variables from the Edit pane
------------------

*Field*            | *Variable* | *Description*
-------------------|------------|--------------
My Name            | $name      | Name of the tunnel/host ||
SSH Host           | $host      | Address or IP of the SSH server to connect to (host:port, or host only for port 22) ||
Local listen-IP    | $ip        | If you have several physical or virtual IP network interfaces on your machine then you can optionally specify the specific interface IP address to be used for the Tunnel. By default we use localhost. ||
Local Port         | $port      | Port to use locally for the tunnel ||
Remote Host        | $rhost     | The remote host to tunnel to ||
Remote Port        | $rport     | Port to use on the remote host for the tunnel. The standard port is 22. Unless you have a special SSH server setup port 22 should be used.  ||
Username           | $uid       | Username for the SSH server ||
Password           | $pwd       | Password for the SSH server ||
SSH Key File       | $kfile     | Optional: SSH key file to avoid having to type the password every time. Note that you must quote it like this "$kfile" if the keyfile path contains spaces. ||
Key password       | $kpwd      | Optional: SSH key file password to avoid having to type the key file password every time ||

Variables from the Passwords pane
------------------

${smv_ssh.uid} and ${smv_ssh.pwd} refers to the identity named _smv_ssh_

Examples without a password
------------------

Open a putty terminal to the machine that is at the other end of the tunnel (popular)

> Name: SSH Term
> Command: ${putty} -l ${uid} -P ${port} ${ip}


Transfer files to/from the machine that is at the other end of the tunnel (popular)

> Name: SSH FTP
> Command: ${winscp} sftp://${uid}@${ip}:${port}


Open a web page on the machine that is at the other end of the tunnel (popular)

> Name: Provisioning web
> Command: ${firefox} http://${ip}:${port}/provisioning/loginForm.jsp


Open an external web site

> Name: Google
> Command: ${firefox} http://www.google.com


Open a putty terminal to the host itself
You do not have to click Connect to use this.

> Name: SSH Term
> Command: ${putty} -l ${uid} -P 22 ${host}


Transfer files to/from the host itself
You do not have to click Connect to use this.

> Name: SSH FTP
> Command: ${winscp} sftp://${uid}@${host}


Examples containing password
------------------
The following examples contain passwords. Use these if you do not want to be prompted for a password when you run them.
Note! For safety reasons you should not use these. When you run them the passwords can easily be discovered by av virus.

Open a putty terminal to the machine that is at the other end of the tunnel (popular)

> Name: SSH Term
> Command: ${putty} -l ${uid} -pw ${pwd} -P ${port} ${ip}


Transfer files to/from the machine that is at the other end of the tunnel (popular)

> Name: SSH FTP
> Command: ${winscp} sftp://${uid}:${pwd}@${ip}:${port}


Open a putty terminal to the host itself
You do not have to click Connect to use this.

> Name: SSH Term
> Command: ${putty} -l ${uid} -pw ${pwd} -P 22 ${host}


Transfer files to/from the host itself
You do not have to click Connect to use this.

> Name: SSH FTP
> Command: ${winscp} sftp://${uid}:${pwd}@${host}

