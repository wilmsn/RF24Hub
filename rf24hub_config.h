/*
rf24hub_config.h ==> all global definitions go here

*/
#ifndef _RF24HUB_CONFIG_H_   
#define _RF24HUB_CONFIG_H_

#define PRGNAME "rf24hub"
#define PRGVERSION "1.4 vom 11.01.2020"

//
// default values: can be overwritten in config file
//
#define RF24HUB_LOGFILE "/var/log/rf24hubd.log"
#define RF24HUB_PIDFILE "/var/run/rf24hubd.pid"
#define RF24HUB_CONFIGFILE "/etc/rf24hub/rf24hubd.cfg"

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
#define HB_DELETEINTERVAL                   1000
#define PARAM_MAXLEN                        80
#define PARAM_MAXLEN_CONFIGFILE             40
#define PARAM_MAXLEN_LOGFILE                40
#define PARAM_MAXLEN_PIDFILE                40
#define PARAM_MAXLEN_RF24NETWORK_CHANNEL    4
#define PARAM_MAXLEN_RF24NETWORK_SPEED      10
#define PARAM_MAXLEN_HOSTNAME               20
#define PARAM_MAXLEN_DB_SCHEMA              20
#define PARAM_MAXLEN_DB_USERNAME            20
#define PARAM_MAXLEN_DB_PASSWORD            20
#define DEBUGSTRINGSIZE                     500
#define SQLSTRINGSIZE                       500
// Verboselevel
#define VERBOSECRITICAL          0b0000000000000001
#define VERBOSESTARTUP           0b0000000000000010
#define VERBOSECONFIG            0b0000000000000100
#define VERBOSEORDER             0b0000000000001000
#define VERBOSEOBUFFER           0b0000000000010000
#define VERBOSETELNET            0b0000000000100000
#define VERBOSESQL               0b0000000001000000
#define VERBOSERF24              0b0000000010000000
#define VERBOSEOTHER             0b0000000100000000
// Payload Message Flags
#define FLAG_EMPTY               0b00000000
#define FLAG_LASTMESSAGE         0b00000001
// Payload Message Type
#define MSGTYPEND                0
#define MSGTYPHB1                51
#define MSGTYPORQ1               71
#define MSGTYPORP1               81

#define TELNETBUFFERSIZE         800


#endif // _RF24HUB_CONFIG_H_
