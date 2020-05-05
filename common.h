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
char * log_ts(char * buf);

/*
 * str_ts: liefert einen Zeitstempel als array of char
 * in der Form: 2020.04.20 18:38:17
 * Wichtig: Es muss ein Speicherplatz zur Aufnahme 
 * des Strings ( Grösse 20 Byte ) übergeben werden!
 */
char * str_ts(char * buf, uint8_t form);

char* alloc_str(uint16_t verboselevel, const char* msgTxt, size_t size);

void free_str(uint16_t verboselevel, const char* msgTxt, char* str);

uint64_t mymillis(void);

uint16_t decodeVerbose(uint16_t oldLevel, char* verboselevel);


#endif // _RF24HUBD_COMMON_H_

