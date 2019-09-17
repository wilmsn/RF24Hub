#include "rf24hubd.h"

void sighandler(int signal) {
//    exit_system(); 
	sprintf(debug, "%s","SIGTERM: Shutting down ... ");
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
    int new_tn_in_socket;
    cfg.processParams(argc, argv);
    bool test = false;
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
                sprintf(debug,"%s","Starting up ...."); cfg.logmsg(VERBOSESTARTUP,debug);
            } else if (pid > 0) {
                // Parentprozess -> exit and return to shell
                // write a message to the console
                sprintf(debug,"%s","Starting rf24hubd as daemon..."); cfg.logmsg(VERBOSESTARTUP,debug);
                // and exit
                exit (0);
            } else {
                // nagativ is an error
				fprintf(stderr,"%s\n","Fork ERROR ... exiting"); 
				cfg.removePidFile();
				exit(1);
            }
        }
    }
        
    if ( cfg.rf24HubTcpPortSet ) {
		openSocket(NULL, cfg.rf24HubTcpPort.c_str(),&tcp_address,&tcp_sockfd,TCP);
	}
    if ( cfg.rf24HubUdpPortSet ) {
		openSocket(NULL, cfg.rf24HubUdpPort.c_str(),&udp_address,&udp_sockfd,UDP);
	}
cout << "test1" << endl;        
    while(1) {
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
                printf("Sensor1_id: %u \n",getSensor(udp_r_data.sensor1));
                printf("Sensor1_value: %f \n",getValue(udp_r_data.sensor1));
                printf("Sensor2_id: %u \n",getSensor(udp_r_data.sensor2));
                printf("Sensor2_value: %f \n",getValue(udp_r_data.sensor2));
                udp_s_data.network_id = udp_r_data.network_id;
                udp_s_data.node_id = udp_r_data.node_id;
                udp_s_data.msg_id = udp_r_data.msg_id;
                if ( test ) {
					udp_s_data.sensor1 = calcTransportValue_uint(21,0);
					test = false;
				} else {
					udp_s_data.sensor1 = calcTransportValue_uint(21,1);
					test = true;
				} 
				switch (clientaddress.ss_family) {
					case AF_INET:
						inet_ntop(clientaddress.ss_family,	&((struct sockaddr_in *)&clientaddress)->sin_addr, ipAddrStr, INET_ADDRSTRLEN);
						sprintf(debug,"GW IPV4: %s",ipAddrStr);  cfg.logmsg(VERBOSETELNET,debug);; 
					break;
					case AF_INET6:
						inet_ntop(clientaddress.ss_family,	&((struct sockaddr_in6 *)&clientaddress)->sin6_addr, ipAddrStr, INET_ADDRSTRLEN);
						sprintf(debug,"GW IPV6: %s",ipAddrStr);  cfg.logmsg(VERBOSETELNET,debug);; 
					break;
				}
				sendUdpMessage( ipAddrStr, cfg.rf24GWUdpPort.c_str(), &udp_s_data); 
            }
        }
	}
    return 0;
}

