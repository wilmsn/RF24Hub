#!/bin/bash
#
# Einstellung ohne BOD
#
if [ 1 -eq $# ]
then
   avrdude -e -c usbasp -P usb -B 10 -p atmega328P -U flash:w:$1 
   # -U eeprom:r:main.eep:i  # -U eeprom:w:main.eep
else
   echo "Usage: $0 <prgfile>.hex"
fi

#echo -n "Hit <Return> to continue"
#read xxxx


