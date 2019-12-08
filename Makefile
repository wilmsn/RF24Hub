#############################################################################
#
# Makefile for rf24hub and rf24gw 
#
# License: GPL (General Public License)
# Author:  Norbert Wilms 
# Date:    08.12.2019
# 
# Description:
# ------------
# use make all and make install to install the rf24hub and rf24gw 
# use make installDB to install a new and empty database (existing database will be deleted)
#
PREFIX=/usr/local
EXECDIR=${PREFIX}/bin
INCLUDEDIR=${PREFIX}/include
INCLUDEDIR1=/usr/include/mariabd
MARIADB_LIBS := $(shell mariadb_config --libs) 
MARIADB_INC := $(shell mariadb_config --cflags)
ARCH := $(shell uname -m)

ifeq "$(ARCH)" "armv7l"
	CCFLAGS=-Ofast -mfpu=vfp -mfloat-abi=hard -march=armv7 -mtune=arm1176jzf-s -std=c++0x -pthread
	RF24FLAGS=-lrf24-bcm
endif

ifeq "$(ARCH)" "x86_64"
	CCFLAGS=-Ofast -std=c++0x -pthread
endif

# make all
all: rf24hubd rf24gwd

# Make the rf24hub deamon
rf24hubd: log.o node.o sensor.o orderbuffer.o order.o config.o telnet.o rf24hubd.cpp
	g++ ${CCFLAGS} -Wall -I ${INCLUDEDIR} -I ${INCLUDEDIR1} -lrf24-bcm ${MYSQLLIBS} $^ -o $@

rf24gwd: log.o config.o telnet.o zahlenformat.o rf24gwd.cpp
	g++ ${CCFLAGS} ${RF24FLAGS} -Wall $^ -o $@

# Test of order object
ordertest: order.o order_test.cpp
	g++ ${CCFLAGS} -Wall -I ${INCLUDEDIR} -I ${INCLUDEDIR1} -lrf24-bcm -lrf24network ${MYSQLLIBS} $^ -o $@

# Test of orderbuffer object
orderbuffertest: orderbuffer.o orderbuffer_test.cpp
	g++ ${CCFLAGS} -Wall -I ${INCLUDEDIR} -I ${INCLUDEDIR1} -lrf24-bcm -lrf24network ${MYSQLLIBS} $^ -o $@

# clear build files
clean:
	rm *.o rf24hubd rf24gwd

# Install the sensorhub
install: 
	./install.sh

