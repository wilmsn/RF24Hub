#ifndef CFG_H   
#define CFG_H

#include <stdio.h>
#include <stdlib.h>
#include <string> // for string class 
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <getopt.h>
#include <stdint.h>
#include "config.h"
#include "common.h"

using namespace std; 

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

extern uint16_t verboselevel;

class Cfg {
    
private:

FILE * pidfile_ptr;
FILE * logfile_ptr;

string configFile;

//Logger* logger;

public:
	
// Hier die verwendeten Variablen:
/**********************************************
 * Variablen für den Gateway
 **********************************************/
// Logfilename for the gw
string gwLogFileName;
// Pidfilename for the gw
string gwPidFileName;
// Hostname of hub
string gwHubHostName;
// UDP port of gw
string gwUdpPortNo;
// TCP port of gw
string gwTcpPortNo;
// gwTcpPortSet is true when an incoming TCP port is set by configuration
bool gwTcpPortSet;
// gwUdpPortSet is true when an incoming UDP port is set by configuration
bool gwUdpPortSet;
// A unique ID for this Gateway
string gwNo;
/**********************************************
 * Variablen für den Hub
 **********************************************/
// UDP port of hub
string hubUdpPortNo;
// Logfilename for the hub
string hubLogFileName;
// Pidfilename for the hub
string hubPidFileName;
// telnet Port for incoming messages
string hubTcpPortNo;
// hubTcpPortSet is true when an incoming TCP port is set by configuration
bool hubTcpPortSet;
// rf24HubUdpPortSet is true when an incoming UDP port is set by configuration
bool hubUdpPortSet;
/**********************************************
 * Database
 **********************************************/
//hostname for database server
string dbHostName;
//port on database server
string dbPortNo;
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
string fhemPortNo;
// fhemHostSet is true when an outgoing fhem hostname is set by configuration
bool fhemHostSet;
// fhemPortSet is true when an outgoing fhem port is set by configuration
bool fhemPortSet;

/**********************************************
 * GENERIC
 **********************************************/
// the programm name goes here
string prgName;
// the programm version goes here
string prgVersion;
// startDaemon is true when start as a deamon is configured
bool startDaemon = false;
// startSniffer is true when start as sniffer is configured
bool startSniffer = false;
// will start the scanner running over all channels
bool startScanner = false;
// set the level for the scanner
int setScanLevel = 0;
// will start a scanner for a single channels if set to 1..128; 0 is deaktivated
int startChannelScanner = 0;

Cfg(string _prgName, string _prgVersion);
Cfg();
~Cfg();

/*
 * trim: get rid of trailing and leading whitespace...
 *       ...including the annoying "\n" from fgets()
 */
//char * trim (char * s);

/*
 * prints a usage message for the programm
 */
void usage(const char* prgname);

/*
 * reads all the command line parameters and processes them
 * after that the config file willb e processed
 * results are stored on parms.*
 */ 
void processParams(const char* prgname, int argc, char* argv[]);

/*
 * prints the current config for gateway
 */
void printConfig_gw (void);

/*
 * prints the current config for hub
 */
void printConfig_hub (void);

/*
 * prints the current config for database
 */
void printConfig_db (void);

/*
 * If a pidfile is defined in the config file it will be set 
 */
int setPidFile(string pidFileName);

/*
 * If a pidfile is set it will be set 
 */
void removePidFile(string pidFileName);

/*
 * checks if a pid file is set (=true)
 */
int checkPidFileSet(string pidFileName);

//void begin(Logger* _logger);

};

#endif  //CFG_H
