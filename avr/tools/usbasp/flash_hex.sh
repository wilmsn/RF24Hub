#!/bin/bash
#
# Einstellung ohne BOD
#
if [ 1 -eq $# ]
then
   avrdude -e -c usbasp -P usb -B 10 -p atmega328P -U lfuse:w:0xE2:m -U hfuse:w:0xD7:m -U efuse:w:0xFF:m -U lock:w:0xFF:m
   avrdude -c usbasp -P usb -B 10 -p atmega328P -U flash:w:$1 
   # -U eeprom:r:main.eep:i  # -U eeprom:w:main.eep
else
   echo "Usage: $0 <prgfile>.hex"
fi

echo "Einstellung: int. Takt 8Mhz, SPIEN, EESAVE, BOD off, no lock"

#echo -n "Hit <Return> to continue"
#read xxxx


