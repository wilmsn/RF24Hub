#include "rf24hubd.h" 

/*******************************************************************************************
*
* All the rest 
*
********************************************************************************************/
void sighandler(int signal) {
    char *debug =  (char*) malloc (DEBUGSTRINGSIZE);
	sprintf(debug, "SIGTERM: Cleanup system ... saving *_im tables ...");
	logger.logmsg(VERBOSECRITICAL, debug);
    exit_system(); 
	sprintf(debug, "SIGTERM: Shutting down ... ");
	logger.logmsg(VERBOSECRITICAL, debug);
//    unlink(parms.pidfilename);
//	msgctl(msqid, IPC_RMID, NULL);
    free(debug);
    exit (0);
}

uint64_t mymillis(void) {
	struct timeval tv;
	uint64_t timebuf;
	gettimeofday(&tv, NULL);
	timebuf = (((tv.tv_sec & 0x000FFFFFFFFFFFFF) * 1000) + (tv.tv_usec / 1000));
//	sprintf(debug, "+++++++++++ Mymillis: -----> %llu", timebuf );
//	logger.logmsg(VERBOSEOTHER, debug);
	return timebuf;
}

void exit_system(void) {
    
}

int main(int argc, char* argv[]) {
    pid_t pid;
    char *debug =  (char*) malloc (DEBUGSTRINGSIZE);
    cfg.processParams(argc, argv);
	logger.set_logmode('i');
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
    if ( cfg.rf24GWUdpPortSet ) {
		openSocket(NULL, cfg.rf24GWUdpPort.c_str(),&udp_address,&udp_sockfd,UDP);
	}
    delay(5);
	// Main Loop
    while(1) {
        /* Handling of incoming messages */
/*		if ( in_port_set ) {
            new_tn_in_socket = accept ( tn_in_socket, (struct sockaddr *) &address, &addrlen );
            if (new_tn_in_socket > 0) {
                thread t2(receive_tn_in, new_tn_in_socket, &address);
                t2.detach();
            }
		} */
        /* Handling of incoming messages */
		if ( cfg.rf24HubTcpPortSet ) {
            new_tn_in_socket = accept ( tcp_sockfd, (struct sockaddr *) &tcp_address, &tcp_addrlen );
            if (new_tn_in_socket > 0) {
                //receive_tn_in(new_tn_in_socket, &address);
                thread t2(receiveTelnetMessage, new_tn_in_socket, &tcp_address);
                t2.detach();
                //close (new_tn_in_socket);
            }
        }
        if ( cfg.rf24HubUdpPortSet ) {
            numbytes = recvfrom(udp_sockfd, &udp_r_data, sizeof(udp_r_data), 0, (struct sockaddr *)&clientaddress,  &clientaddress_len);
            if (numbytes >0) {
                printf("listener: packet is %d bytes long\n", numbytes);
                printf("listener: packet received \n");
                printf("Network_number: %u \n",udp_r_data.network_id);
                printf("Node_number: %u \n",udp_r_data.node_id);
                printf("Msg_number: %u \n",udp_r_data.msg_id);
//                printf("Sensor1_id: %u \n",getSensor(udp_r_data.sensor1));
//                printf("Sensor1_value: %f \n",getValue(udp_r_data.sensor1));
//                printf("Sensor2_id: %u \n",getSensor(udp_r_data.sensor2));
//                printf("Sensor2_value: %f \n",getValue(udp_r_data.sensor2));
                udp_s_data.network_id = udp_r_data.network_id;
                udp_s_data.node_id = udp_r_data.node_id;
                udp_s_data.msg_id = udp_r_data.msg_id;
/*                if ( test ) {
					udp_s_data.sensor1 = calcTransportValue_uint(21,0);
					test = false;
				} else {
					udp_s_data.sensor1 = calcTransportValue_uint(21,1);
					test = true;
				} */
				switch (clientaddress.ss_family) {
					case AF_INET:
						inet_ntop(clientaddress.ss_family,	&((struct sockaddr_in *)&clientaddress)->sin_addr, ipAddrStr, INET_ADDRSTRLEN);
						sprintf(debug,"GW IPV4: %s",ipAddrStr);  logger.logmsg(VERBOSETELNET,debug);; 
					break;
					case AF_INET6:
						inet_ntop(clientaddress.ss_family,	&((struct sockaddr_in6 *)&clientaddress)->sin6_addr, ipAddrStr, INET_ADDRSTRLEN);
						sprintf(debug,"GW IPV6: %s",ipAddrStr);  logger.logmsg(VERBOSETELNET,debug);; 
					break;
				}
//				sendUdpMessage( ipAddrStr, cfg.rf24GWUdpPort.c_str(), &udp_s_data); 
            }
        }
	} // while(1)
	return 0;
    free(debug);
}

