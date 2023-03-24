#!/bin/bash
#
# Einstellung ohne BOD
#
avrdude -B 10 -c arduino -P /dev/ttyUSB0 -p atmega328P -n -v

echo "Fuses should be: E:FF, H:D7, L:E2"
echo -n "Hit <Return> to continue"
read xxxx


