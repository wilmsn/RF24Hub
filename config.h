#ifndef RF24HUB_CONFIG_H   
#define RF24HUB_CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream> 
#include <cstdio>
#include <string> // for string class 
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <getopt.h>
#include <fstream>
#include <stdint.h>
#include "rf24hub_config.h"

using namespace std; 

#define PRG_NAME_LENGTH 30
#define PRG_VERSION_LENGTH 10

/* 
 * Grundsätzliches zum Ablauf einer Initialisierung:
 * * Übergebene Parameter überschreiben Einträge im Config File
 * * In den Config-Variablen werden die letztgültigen Werte abgelegt
 *
 * Folgende Parameter des Config Files können durch 
 * übergebene Parameter überschrieben werden:
 * 1. Logfile
 * 2. Verbosemode
 * 
 */
class Config {
    
private:

FILE * pidfile_ptr;
FILE * logfile_ptr;

string configFile;
//bool prgIsHub;
//bool prgIsGW;
uint8_t mk_addr_byte(char byte1, char byte2);

/*
 * trim: get rid of trailing and leading whitespace...
 *       ...including the annoying "\n" from fgets()
 */
char * trim (char * s);


public:
	
// Hier die verwendeten Variablen:

/**********************************************
 * rf24Hub
 **********************************************/
// telnet Port for incoming messages
string rf24HubTcpPort;
// rf24HubTelnetPortSet is true when an incomine telnet port is set by configuration
bool rf24HubTcpPortSet;
// Port for incoming datagrams on Hub
//string rf24HubUdpPort;
// rf24HubUdpPortSet is true when an incomine udp port on Hub is set by configuration
//bool rf24HubUdpPortSet;
/**********************************************
 * rf24Gateway
 **********************************************/
// TX Address for the gateway
uint8_t tx_address[6];
// RX Address 1 
uint8_t rx_address1[6];
// RX Address 1 
uint8_t rx_address2[6];
// the hostname of the hub used by the gateway
string rf24HubHostName;
// Speed for rf24 Network, can be one of: "RF24_2MBPS", "RF24_250KBPS", "RF24_1MBPS" 
string rf24Speed;
// Channel for rf24 Network
uint8_t rf24Channel;
/**********************************************
 * Database
 **********************************************/
//hostname for database server
string dbHostName;
//port on database server
string dbPort;
//Schema inside database
string dbSchema;
//Username for database
string dbUserName;
//Password for database
string dbPassWord;
/**********************************************
 * FHEM
 **********************************************/
//Hostname for FHEM server 
string fhemHostName;
//Port for FHEM
string fhemPort;
// fhemHostSet is true when an outgoing fhem hostname is set by configuration
bool fhemHostSet;
// fhemPortSet is true when an outgoing fhem port is set by configuration
bool fhemPortSet;

/**********************************************
 * GENERIC
 **********************************************/
string pidFileName;
string logFileName;

// the programm name goes here
string prgName;
// the programm version goes here
string prgVersion;
// startDaemon is true when start as a deamon is configured
bool startDaemon;
// interactiveMode = true: prints logs to console
bool interactiveMode = false;
// logfileMode = true: prints logs to file 
bool logFileMode = false;
// verboseLevel: Use combination of symbols as default here
int verboseLevel = VERBOSECRITICAL | VERBOSESTARTUP;
// will start the scanner running over all channels
bool startScanner = false;
// set the level for the scanner
int setScanLevel = 0;
// will start a scanner for a single channels if set to 1..128; 0 is deaktivated
int startChannelScanner = 0;



Config();
Config(string _prgName, string _prgVersion);
~Config();

/*
 * prints a usage message for the programm
 */
void usage(void);

/*
 * reads all the command line parameters and processes them
 * after that the config file willb e processed
 * results are stored on parms.*
 */ 
void processParams(int argc, char* argv[]);

/*
 * prints the current config
 */
void printConfig (void);

/*
 * If a pidfile is defined in the config file it will be set 
 */
int setPidFile(void);

/*
 * If a pidfile is set it will be set 
 */
void removePidFile(void);

/*
 * checks if a pid file is set (=true)
 */
int checkPidFileSet(void);

};

#endif  //RF24HUB_CONFIG_H
