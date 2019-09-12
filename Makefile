#############################################################################
#
# Makefile for rf24hub and gateway
#
# License: GPL (General Public License)
# Author:  Norbert Wilms 
# Date:    2019/08/22 
# 
# Description:
# ------------
# use "make" all and make install to install the sensorhub 
# use "make installDB" to install a new and empty database (existing database will be deleted)
#
PREFIX=/usr/local
EXECDIR=${PREFIX}/bin
CC=g++
MARIADB_LIBS := $(shell mariadb_config --libs) 
MARIADB_INC := $(shell mariadb_config --cflags)
ARCH := $(shell uname -m)

ifeq "$(ARCH)" "armv7l"
	CCFLAGS=-Ofast -mfpu=vfp -mfloat-abi=hard -march=armv7-a -mtune=arm1176jzf-s -std=c++0x -pthread
	RF24FLAGS=-lrf24-bcm
endif

ifeq "$(ARCH)" "x86_64"
	CCFLAGS=-Ofast -std=c++0x -pthread
endif

# make all
all: rf24hubd rf24gwd

# Make the rf24hub deamon
rf24hubd: config.o database.o fhem.o telnet.o rf24hubd.cpp
	g++ ${CCFLAGS} -Wall -I ${MARIADB_INC} ${MARIADB_LIBS} $^ -o $@

# Make the rf24gateway deamon
rf24gwd: config.o telnet.o rf24gwd.cpp
	g++ ${CCFLAGS} ${RF24FLAGS} -Wall $^ -o $@

receiver: 
	g++ ${CCFLAGS} -Wall -lrf24-bcm $@.cpp -o $@

test_config: config.o test_config.o
	$(CC) ${CCFLAGS} $^ -o $@
	#./$@ -c rf24hubd.cfg

test_telnet: config.o telnet.o test_telnet.o
	$(CC) ${CCFLAGS} $^ -o $@
	#./$@ -c rf24hubd.cfg

# clear build files
clean:
	rm rf24hubd rf24gwd *.o

# Install the sensorhub
install: 
	./install.sh

