/*
rf24hub_config.h ==> all global definitions go here

*/
#ifndef _RF24HUB_CONFIG_H_   
#define _RF24HUB_CONFIG_H_

//
// default values: can be overwritten in config file
//

#define RF24HUB_PRGNAME "rf24hub"
#define RF24HUB_PRGVERSION "1.3 vom 30.11.2019"
#define RF24HUB_CONFIGFILE "/etc/rf24hub/rf24hub.cfg"
#define RF24HUB_LOGFILE "/var/log/rf24hubd.log"
#define RF24HUB_PIDFILE "/var/run/rf24hubd.pid"

#define RF24GW_PRGNAME "rf24gw"
#define RF24GW_PRGVERSION "0.1"
#define RF24GW_CONFIGFILE "/etc/rf24gw/rf24gw.cfg"
#define RF24GW_LOGFILE "/var/log/rf24gwd.log"
#define RF24GW_PIDFILE "/var/run/rf24gwd.pid"

//
// END default values: can be overwritten in config file
//
//-------------------------------------------------------
//
// How long do we try to deliver (in msec) => 20 Min.
#define KEEPINBUFFERTIME 1200000    
// length in char of your FHEM devices
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
#define DEBUGSTRINGSIZE 500
#define SQLSTRINGSIZE 500
// length of the command string send to FHEM (MUST be higher than FHEMDEVLENGTH)
#define FHEMCMDSTRINGSIZE 80
// Verboselevel
#define VERBOSECRITICAL 1
#define VERBOSESTARTUP 2
#define VERBOSECONFIG 3
#define VERBOSEORDERMIN 4
#define VERBOSEORDER 5
#define VERBOSETELNET 6
#define VERBOSESQL 7
#define VERBOSERF24 8
#define VERBOSEOTHER 9
#define TELNETBUFFERSIZE 800


#endif // _RF24HUB_CONFIG_H_
