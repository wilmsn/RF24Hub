/**
 * @file config.h 
 * @brief Headerdatei mit Configurationen für Hub und Gateway
 * */

#ifndef _CONFIG_H_   /* Include guard */
#define _CONFIG_H_

/**
 * @defgroup cfgprm "Config Parameter"
 * @{
 */
/// Hier wird der Name des übergreifenden Config-Files festgelegt
#define DEFAULT_CONFIG_FILE "/etc/rf24hub/rf24hub.cfg"
#define PARAM_MAXLEN 80
// The Key for the Messagebuffer (Hub)
#define MSGKEYHUB 3452
// The Key for the Messagebuffer (GW)
#define MSGKEYGW 3453
// Interval to send/repeat a request in millisec.
#define SENDINTERVAL 100
// Number of stopmessages to send
#define SENDSTOPCOUNT 3
// Interval for deleting unanswered requests in millisec.
#define DELETEINTERVAL 500
// Intervall to sync sensordata_d to sensordata 
// Time in Seconds: 86400 = 24 Hours
#define DBSYNCINTERVAL   86400
// array of char sizes
#define DEBUGSTRINGSIZE 600
#define FHEMDEVLENGTH 50
#define TELNETBUFFERSIZE 200
#define TSBUFFERSIZE 30
#define SQLSTRINGSIZE 500
#define TSBUFFERSTRING "                               "
#define NODENAMESIZE 50
#define IPADDRESSLENGTH 20

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
#define VERBOSENODE              0b0100000000000000
#define VERBOSEALL               0b0111111111111111
#define VERBOSENONE              0b0000000000000011

#define STARTUPVERBOSELEVEL      0b0000000000000011 
//#define STARTUPVERBOSELEVEL      0b0011111111111111 

/// @}
#endif // _CONFIG_H_

