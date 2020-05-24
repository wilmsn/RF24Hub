#!/bin/bash
#
# Einstellung ohne BOD
#
if [ 1 -eq $# ]
then
   avrdude -c usbasp -P usb -B 10 -p atmega328P -U flash:w:$1 
else
   echo "Usage: $0 <prgfile>.hex"
fi

#echo -n "Hit <Return> to continue"
#read xxxx


