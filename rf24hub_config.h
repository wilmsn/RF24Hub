/*
rf24hub_config.h ==> all global definitions go here

*/
#ifndef _RF24HUB_CONFIG_H_   
#define _RF24HUB_CONFIG_H_


//
// default values: can be overwritten in config file
//
#define LOGFILE "/var/log/sensorhubd.log"
#define PIDFILE "/var/run/sensorhubd.pid"
#define DEFAULT_CONFIG_FILE "/etc/sensorhub/sensorhub.cfg"
// The radiochannel for the sensorhub
#define RADIOCHANNEL 10
// Transmission speed
#define RADIOSPEED RF24_1MBPS
#define PRGNAME "Sensorhub"
//
// END default values: can be overwritten in config file
//
//-------------------------------------------------------
//
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

#endif // _RF24HUB_CONFIG_H_
