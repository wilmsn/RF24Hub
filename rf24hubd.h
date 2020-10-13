/*
rf24hub.cpp
A unix-deamon to handle and store the information from/to all connected sensornodes. 
All information is stored in a MariaDB database.
rf24hub is the successor of sensorhub.
*/

#ifndef _RF24HUBD_H_   /* Include guard */
#define _RF24HUBD_H_

//--------- End of global define -----------------

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string> 
#include <iomanip>
#include <RF24/RF24.h>
#include <RF24/utility/RPi/bcm2835.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <syslog.h>
#include <signal.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include "node.h"
#include "sensor.h"
#include "order.h"
#include "orderbuffer.h"
#include "gateway.h"
#include "common.h"
#include "config.h"
#include "cfg.h"
#include "database.h"
#include "rf24_config.h"
#include "rf24hub_config.h"
#include "rf24hub_text.h"
#include "dataformat.h"
#include "version.h"

#define BUF 1024

using namespace std;

int sockfd;
struct sockaddr_in serv_addr;
struct hostent *server;
FILE * pidfile_ptr;
FILE * logfile_ptr;
udpdata_t udpdata;

uint16_t verboselevel = STARTUPVERBOSELEVEL;
char* buf;
char* tsbuf;

// Setup for GPIO 25 CE and CE0 CSN with SPI Speed @ 8Mhz
//RF24 radio(RPI_V2_GPIO_P1_22, BCM2835_SPI_CS0, BCM2835_SPI_SPEED_8MHZ);  
RF24 radio(RPI_V2_GPIO_P1_22, BCM2835_SPI_CS0, BCM2835_SPI_CLOCK_DIVIDER_32768);
//RF24 radio(22,0,BCM2835_SPI_SPEED_1MHZ);

struct thread_db_data {
   uint32_t     sensor_id;
   char         buf[20];
};

Order           order;
OrderBuffer     orderbuffer;
Sensor          sensor;
Node            node;
Database        database;
Gateway         gateway;
Cfg             cfg;

/*******************************************************************************************
*
* Telnethandling
* Used for communication with FHEM
*
********************************************************************************************/

void do_tn_cmd(NODE_DATTYPE node, uint8_t sensor, char* value);

void process_tn_in(int new_socket, char* buffer, char* client_message);

//uint32_t packData(uint8_t mychannel, char* wort4);

/*******************************************************************************************
*
* Nodehandling 
* Used for communication with the nodes
*
********************************************************************************************/

void init_system(void);

void exit_system(void);

void make_order(NODE_DATTYPE node_id, uint8_t mytype);

void process_sensor(NODE_DATTYPE node_id, uint32_t data);

/*******************************************************************************************
*
* All the rest 
*
********************************************************************************************/
void sighandler(int signal);

//void printPayload(payload_t * mypayload);

void process_payload(payload_t* mypayload);

int main(int argc, char* argv[]);

#endif // _RF24HUBD_H_
