#include "rf24hubd.h"


int main(int argc, char* argv[]) {
    pid_t pid;
    cfg.processParams(argc, argv);
	// check if started as root
	if ( getuid()!=0 ) {
		cout << "rf24hubd has to be startet as user root" << endl; 
        exit(1);
    }
    // check for PID file, if exists terminate else create it
    if ( cfg.checkPidFileSet() ) {
        return 1;
    }
	cout << "Startup Parameters:" << endl; 
    cfg.printConfig();
    // starts logging
	if ( getuid()==0 ) {
       cfg.setPidFile();
    }

    if (cfg.startDaemon) {
        // make sure that we have a logfile
        if ( ! cfg.logfileMode ) {
            cout << "Logfile is needed if runs as deamon ... exiting" << endl; 
            cfg.removePidFile();
            exit(1);
        } else {
            // starts rf24hubd as a deamon
            // no messages to console!
            pid = fork ();
            if (pid == 0) {
                // Child prozess
                chdir ("/");
                umask (0);
                debug = "Starting up ....";
                cfg.logmsg(2,debug);
            } else if (pid > 0) {
                // Parentprozess -> exit and return to shell
                // write a message to the console
                debug = "Starting rf24hubd as daemon...";
                cfg.logmsg(2,debug);
                cout << debug << endl;
                // and exit
                exit (0);
            } else {
                // nagativ is an error
            cout << "Fork ERROR ... exiting" << endl; 
            cfg.removePidFile();
            exit(1);
            }
        }
    }
    
    if (cfg.startScanner) {
        printf("Starte Scanner mit level: %d\n",cfg.setScanLevel);
    }
    
    if (cfg.startChannelScanner) {
        printf("Starte Channelscanner on channel: %d\n",cfg.startChannelScanner);
    }
        
    printf(">>>Logfile_mode: %d \n",cfg.logfileMode);
    cout << ">>>Logfile: " << cfg.parms.logFilename << endl;
    cfg.logmsg(1, "Test einer Logmessage");

    cfg.logmsg(1, "###### Test #######");
    cfg.logmsg(2, "###### Test #######");
    debug = "###### Test debug #######";
    cfg.logmsg(3, debug);
    cfg.logmsg(4, debug);
    cfg.logmsg(5, debug);
    cfg.logmsg(6, debug);

    cfg.logmsg(1, "Ausgabe der Usage:");
    
    cfg.usage();
 
	if ( getuid()==0 ) {
        cfg.removePidFile();
    }
    return 0;
}

