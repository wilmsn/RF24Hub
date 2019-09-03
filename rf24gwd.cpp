#include "rf24gwd.h"


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
    int loopCount = 0;
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
            // starts rf24gateway as a deamon
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
                debug = "Starting rf24gateway as daemon...";
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
    if ( cfg.rf24GWUdpPortSet ) {
		openSocket(NULL, cfg.rf24GWUdpPort.c_str(),&udp_address,&udp_sockfd,UDP);
	} 
            udp_s_data.msg_id=100;
            udp_s_data.network_id=2711;
            udp_s_data.sensor_id=17;
            udp_s_data.value=1.11;
    while(1) {
        loopCount++;
        if ( loopCount > 1000 ) {     
			sendUdpMessage(cfg.rf24HubHostName.c_str(), cfg.rf24HubUdpPort.c_str(), &udp_s_data); 
            cout << "Send data: " << udp_s_data.msg_id << endl; 
			udp_s_data.msg_id++;
			loopCount = 0;
		}
        if ( cfg.rf24GWUdpPortSet ) {
            numbytes = recvfrom(udp_sockfd, &udp_r_data, sizeof(udp_r_data), 0, (struct sockaddr *)&udp_address,  &udp_addrlen);
            if (numbytes >0) {
                printf("listener: packet is %d bytes long\n", numbytes);
                printf("listener: packet received \n");
                printf("Sender: %s\n", inet_ntoa (udp_address.sin_addr));
                printf("Network_number: %u \n",udp_r_data.network_id);
                printf("Msg_number: %u \n",udp_r_data.msg_id);
                printf("Sensor_id: %u \n",udp_r_data.sensor_id);
                printf("Sensor_value: %f \n",udp_r_data.value);
            }
        }
		usleep(1000);
	}
    return 0;
}

 
