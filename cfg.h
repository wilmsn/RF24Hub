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
 * Gemeinsam verwendet
 *********************************************/ 
// UDP port of hub
string udp_hubPortno;
// UDP port of gw
string udp_gwPortno;
/**********************************************
 * rf24Hub
 **********************************************/
// Logfilename for the gw
string gw_logFileName;
// Pidfilename for the gw
string gw_pidFileName;
// Hostname of hub
string gw_hubHostname;
// A unique ID for this Gateway
string gw_gwID;
/**********************************************
 * rf24Hub
 **********************************************/
// Logfilename for the hub
string hub_logFileName;
// Pidfilename for the hub
string hub_pidFileName;
// telnet Port for incoming messages
string hub_incomingPort;
// rf24HubTelnetPortSet is true when an incomine telnet port is set by configuration
bool hub_incomingPortSet;
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
string fhemHost;
//Port for FHEM
string fhemPort;
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
