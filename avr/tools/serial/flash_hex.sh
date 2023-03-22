#!/bin/bash
#
# Einstellung ohne BOD
#
if [ 1 -eq $# ]
then
   avrdude -c arduino -P /dev/ttyUSB0 -B 10 -p atmega328P -U flash:w:$1 
   # -U eeprom:r:main.eep:i  # -U eeprom:w:main.eep
else
   echo "Usage: $0 <prgfile>.hex"
fi

echo "Einstellung: int. Takt 8Mhz, SPIEN, EESAVE, BOD off, no lock"

#echo -n "Hit <Return> to continue"
#read xxxx


