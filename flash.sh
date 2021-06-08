#!/bin/bash
set -e

#echo 'Make sure your device is connected via USB and flashing yellow.'
#read -r -p 'Do you want to continue? [Y/n] ' key

#key=${key,,} # to lower

#if [ "$key" = 'n' ]; then
#	exit
#fi

#particle flash --usb firmware.bin

particle flash splendid_pirate firmware.bin
