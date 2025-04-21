#!/bin/bash
set -e

if ! [ -d /tmp/buildout ] ; then
    mkdir /tmp/buildout
fi 

SCRIPT_DIR=$( cd ${0%/*} && pwd -P )
cd "$SCRIPT_DIR/docker-ubuntu1604"

CMD="docker-compose run --rm --user $(id -u):$(id -g) doffen /buildin/build/docker-ubuntu1604/build_ubuntu.sh"
echo $CMD
eval $CMD

