/*
rf24hub.cpp
A unix-deamon to handle and store the information from/to all connected sensornodes. 
All information is stored in a MariaDB database.
rf24hub is the successor of sensorhub.

Version history:
V1.0 Initial version after comming from sensorhub


*/
#ifndef _RF24HUBD_H_   /* Include guard */
#define _RF24HUBD_H_

//--------- End of global define -----------------

#include "rf24hub_common.h"
#include "rf24hub_config.h"

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string> 
#include <RF24/RF24.h>
#include <RF24/utility/RPi/bcm2835.h>
#include <RF24Network/RF24Network.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <stdio.h>
#include <mysql/my_config.h>
#include <mysql/my_global.h>
#include <mysql/mysql.h>
#include <unistd.h>
#include <getopt.h>
#include <syslog.h>
#include <signal.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>
#include <fcntl.h>
#define BUF 1024

enum logmode_t { systemlog, interactive, logfile };
logmode_t logmode;
int verboselevel = 2;
int sockfd;
bool start_daemon=false, debugmode=false, tn_host_set = false, tn_port_set = false, tn_active = false, in_port_set = false;
char logfilename[300];
char tn_hostname[20], tn_portno[7];
struct sockaddr_in serv_addr;
struct hostent *server;
FILE * pidfile_ptr;
FILE * logfile_ptr;
MYSQL     *db;
MYSQL_RES *res;
MYSQL_ROW row;
char* pEnd;

// Setup for GPIO 25 CE and CE0 CSN with SPI Speed @ 8Mhz
RF24 radio(RPI_V2_GPIO_P1_22, BCM2835_SPI_CS0, BCM2835_SPI_SPEED_8MHZ);  
//RF24 radio(22,0,BCM2835_SPI_SPEED_1MHZ);

RF24Network network(radio);

uint16_t orderno, init_orderno;

// Structure to handle the orderqueue
struct order_t {
  uint16_t 	     orderno;     // the orderno as primary key for our message for the nodes
  uint16_t       to_node;     // the destination node
  unsigned char  channel;     // The channel to address the sensor
  char			 name[30];    // The name of this sensor in FHEM 
  char 		     value[5];    // the information that is send to the node
};
order_t order[7]; // we do not handle more than 6 orders (one per subnode 1...6) at one time

struct config_parameters {
  char logfilename[PARAM_MAXLEN_LOGFILE];
  char pidfilename[PARAM_MAXLEN_PIDFILE];
  char db_hostname[PARAM_MAXLEN_HOSTNAME];
  int db_port;
  char db_schema[PARAM_MAXLEN_DB_SCHEMA];
  char db_username[PARAM_MAXLEN_DB_USERNAME];
  char db_password[PARAM_MAXLEN_DB_PASSWORD];
  char telnet_hostname[PARAM_MAXLEN_HOSTNAME];
  int telnet_port;
  int incoming_port;
  rf24_datarate_e rf24network_speed;
  uint8_t rf24network_channel;
};

struct config_parameters parms;

int orderloopcount=0;
int ordersqlexeccount=0;
bool ordersqlrefresh=true;
bool log2logfile=false;
bool rf24_carrier=false;
bool rf24_rpd=false;

RF24NetworkHeader rxheader;
RF24NetworkHeader txheader;

char buffer1[50];
char buffer2[50];
char debug[DEBUGSTRINGSIZE];
char sql_stmt[SQLSTRINGSIZE];

uint16_t getnodeadr(char *node);
char config_file[PARAM_MAXLEN_CONFIGFILE];

char * trim (char * s);

uint16_t node_init(MYSQL *db, uint16_t initnode, uint16_t orderno ); 

long runtime(long starttime);

void logmsg(int mesgloglevel, char *mymsg);

void log_db_err(int rc, char *errstr, char *mysql);

bool is_jobbuffer_entry(MYSQL *db, uint16_t orderno);

void del_jobbuffer_entry(MYSQL *db, uint16_t orderno);

void exec_tn_cmd(const char *tn_cmd);

void prepare_tn_cmd(MYSQL *db,  uint16_t orderno, char *value);

void do_sql(MYSQL *db, char *sqlstmt);

void store_sensor_value(MYSQL *db, uint16_t orderno, char *value);

void sighandler(int signal);

void usage(const char *prgname);

int main(int argc, char* argv[]);

#endif // _RF24HUBD_H_
