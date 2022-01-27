#!/bin/bash

avrdude  -B 10 -c arduino -P /dev/ttyUSB0 -p atmega328P -U flash:r:flash.hex:i
