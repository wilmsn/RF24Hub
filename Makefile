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
endif

# The recommended compiler flags for the Raspberry Pi
#CCFLAGS=-Ofast -mfpu=vfp -mfloat-abi=hard -march=armv6zk -mtune=arm1176jzf-s
CCFLAGS=-Ofast -mfpu=vfp -mfloat-abi=hard -march=$(ARCH) -mtune=arm1176jzf-s -std=c++0x -pthread
DEBUGFLAGS=-ggdb3 -O0

# make all
all: rf24hubd rf24gwd
debug: rf24hubd_debug

# Make the rf24hub deamon in debug mode
rf24hubd_debug: cfg.o dataformat.o database.o node.o sensor.o orderbuffer.o order.o common.o rf24hubd.cpp
	g++ ${CCFLAGS} ${DEBUGFLAGS} -Wall -I ${INCLUDEDIR} -I ${INCLUDEDIR1} -lrf24-bcm -lrf24 ${MYSQLLIBS} $^ -o $@

# Make the rf24hub deamon
rf24hubd: cfg.o gateway.o dataformat.o database.o node.o sensor.o orderbuffer.o order.o common.o rf24hubd.cpp
	g++ ${CCFLAGS} -Wall -I ${INCLUDEDIR} -I ${INCLUDEDIR1} -lrf24-bcm -lrf24 ${MYSQLLIBS} $^ -o $@

# Make the rf24gw deamon
rf24gwd: cfg.o common.o dataformat.o rf24gwd.cpp
	g++ ${CCFLAGS} -Wall -I ${INCLUDEDIR} -lrf24-bcm -lrf24 $^ -o $@

# Test of order object
ordertest: sensor.o node.o orderbuffer.o common.o dataformat.o order.o order_test.cpp
	g++ ${CCFLAGS} -Wall -I ${INCLUDEDIR} -I ${INCLUDEDIR1} -lrf24-bcm -lrf24network ${MYSQLLIBS} $^ -o $@

# Test of orderbuffer object
orderbuffertest: orderbuffer.o orderbuffer_test.cpp
	g++ ${CCFLAGS} -Wall -I ${INCLUDEDIR} -I ${INCLUDEDIR1} -lrf24-bcm -lrf24network ${MYSQLLIBS} $^ -o $@

# clear build files
clean:
	rm *.o rf24hubd rf24gwd rf24hubd_debug

# Install the rf24hub and rf24gw
install:  install_gw install_hub

# Install the Gateway
install_gw:
	./install_gw.sh

# Install the Hub
install_hub:
	./install_hub.sh

