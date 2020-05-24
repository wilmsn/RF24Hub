/*
rf24hub_config.h ==> all global definitions go here

*/
#include "version.h"
#ifndef _RF24HUB_CONFIG_H_   
#define _RF24HUB_CONFIG_H_

#define PRGNAME "rf24hub"
//#define PRGVERSION "1.5.1 vom 17.05.2020"

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
// The Key for the Messagebuffer
#define MSGKEY 3452
// How long do we try to deliver (in msec) => 20 Min.
#define KEEPINBUFFERTIME 1200000    
// Interval to send a request to the regular node in millisec.
#define SENDINTERVAL 500
// Interval to send a request to the regular node in millisec.
#define SENDINTERVAL_HB 100
// Number of stopmessages to send
#define SENDSTOPCOUNT 3
// Interval for deleting unsend requests for order for regular nodes in millisec.
// 60 Sec = 1 Min.
#define DELETEINTERVAL 2000
// Interval for deleting unsend requests for order for Heartbeat nodes in millisec.
// 1 Sec.
#define DELETEINTERVAL_HB 1000
// Intervall to sync sensordata_im to sensordata 
// In case of server crash you will loose data
// in sensordata_im !!!!!!
// Time in Seconds: 21600 = 6 Hours
#define DBSYNCINTERVAL   21600

#define PARAM_MAXLEN 80
// array of char sizes
#define DEBUGSTRINGSIZE 600
#define FHEMDEVLENGTH 50
#define TELNETBUFFERSIZE 200
#define TSBUFFERSIZE 30
#define SQLSTRINGSIZE 500
#define TSBUFFERSTRING "                               "
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
#define VERBOSEORDEREXT          0b0000001000000000
#define VERBOSEBUFFERPOINTER     0b0000010000000000
#define VERBOSEOBUFFEREXT        0b0000100000000000
#define VERBOSEPOINTER           0b0001000000000000
#define VERBOSESENSOR            0b0010000000000000
#define VERBOSEALL               0b0011111111111111
#define VERBOSENONE              0b0000000000000011

#define STARTUPVERBOSELEVEL      0b0000000000000011 


#endif // _RF24HUB_CONFIG_H_
