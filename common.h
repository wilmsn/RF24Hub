// Common functions
//
#ifndef _COMMON_H_   /* Include guard */
#define _COMMON_H_

#include <stdint.h>
#include <cstring>
#include <ctype.h> 
#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <cstdio>
#include "rf24hub_config.h"

static const char default_format1[] = "%d.%m.%Y %H:%M:%S";
static const char default_format2[] = "%Y.%m.%d %H:%M:%S";

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

/*
 * log_ts: liefert einen Zeitstempel für Logausgaben
 * in der Form: [2020.04.20 18:38:17.233]
 * Wichtig: Es muss ein Speicherplatz zur Aufnahme 
 * des Strings ( Grösse 26 Byte ) übergeben werden!
 */
//char * log_ts(char * buf);


char* alloc_str(uint16_t verboselevel, const char* msgTxt, size_t size);

void free_str(uint16_t verboselevel, const char* msgTxt, char* str);

uint64_t mymillis(void);

char* printVerbose(uint16_t verboseLevel, char* buf);

uint16_t decodeVerbose(uint16_t oldLevel, char* verboselevel);


#endif // _RF24HUBD_COMMON_H_

