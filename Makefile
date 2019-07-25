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
CC=g++
MARIADB_LIBS := $(shell mariadb_config --libs) 
MARIADB_INC := $(shell mariadb_config --cflags)
#ARCH=armv6zk
ifeq "$(shell uname -m)" "armv7l"
ARCH=armv7-a
# The recommended compiler flags for the Raspberry Pi
CCFLAGS=-Ofast -mfpu=vfp -mfloat-abi=hard -march=$(ARCH) -mtune=arm1176jzf-s -std=c++0x
MYSQLLIBS := $(shell mysql_config --libs)
endif

ifeq "$(shell uname -m)" "x86_64"
# For test builds on Linux PC
CCFLAGS=-pthread 

endif

# make all
all: rf24hubd

test_orderQueue: orderQueue.o test_orderQueue.o
	$(CC) ${CCFLAGS} $^ -o $@
test_sensorBuffer: sensorBuffer.o test_sensorBuffer.o
	$(CC) ${CCFLAGS} $^ -o $@
test_orderBuffer: orderBuffer.o test_orderBuffer.o
	$(CC) ${CCFLAGS} $^ -o $@
	./$@
test_config: config.o test_config.o
	$(CC) ${CCFLAGS} $^ -o $@
	#./$@ -c rf24hubd.cfg
test_telnet: logmsg.o telnet.o config.o test_telnet.o
	$(CC) ${CCFLAGS} -pthread $^ -o $@
	#./$@
test_sql: logmsg.o config.o test_sql.o
	$(CC) ${CCFLAGS} $^ -o $@ ${MARIADB_LIBS}
	./$@
# Make the sensorhub deamon
rf24hubd: rf24hub_main.o config.o telnet.o DB-mariaDB.o sensorBuffer.o orderBuffer.o
	$(CC) ${CCFLAGS} -Wall ${MYSQLLIBS} $^ -o $@ ${MARIADB_LIBS}
	#./$@

# clear build files
clean:
	rm rf24hubd *.o

# Install the sensorhub
install: 
	./install.sh

