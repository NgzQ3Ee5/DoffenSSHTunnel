#!/usr/bin/env bash
cmd="${@:1}"
$cmd
status=$?
if ! test $status -eq 0; then
    read -n 1 -s -r -p "Press any key to continue"
fi
