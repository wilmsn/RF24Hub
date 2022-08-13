/**
 * @file rf24gw_config.h Definitions for rf24gwd
*/
#ifndef _RF24GW_CONFIG_H_   
#define _RF24GW_CONFIG_H_

#define PRGNAME "rf24gwd"

//
// default values: can be overwritten in config file
//
#define LOGFILE "/var/log/rf24gwd.log"
#define PIDFILE "/var/run/rf24gwd.pid"

//
// END default values: can be overwritten in config file
//
//-------------------------------------------------------
//
// Define an empty string for displaying data in all its forms

#define BUF 1024
#define LOOPSLEEPTIME_QUIET  200000
#define LOOPSLEEPTIME_BUSY   50000

#endif // _RF24GW_CONFIG_H_
