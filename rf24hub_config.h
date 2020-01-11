/*
rf24hub_config.h ==> all global definitions go here

*/
#ifndef _RF24HUB_CONFIG_H_   
#define _RF24HUB_CONFIG_H_

#define PRGNAME "rf24hub"
#define PRGVERSION "1.3.1 vom 07.12.2019"

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
#define HB_DELETEINTERVAL 1000
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
// Verboselevel
#define VERBOSECRITICAL          0b0000000000000001
#define VERBOSESTARTUP           0b0000000000000010
#define VERBOSECONFIG            0b0000000000000100
#define VERBOSEORDER             0b0000000000001000
#define VERBOSEORDERBUFFER       0b0000000000010000
#define VERBOSETELNET            0b0000000000100000
#define VERBOSESQL               0b0000000001000000
#define VERBOSERF24              0b0000000010000000
#define VERBOSEOTHER             0b0000000100000000
// Payload Message Flags
#define PAYLOAD_FLAG_EMPTY       0b00000000
#define PAYLOAD_FLAG_LASTMESSAGE 0b00000001
// Payload Message Type
#define PAYLOAD_TYPE_HB          51
#define PAYLOAD_TYPE_HBSESP      52
#define PAYLOAD_TYPE_NORMAL      61

#define TELNETBUFFERSIZE 800


#endif // _RF24HUB_CONFIG_H_
