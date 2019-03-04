#include "rf24hub_common.h"
#include "config.h"
#include <unistd.h>
#include <stdlib.h>

bool tn_port_set = false;
bool tn_host_set = false;
bool in_port_set = false;
bool start_daemon = false;
logmode_t logmode;
int verboselevel = 2;
FILE * logfile_ptr;
FILE * pidfile_ptr;

struct config_parameters parms;
char config_file[PARAM_MAXLEN_CONFIGFILE];

int main(int argc, char* argv[]) {
    char debug[DEBUGSTRINGSIZE];

    processParams(argc, argv);
	// check if started as root
/*	if ( getuid()!=0 ) {
           fprintf(stdout, "rf24hubd has to be startet as user root\n");
          exit(1);
        }
*/
    parseConfigFile (config_file);
    printf ("Startup Parameters:\n");
    printConfig();
    // check for PID file, if exists terminate else create it
    if( access( parms.pidfilename, F_OK ) != -1 ) {
        fprintf(stderr, "PIDFILE: %s exists, terminating\n\n", parms.pidfilename);
        exit(1);
    }
	
    // starts logging
	if ( getuid()==0 ) {
        setPidfile();
    }
    sprintf(debug,"%s","Test einer Logmessage"); 
    logmsg(1, debug);

    sprintf(debug,"%s","###### Test #######"); 
    logmsg(1, debug);
    logmsg(1, debug);
    logmsg(1, debug);
    logmsg(1, debug);
    logmsg(1, debug);
    logmsg(1, debug);

    sprintf(debug,"%s","Ausgabe der Usage:"); 
    logmsg(1, debug);
    
    usage();
    
	if ( getuid()==0 ) {
        removePidfile();
    }
    return 0;
}

