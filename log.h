/*


*/
#ifndef _LOG_H_   
#define _LOG_H_
#include <time.h>
#include <sys/time.h>
#include <cstdio>
#include <cstddef>
#include <cstdlib>
#include <stdint.h>
#include "rf24hub_config.h"

class Logger {
    
private:

FILE * logfile_ptr;
    
public:
    
enum logmode_t { systemlog, interactive, logfile };
logmode_t logmode;

char* logfilename;
uint16_t verboselevel;
void logmsg(int mesgloglevel, char *mymsg);
void set_logfile(char*);
void set_logmode(char);
char get_logmode(void);

Logger(void);

};

#endif // _LOG_H_
