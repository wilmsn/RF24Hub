/*
rf24hub_config.h ==> all global definitions go here

*/
#ifndef _RF24HUB_CONFIG_H_   
#define _RF24HUB_CONFIG_H_

#define PRGNAME "rf24hub"
//
// default values: can be overwritten in config file
//
#define LOGFILE "/var/log/rf24hubd.log"
#define PIDFILE "/var/run/rf24hubd.pid"

//
// END default values: can be overwritten in config file
//
//-------------------------------------------------------
//

// array of char sizes
#define DEBUGSTRINGSIZE 600
#define FHEMDEVLENGTH 50
#define TELNETBUFFERSIZE 200
#define TSBUFFERSIZE 30
#define SQLSTRINGSIZE 500
#define TSBUFFERSTRING "                               "
#define LOOPSLEEPTIME_QUIET  200000
#define LOOPSLEEPTIME_BUSY   20000


#endif // _RF24HUB_CONFIG_H_
