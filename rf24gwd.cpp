#include "rf24gwd.h"
#include "zahlenformat.h"

//#define DEBUG

void sighandler(int signal) {
//    exit_system(); 
//	sprintf(debug, "SIGTERM: Shutting down ... ");
    sprintf(debug,"%s","SIGTERM: Shutting down ... ");
	cfg.logmsg(VERBOSECRITICAL, debug);
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
            printf("%s\n","Logfile is needed if runs as deamon ... exiting"); 
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
                sprintf(debug,"%s","Starting up ...."); cfg.logmsg(VERBOSESTARTUP,debug);
            } else if (pid > 0) {
                // Parentprozess -> exit and return to shell
                // write a message to the console
                sprintf(debug,"%s","Starting rf24gateway as daemon..."); cfg.logmsg(VERBOSESTARTUP,debug);
                cout << debug << endl;
                // and exit
                exit (0);
            } else {
                // nagativ is an error
            printf("%s","Fork ERROR ... exiting\n");; 
            cfg.removePidFile();
            exit(1);
            }
        }
    }
    if ( cfg.rf24GWUdpPortSet ) {
		openSocket(NULL, cfg.rf24GWUdpPort.c_str(),&udp_address,&udp_sockfd,UDP);
	}
    radio.begin();
//    radio.setPALevel(RF24_PA_MIN);
    radio.setPALevel( RF24_PA_MAX ) ;
    radio.setDataRate(RF24_250KBPS);
    radio.setChannel(10);
    radio.setAutoAck(true);
//	radio.setRetries(15,15);
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(0,addresses[0]);
    radio.openReadingPipe(1,addresses[1]);
	// Start the radio listening for data
	radio.startListening();
    delay(5);
    sprintf(debug,"%s","starting network ... "); cfg.logmsg(VERBOSESTARTUP, debug);
//    radio.setDataRate(parms.rf24network_speed);
    if (cfg.verboseLevel >= VERBOSECONFIG) { radio.printDetails(); }
    while(1) {
// Is there some data for us via rf24?
		bool goodSignal = radio.testRPD();
        if ( radio.available() ){  
            sprintf(debug,"%s",">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");		cfg.logmsg(VERBOSEOTHER, debug);
			sprintf(debug,"Node %s", goodSignal ? "Strong signal > 64dBm" : "Weak signal < 64dBm" ); cfg.logmsg(VERBOSEOTHER, debug);
			radio.read( &udp_node_data, sizeof(udp_node_data) );
    	    sprintf(debug,"Node Network_number: %u", udp_node_data.network_id); cfg.logmsg(VERBOSEOTHER, debug);
            sprintf(debug,"Node Node_number: %u", udp_node_data.node_id);		cfg.logmsg(VERBOSEOTHER, debug);
            sprintf(debug,"Node Msg_number: %u", udp_node_data.msg_id);			cfg.logmsg(VERBOSEOTHER, debug);
            sprintf(debug,"Node Flags: %04x", udp_node_data.flags);				cfg.logmsg(VERBOSEOTHER, debug);
            sprintf(debug,"Node Sensor1_id: %u", getSensor(udp_node_data.sensor1));		cfg.logmsg(VERBOSEOTHER, debug);
            sprintf(debug,"Node Sensor1_value: %f", getValue(udp_node_data.sensor1));		cfg.logmsg(VERBOSEOTHER, debug);
            sprintf(debug,"Node Sensor2_id: %u", getSensor(udp_node_data.sensor2));		cfg.logmsg(VERBOSEOTHER, debug);
            sprintf(debug,"Node Sensor2_value: %f", getValue(udp_node_data.sensor2));		cfg.logmsg(VERBOSEOTHER, debug);
            sprintf(debug,"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");			cfg.logmsg(VERBOSEOTHER, debug);
//            sprintf(debug,"######> %s %s",cfg.rf24HubHostName.c_str(), cfg.rf24HubUdpPort.c_str());  cfg.logmsg(VERBOSEOTHER, debug);
			sendUdpMessage(cfg.rf24HubHostName.c_str(), cfg.rf24HubUdpPort.c_str(), &udp_node_data); 
		}
// Is there some data for us via UDP?
        if ( cfg.rf24GWUdpPortSet ) {
            numbytes = recvfrom(udp_sockfd, &udp_hub_data, sizeof(udp_hub_data), 0, (struct sockaddr *)&clientaddress,  &clientaddress_len);
            if (numbytes >0) {
                sprintf(debug,"packet from Hub via UDP received, legth: %u",numbytes);	cfg.logmsg(VERBOSEOTHER, debug);
                sprintf(debug,"Hub: Network_number: %u",udp_hub_data.network_id);		cfg.logmsg(VERBOSEOTHER, debug);
                sprintf(debug,"Hub: Mode_number: %u",udp_hub_data.node_id);				cfg.logmsg(VERBOSEOTHER, debug);
                sprintf(debug,"Hub: Msg_number: %u",udp_hub_data.msg_id);				cfg.logmsg(VERBOSEOTHER, debug);
//                sprintf(debug,"Hub: Sensor1_id: %u",udp_hub_data.sensor1_id);			cfg.logmsg(VERBOSEOTHER, debug);
//                sprintf(debug,"Hub: Sensor1_value: %f",cfg.sensorValue(udp_hub_data.value1));				cfg.logmsg(VERBOSEOTHER, debug);
//                sprintf(debug,"Hub: Sensor2_id: %u",udp_hub_data.sensor2_id);			cfg.logmsg(VERBOSEOTHER, debug);
//                sprintf(debug,"Hub: Sensor2_value: %f",cfg.sensorValue(udp_hub_data.value2));				cfg.logmsg(VERBOSEOTHER, debug);
				switch (clientaddress.ss_family) {
					case AF_INET:
						inet_ntop(clientaddress.ss_family,	&((struct sockaddr_in *)&clientaddress)->sin_addr, ipAddrStr, INET_ADDRSTRLEN);
						sprintf(debug,"Hub IPV4: %s",ipAddrStr);  cfg.logmsg(VERBOSETELNET,debug);; 
					break;
					case AF_INET6:
						inet_ntop(clientaddress.ss_family,	&((struct sockaddr_in6 *)&clientaddress)->sin6_addr, ipAddrStr, INET_ADDRSTRLEN);
						sprintf(debug,"Hub IPV6: %s",ipAddrStr);  cfg.logmsg(VERBOSETELNET,debug);; 
					break;
				}
				radio.stopListening();
				if ( radio.write( &udp_hub_data, sizeof(udp_hub_data) ) ) {
					sprintf(debug,"%s","Writing to Node -> OK"); cfg.logmsg(VERBOSERF24, debug);
				} else {
					sprintf(debug,"%s","Writing to Node -> ERROR"); cfg.logmsg(VERBOSERF24, debug);
				}					
				radio.startListening();
            }
        }
		usleep(1000);
	}
    return 0;
}

 
