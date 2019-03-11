#ifndef RF24HUB_CONFIG_H   
#define RF24HUB_CONFIG_H

#include <stdio.h>
#include <iostream> 
#include <string> // for string class 
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <getopt.h>
#include <fstream>
#include <bits/stdint-uintn.h>
#include "rf24hub_common.h"

using namespace std; 

#define PRG_NAME_LENGTH 30
#define PRG_VERSION_LENGTH 10
#define PARAM_MAXLEN_CONFIGFILE 50

/* 
 * Grundsätzliches zum Ablauf einer Initialisierung:
 * * Übergebene Parameter überschreiben Einträge im Config File
 * * In der Struktur params werden die letztgültigen Werte abgelegt
 *
 * Folgende Parameter des Config Files können durch 
 * übergebene Parameter überschrieben werden:
 * 1. Logfile
 * 2. Verbosemode
 * 
 */
class CONFIG {
    
private:
    
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
  int verboselevel;
  rf24_datarate_e rf24network_speed;
  uint8_t rf24network_channel;
};

FILE * pidfile_ptr;

char config_file[PARAM_MAXLEN_CONFIGFILE];

public:

// all the setings are stored in this struct
config_parameters parms;
// the programm name goes here
string prgName;
// the programm version goes here
string prgVersion;
// tn_host_set is true when an outgoing telnet hostname is set by configuration
bool tn_host_set;
// tn_port_set is true when an outgoing telnet port is set by configuration
bool tn_port_set;
// in_port_set is true when an incomine telnet port is set by configuration
bool in_port_set;
// start_daemon is true when start as a deamon is configured
bool start_daemon;
// interactive_mode = true: prints logs to console
bool interactive_mode = false;
// logfile_mode = true: prints logs to file 
bool logfile_mode = false;
// verboselevel(1..9): higher number more detailed logs
int verboselevel = 2;


CONFIG();
CONFIG(string prgName, string prgVersion);
~CONFIG();

/*
 * trim: get rid of trailing and leading whitespace...
 *       ...including the annoying "\n" from fgets()
 */
char * trim (char * s);

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

/*
 * sets and opens the logfile to the given file
 */
void setLog2File(std::string);

/*
 * prints out a logmessage
 * to console and/or to file
 */
void logmsg(int, std::string);


};

#endif  //RF24HUB_CONFIG_H
