#include "config.h"

using namespace std; 

string debug;

CONFIG cfg(PRGNAME, PRGVERSION);

int main(int argc, char* argv[]) {
    cfg.processParams(argc, argv);
	// check if started as root
	if ( getuid()!=0 ) {
		cout << "rf24hubd has to be startet as user root" << endl; 
        exit(1);
    }
    // check for PID file, if exists terminate else create it
    if ( cfg.checkPidFileSet() ) {
        cout << "PID file is set" << endl;
        exit(1);
    }
	cout << endl << "Startup Parameters:" << endl; 
    cfg.printConfig();
    // starts logging
	if ( getuid()==0 ) {
       cfg.setPidFile();
    }

    if (cfg.startDaemon) {
        cout << ">>>>> Starte als Deamon" << endl;
    }
    
    if (cfg.startScanner) {
        cout << "Starte Scanner mit level: " << cfg.setScanLevel << endl;
    }
    
    if (cfg.startChannelScanner) {
        cout << "Starte Channelscanner on channel: " << cfg.startChannelScanner << endl;
    }
        
    cout << ">>>Logfile_mode: " << cfg.logfileMode          << endl;
    cout << ">>>Logfile: "      << cfg.parms.logFilename    << endl;
    cfg.logmsg(1, "Test einer Logmessage");

    cfg.logmsg(1, "###### Test Level 1 #######");
    cfg.logmsg(2, "###### Test Level 2 #######");
    cfg.logmsg(3, "###### Test Level 3 #######");
    cfg.logmsg(4, "###### Test Level 4 #######");
    cfg.logmsg(5, "###### Test Level 5 #######");
    cfg.logmsg(6, "###### Test Level 6 #######");

    cout << "Ausgabe der Usage:" << endl;
    
    cfg.usage();
 
	if ( getuid()==0 ) {
        cfg.removePidFile();
    }
    return 0;
}

