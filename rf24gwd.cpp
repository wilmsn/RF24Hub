#include "rf24gwd.h"

//#define DEBUG

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
    radio.begin();
    radio.setPALevel(RF24_PA_MIN);
//    radio.setPALevel( RF24_PA_MAX ) ;
    radio.setChannel(90);
    radio.setAutoAck(true);
//	radio.setRetries(15,15);
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(0,addresses[0]);
    radio.openReadingPipe(1,addresses[1]);
	// Start the radio listening for data
	radio.startListening();
    delay(5);
    cfg.logmsg(VERBOSESTARTUP, "starting network ... ");
//    radio.setDataRate(parms.rf24network_speed);
    if (cfg.verboseLevel >= VERBOSECONFIG) { radio.printDetails(); }
    while(1) {
// Is there some data for us via rf24?
        if ( radio.available() ){  
			radio.read( &udp_node_data, sizeof(udp_node_data) );
            printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
			printf("Node Network_number: %u \n",udp_node_data.network_id);
            printf("Node Node_number: %u \n",udp_node_data.node_id);
            printf("Node Msg_number: %u \n",udp_node_data.msg_id);
            printf("Node Sensor1_id: %u \n",udp_node_data.sensor1_id);
            printf("Node Sensor1_value: %f \n",udp_node_data.value1);
            printf("Node Sensor2_id: %u \n",udp_node_data.sensor2_id);
            printf("Node Sensor2_value: %f \n",udp_node_data.value2);
            printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
			sendUdpMessage(cfg.rf24HubHostName.c_str(), cfg.rf24HubUdpPort.c_str(), &udp_node_data); 
		}
// Is there some data for us via UDP?
        if ( cfg.rf24GWUdpPortSet ) {
            numbytes = recvfrom(udp_sockfd, &udp_r_data, sizeof(udp_r_data), 0, (struct sockaddr *)&clientaddress,  &clientaddress_len);
            if (numbytes >0) {
                printf("listener: packet is %d bytes long\n", numbytes);
                printf("listener: packet received \n");
                printf("Hub: Network_number: %u \n",udp_r_data.network_id);
                printf("Hub: Mode_number: %u \n",udp_r_data.node_id);
                printf("Hub: Msg_number: %u \n",udp_r_data.msg_id);
                printf("Hub: Sensor1_id: %u \n",udp_r_data.sensor1_id);
                printf("Hub: Sensor1_value: %f \n",udp_r_data.value1);
                printf("Hub: Sensor2_id: %u \n",udp_r_data.sensor2_id);
                printf("Hub: Sensor2_value: %f \n",udp_r_data.value2);
				switch (clientaddress.ss_family) {
					case AF_INET:
						inet_ntop(clientaddress.ss_family,	&((struct sockaddr_in *)&clientaddress)->sin_addr, ipAddrStr, INET_ADDRSTRLEN);
						debug = "Hub IPV4: "; 
					break;
					case AF_INET6:
						inet_ntop(clientaddress.ss_family,	&((struct sockaddr_in6 *)&clientaddress)->sin6_addr, ipAddrStr, INET_ADDRSTRLEN);
						debug = "Hub IPV6: "; 
					break;
				}
				debug += ipAddrStr;  cfg.logmsg(VERBOSETELNET,debug);
				radio.stopListening();
				radio.write( &udp_r_data, sizeof(udp_r_data) );
				radio.startListening();
            }
        }
		usleep(1000);
	}
    return 0;
}

 
