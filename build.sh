#!/bin/bash
set -e

#command command destination dirctory
#                  device
particle compile    argon    .       --saveTo firmware.bin

if [ "$1" == 'flash' ]; then
	./flash.sh
fi
