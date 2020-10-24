/*
*/

#ifndef _RF24GWD_H_   /* Include guard */
#define _RF24GWD_H_

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

#include "version.h"
#include "config.h"
#include "common.h"
#include "cfg.h"
#include "rf24_config.h"
#include "rf24gw_config.h"
#include "rf24gw_text.h"

using namespace std;

FILE * pidfile_ptr;
FILE * logfile_ptr;

struct sockaddr_in udp_address;
int udp_sockfd;
uint16_t verboselevel = STARTUPVERBOSELEVEL;
payload_t payload;
udpdata_t udpdata;
char* buf;
char* tsbuf;

// Setup for GPIO 25 CE and CE0 CSN with SPI Speed @ 8Mhz
//RF24 radio(RPI_V2_GPIO_P1_22, BCM2835_SPI_CS0, BCM2835_SPI_SPEED_8MHZ);  
RF24 radio(RPI_V2_GPIO_P1_22, BCM2835_SPI_CS0, BCM2835_SPI_CLOCK_DIVIDER_32768);
//RF24 radio(22,0,BCM2835_SPI_SPEED_1MHZ);

Cfg          cfg(SWVERSION_STR,SWDATUM);

static void* receive_tn_in (void *arg);
bool process_tn_in( char* inbuffer, int tn_socket);
void exit_system(void);
void init_system(void);
void channelscanner (uint8_t channel);
void scanner(char scanlevel);

void sighandler(int signal);

int main(int argc, char* argv[]);

#endif // _RF24GWD_H_
