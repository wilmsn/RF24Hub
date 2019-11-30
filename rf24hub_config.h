/*
rf24hub_config.h ==> all global definitions go here

*/
#ifndef _RF24HUB_CONFIG_H_   
#define _RF24HUB_CONFIG_H_

#define PRGNAME "rf24hub"
#define PRGVERSION "1.3 vom 30.11.2019"

//
// default values: can be overwritten in config file
//
#define LOGFILE "/var/log/rf24hubd.log"
#define PIDFILE "/var/run/rf24hubd.pid"
#define DEFAULT_CONFIG_FILE "/etc/rf24hub/rf24hubd.cfg"

//
// END default values: can be overwritten in config file
//
//-------------------------------------------------------
//
// How long do we try to deliver (in msec) => 20 Min.
#define KEEPINBUFFERTIME 1200000    
#define FHEMDEVLENGTH 50
// Interval to send a request to the regular node in millisec.
#define SENDINTERVAL 500
// Interval to send a request to the regular node in millisec.
#define HB_SENDINTERVAL 100
// Interval for deleting unsend requests for order for regular nodes in millisec.
// 600 Sec = 10 Min.
#define DELETEINTERVAL 600000
// Interval for deleting unsend requests for order for Heartbeat nodes in millisec.
// 10 Sec.
#define HB_DELETEINTERVAL 10000
#define PARAM_MAXLEN 80
#define PARAM_MAXLEN_CONFIGFILE 40
#define PARAM_MAXLEN_LOGFILE 40
#define PARAM_MAXLEN_PIDFILE 40
#define PARAM_MAXLEN_RF24NETWORK_CHANNEL 4
#define PARAM_MAXLEN_RF24NETWORK_SPEED 10
#define PARAM_MAXLEN_HOSTNAME 20
#define PARAM_MAXLEN_DB_SCHEMA 20
#define PARAM_MAXLEN_DB_USERNAME 20
#define PARAM_MAXLEN_DB_PASSWORD 20
#define ERRSTR "ERROR: "
#define DEBUGSTR "DEBUG: "
#define DEBUGSTRINGSIZE 500
#define SQLSTRINGSIZE 500
// Verboselevel
#define VERBOSECRITICAL 1
#define VERBOSESTARTUP 2
#define VERBOSECONFIG 3
#define VERBOSEORDER 5
#define VERBOSETELNET 6
#define VERBOSESQL 7
#define VERBOSERF24 8
#define VERBOSEOTHER 9
#define TELNETBUFFERSIZE 800


#endif // _RF24HUB_CONFIG_H_
