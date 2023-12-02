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
INCLUDEDIR1=/usr/include/mariadb
MYSQLLIBS := $(shell mariadb_config --libs) 

ARCH=armv6zk
ifeq "$(shell uname -m)" "armv7l"
ARCH=armv7-a
MARCH=arm
CCFLAGS=-Ofast -mfpu=vfp -mfloat-abi=hard -march=$(ARCH) -mtune=arm1176jzf-s -std=c++0x -pthread
all: rf24hubd rf24gwd
debug: rf24hubd_debug
simu: clean4simulator simulator
endif

ifeq "$(shell uname -m)" "aarch64"
ARCH=aarch64
MARCH=arm
CCFLAGS=-Ofast -std=c++0x -pthread
all: rf24hubd rf24gwd
debug: rf24hubd_debug
endif

ifeq "$(shell uname -m)" "x86_64"
ARCH=x86-64
MARCH=x86
CCFLAGS=-Ofast -march=$(ARCH) -std=c++0x -pthread
all: rf24hubd
endif

# The recommended compiler flags for the Raspberry Pi
#CCFLAGS=-Ofast -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s
#CCFLAGS=-Ofast -mfpu=vfp -mfloat-abi=hard -march=$(ARCH) -mtune=arm1176jzf-s -std=c++0x -pthread
DEBUGFLAGS=-ggdb3 -O0

# make all
#all: rf24hubd rf24gwd
#debug: rf24hubd_debug
#simu: clean4simulator simulator

# Make the rf24hub deamon in debug mode
rf24hubd_debug: cfg.o dataformat.o database.o nodeclass.o sensorclass.o orderbuffer.o order.o common.o rf24hubd.cpp
	g++ ${CCFLAGS} ${DEBUGFLAGS} -Wall -I ${INCLUDEDIR} -I ${INCLUDEDIR1} -lrf24-bcm -lrf24 ${MYSQLLIBS} $^ -o $@

# Make the rf24hub deamon
rf24hubd: cfg.o gatewayclass.o dataformat.o database.o nodeclass.o sensorclass.o orderbuffer.o order.o common.o rf24hubd.cpp
	g++ ${CCFLAGS} -Wall -I ${INCLUDEDIR} -I ${INCLUDEDIR1} $^ -o $@ ${MYSQLLIBS}

# Make the rf24gw deamon
rf24gwd: cfg.o common.o dataformat.o rf24gwd.cpp
	g++ ${CCFLAGS} -Wall -I ${INCLUDEDIR} -lrf24 $^ -o $@ -lrf24-bcm

	
# Clean the needed *.o files
clean4simulator:
	rm -f dataformat.o common.o

# Make the simulator
simulator: dataformat.o common.o simulator.c
	g++ -Wall -I ${INCLUDEDIR} $^ -o $@

# Make dataformattest
dataformattest: dataformat.o dataformattest.c
	g++ -Wall -I ${INCLUDEDIR} $^ -o $@

# Test of order object
ordertest: sensorclass.o nodeclass.o orderbuffer.o common.o dataformat.o order.o order_test.cpp
	g++ ${CCFLAGS} -Wall -I ${INCLUDEDIR} -I ${INCLUDEDIR1} -lrf24-bcm -lrf24network ${MYSQLLIBS} $^ -o $@

# Test of orderbuffer object
orderbuffertest: orderbuffer.o orderbuffer_test.cpp
	g++ ${CCFLAGS} -Wall -I ${INCLUDEDIR} -I ${INCLUDEDIR1} -lrf24-bcm -lrf24network ${MYSQLLIBS} $^ -o $@

# clear build files
clean:
	rm -f *.o rf24hubd rf24gwd rf24hubd_debug

# Install the rf24hub and rf24gw
install:  install_gw install_hub

# Install the Gateway
install_gw:
	./install_gw.sh

# Install the Hub
install_hub:
	./install_hub.sh

