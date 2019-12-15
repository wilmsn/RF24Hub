#include "rf24hubd.h" 

void send_fhem_cmd(const char* fhem_server, int fhem_port, char* fhem_dev, float value) {
    char *debug =  (char*) malloc (DEBUGSTRINGSIZE);
    char *tn_cmd =  (char*) malloc (TELNETBUFFERSIZE);
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
	sprintf(tn_cmd,"set %s %f\n", fhem_dev, value);
	sprintf(debug,"do_tn_cmd: %s", tn_cmd);
	logger.logmsg(VERBOSETELNET,debug);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        sprintf(debug,"do_tn_cmd: error opening socket");
		logger.logmsg(VERBOSECRITICAL,debug);
	}	
    server = gethostbyname(fhem_server);
    if (server == NULL) {
        sprintf(debug,"do_tn_cmd: no such host");
		logger.logmsg(VERBOSECRITICAL,debug);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(fhem_port);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) { 
        sprintf(debug,"do_tn_cmd: error connecting");
		logger.logmsg(VERBOSECRITICAL,debug);
	} else {	
		n = write(sockfd,tn_cmd,strlen(tn_cmd));
		if (n < 0) {
			sprintf(debug,"do_tn_cmd: error writing to socket");
			logger.logmsg(VERBOSECRITICAL,debug);
		} else {
			sprintf(debug,"do_tn_cmd: Telnet to %s Port %d CMD: %s successfull", fhem_server, fhem_port, tn_cmd);
			logger.logmsg(VERBOSETELNET,debug);
		}		
	}		 
    close(sockfd);
    free(tn_cmd);
    free(debug);
}

void make_order(uint16_t mynode, uint8_t mytype) {
    uint8_t channel; 
    float value;
    void* ret_ptr;
    ret_ptr = orderbuffer.find_order4node(mynode, NULL, &channel, &value);
    if (ret_ptr) {
        order.add_order(mynode, mytype, node.is_HB_node(mynode), channel, value, mymillis());
        ret_ptr = orderbuffer.find_order4node(mynode, ret_ptr, &channel, &value);
        if (ret_ptr) {
            order.modify_order(mynode, 2, channel, value);
            ret_ptr = orderbuffer.find_order4node(mynode, ret_ptr, &channel, &value);
            if (ret_ptr) {
                order.modify_order(mynode, 3, channel, value);
                ret_ptr = orderbuffer.find_order4node(mynode, ret_ptr, &channel, &value);
                if (ret_ptr) {
                    order.modify_order(mynode, 4, channel, value);
                    ret_ptr = orderbuffer.find_order4node(mynode, ret_ptr, &channel, &value);
                    if (ret_ptr) {
                        order.modify_orderflags(mynode,0x00);
                    } else {
                        order.modify_orderflags(mynode,0x01);
                    }                        
                } else {
                    order.modify_orderflags(mynode,0x01);
                }                        
            } else {
                order.modify_orderflags(mynode,0x01);
            }                        
        } else {
            order.modify_orderflags(mynode,0x01);
        }                        
    }
}


