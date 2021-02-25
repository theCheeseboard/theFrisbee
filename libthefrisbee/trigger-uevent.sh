#!/bin/bash

if [ $EUID -ne 0 ]; then
    echo "This script needs to be run with root priviliges";
    exit 1;
fi;
if [ $# -eq 0 ]; then
    echo "Usage: $0 [device]"
    exit 1;
fi;
if ! [ -b $1 ]; then
    echo "error: $1 is not a block device"
    exit 1;
fi;

udevadm trigger $1
