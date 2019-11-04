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
INCLUDEDIR1=/usr/include/mariabd
MYSQLLIBS := $(shell mariadb_config --libs) 

ARCH=armv6zk
ifeq "$(shell uname -m)" "armv7l"
ARCH=armv7-a
endif

# The recommended compiler flags for the Raspberry Pi
#CCFLAGS=-Ofast -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s
CCFLAGS=-Ofast -mfpu=vfp -mfloat-abi=hard -march=$(ARCH) -mtune=arm1176jzf-s -std=c++0x -pthread

# make all
all: rf24hubd 

# Make the sensorhub deamon
rf24hubd: rf24hubd.cpp
	g++ ${CCFLAGS} -Wall -I ${INCLUDEDIR} -I ${INCLUDEDIR1} -lrf24-bcm -lrf24network ${MYSQLLIBS} $^ -o $@

# clear build files
clean:
	rm rf24hubd

# Install the sensorhub
install: 
	./install.sh

