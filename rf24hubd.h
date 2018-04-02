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

#define PRGNAME "rf24hub"
#define PRGVERSION "1.0"
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
bool start_daemon=false, tn_host_set = false, tn_port_set = false, tn_active = false, in_port_set = false, order_waiting = false;
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
const char* prgversion=PRGVERSION;


// Setup for GPIO 25 CE and CE0 CSN with SPI Speed @ 8Mhz
RF24 radio(RPI_V2_GPIO_P1_22, BCM2835_SPI_CS0, BCM2835_SPI_SPEED_8MHZ);  
//RF24 radio(22,0,BCM2835_SPI_SPEED_1MHZ);

RF24Network network(radio);

uint16_t orderno, init_orderno;

// structure to handle the sensors, filled from DB
struct sensor_t {
	uint32_t     	sensor;
	uint16_t       	node;   		// the destination node
	uint16_t     	channel;
    char			s_type;
	char			fhem_dev[FHEMDEVLENGTH];
    float			last_val;	
};
struct sensor_t 	sensor[SENSORLENGTH];

// Structure to handle the orderqueue
struct order_t {
	uint16_t 	    orderno;   		// the orderno as primary key for our message for the nodes
	uint16_t       	node;   		// the destination node
	unsigned char  	type;      		// Becomes networkheader.type
	unsigned int   	flags;     		// Some flags as part of payload
	unsigned char  	channel1;		// The channel for the sensor 1
	float		    value1;    		// the information that is send to sensor 1
	unsigned char  	channel2;  		// The channel for the sensor 2
	float		    value2;   	 	// the information that is send to sensor 2
	unsigned char  	channel3;  		// The channel for the sensor 3
	float		    value3;    		// the information that is send to sensor 3
	unsigned char  	channel4;  		// The channel for the sensor 1
	float		    value4;    		// the information that is send to sensor 4
	uint64_t		entrytime;
	uint64_t	  	last_send;		// Timestamp for last sending of this record
};
struct order_t 	order[ORDERLENGTH]; 

// structure for the order_buffer
struct order_buffer_t {
	uint16_t 	    orderno;   	// the orderno as primary key for our message for the nodes
	uint64_t		entrytime;
	uint16_t     	node;
	uint16_t     	channel;
	float        	value;
};
struct order_buffer_t 		order_buffer[ORDERBUFFERLENGTH];


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


/*******************************************************************************************
*
* Configfilehandling
* default place to look at is: DEFAULT_CONFIG_FILE (see sensorhub.h)
*
********************************************************************************************/

void init_parameters (struct config_parameters * parms);

void parse_config (struct config_parameters * parms);

void print_config (struct config_parameters * parms);

void usage(const char *prgname);

/*
 * trim: get rid of trailing and leading whitespace...
 *       ...including the annoying "\n" from fgets()
 */
char * trim (char * s);

/*******************************************************************************************
*
* Telnethandling
* Used for communication with FHEM
*
********************************************************************************************/

void exec_tn_cmd(const char *tn_cmd);

void prepare_tn_cmd(MYSQL *db,  uint16_t node, uint8_t sensor, float value);

void process_tn_in(MYSQL *db, int new_socket, char* buffer, char* client_message);

/*******************************************************************************************
*
* Nodehandling 
* Used for communication with the nodes
*
********************************************************************************************/

void init_node(MYSQL *db, uint16_t initnode );

void print_sensor(void);

void init_system(MYSQL *db);

uint16_t getnodeadr(char *node);

void init_order(unsigned int element);

void print_order(void);

void init_order_buffer(unsigned int element);

void print_order_buffer(void);

void fill_order_buffer( uint16_t node, uint16_t channel, float value);

bool is_valid_orderno(uint16_t myorderno);

bool delete_orderno(uint16_t myorderno);

void get_order(uint16_t node);

uint16_t set_sensor(uint32_t mysensor, float value);

uint16_t get_sensor(uint32_t mysensor);

bool node_is_next(uint16_t node);

/*******************************************************************************************
*
* Databasehandling 
* Used for communication with MariaDB
*
********************************************************************************************/

void db_check_error(MYSQL *db);

void do_sql(MYSQL *db, char *sqlstmt);

void store_sensor_value(MYSQL *db, uint16_t node, uint8_t sensor, float value, bool d1, bool d2);

void process_sensor(MYSQL *db, uint16_t node, uint8_t sensor, float value, bool d1, bool d2);

/*******************************************************************************************
*
* All the rest 
*
********************************************************************************************/
uint64_t mymillis(void);

void sighandler(int signal);

void logmsg(int mesgloglevel, char *mymsg);



int main(int argc, char* argv[]);

#endif // _RF24HUBD_H_