void receiveTelnetMessage(int new_tn_in_socket, struct sockaddr_in * address) {
    char *buffer =  (char*) malloc (TELNETBUFFERSIZE);
    char *client_message =  (char*) malloc (TELNETBUFFERSIZE);
    char *debug =  (char*) malloc (DEBUGSTRINGSIZE);
    ssize_t MsgLen;
    // send something like a prompt. perl telnet is waiting for it otherwise we get error
    // use this in perl: my $t = new Net::Telnet (Timeout => 2, Port => 7001, Prompt => '/rf24hub>/');
    sprintf(client_message,"rf24hub> ");
    write(new_tn_in_socket , client_message , strlen(client_message));
//	cout << "Client " <<  inet_ntoa (address->sin_addr) << "ist connected ..." << endl;
    memset(buffer,0,sizeof(buffer));
    MsgLen = recv(new_tn_in_socket, buffer, TELNETBUFFERSIZE, 0);
    sprintf(debug, "Telnet Data: %s", cfg.trim(buffer));
	logger.logmsg(VERBOSETELNET, debug);
//    cout << "Msglen: " << MsgLen << " Buffer: " << buffer << endl;


//    write(new_tn_in_socket , client_message , strlen(client_message));
//    char msglen_str[10];
//    sprintf(msglen_str,"%ld",MsgLen);
//	sprintf (debug,"Buffer: %s MsgLen: %d ", cfg.trim(buffer), MsgLen);
//	cfg.logmsg(VERBOSETELNET, debug);

//    debug = "Buffer: \"";
//    debug += trim(buffer);
//    debug += "\" Msglen: ";
//    debug += msglen_str;
//    cfg.logmsg(VERBOSETELNET, debug);
    if (MsgLen>0) {
        process_tn_in(new_tn_in_socket, buffer, client_message);
    }
    close (new_tn_in_socket);
    free(buffer);
    free(client_message);
    free(debug);
    //                 exit(0);
}

