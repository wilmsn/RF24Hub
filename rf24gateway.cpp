#include "rf24gateway.h"


void sighandler(int signal) {
//    exit_system(); 
//	sprintf(debug, "SIGTERM: Shutting down ... ");
	cfg.logmsg(VERBOSECRITICAL, "SIGTERM: Shutting down ... ");
    cfg.removePidFile();
//    unlink(parms.pidfilename);
//	msgctl(msqid, IPC_RMID, NULL);
    exit (0);
}

void error_exit(int myerrno, char* error) {
//    exit_system(); 
//	sprintf(debug, "SIGTERM: Shutting down ... ");
//	logmsg(VERBOSECRITICAL, debug);
    cfg.removePidFile();
    //unlink(parms.pidfilename);
    exit (myerrno);
}    


int main(int argc, char* argv[]) {
    pid_t pid;
    cfg.processParams(argc, argv);
	// check if started as root
	if ( getuid()!=0 ) {
		cout << "rf24gateway has to be startet as user root" << endl; 
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

    signal(SIGTERM, sighandler);
    signal(SIGINT, sighandler);

    if (cfg.startDaemon) {
        // make sure that we have a logfile
        if ( ! cfg.logFileMode ) {
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
        
    if ( cfg.udpPortSet ) {
		openSocket(cfg.udpPort.c_str(),&udp_address,&udp_sockfd,UDP);
	}
        
    while(1) {
        if ( cfg.udpPortSet ) {
            numbytes = sendto(udp_sockfd, &udp_data, sizeof(udp_data), 0, (struct sockaddr *)&udp_address,  &udp_addrlen);
            if (numbytes >0) {
            }
        }
	}
    return 0;
}

 
