#!/usr/bin/env bash
cmd="${@:1}"
$cmd
read -n 1 -s -r -p "Press any key to continue"
