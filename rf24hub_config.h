/*
rf24hub_config.h ==> all global definitions go here

*/
#ifndef _RF24HUB_CONFIG_H_   
#define _RF24HUB_CONFIG_H_


//
// default values: can be overwritten in config file
//
#define LOGFILE "/var/log/rf24hubd.log"
#define PIDFILE "/var/run/rf24hubd.pid"
#define DEFAULT_CONFIG_FILE "/etc/rf24hub/rf24hub.cfg"
// The radiochannel for the sensorhub
#define RADIOCHANNEL 10
// Transmission speed
#define RADIOSPEED RF24_1MBPS
#define PRGNAME "rf24hub"
//
// END default values: can be overwritten in config file
//
//-------------------------------------------------------
//
#define ORDERLENGTH 80
#define ORDERBUFFERLENGTH 20
#define SENSORLENGTH 80
#define FHEMDEVLENGTH 40
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
#define VERBOSECONFIG 5
#define VERBOSERF24 6
#define VERBOSETELNET 7
#define VERBOSESQL 8
#define VERBOSEOTHER 9


#endif // _RF24HUB_CONFIG_H_