void process_tn_in(int new_tn_in_socket, char* buffer, char* client_message) {
/* Messages can llook like this:
       <word1		word2		word3		word4 				function>
		init													Reinitialization of rf24hubd (reads actual values from database)
		list		order										Lists open orders in textform
		html 		order										Lists open orders in HTML form
		set			sensor		<sensor#> 	<value>				Sets Sensor to Value (Store in Orderbuffer only)
					node		<node#>		init				Initialize this Node (Send Initdata to the Node) 
					verbose		<value>							Sets Verboselevel to new Value (1...9)
		push		<node>      <channel>   <value>	            Pushes a value direct to a channel into a node 
		setlast		sensor		<sensor#> 	<value>				Sets Sensor to Value and starts sending (Store in Orderbuffer and transfer all requests for this Node to Order)
			
*/
			
	char cmp_init[]="init", 
		 cmp_sensor[]="sensor",
		 cmp_set[]="set",
		 cmp_setlast[]="setlast",
		 cmp_node[]="node",
		 cmp_list[]="list",
		 cmp_html[]="html",
		 cmp_order[]="order",	 
		 cmp_verbose[]="verbose",	
         cmp_push[]="push";
	char *wort1a, *wort2a, *wort3a, *wort4a;
	char *wort1, *wort2, *wort3, *wort4;
    char *debug =  (char*) malloc (DEBUGSTRINGSIZE);
	bool tn_input_ok=false;
	char delimiter[] = " ";
	trim(buffer);
	sprintf(debug,"Incoming telnet data: %s ",buffer);
	logger.logmsg(VERBOSETELNET, debug);
	wort1a = strtok(buffer, delimiter);
	wort2a = strtok(NULL, delimiter);
	wort3a = strtok(NULL, delimiter);
	wort4a = strtok(NULL, delimiter);
    if (wort1a) { 
        wort1 = static_cast<char*>(malloc(strlen(wort1a)+1));
        strcpy(wort1, wort1a);
    } else { 
        wort1 = static_cast<char*>(malloc(1));
        wort1[0] = '\0';
    }
    if (wort2a) { 
        wort2 = static_cast<char*>(malloc(strlen(wort2a)+1));
        strcpy(wort2, wort2a);
    } else { 
        wort2 = static_cast<char*>(malloc(1));
        wort2[0] = '\0';
    }
    if (wort3a) { 
        wort3 = static_cast<char*>(malloc(strlen(wort3a)+1));
        strcpy(wort3, wort3a);
    } else { 
        wort3 = static_cast<char*>(malloc(1));
        wort3[0] = '\0';
    }
    if (wort4a) { 
        wort4 = static_cast<char*>(malloc(strlen(wort4a)+1));
        strcpy(wort4, wort4a);
    } else { 
        wort4 = static_cast<char*>(malloc(1));
        wort4[0] = '\0';
    }
	// set/setlast sensor <sensor> <value>
	// sets a sensor to a value, setlast starts the request over air
	if ( (( strcmp(wort1,cmp_set) == 0 ) || ( strcmp(wort1,cmp_setlast) == 0 )) && (strcmp(wort2,cmp_sensor) == 0) && (strlen(wort3) > 0) && (strlen(wort4) > 0) ) {
		tn_input_ok = true;
		// In word3 we may have a) the number of the sensor b) the name of the sensor c) the fhem_dev of a sensor
		// for the processing we need the number of the sensor ==> find it!
        uint16_t mynode = 0;
        uint8_t mychannel = 0;
        sensor.find_node_chanel(&mynode, &mychannel, wort3, strtoul(wort3, &pEnd, 10));
        orderbuffer.add_orderbuffer(mymillis(),mynode,mychannel,strtof(wort4, &pEnd));
		if ( strcmp(wort1,cmp_setlast) == 0 && ! node.is_HB_node(mynode) ) {
			make_order(mynode, 61);
		}
    }
	// push <node> <channel> <value>
	// Pushes a value direct to a channel into a node
	if (( strcmp(wort1,cmp_push) == 0 ) && (strlen(wort2) > 1) && (strlen(wort3) > 0) && (strlen(wort4) > 0) ) {
		tn_input_ok = true;
        orderbuffer.add_orderbuffer(mymillis(),strtol(wort2, &pEnd, 10), strtol(wort3, &pEnd, 10), strtof(wort4, &pEnd));
    }
    // set node <node> init
	// sends the init sequence to a node
//	if (( strcmp(wort1,cmp_set) == 0 ) && (strcmp(wort2,cmp_node) == 0) && (strlen(wort3) > 1) && (strcmp(wort4,cmp_init) == 0) ) {
//		tn_input_ok = true;
//		init_node(strtol(wort3, &pEnd, 10));
//	}
    // set verbose <new verboselevel>
	// sets the new verboselevel
	if (( strcmp(wort1,cmp_set) == 0 ) && (strcmp(wort2,cmp_verbose) == 0) && (strlen(wort3) == 1) && (strlen(wort4) == 0) ) {
        if ( wort3[0] > '0' && wort3[0] < '9' + 1 ) {
			tn_input_ok = true;
			verboselevel = (wort3[0] - '0') * 1;
            logger.verboselevel = verboselevel;
		}	
	}
    // list order
	// lists the current orderbuffer
	if (( strcmp(wort1,cmp_list) == 0 ) && (strcmp(wort2,cmp_order) == 0) && (strlen(wort3) == 0) && (strlen(wort4) == 0) ) {
		tn_input_ok = true;
        orderbuffer.print_buffer(new_tn_in_socket);
        order.print_buffer(new_tn_in_socket);
	}	
    // list sensor
	// lists the current node- and sensorbuffer
	if (( strcmp(wort1,cmp_list) == 0 ) && (strcmp(wort2,cmp_sensor) == 0) && (strlen(wort3) == 0) && (strlen(wort4) == 0) ) {
		tn_input_ok = true;
        node.print_buffer2tn(new_tn_in_socket);
        sensor.print_buffer2tn(new_tn_in_socket);
	}	
	// html order
	// lists the current order/orderbuffer for html page
	if (( strcmp(wort1,cmp_html) == 0 ) && (strcmp(wort2,cmp_order) == 0) && (strlen(wort3) == 0) && (strlen(wort4) == 0) ) {
		tn_input_ok = true;
        orderbuffer.html_buffer(new_tn_in_socket);
        order.html_buffer(new_tn_in_socket);
    }	
    // init
	// initialisation of rf24hubd: reloads data from database
	if ( (strcmp(wort1,cmp_init) == 0) && (strlen(wort2) == 0) && (strlen(wort3) == 0) && (strlen(wort4) == 0) ) {
		tn_input_ok = true;
		init_system();
	}
	if ( ! tn_input_ok) {
		sprintf(client_message,"Usage:\n");
		write(new_tn_in_socket , client_message , strlen(client_message));
		sprintf(client_message,"set[last] sensor <sensornumber> <sensorvalue>\n");
		write(new_tn_in_socket , client_message , strlen(client_message));
		sprintf(client_message,"   Sets the sensor <sensornumber> to the value <sensorvalue>\n");
		write(new_tn_in_socket , client_message , strlen(client_message));
		sprintf(client_message,"   'set' stores only; 'setlast' executes all settings for this node\n");
		write(new_tn_in_socket , client_message , strlen(client_message));
		sprintf(client_message,"push <node> <channel> <sensorvalue>\n");
		write(new_tn_in_socket , client_message , strlen(client_message));
		sprintf(client_message,"   Pushes a value direct to a channel inside a node\n");
		write(new_tn_in_socket , client_message , strlen(client_message));
		sprintf(client_message,"set node <nodenumber> init\n");
		write(new_tn_in_socket , client_message , strlen(client_message));
		sprintf(client_message,"   Inits the Node <nodenumber>. Use <nodenumber> like '041'\n");
		write(new_tn_in_socket , client_message , strlen(client_message));
		sprintf(client_message,"init \n");
		write(new_tn_in_socket , client_message , strlen(client_message));
		sprintf(client_message,"   Reinitialisation of rf24hub (all open ordes will be deleted)\n");
		write(new_tn_in_socket , client_message , strlen(client_message));
		sprintf(client_message,"list order\n");
		write(new_tn_in_socket , client_message , strlen(client_message));
		sprintf(client_message,"   lists the content of the order queue\n");
		sprintf(client_message,"list sensor\n");
		write(new_tn_in_socket , client_message , strlen(client_message));
		sprintf(client_message,"   lists the nodes and sensors in system\n");
		write(new_tn_in_socket , client_message , strlen(client_message));
		sprintf(client_message,"set verbose <verboselevel>\n");
		write(new_tn_in_socket , client_message , strlen(client_message));
		sprintf(client_message,"   sets a new verboselevel <1..9> are valid levels\n");
		write(new_tn_in_socket , client_message , strlen(client_message));
		sprintf(client_message,"\n");
		write(new_tn_in_socket , client_message , strlen(client_message));
		sprintf(client_message,"%s version %s\n", cfg.prgName.c_str(), cfg.prgVersion.c_str());
		write(new_tn_in_socket , client_message , strlen(client_message));
	} else {
		sprintf(client_message,"Command received => OK\n");
		write(new_tn_in_socket , client_message , strlen(client_message));
	}		
	free(wort1);
    free(wort2);
    free(wort3);
    free(wort4);
    free(debug);
}



