#include "config.h"
#include <unistd.h>
#include <stdlib.h>
#include <iostream> 
#include <string> // for string class 


using namespace std; 


int main(int argc, char* argv[]) {
    CONFIG cfg(PRGNAME, PRGVERSION);
    cfg.processParams(argc, argv);
	// check if started as root
	if ( getuid()!=0 ) {
		std::cout << "rf24hubd has to be startet as user root" << std::endl; 
        exit(1);
    }
    // check for PID file, if exists terminate else create it
    if ( cfg.checkPidFileSet() ) {
        return 1;
    }
	std::cout << "Startup Parameters:" << std::endl; 
    cfg.printConfig();
    // starts logging
	if ( getuid()==0 ) {
       cfg.setPidFile();
    }

    if (cfg.start_daemon) {
        printf(">>>>> Starte als Deamon\n");
    }
    
    if (cfg.startScanner) {
        printf("Starte Scanner mit level: %d\n",cfg.setScanLevel);
    }
    
    if (cfg.startChannelScanner) {
        printf("Starte Channelscanner on channel: %d\n",cfg.startChannelScanner);
    }
        
    printf(">>>Logfile_mode: %d \n",cfg.logfile_mode);
    printf(">>>Logfile: %s \n",cfg.parms.logfilename);
    cfg.logmsg(1, "Test einer Logmessage");

    cfg.logmsg(1, "###### Test #######");
    cfg.logmsg(2, "###### Test #######");
    cfg.logmsg(3, "###### Test #######");
    cfg.logmsg(4, "###### Test #######");
    cfg.logmsg(5, "###### Test #######");
    cfg.logmsg(6, "###### Test #######");

    cfg.logmsg(1, "Ausgabe der Usage:");
    
    cfg.usage();
 
	if ( getuid()==0 ) {
        cfg.removePidFile();
    }
    return 0;
}

