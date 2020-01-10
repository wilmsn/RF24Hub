/*
rf24gateway.cpp
A gateway to the rf24 network. 
The gateway will manage the trafic between rf24hub and nodes.
Version history:
V0.1 Initial version


*/
#ifndef _RF24GATEWAY_H_   /* Include guard */
#define _RF24GATEWAY_H_

//--------- End of global define -----------------

#include "rf24hub_common.h"
#include "rf24hub_config.h"
#include "zahlenformat.h"
#include "log.h"
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string> 
#include <RF24/RF24.h>
#include <RF24/utility/RPi/bcm2835.h>
//#include <RF24Network/RF24Network.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <stdio.h>
//#include <mariadb/my_config.h>
//#include <mariadb/my_global.h>
//#include <mariadb/mysql.h>
#include <unistd.h>
#include <getopt.h>
#include <syslog.h>
#include <signal.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>
#include <fcntl.h>
#include <thread>

#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUF 1024

using namespace std;



// Setup for GPIO 25 CE and CE0 CSN with SPI Speed @ 8Mhz
RF24 radio(RPI_V2_GPIO_P1_22, BCM2835_SPI_CS0, BCM2835_SPI_SPEED_8MHZ);  
//RF24 radio(22,0,BCM2835_SPI_SPEED_1MHZ);

//RF24Network network(radio);

uint16_t orderno, init_orderno;

// a test structure for values comming via UDP
udp_data_t udp_hub_data, udp_r_data, udp_node_data;
payload_t payload;
struct sockaddr_in udp_address;
//socklen_t udp_addrlen;
int udp_sockfd;
int numbytes;
//struct sockaddr_storage udp_client_addr; 
//socklen_t tcp_addrlen, udp_addrlen;

struct sockaddr_storage clientaddress;
socklen_t clientaddress_len=sizeof(clientaddress);


char ipAddrStr[INET_ADDRSTRLEN];

//RF24NetworkHeader rxheader;
//RF24NetworkHeader txheader;



char debug[DEBUGSTRINGSIZE];

Logger logger;

int main(int argc, char* argv[]);

#endif // _RF24GATEWAY_H_
