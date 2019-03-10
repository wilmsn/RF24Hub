#ifndef RF24HUB_CONFIG_H   
#define RF24HUB_CONFIG_H

#include <stdio.h>
#include "logmsg.h"
#include<iostream> 
#include<string> // for string class 

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
class CONFIG : public LOGMSG{
    
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

config_parameters parms;

bool tn_host_set;
bool tn_port_set;
bool in_port_set;
bool start_daemon;
bool interactive_mode = false;
bool logfile_set = false;

int verboselevel;
FILE * pidfile_ptr;

string prgName;
string prgVersion;
char config_file[PARAM_MAXLEN_CONFIGFILE];

public:
/*
 * trim: get rid of trailing and leading whitespace...
 *       ...including the annoying "\n" from fgets()
 */
//char * trim (char * s);

CONFIG(string prgName, string prgVersion);


void usage(void);

void processParams(int argc, char* argv[]);

void printConfig (void);

void parseConfigFile (char * config_file);

int setPidFile(void);

void removePidFile(void);

int checkPidFileSet(void);

int startAsDeamon(void);

int checkLogFileSet(void);
};

#endif  //RF24HUB_CONFIG_H
