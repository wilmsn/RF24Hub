/**************************************************
 * Common functions
 * used in rf24gwd and rf24hubd
 * 
 **************************************************/

#ifndef _COMMON_H_   /* Include guard */
#define _COMMON_H_

#include <stdint.h>
#include <cstring>
#include <ctype.h> 
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <cstdio>
#include <netinet/in.h>
#include <fcntl.h>
#include "config.h"
#include "dataformat.h"
#include "rf24_config.h"
#include "version.h"

enum sockType_t { TCP, UDP};

struct TnData_t {
        char tntext[DEBUGSTRINGSIZE];
        int  tn_socket;    
};

struct TnMsg_t {
        long mtype;
        struct TnData_t TnData;
};

struct thread_tn_data {
   int tnsocket;
};

static const char date_format1[] = "%d.%m.%Y %H:%M:%S";
static const char date_format2[] = "%Y.%m.%d %H:%M:%S";

/*
 * utime2str: konvertiert den übergebenen
 * Unix Zeitstempel utime in einen String
 * Ist utime = 0 wird die aktuelle Zeit genommen.
 * Für form gilt:
 * 1 => 20.04.2020 18:38:17
 * 2 => 2020.04.20 18:38:17
 * Wichtig: Es muss ein Speicherplatz buf zur Aufnahme 
 * des Strings ( Grösse 20 Byte ) übergeben werden!
 */
char* utime2str(time_t utime, char* buf, uint8_t form);

char* ts(char* buf);

/******************************************************
 * trim: get rid of trailing and leading whitespace...
 *       ...including the annoying "\n" from fgets()
 ******************************************************/
char * trim (char * s);

char* alloc_str(uint16_t verboselevel, const char* msgTxt, size_t size, char* timestring);

void free_str(uint16_t verboselevel, const char* msgTxt, char* str, char* timestring);

uint64_t mymillis(void);

char* printVerbose(uint16_t verboseLevel, char* buf);

uint16_t decodeVerbose(uint16_t oldLevel, char* verboselevel);

uint32_t packData(uint8_t mychannel, char* wort4);

char* unpackData(uint32_t data, char* buf);

void sendUdpMessage(const char* host, const char* port, udpdata_t * udpdata );

bool openSocket(const char* port, struct sockaddr_in *address, int* handle, sockType_t sockType );

void printPayload(char* ts, const char* header, payload_t* mypayload);

#endif // _RF24HUBD_COMMON_H_

