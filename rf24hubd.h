/*
rf24hub.cpp
A unix-deamon to handle and store the information from/to all connected sensornodes. 
All information is stored in a MariaDB database.
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
Cfg             cfg(SWVERSION_STR,SWDATUM);

/**
 * @brief send a telnet comand to the fhem-host
 * 
 * Die Eventverwaltung wird in meinem System durch FHEM realisiert.
 * Dazu ist es nötig das alle geänderten Sensorwerte an FHEM übertragen werden.
 */ 
void send_fhem_cmd(NODE_DATTYPE node, uint8_t channel, char* value);

/**
 * Verabeitet eine eingehende Telnet Nachricht
 */
void process_tn_in(int new_socket, char* buffer, char* client_message);

/**
 * Initialisierung des Systems
 */
void init_system(void);

/**
 * Aufräumen bei Programmende
 */
void exit_system(void);

/**
 * @brief Erzeugung eines Datensatzes zur Übertragung an einen Node
 * 
 * In dieser Prozedure werden alle offenen (bis zu max. 6) Anweisungen für einen Node gesammelt und für die Übertragung vorbereitet.
 * @param node_id Der Node
 * @param msg_type Der Nachrichtentyp
 */
void make_order(NODE_DATTYPE node_id, uint8_t msg_type);

/**
 * @brief Verarbeitet einen empfangenen Wert
 * 
 * @param node_id Der sendende Node
 * @param data Der Transportwert
 */
void process_sensor(NODE_DATTYPE node_id, uint32_t data);

/**
 * Handler für die Interruptverarbeitung (z.B. CTRL-C)
 */
void sighandler(int signal);

/**
 * Verarbeitet die empfangenen Daten einer Funksendung
 */
void process_payload(payload_t* payload);

/**
 * @brief Das Hauptprogramm
 * 
 */
int main(int argc, char* argv[]);

#endif // _RF24HUBD_H_
