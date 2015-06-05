#!/bin/bash

APP_DIR=`dirname $0`

mkdir -p $APP_DIR/tmp/

if [[ $EUID -ne "0" ]]; then
    echo "This script must be run as root."
    exit 1
fi

[ -d $APP_DIR/tmp/libvpx ] && (cd $APP_DIR/tmp/libvpx && ./configure --enable-shared --disable-static && make install) || (echo "Please clone libvpx into $APP_DIR/tmp/libvpx , then re-run this script."; exit 1)

