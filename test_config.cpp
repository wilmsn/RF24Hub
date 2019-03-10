#include "rf24hub_common.h"
#include "config.h"
#include <unistd.h>
#include <stdlib.h>
#include <iostream> 
#include <string> // for string class 

using namespace std; 


int main(int argc, char* argv[]) {
    char debug[DEBUGSTRINGSIZE];
    LOGMSG logmsg;
    CONFIG cfg(PRGNAME, PRGVERSION, &logmsg);
    cfg.processParams(argc, argv);
	// check if started as root
	if ( getuid()!=0 ) {
		std::cout << "rf24hubd has to be startet as user root" << std::endl; 
        exit(1);
    }
	std::cout << "Startup Parameters:" << std::endl; 
    cfg.printConfig();
    // check for PID file, if exists terminate else create it
    if ( ! cfg.checkPidFileSet() ) return 1;
	
    // starts logging
	if ( getuid()==0 ) {
       cfg.setPidFile();
    }
    
    if (cfg.startAsDeamon()) {
        printf(">>>>> Starte als Deamon\n");
    }
    
    logmsg.logmsg(1, "Test einer Logmessage");

    logmsg.logmsg(1, "###### Test #######");
    logmsg.logmsg(2, "###### Test #######");
    logmsg.logmsg(3, "###### Test #######");
    logmsg.logmsg(4, "###### Test #######");
    logmsg.logmsg(5, "###### Test #######");
    logmsg.logmsg(6, "###### Test #######");

    logmsg.logmsg(1, "Ausgabe der Usage:");
    
    cfg.usage();
 
	if ( getuid()==0 ) {
        cfg.removePidFile();
    }
    return 0;
}

