/*
rf24hub.cpp
A unix-deamon to handle and store the information from/to all connected sensornodes. 
All information is stored in a MariaDB database.
rf24hub is the successor of sensorhub.


Ablaufbeschreibungen:

Grundsetzliches:
Da Sensoren und Aktoren gleich behandelt werden wird ein Sensor oder Aktor immer gesetzt und immer als Sensor bezeichnet.
Der zurückgelieferte Wert ist beim Aktor der gesetzte Wert, 
beim Sensor kann ein beliebiger Wert gesetzt werden, zurückgeliefert wird der Messwert des Sensors.

1) Aetzen eines Aktors
Quelle: Telnet request in der Form: "set<last> sensor <sensor> <wert>"
Verarbeitung:
UP: process_tn_in       => Wertet die telnet Eingabe aus.
        Übersetzt den übergebenen "sensor" in die ID des Sensors. 
        Übergibt die Sensor-ID und den Wert an die UP: "set_sensor" Rückgabewert: Node-ID
        Node-ID <> 0:
            nein:  Initialisierung des Systems mittels UP "init_system"
            ja:    
                Wenn "setlast" aufgerufen wurde:
                    UP: get_order
                    UP: print_order_buffer
UP: get_order           =>  Überträgt Elemente aus dem ARRAY "order_buffer" in das ARRAY "order"
        Arbeitsschritte:
        1) Alte Orders für den aktuellen Node löschen
        2) Die ersten 4 Orders für den aktuellen Node in das ARRAY "order" als 1 DS einfügen.


UP: set_sensor          =>  Zur Sensor-ID werden Node und Channel ermittelt
                            und an die UP "fill_order_buffer" übergeben.
                        
UP: fill_order_buffer   =>  Füllt das ARRAY "order_buffer" mit dem übergebenen Sensor.
                            Dabei wird die "entytime" auf den aktuellen Unix-Zeitstempel und die "orderno" auf "0" gesetzt.




*/

#ifndef _RF24HUBD_H_   /* Include guard */
#define _RF24HUBD_H_

#define PRGNAME "rf24hub"
#define PRGVERSION "1.2 vom 04.11.2019"
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
//#include <mysql/my_config.h>
//#include <mysql/my_global.h>
//#include <mysql/mysql.h>
#include <mariadb/mysql.h>
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
#include "log.h"
#include "node.h"
#include "sensor.h"
#include "order.h"
#include "orderbuffer.h"

#define BUF 1024

using namespace std;

int sockfd;
int verboselevel = 2;
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
uint64_t start_time;
struct Order::order_t singleorder;

// Setup for GPIO 25 CE and CE0 CSN with SPI Speed @ 8Mhz
RF24 radio(RPI_V2_GPIO_P1_22, BCM2835_SPI_CS0, BCM2835_SPI_SPEED_8MHZ);  
//RF24 radio(22,0,BCM2835_SPI_SPEED_1MHZ);

RF24Network     network(radio);
Order           order;
OrderBuffer     orderbuffer;
Sensor          sensor;
Node            node;
Logger          logger;

uint16_t orderno, init_orderno;

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

void do_tn_cmd(uint16_t node, uint8_t sensor, float value);

void process_tn_in(int new_socket, char* buffer, char* client_message);

/*******************************************************************************************
*
* Nodehandling 
* Used for communication with the nodes
*
********************************************************************************************/

void init_node(uint16_t initnode );

void print_sensor(void);

void init_system(void);

void exit_system(void);

uint16_t getnodeadr(char *node);

void init_order(unsigned int element);

void print_order(void);

void init_order_buffer(unsigned int element);

void print_orderbuffer(void);

void fill_orderbuffer( uint16_t node, unsigned char channel, float value);

bool is_valid_orderno(uint16_t myorderno);

bool get_order(uint16_t node);

uint16_t set_sensor(uint32_t mysensor, float value);

uint16_t get_sensor(uint32_t mysensor);

bool node_is_next(uint16_t node);

bool is_HB_node(uint16_t node);

/*******************************************************************************************
*
* Databasehandling 
* Used for communication with MariaDB
*
********************************************************************************************/

void db_check_error(void);

void do_sql(char *sqlstmt);

void store_sensor_value(uint16_t node, uint8_t sensor, float value, bool d1, bool d2);

void process_sensor(uint16_t node, uint8_t sensor, float value, bool d1, bool d2);

/*******************************************************************************************
*
* All the rest 
*
********************************************************************************************/
uint64_t mymillis(void);

void sighandler(int signal);

int main(int argc, char* argv[]);

#endif // _RF24HUBD_H_
