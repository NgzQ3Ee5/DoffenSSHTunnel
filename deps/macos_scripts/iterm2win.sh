#!/usr/bin/env bash
#
# Open new iTerm window from the command line using v3 syntax for applescript as needed in iTerm2 Version 3+
# This script blocks until the cmd is executed in the new iTerm2 window.  It then leaves the window open. 
# See also https://www.iterm2.com/documentation-scripting.html
#
# Usage:
#     iterm "[WINDOW TITLE]" [CMD] ...  You can prob'ly guess
#
# Example:
#     /Applications/DoffenSSHTunnel.app/Contents/MacOS/iterm2win.sh "AESTAGEJIT Adaptation Engine - 52.19.177.146" ssh -oHostKeyAlgorithms=+ssh-dss -l ubuntu -i ~/.ssh/ae/key-stagejit.pem -o StrictHostKeyChecking=no -p 2309 localhost
#
# References:
#     iTerm AppleScript Examples:
#     https://gitlab.com/gnachman/iterm2/wikis/Applescript
#
# OSX only
[ `uname -s` != "Darwin" ] && echo 'OS X Only' &&return
function iterm () {

local wd=""
local name="$1"
local cmd="${@:2}"

# osascript &>/dev/null <<EOF
    osascript <<EOF
tell application "iTerm"
    activate
    set new_window to (create window with default profile)
    set cSession to current session of new_window
    tell new_window
        tell cSession
            set name to "$name"
#delay 1
            write text "cd $wd;$cmd"
#           delay 2
            repeat
                delay 0.1
                --          display dialog cSession is at shell prompt
                set isdone to is at shell prompt
                if isdone then exit repeat
            end repeat
        end tell
    end tell
end tell
EOF
}
iterm "$@"
