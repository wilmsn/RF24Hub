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
// Define an empty string for displaying data in all its forms
//#define DATASTRING '----------'
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

// array of char sizes
#define DEBUGSTRINGSIZE 600
#define FHEMDEVLENGTH 50
#define TELNETBUFFERSIZE 200
#define TSBUFFERSIZE 30
#define SQLSTRINGSIZE 500
#define TSBUFFERSTRING "                               "


#endif // _RF24HUB_CONFIG_H_
