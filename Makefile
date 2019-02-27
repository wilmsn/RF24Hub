#############################################################################
#
# Makefile for rf24hub on Raspberry Pi
#
# License: GPL (General Public License)
# Author:  Norbert Wilms 
# Date:    2013/12/10 
# 
# Description:
# ------------
# use make all and make install to install the sensorhub 
# use make installDB to install a new and empty database (existing database will be deleted)
#
PREFIX=/usr/local
EXECDIR=${PREFIX}/bin
INCLUDEDIR=${PREFIX}/include
INCLUDEDIR1=/usr/include/mysql
CC=g++

#ARCH=armv6zk
ifeq "$(shell uname -m)" "armv7l"
ARCH=armv7-a
# The recommended compiler flags for the Raspberry Pi
CCFLAGS=-Ofast -mfpu=vfp -mfloat-abi=hard -march=$(ARCH) -mtune=arm1176jzf-s -std=c++0x
MYSQLLIBS := $(shell mysql_config --libs)
endif

ifeq "$(shell uname -m)" "x86_64"
# For test builds on Linux PC
CCFLAGS=-std=c++0x -pthread

endif

test_orderBuffer: orderBuffer.o test_orderBuffer.o
	$(CC) ${CCFLAGS} orderBuffer.o test_orderBuffer.o -o $@

# make all
all: rf24hubd 

# Make the sensorhub deamon
rf24hubd: rf24hubd.cpp
	g++ ${CCFLAGS} -Wall -I ${INCLUDEDIR} -I ${INCLUDEDIR1} -lrf24-bcm -lrf24network ${MYSQLLIBS} $^ -o $@

# clear build files
clean:
	rm rf24hubd *.o

# Install the sensorhub
install: 
	./install.sh