void process_sensor(uint16_t node, uint32_t tval) {
    char *debug =  (char*) malloc (DEBUGSTRINGSIZE);
    uint8_t channel = getChannel(tval);
    float value = getValue_f(tval);
    
	switch (channel) {
		case 1 ... 99: {
		// Sensor or Actor
			sprintf(debug, "Value of Channel: %u on Node: %u is %f ", channel, node, value);
			logger.logmsg(VERBOSECONFIG, debug);       
			database.storeSensorValue(node, channel, value);
		}
		break; 
		case 101: {
		// battery voltage
			sprintf(debug, "Voltage of Node: 0%o is %f ", node, value);
			logger.logmsg(VERBOSECONFIG, debug);        
//			sprintf(sql_stmt,"update node set u_batt = %f, signal_quality = '%d%d', last_contact = unix_timestamp() where node_id = '0%o'", value, d1, d2, node);
// TODO
//			do_sql(sql_stmt);
			database.storeSensorValue(node, channel, value);
		}
		break; 
		case 102 ... 127: { // System settings
			sprintf(debug, "Node: 0%o Channel: %u is set to %f ", node, channel, value);
			logger.logmsg(VERBOSECONFIG, debug);  
            database.storeNodeConfig(node, channel, value);
		}	
		break; 
		default: { 
			sprintf(debug, "Message dropped!!!! Node: 0%o Channel: %u Value: %f ", node, channel, value);
			logger.logmsg(VERBOSECONFIG, debug);        
		}
	}	
//	orderbuffer.del_node_channel(node, channel);
    char fhemdev[55];
    sprintf(fhemdev,"testsensor");
 //   sprintf(fhemdev,"set testsensor %f \n quit\n", value);
    send_fhem_cmd(cfg.fhemHostName.c_str(), stoi(cfg.fhemPort),fhemdev, value);
 //   sendTcpMessage(cfg.fhemHostName.c_str(), cfg.fhemPort.c_str(),fhemdev);
 //   openSocket(cfg.fhemHostName.c_str(), cfg.fhemPort.c_str(),&fhem_address,&fhem_sockfd,TCP);
 //printf("#>>>>>  %s\n", strerror(errno));
 //printf("#####>> %s %s %s %d \n", fhemdev, cfg.fhemHostName.c_str(), cfg.fhemPort.c_str(), fhem_sockfd);
//int n = write(fhem_sockfd,fhemdev,strlen(fhemdev));
 //printf("##>>  %s\n", strerror(errno));
 //printf("#####>>>>> write %d \n", n);
    //sendFHEM(fhemdev, value);
 //   close(fhem_sockfd);
    free(debug);
}	

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
    cfg.removePidFile();
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


