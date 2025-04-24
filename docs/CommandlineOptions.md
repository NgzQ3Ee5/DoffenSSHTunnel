~~~
Options:
  -?, -h, --help              Displays help on commandline options.
  --help-all                  Displays help including Qt specific options.
  -v, --version               Displays version information.
  -d, --data-dir <directory>  Directory where application data is stored.
                              Defaults to:
                              -Windows:
                               directory of DoffenSSHTunnel.exe
                              -macOS:
                               ~/Library/Application Support/DoffenSSHTunnel
                              -Linux:
                               ~/.local/share/DoffenSSHTunnel
  -p, --pwd-file <file>       Path to the password file.
  -t, --title <title>         Set the main window title.
                              Defaults to the name of the data-file.

Arguments:
  data-file                   Optional INI file to read/write tunnel data.
                              Defaults to "DoffenSSHTunnel.ini".
                              If only a filename is given (no path), it will be
                              stored inside the data-dir.
~~~