#ifndef RF24HUB_LOGMSG_H   
#define RF24HUB_LOGMSG_H

#include "rf24hub_common.h"
#include <string.h>
#include <stdio.h>
#include <time.h>

extern FILE * logfile_ptr;
enum logmode_t { interactive, logfile };
extern logmode_t logmode;
extern int verboselevel;


void logmsg(int mesgloglevel, char *mymsg);

#endif //RF24HUB_LOGMSG_H
