#ifndef RF24HUB_CONFIG_H   
#define RF24HUB_CONFIG_H

#include "rf24hub_common.h"
#include "logmsg.h"


extern bool tn_host_set;
extern bool tn_port_set;
extern bool in_port_set;
extern bool start_daemon;

extern logmode_t logmode;
extern int verboselevel;
extern FILE * logfile_ptr;
extern FILE * pidfile_ptr;


/*
 * trim: get rid of trailing and leading whitespace...
 *       ...including the annoying "\n" from fgets()
 */
char * trim (char * s);

void usage(void);

void processParams(int argc, char* argv[]);

void printConfig (void);

void parseConfigFile (char * config_file);

int setPidfile(void);

void removePidfile(void);


#endif  //RF24HUB_CONFIG_H
