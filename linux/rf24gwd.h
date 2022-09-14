/**
 * @file rf24gwd.h Header File für den rf24gwd
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
#include "secrets.h"

using namespace std;

/// Ein Pointer für das PID File. Ist dieses angelegt, kann der rf24gwd nicht (nochmal) gestartet werden.
FILE * pidfile_ptr;
/// Ein Pointer für das Log File.
FILE * logfile_ptr;

struct sockaddr_in udp_address;
/// Ein handler für den UDP Socket
int udp_sockfd;
/// Eine Variable für das aktuelle Verboselevel
uint16_t verboseLevel = STARTUPVERBOSELEVEL;
/// Die Payloadstruktur als Variable zur Übertragung an den Node
payload_t payload;
/// Die Udpdatastruktur als Variable
udpdata_t udpdata;
/// Ein generischer Buffer
char* buf;
/// Ein Buffer zur Aufnahme des Zeitstrings
char* tsbuf;

// Setup for GPIO 25 CE and CE0 CSN with SPI Speed @ 8Mhz
//RF24 radio(RPI_V2_GPIO_P1_22, BCM2835_SPI_CS0, BCM2835_SPI_SPEED_8MHZ);
/// Initialisierung des Radio
RF24 radio(RPI_V2_GPIO_P1_22, BCM2835_SPI_CS0, BCM2835_SPI_CLOCK_DIVIDER_32768);
//RF24 radio(22,0,BCM2835_SPI_SPEED_1MHZ);

/// Eine Instanz der Klasse cfg zur Konfiguration für den Gatewaydeamon
Cfg          cfg(SWVERSION_STR,__DATE__);

/**
 * Die Thread-Funktion fuer den Empfang von telnet Daten
 * Diese Funktion muss als Thread implementiert werden, weil sonst die Telnetverbindung den Deamon "pausiert" solange diese geöffnet ist.
 */
static void* receive_tn_in (void *arg);

/**
 * Werte die Eingaben, die über telnet gekommen sind aus.
 */
bool process_tn_in( char* inbuffer, int tn_socket);

/**
 * Aufräumen, Daten speichern (falls erforderlich) und Deamon sauber verlassen/beenden
 */
void exit_system(void);

/**
 * Deamon initialisieren, Daten aus DB laden (falls erforderlich) und Deamon sauber hochfahren
 */
void init_system(void);

/**
 * Ein eingebauter Scanner zur Beurteilung des übergebenen Kanals
 * @param channel Der WLAN Kanal
 */ 
void channelscanner (uint8_t channel);

/**
 * Ein eingebauter Scanner über alle WLAN Kanäle
 */ 
void scanner(char scanlevel);

/**
 * Ein Signalhandler, der es ermöglicht den Deamon z.B. mit CTRL-C oder mit "kill" zu beenden
 */
void sighandler(int signal);

/**
 * Das Haupprogramm
 */
int main(int argc, char* argv[]);

#endif // _RF24GWD_H_