void init_system(void) {
    database.fillNode(&node);
    database.fillSensor(&sensor);
    node.print_buffer2log();
    sensor.print_buffer2log();
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
    signal(SIGPIPE,SIG_IGN); //ignoring the broken pipe signal

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
                sprintf(debug,"%s","Starting rf24hub as daemon..."); logger.logmsg(VERBOSESTARTUP,debug);
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
    if ( cfg.hubUdpPortSet ) {
		openSocket(NULL, cfg.hubUdpPort.c_str(),&udp_address,&udp_sockfd,UDP);
	}
	if ( cfg.hubTcpPortSet ) {
        openSocket(NULL, cfg.hubTcpPort.c_str(),&tcp_address,&tcp_sockfd,TCP);
	}
    sleep(2);
    node.begin(&logger);
    sensor.begin(&logger);
    order.begin(&logger);
    orderbuffer.begin(&logger);
    database.begin(&logger);
    if (database.connect(cfg.dbHostName, cfg.dbUserName, cfg.dbPassWord, cfg.dbSchema, cfg.dbPort)) {
//	printf("#3-OK\n");
    } else {
//	printf("#3-ERROR\n");
    }
    init_system();
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
		if ( cfg.hubTcpPortSet ) {
            new_tn_in_socket = accept ( tcp_sockfd, (struct sockaddr *) &tcp_address, &tcp_addrlen );
            if (new_tn_in_socket > 0) {
                //receive_tn_in(new_tn_in_socket, &address);
                thread t2(receiveTelnetMessage, new_tn_in_socket, &tcp_address);
                t2.detach();
                //close (new_tn_in_socket);
            }
        }
        if ( cfg.hubUdpPortSet ) {
            numbytes = recvfrom(udp_sockfd, &udp_r_data, sizeof(udp_r_data), 0, (struct sockaddr *)&clientaddress,  &clientaddress_len);
            if (numbytes >0) {
                printf("listener: packet is %d bytes long\n", numbytes);
                printf("listener: packet received \n");
                printf("Network_number: %u \n",udp_r_data.network_id);
                printf("Node_number: %u \n",udp_r_data.node_id);
                printf("Msg_number: %u \n",udp_r_data.msg_id);
                printf("Sensor1_id: %u \n",getChannel(udp_r_data.sensor1));
                printf("Sensor1_value: %f \n",getValue_f(udp_r_data.sensor1));
                process_sensor(udp_r_data.node_id, udp_r_data.sensor1);
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
				// temporär Daten an den GW zurückschicken
				sendUdpMessage( ipAddrStr, cfg.gwUdpPort.c_str(), &udp_s_data); 
            }
        }
	} // while(1)
	return 0;
    free(debug);
}

