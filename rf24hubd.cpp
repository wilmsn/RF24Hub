#include "rf24hubd.h"

void sighandler(int signal) {
//    exit_system(); 
//	sprintf(debug, "SIGTERM: Shutting down ... ");
	cfg.logmsg(VERBOSECRITICAL, "SIGTERM: Shutting down ... ");
    cfg.removePidFile(cfg.syspart_hub);
//    unlink(parms.pidfilename);
//	msgctl(msqid, IPC_RMID, NULL);
    exit (0);
}

void error_exit(int myerrno, char* error) {
//    exit_system(); 
//	sprintf(debug, "SIGTERM: Shutting down ... ");
//	logmsg(VERBOSECRITICAL, debug);
    cfg.removePidFile(cfg.syspart_hub);
    //unlink(parms.pidfilename);
    exit (myerrno);
}    


int main(int argc, char* argv[]) {
    pid_t pid;
    int new_tn_in_socket;
    cfg.processParams(argc, argv);
	// check if started as root
	if ( getuid()!=0 ) {
		cout << "rf24hubd has to be startet as user root" << endl; 
        exit(1);
    }
    // check for PID file, if exists terminate else create it
    if ( cfg.checkPidFileSet(cfg.syspart_hub) ) {
        return 1;
    }
	cout << "Startup Parameters:" << endl; 
    cfg.printConfig();
    // starts logging
	if ( getuid()==0 ) {
       cfg.setPidFile(cfg.syspart_hub);
    }

    signal(SIGTERM, sighandler);
    signal(SIGINT, sighandler);

    if (cfg.startDaemon) {
        // make sure that we have a logfile
        if ( ! cfg.logFileMode ) {
            cout << "Logfile is needed if runs as deamon ... exiting" << endl; 
            cfg.removePidFile(cfg.syspart_hub);
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
            cfg.removePidFile(cfg.syspart_hub);
            exit(1);
            }
        }
    }
        
    if ( cfg.telnetPortSet ) {
		openSocket(cfg.rf24HubTelnetPort.c_str(),&tcp_address,&tcp_sockfd,TCP);
	}
    if ( cfg.udpPortSet ) {
		openSocket(cfg.rf24HubUdpPort.c_str(),&udp_address,&udp_sockfd,UDP);
	}
cout << "test1" << endl;        
    while(1) {
        /* Handling of incoming messages */
		if ( cfg.telnetPortSet ) {
            new_tn_in_socket = accept ( tcp_sockfd, (struct sockaddr *) &tcp_address, &tcp_addrlen );
            if (new_tn_in_socket > 0) {
                //receive_tn_in(new_tn_in_socket, &address);
                thread t2(receiveTelnetMessage, new_tn_in_socket, &tcp_address);
                t2.detach();
                //close (new_tn_in_socket);
            }
        }
        if ( cfg.udpPortSet ) {
            numbytes = recvfrom(udp_sockfd, &udp_data, sizeof(udp_data), 0, (struct sockaddr *)&udp_address,  &udp_addrlen);
            if (numbytes >0) {
                printf("listener: packet is %d bytes long\n", numbytes);
                printf("listener: packet received \n");
                printf("Network_number: %u \n",udp_data.network_id);
                printf("Msg_number: %u \n",udp_data.msg_id);
                printf("Sensor_id: %u \n",udp_data.sensor_id);
                printf("Sensor_value: %f \n",udp_data.value);
            }
        }
	}
    return 0;
}

