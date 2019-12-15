#include "rf24gwd.h"
#include "zahlenformat.h"

//#define DEBUG

void sighandler(int signal) {
//    exit_system(); 
//	sprintf(debug, "SIGTERM: Shutting down ... ");
    sprintf(debug,"%s","SIGTERM: Shutting down ... ");
	logger.logmsg(VERBOSECRITICAL, debug);
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
                sprintf(debug,"%s","Starting up ...."); logger.logmsg(VERBOSESTARTUP,debug);
            } else if (pid > 0) {
                // Parentprozess -> exit and return to shell
                // write a message to the console
                sprintf(debug,"%s","Starting rf24gateway as daemon..."); logger.logmsg(VERBOSESTARTUP,debug);
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
    logger.verboselevel = cfg.verboseLevel;
    if ( cfg.gwUdpPortSet ) {
		openSocket(NULL, cfg.gwUdpPort.c_str(),&udp_address,&udp_sockfd,UDP);
	}
    radio.begin();
//    radio.setPALevel(RF24_PA_MIN);
    radio.setPALevel( RF24_PA_MAX ) ;
    radio.setChannel(cfg.rf24Channel);
    radio.setAutoAck(true);
    if (cfg.rf24Speed == "RF24_2MBPS") radio.setDataRate(RF24_2MBPS);
    if (cfg.rf24Speed == "RF24_1MBPS") radio.setDataRate(RF24_1MBPS);
    if (cfg.rf24Speed == "RF24_250KBPS") radio.setDataRate(RF24_250KBPS);

//	radio.setRetries(15,15);
uint8_t rx_address1[] = { 0x33, 0xcc, 0xcc, 0xcc, 0xcc};
//uint8_t rx_address2[] = { 0xcc, 0xcc, 0xcc, 0xcc, 0xcc};
uint8_t  tx_address[] = { 0xf0, 0xcc, 0xcc, 0xcc, 0xcc};
    radio.openWritingPipe(tx_address);
    radio.openReadingPipe(1,rx_address1);
//    radio.openReadingPipe(2,rx_address2);
//    radio.openWritingPipe(cfg.tx_address);
//    radio.openReadingPipe(1,cfg.rx_address1);
//    radio.openReadingPipe(2,cfg.rx_address2);
	// Start the radio listening for data
	radio.startListening();
    usleep(500);
    sprintf(debug,"%s","starting network ... "); logger.logmsg(VERBOSESTARTUP, debug);
//    radio.setDataRate(parms.rf24network_speed);
    if (cfg.verboseLevel >= VERBOSECONFIG) { radio.printDetails(); }
    while(1) {
// Is there some data for us via rf24?
		bool goodSignal = radio.testRPD();
        if ( radio.available() ){  
            sprintf(debug,"%s",">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");		logger.logmsg(VERBOSEOTHER, debug);
			sprintf(debug,"Node %s", goodSignal ? "Strong signal > 64dBm" : "Weak signal < 64dBm" ); logger.logmsg(VERBOSEOTHER, debug);
			radio.read( &payload, sizeof(payload) );
    	    sprintf(debug,"Node Network_number: %u", payload.network_id); logger.logmsg(VERBOSEOTHER, debug);
            sprintf(debug,"Node Node_number: %u", payload.node_id);		logger.logmsg(VERBOSEOTHER, debug);
            sprintf(debug,"Node Msg_number: %u", payload.msg_id);			logger.logmsg(VERBOSEOTHER, debug);
            sprintf(debug,"Node Msg_type: %u", payload.msg_type);			logger.logmsg(VERBOSEOTHER, debug);
            sprintf(debug,"Node Msg_flags: %04x", payload.msg_flags);		logger.logmsg(VERBOSEOTHER, debug);
            sprintf(debug,"Node OrderNo: %u", payload.orderno);	      	logger.logmsg(VERBOSEOTHER, debug);
            sprintf(debug,"Node Data1: %u:%f", getChannel(payload.data1), getValue_f(payload.data1));
                logger.logmsg(VERBOSEOTHER, debug);
            sprintf(debug,"Node Data2: %u:%f", getChannel(payload.data2), getValue_f(payload.data2));
                logger.logmsg(VERBOSEOTHER, debug);
            sprintf(debug,"Node Data3: %u:%f", getChannel(payload.data3), getValue_f(payload.data3));
                logger.logmsg(VERBOSEOTHER, debug);
            sprintf(debug,"Node Data4: %u:%f", getChannel(payload.data4), getValue_f(payload.data4));
                logger.logmsg(VERBOSEOTHER, debug);
            sprintf(debug,"Node Data5: %u:%f", getChannel(payload.data5), getValue_f(payload.data5));
                logger.logmsg(VERBOSEOTHER, debug);
            sprintf(debug,"Node Data6: %u:%f", getChannel(payload.data6), getValue_f(payload.data6));
                logger.logmsg(VERBOSEOTHER, debug);
            sprintf(debug,"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");			logger.logmsg(VERBOSEOTHER, debug);
            udp_node_data.network_id = payload.network_id;
            udp_node_data.node_id = payload.node_id;
            udp_node_data.msg_id = payload.msg_id;
            udp_node_data.msg_flags = payload.msg_flags;
            udp_node_data.msg_type = payload.msg_type;
            udp_node_data.orderno = payload.orderno;
            udp_node_data.data1 = payload.data1;
            udp_node_data.data2 = payload.data2;
            udp_node_data.data3 = payload.data3;
            udp_node_data.data4 = payload.data4;
            udp_node_data.data5 = payload.data5;
            udp_node_data.data6 = payload.data6;
			sendUdpMessage(cfg.hubHostName.c_str(), cfg.hubUdpPort.c_str(), &udp_node_data); 
		}
// Is there some data for us via UDP?
        if ( cfg.gwUdpPortSet ) {
            numbytes = recvfrom(udp_sockfd, &udp_hub_data, sizeof(udp_hub_data), 0, (struct sockaddr *)&clientaddress,  &clientaddress_len);
            if (numbytes >0) {
                sprintf(debug,"packet from Hub via UDP received, legth: %u",numbytes);	logger.logmsg(VERBOSEOTHER, debug);
                sprintf(debug,"Hub: Network_number: %u",udp_hub_data.network_id);		logger.logmsg(VERBOSEOTHER, debug);
                sprintf(debug,"Hub: Mode_number: %u",udp_hub_data.node_id);				logger.logmsg(VERBOSEOTHER, debug);
                sprintf(debug,"Hub: Msg_number: %u",udp_hub_data.msg_id);				logger.logmsg(VERBOSEOTHER, debug);
//                sprintf(debug,"Hub: Sensor1_id: %u",udp_hub_data.sensor1_id);			log.logmsg(VERBOSEOTHER, debug);
//                sprintf(debug,"Hub: Sensor1_value: %f",cfg.sensorValue(udp_hub_data.value1));				log.logmsg(VERBOSEOTHER, debug);
//                sprintf(debug,"Hub: Sensor2_id: %u",udp_hub_data.sensor2_id);			log.logmsg(VERBOSEOTHER, debug);
//                sprintf(debug,"Hub: Sensor2_value: %f",cfg.sensorValue(udp_hub_data.value2));				log.logmsg(VERBOSEOTHER, debug);
				switch (clientaddress.ss_family) {
					case AF_INET:
						inet_ntop(clientaddress.ss_family,	&((struct sockaddr_in *)&clientaddress)->sin_addr, ipAddrStr, INET_ADDRSTRLEN);
						sprintf(debug,"Hub IPV4: %s",ipAddrStr);  logger.logmsg(VERBOSETELNET,debug);; 
					break;
					case AF_INET6:
						inet_ntop(clientaddress.ss_family,	&((struct sockaddr_in6 *)&clientaddress)->sin6_addr, ipAddrStr, INET_ADDRSTRLEN);
						sprintf(debug,"Hub IPV6: %s",ipAddrStr);  logger.logmsg(VERBOSETELNET,debug);; 
					break;
				}
				radio.stopListening();
				if ( radio.write( &udp_hub_data, sizeof(udp_hub_data) ) ) {
					sprintf(debug,"%s","Writing to Node -> OK"); logger.logmsg(VERBOSERF24, debug);
				} else {
					sprintf(debug,"%s","Writing to Node -> ERROR"); logger.logmsg(VERBOSERF24, debug);
				}					
				radio.startListening();
            }
        } 
		usleep(100000);
	}
    return 0;
}

 
