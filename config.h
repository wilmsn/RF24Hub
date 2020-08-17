#ifndef RF24HUB_CONFIG_H   
#define RF24HUB_CONFIG_H

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
#include "rf24hub_config.h"
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

class Config {
    
private:

FILE * pidfile_ptr;
FILE * logfile_ptr;

string configFile;

//Logger* logger;

public:
	
// Hier die verwendeten Variablen:

/**********************************************
 * rf24Hub
 **********************************************/
// Logfilename for the hub
string logFileName;
// Pidfilename for the hub
string pidFileName;
// telnet Port for incoming messages
string incomingPort;
// rf24HubTelnetPortSet is true when an incomine telnet port is set by configuration
bool incomingPortSet;
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
// will start the scanner running over all channels
bool startScanner = false;
// set the level for the scanner
int setScanLevel = 0;
// will start a scanner for a single channels if set to 1..128; 0 is deaktivated
int startChannelScanner = 0;



//Config();
Config(string _prgName, string _prgVersion);
Config();
~Config();

/*
 * trim: get rid of trailing and leading whitespace...
 *       ...including the annoying "\n" from fgets()
 */
//char * trim (char * s);

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

//void begin(Logger* _logger);

};

#endif  //RF24HUB_CONFIG_H
