/*


*/
#ifndef _LOG_H_   
#define _LOG_H_
#include <time.h>
#include <sys/time.h>
#include <cstdio>
#include <cstddef>
#include <cstdlib>
#include "rf24hub_config.h"

class Logger {
    
private:

FILE * logfile_ptr;
    
public:
    
enum logmode_t { systemlog, interactive, logfile };
logmode_t logmode;

char* logfilename;
int verboselevel;
void logmsg(int mesgloglevel, char *mymsg);
void set_logfile(char*);
void set_logmode(char);

Logger(void);

};

#endif // _LOG_H_