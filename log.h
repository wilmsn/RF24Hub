/*


*/
#ifndef _LOG_H_   
#define _LOG_H_
#include <stdio.h>
#include <stdlib.h>
#include <iostream> 
#include <cstdio>
#include <string> // for string class 
#include <time.h>
#include <sys/time.h>
#include <cstdio>
#include <cstddef>
#include <cstdlib>
#include <stdint.h>
#include "rf24hub_config.h"

using namespace std; 

class Logger {
    
private:

FILE * logfile_ptr;
    
public:
    
enum logmode_t { systemlog, interactive, logfile };
logmode_t logmode;

string logfilename;
uint16_t verboselevel;
void logmsg(int mesgloglevel, char *mymsg);
bool set_logfile(string);
void set_logmode(char);
char get_logmode(void);

Logger(void);

};

#endif // _LOG_H_
