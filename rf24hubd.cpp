#include "rf24hubd.h" 

/*******************************************************************************************
*
* Telnethandling
* Used for communication with FHEM
*
********************************************************************************************/
// do_tn_cmd ==> send a telnet comand to the fhem-host
// usage example: do_tn_cmd("set device1 on");
void do_tn_cmd(uint16_t node, uint8_t channel, float value) {
    char *debug =  (char*) malloc (DEBUGSTRINGSIZE);
    char *tn_cmd =  (char*) malloc (TELNETBUFFERSIZE);
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char fhem_dev[] = {'\0'};
    if ( logger.verboselevel & VERBOSETELNET) {    
        sprintf(debug,"do_tn_cmd: Node: %u Channel: %u Value %f", node, channel, value);
        logger.logmsg(VERBOSEORDER,debug);
    }
    sensor.find_fhem_dev(&node, &channel, fhem_dev); 
	sprintf(tn_cmd,"set %s %f\n", fhem_dev, value);
    if ( logger.verboselevel & VERBOSETELNET) {    
        sprintf(debug,"do_tn_cmd: %s", tn_cmd);
        logger.logmsg(VERBOSETELNET,debug);
    }
    portno = std::stoi(cfg.rf24HubTcpPort);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        sprintf(debug,"do_tn_cmd: error opening socket");
		logger.logmsg(VERBOSECRITICAL,debug);
	}	
    server = gethostbyname(cfg.fhemHostName.c_str());
    if (server == NULL) {
        sprintf(debug,"do_tn_cmd: no such host");
		logger.logmsg(VERBOSECRITICAL,debug);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) { 
        sprintf(debug,"do_tn_cmd: error connecting");
		logger.logmsg(VERBOSECRITICAL,debug);
	} else {	
		n = write(sockfd,tn_cmd,strlen(tn_cmd));
		if (n < 0) {
			sprintf(debug,"do_tn_cmd: error writing to socket");
			logger.logmsg(VERBOSECRITICAL,debug);
		} else {
            if ( logger.verboselevel & VERBOSETELNET) {    
                sprintf(debug,"do_tn_cmd: Telnet to %s Port %d CMD: %s successfull",cfg.fhemHostName.c_str(), portno, tn_cmd);
                logger.logmsg(VERBOSETELNET,debug);
            }
		}		
	}		 
    close(sockfd);
    free(tn_cmd);
    free(debug);
}

void receive_tn_in(int new_tn_in_socket, struct sockaddr_in * address) {
    char *buffer =  (char*) malloc (TELNETBUFFERSIZE);
    char *debug =  (char*) malloc (DEBUGSTRINGSIZE);
    char *client_message =  (char*) malloc (TELNETBUFFERSIZE);
    ssize_t MsgLen;
    // send something like a prompt. perl telnet is waiting for it otherwise we get error
    // use this in perl: my $t = new Net::Telnet (Timeout => 2, Port => 7001, Prompt => '/rf24hub>/');
    sprintf(client_message,"rf24hub> ");
    write(new_tn_in_socket , client_message , strlen(client_message));
    if ( logger.verboselevel & VERBOSETELNET) {    
        sprintf (debug,"Client %s ist connected ...", inet_ntoa (address->sin_addr));
        logger.logmsg(VERBOSETELNET, debug);
    }
    sprintf(buffer,"                                 ");
    MsgLen = recv(new_tn_in_socket, buffer, TELNETBUFFERSIZE, 0);
    if (MsgLen>0) {
        process_tn_in(new_tn_in_socket, buffer, client_message);
    } else {
        if ( logger.verboselevel & VERBOSETELNET) {    
            sprintf (debug,"Nicht verarbeitete telnet message: %s MsgLen: %d ", trim(buffer), MsgLen);
            logger.logmsg(VERBOSETELNET, debug);
        }
    }
    close (new_tn_in_socket);
    free(buffer);
    free(client_message);
    free(debug);
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
    if ( logger.verboselevel & VERBOSETELNET) {    
        sprintf(debug,"Incoming telnet data: %s ",buffer);
        logger.logmsg(VERBOSETELNET, debug);
    }
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
//        if ( order.has_order(mynode) ) {
// TODO             order.
//        }
		if ( strcmp(wort1,cmp_setlast) == 0 && ! node.is_HB_node(mynode) ) {
			make_order(mynode, MSGTYPORQ1);
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
//		init_node(getnodeadr(wort3));
//	}
    // set verbose <new verboselevel>
	// sets the new verboselevel
	if (( strcmp(wort1,cmp_set) == 0 ) && (strcmp(wort2,cmp_verbose) == 0) && (strlen(wort3) > 0) && (strlen(wort4) == 0) ) {
//        if ( wort3[0] > '0' && wort3[0] < '9' + 1 ) {
		tn_input_ok = true;
        if ( ! decodeVerbose(&logger.verboselevel, wort3) ) {
    		sprintf(client_message,"Verbose usage:\n");
            write(new_tn_in_socket , client_message , strlen(client_message));
    		sprintf(client_message,"<+/-><item>: Puts <on/off> <item> related Messages\n");
            write(new_tn_in_socket , client_message , strlen(client_message));
    		sprintf(client_message,"Example \"+rf24\": Puts on rf24 related Messages\n");
            write(new_tn_in_socket , client_message , strlen(client_message));
    		sprintf(client_message,"Posible settings:\n");
            write(new_tn_in_socket , client_message , strlen(client_message));
    		sprintf(client_message,"rf24: RF24 related Messages\n");
            write(new_tn_in_socket , client_message , strlen(client_message));
    		sprintf(client_message,"telnet: Telnet related Messages\n");
            write(new_tn_in_socket , client_message , strlen(client_message));
    		sprintf(client_message,"order: Order related Messages\n");
            write(new_tn_in_socket , client_message , strlen(client_message));
    		sprintf(client_message,"obuffer: OrderBuffer related Messages\n");
            write(new_tn_in_socket , client_message , strlen(client_message));
    		sprintf(client_message,"ocont: Prints the content of the present stack of ORDER\n");
            write(new_tn_in_socket , client_message , strlen(client_message));
    		sprintf(client_message,"obcont: Prints the content of the present stack of ORDERBUFFER\n");
            write(new_tn_in_socket , client_message , strlen(client_message));
    		sprintf(client_message,"sql: Prints executed SQL statements\n");
            write(new_tn_in_socket , client_message , strlen(client_message));
        }
//            logger.verboselevel = verboselevel;
//		}	
	}
    // list verbose level currently set
	if (( strcmp(wort1,cmp_list) == 0 ) && (strcmp(wort2,cmp_verbose) == 0) && (strlen(wort3) == 0) && (strlen(wort4) == 0) ) {
		tn_input_ok = true;
		sprintf(client_message,"Verbose level is now:\n");
		write(new_tn_in_socket , client_message , strlen(client_message));
		if (logger.verboselevel & VERBOSECRITICAL) {
			sprintf(client_message,"CRITICAL ");
			write(new_tn_in_socket , client_message , strlen(client_message));
		}
		if (logger.verboselevel & VERBOSESTARTUP) {
			sprintf(client_message,"STARTUP ");
			write(new_tn_in_socket , client_message , strlen(client_message));
		}
		if (logger.verboselevel & VERBOSECONFIG) {
			sprintf(client_message,"CONFIG ");
			write(new_tn_in_socket , client_message , strlen(client_message));
		}
		if (logger.verboselevel & VERBOSEORDER) {
			sprintf(client_message,"order ");
			write(new_tn_in_socket , client_message , strlen(client_message));
		}
		if (logger.verboselevel & VERBOSEOBUFFER) {
			sprintf(client_message,"obuffer ");
			write(new_tn_in_socket , client_message , strlen(client_message));
		}
		if (logger.verboselevel & VERBOSETELNET) {
			sprintf(client_message,"telnet ");
			write(new_tn_in_socket , client_message , strlen(client_message));
		}
		if (logger.verboselevel & VERBOSESQL) {
			sprintf(client_message,"sql ");
			write(new_tn_in_socket , client_message , strlen(client_message));
		}
		if (logger.verboselevel & VERBOSERF24) {
			sprintf(client_message,"rf24 ");
			write(new_tn_in_socket , client_message , strlen(client_message));
		}
		if (logger.verboselevel & VERBOSEOTHER) {
			sprintf(client_message,"other ");
			write(new_tn_in_socket , client_message , strlen(client_message));
		}
		if (logger.verboselevel & VERBOSECONTENTORDER) {
			sprintf(client_message,"ocont ");
			write(new_tn_in_socket , client_message , strlen(client_message));
		}
		if (logger.verboselevel & VERBOSECONTENTOBUFFER) {
			sprintf(client_message,"obcont ");
			write(new_tn_in_socket , client_message , strlen(client_message));
		}
        uint16_t tmpverb=0b1000000000000000;
		sprintf(client_message,"Value: 0b");
		while (tmpverb > 0) {
			if (tmpverb & logger.verboselevel) {
				strncat(client_message,"1",2);
			} else {
				strncat(client_message,"0",2);
			}
			tmpverb >>= 1;
		}
        strncat(client_message,"\n",2);
		write(new_tn_in_socket , client_message , strlen(client_message));
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
        exit_system();
        node.cleanup();
        sensor.cleanup();
		init_system();
        node.print_buffer2tn(new_tn_in_socket);
        sensor.print_buffer2tn(new_tn_in_socket);
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
		sprintf(client_message,"list verbose\n");
		write(new_tn_in_socket , client_message , strlen(client_message));
		sprintf(client_message,"   lists the present settings of verboselevel\n");
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
	
/*******************************************************************************************
*
* END Telnethandling
*
********************************************************************************************/
/*******************************************************************************************
*
* Nodehandling 
* Used for communication with the nodes
*
********************************************************************************************/

void make_order(uint16_t mynode, uint8_t mytype) {
    uint8_t channel; 
    float value;
    void* ret_ptr;
    uint32_t data;
    order.del_node(mynode);
    ret_ptr = orderbuffer.find_order4node(mynode, NULL, &channel, &value);
    data = calcTransportValue_f(channel, value);
    if (ret_ptr) {
        order.add_order(mynode, mytype, node.is_HB_node(mynode), data, mymillis());
        ret_ptr = orderbuffer.find_order4node(mynode, ret_ptr, &channel, &value);
        data = calcTransportValue_f(channel, value);
        if (ret_ptr) {
            order.modify_order(mynode, 2, data);
            ret_ptr = orderbuffer.find_order4node(mynode, ret_ptr, &channel, &value);
            data = calcTransportValue_f(channel, value);
            if (ret_ptr) {
                order.modify_order(mynode, 3, data);
                ret_ptr = orderbuffer.find_order4node(mynode, ret_ptr, &channel, &value);
                data = calcTransportValue_f(channel, value);
                if (ret_ptr) {
                    order.modify_order(mynode, 4, data);
                    ret_ptr = orderbuffer.find_order4node(mynode, ret_ptr, &channel, &value);
                    data = calcTransportValue_f(channel, value);
                    if (ret_ptr) {
                        order.modify_order(mynode, 5, data);
                        ret_ptr = orderbuffer.find_order4node(mynode, ret_ptr, &channel, &value);
                        data = calcTransportValue_f(channel, value);
                        if (ret_ptr) {
                            order.modify_order(mynode, 4, data);
                            ret_ptr = orderbuffer.find_order4node(mynode, ret_ptr, &channel, &value);
                            if (ret_ptr) {
                                order.modify_orderflags(mynode, FLAG_EMPTY );
                            } else {
                                order.modify_orderflags(mynode, FLAG_LASTMESSAGE );
                            }                        
                        } else {
                            order.modify_orderflags(mynode, FLAG_LASTMESSAGE );
                        }                        
                    } else {
                        order.modify_orderflags(mynode, FLAG_LASTMESSAGE );
                    }                        
                } else {
                    order.modify_orderflags(mynode, FLAG_LASTMESSAGE );
                }                        
            } else {
                order.modify_orderflags(mynode, FLAG_LASTMESSAGE );
            }                        
        } else {
            order.modify_orderflags(mynode, FLAG_LASTMESSAGE );
        }                        
    }
}

/*******************************************************************************************
*
* END Nodehandling 
*
********************************************************************************************/

void exit_system(void) {
    // Save data from sensordata_im and sensor_im to persistant tables
    database.exitSystem();
}

void init_system(void) {
    database.initSystem();
    database.initSensor(&sensor);
    database.initNode(&node);
    node.debug_print_buffer(VERBOSECONFIG);
    sensor.debug_print_buffer(VERBOSECONFIG);
}

void store_sensor_value(uint16_t mynode, uint8_t mychannel, float myvalue) {
    uint32_t mysensor = sensor.getSensor(mynode, mychannel);
    if ( sensor.update_last_val(mysensor, myvalue, mymillis() )) {    
        database.storeSensorValue(mysensor, myvalue);
        if ( tn_active ) { 
            do_tn_cmd(mynode, mychannel, myvalue); 
        }
    }
}

void process_sensor(uint16_t node, uint32_t mydata) {
    char *debug =  (char*) malloc (DEBUGSTRINGSIZE);                  
    uint8_t channel = getChannel(mydata);
    float value = getValue_f(mydata);
	switch (channel) {
		case 1 ... 99: {
		// Sensor or Actor
            if ( logger.verboselevel & VERBOSECONFIG) {    
                sprintf(debug, "Value of Channel: %u on Node: %u is %f ", channel, node, value);
                logger.logmsg(VERBOSECONFIG, debug);
            }
			store_sensor_value(node, channel, value);
		}
		break; 
		case 101: {
		// battery voltage
            if ( logger.verboselevel & VERBOSECONFIG) {    
                sprintf(debug, "Voltage of Node: %u is %f ", node, value);
                logger.logmsg(VERBOSECONFIG, debug); 
            }
// TODO
//			do_sql(sql_stmt);
			store_sensor_value(node, channel, value);
		}
		break; 
		case 102 ... 127: { // System settings
            if ( logger.verboselevel & VERBOSECONFIG) {    
                sprintf(debug, "Node: %u Channel: %u is set to %f ", node, channel, value);
                logger.logmsg(VERBOSECONFIG, debug);
            }
            database.storeNodeConfig(node, channel, value);
		}	
		break; 
		default: { 
            if ( logger.verboselevel & VERBOSECONFIG) {    
                sprintf(debug, "Message dropped!!!! Node: %u Channel: %u Value: %f ", node, channel, value);
                logger.logmsg(VERBOSECONFIG, debug);   
            }
		}
	}	
	orderbuffer.del_node_channel(node, channel);
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
//	msgctl(msqid, IPC_RMID, NULL);
    free(debug);
    exit (0);
}


void channelscanner (uint8_t channel) {
  int values=0;
  printf("Scanning channel %u: \n",channel);
  radio.begin();
  for (int i=0; i < 1000; i++) {
    radio.setChannel(channel);

    // Listen for a little
    radio.startListening();
    usleep(1000);

    // Did we get a carrier?
    if ( radio.testCarrier() ){
      values++;
      printf("X");
    } else {
      printf(".");
    }
    radio.stopListening();

  }
  printf("\n\n 1000 passes: Detect %d times a carrier \n", values);
}

void scanner(char scanlevel) {
  // we have channel 0...125 => 126 channels
  const uint8_t num_channels = 126;
  uint8_t values[num_channels];
  int num_reps;
  int wait;

  radio.begin();
  switch (scanlevel) {
    case '0':
       num_reps=1;
       wait=100;
    break;
    case '1':
       num_reps=5;
       wait=100;
    break;
    case '2':
       num_reps=10;
       wait=200;
    break;
    case '3':
       num_reps=20;
       wait=200;
    break;
    case '4':
       num_reps=30;
       wait=200;
    break;
    case '5':
       num_reps=30;
       wait=500;
    break;
    case '6':
       num_reps=50;
       wait=500;
    break;
    case '7':
       num_reps=100;
       wait=500;
    break;
    case '8':
       num_reps=200;
       wait=500;
    break;
    case '9':
       num_reps=500;
       wait=1000;
    break;
    default:
       num_reps=30;
       wait=500;
  }
  for (uint8_t i = 0; i < num_channels; i++) {
    values[i]=0;
  }
  printf("Scanning all channels %d passes, listen %d microseconds to each channel\n", num_reps, wait);
  printf("\t\t000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000011111111111111111111111111\n");
  printf("\t\t000000000011111111112222222222333333333344444444445555555555666666666677777777778888888888999999999900000000001111111111222222\n");
  printf("\t\t012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345\n");
  printf("\t\t------------------------------------------------------------------------------------------------------------------------------\n");
  for (int rep_counter = 0; rep_counter < num_reps; rep_counter++) {
    printf("\nPass: %d/%d \t", rep_counter+1, num_reps);
    for (uint8_t i = 0; i < num_channels; i++) {

      // Select this channel
      radio.setChannel(i);
 
      // Listen for a little
      radio.startListening();
      usleep(wait);
      
      // Did we get a carrier?
      if ( radio.testCarrier() ){
        values[i]++;
        printf("X");
      } else {
        printf(".");
      }
      radio.stopListening();
    }
  }
  printf("\n\n");
  printf("\t\t000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000011111111111111111111111111\n");
  printf("\t\t000000000011111111112222222222333333333344444444445555555555666666666677777777778888888888999999999900000000001111111111222222\n");
  printf("\t\t012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345\n"); 
  printf("\t\t------------------------------------------------------------------------------------------------------------------------------\n");
  printf("\t\t");
  for (uint8_t i = 0; i < num_channels; i++) {
    if (values[i] == 0) {
      printf(".");
    } else {
      if (values[i] > 9 ) {
        printf("X");
      } else {
        printf("%u",values[i]);
      }
    }
  }
  printf("\n"); 
}

void debug_print_payload(uint16_t loglevel, const char* msg_header, const char* result, payload_t* mypayload) {
	if ( logger.verboselevel & loglevel  ) {
        char *debug =  (char*) malloc (DEBUGSTRINGSIZE);
        sprintf(debug, "%s: N:%hhu T:%hhu F:%02x O:%hhu (%hhu/%g) (%hhu/%g) (%hhu/%g) (%hhu/%g) (%hhu/%g) (%hhu/%g) %s"
            ,msg_header
            ,mypayload->node_id, mypayload->type, mypayload->flags, mypayload->orderno
            ,getChannel(mypayload->data1), getValue_f(mypayload->data1)
            ,getChannel(mypayload->data2), getValue_f(mypayload->data2)
            ,getChannel(mypayload->data3), getValue_f(mypayload->data3)
            ,getChannel(mypayload->data4), getValue_f(mypayload->data4)
            ,getChannel(mypayload->data5), getValue_f(mypayload->data5)
            ,getChannel(mypayload->data6), getValue_f(mypayload->data6)  
            ,result );
        logger.logmsg(loglevel, debug);
        free(debug);
    }
}

void process_payload(payload_t* mypayload) {
	if ( mypayload->data1 > 0 ) process_sensor(mypayload->node_id, mypayload->data1);
	if ( mypayload->data2 > 0 ) process_sensor(mypayload->node_id, mypayload->data2);
	if ( mypayload->data3 > 0 ) process_sensor(mypayload->node_id, mypayload->data3);
	if ( mypayload->data4 > 0 ) process_sensor(mypayload->node_id, mypayload->data4);
	if ( mypayload->data5 > 0 ) process_sensor(mypayload->node_id, mypayload->data5);
	if ( mypayload->data6 > 0 ) process_sensor(mypayload->node_id, mypayload->data6);
}

void do_transmit(uint8_t address[5], payload_t* payload) {
	radio.stopListening();
	radio.openWritingPipe(address);
	if (radio.write(payload,sizeof(payload_t))) {
		radio.startListening();
		debug_print_payload(VERBOSERF24, "Snd", "OK", payload);
	} else {
		radio.startListening();
		debug_print_payload(VERBOSERF24, "Snd", "Fail", payload);
	}
}

int main(int argc, char* argv[]) {
    pid_t pid;
    payload_t payload;
    uint8_t pipe_num;
	logger.set_logmode('i');
    char *debug =  (char*) malloc (DEBUGSTRINGSIZE);
    const int y = 1;
	long save_fd;
	/* vars for telnet socket handling */
	int tn_in_socket=0;
	int new_tn_in_socket=0;
	socklen_t addrlen;
//	char *buffer =  (char*) malloc (BUF);
	struct sockaddr_in address;
//	bool wait4message = false;
	
    // processing argc and argv[]
    cfg.processParams(argc, argv);
	logger.set_logmode('i');
	if ( ! logger.set_logfile(cfg.logFileName) ) {
		cout << "Error opening Logfile: " << cfg.logFileName << endl;
		exit(1);
	}
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


    // starts logging
    logger.verboselevel = VERBOSECRITICAL | VERBOSESTARTUP | VERBOSECONFIG | VERBOSERF24;
    node.begin(&logger);
    sensor.begin(&logger);
    order.begin(&logger);
    orderbuffer.begin(&logger);
    database.begin(&logger);
    // open database
    database.connect(cfg.dbHostName, cfg.dbUserName, cfg.dbPassWord, cfg.dbSchema, cfg.dbPort);
    // init SIGTERM and SIGINT handling
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
	if ( cfg.rf24HubTcpPortSet ) {
    /* open incoming port for messages */
		if ((tn_in_socket=socket( AF_INET, SOCK_STREAM, 0)) > 0) {
			sprintf (debug,"Socket für eingehende Messages auf Port %i angelegt", stoi(cfg.rf24HubTcpPort) );
			logger.logmsg(VERBOSESTARTUP, debug);
		}
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
		address.sin_port = htons (stoi(cfg.rf24HubTcpPort));
		setsockopt( tn_in_socket, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(int) );
		if (bind( tn_in_socket, (struct sockaddr *) &address, sizeof (address)) == 0 ) {
			sprintf (debug,"Binding Socket OK");
			logger.logmsg(VERBOSESTARTUP, debug);
		}
		listen (tn_in_socket, 5);
		addrlen = sizeof (struct sockaddr_in);
		save_fd = fcntl( tn_in_socket, F_GETFL );
		save_fd |= O_NONBLOCK;
		fcntl( tn_in_socket, F_SETFL, save_fd );
	}
    sleep(2);
    uint8_t myPALevel=RF24_PA_MIN;
    rf24_datarate_e myDataRate=RF24_250KBPS;
    sprintf(debug, "starting radio on channel ... %d ", cfg.rf24Channel);
    logger.logmsg(VERBOSESTARTUP, debug);
    radio.begin();
    radio.setChannel( cfg.rf24Channel );
    radio.setAutoAck( true );
//    radio.setPALevel( RF24_PA_LOW ) ;
    radio.enableDynamicPayloads();
    if ( strcmp(cfg.rf24Speed.c_str(), "RF24_250KBPS") == 0 ) {
        myDataRate = RF24_250KBPS;
    }
    if ( strcmp(cfg.rf24Speed.c_str(), "RF24_1MBPS") == 0 ) {
        myDataRate = RF24_1MBPS;
    }
    if ( strcmp(cfg.rf24Speed.c_str(), "RF24_2MBPS") == 0 ) {
        myDataRate = RF24_2MBPS;
    }
    sprintf(debug, "setting radio speed to %s ", cfg.rf24Speed.c_str() );
    logger.logmsg(VERBOSESTARTUP, debug);
    radio.setDataRate(myDataRate);
    
    if ( strcmp(cfg.rf24PALevel.c_str(), "RF24_PA_MIN") == 0 ) {
        myPALevel = RF24_PA_MIN;
    }
    if ( strcmp(cfg.rf24PALevel.c_str(), "RF24_PA_LOW") == 0 ) {
        myPALevel = RF24_PA_LOW;
    }
    if ( strcmp(cfg.rf24PALevel.c_str(), "RF24_PA_HIGH") == 0 ) {
        myPALevel = RF24_PA_HIGH;
    }
    if ( strcmp(cfg.rf24PALevel.c_str(), "RF24_PA_MAX") == 0 ) {
        myPALevel = RF24_PA_MAX;
    }
    sprintf(debug, "setting radio PA level to %s ", cfg.rf24PALevel.c_str() );
    logger.logmsg(VERBOSESTARTUP, debug);
    radio.setPALevel( myPALevel ) ;
    
	radio.setRetries(15,15);
    radio.openWritingPipe(cfg.rf24TXAddress);
    radio.openReadingPipe(1,cfg.rf24RXAddress1);
    radio.startListening();
    
    radio.printDetails();
    sprintf(debug,"\%s up and running .... ", cfg.prgName.c_str());
    logger.logmsg(VERBOSESTARTUP, debug);
	
	// Init Arrays
    init_system();
    
	// Main Loop
    while(1) {
        /* Handling of incoming messages */
		if ( cfg.rf24HubTcpPortSet ) {
            new_tn_in_socket = accept ( tn_in_socket, (struct sockaddr *) &address, &addrlen );
            if (new_tn_in_socket > 0) {
                thread t2(receive_tn_in, new_tn_in_socket, &address);
                t2.detach();
            }
		}
//		network.update();
		if ( radio.isValid() && radio.available(&pipe_num) ) {
//
// Receive loop: react on the message from the nodes
//
//            rf24_carrier = radio.testCarrier();
//			rf24_rpd = radio.testRPD();
//            network.peek(rxheader);
//            
			radio.read(&payload,sizeof(payload));
			debug_print_payload(VERBOSERF24, "Rec", " ", &payload);
			switch ( payload.type ) {
				case MSGTYPHB1: { // heartbeat typ 1!!
					if (node.is_new_HB(payload.node_id, mymillis())) {  // Got a new Heaqrtbeat -> process it!
						process_payload(&payload);
						if ( orderbuffer.node_has_entry(payload.node_id) ) {  // WE have orders for this node
							make_order(payload.node_id, MSGTYPORQ1);                    
							if ( logger.verboselevel & VERBOSEORDER ) {
								sprintf(debug, "Entries for Heartbeat Node found, sending them");
								logger.logmsg(VERBOSEORDER, debug);
							}
						} else {
							if ( logger.verboselevel & VERBOSEORDER) {
								sprintf(debug, "No Entries for Heartbeat Node found, sending Endmessage");
								logger.logmsg(VERBOSEORDER, debug);
							}
							payload.data1=0;payload.data2=0;payload.data3=0;payload.data4=0;payload.data5=0;payload.data6=0;
							payload.type=MSGTYPEND; payload.orderno=0; payload.flags=FLAG_LASTMESSAGE;
							do_transmit(cfg.rf24TXAddress, &payload);
						}
					}
				}
				break;    
                case MSGTYPORP1: { // Response auf einen "normalen" Orderrequest ==> Nur Verarbeitung, keine Antwort
                    if ( logger.verboselevel & VERBOSEORDER) {
                        sprintf(debug, "Processing N: %hhu T: %hhu F: %hhu O: %hhu"
                                    , payload.node_id, payload.type, payload.flags, payload.orderno);
									logger.logmsg(VERBOSEORDER, debug);
                    }
                    if ( order.is_orderno(payload.orderno) ) {
                        process_payload(&payload);
                        // Order compleate => delete it!
                        order.del_orderno(payload.orderno);                        
                        // Check if we still have orders for this node
                        make_order(payload.node_id, MSGTYPORQ1);
                    }
                    
                }
                break;
                default: {	
                    if ( logger.verboselevel & VERBOSEORDER) {
                        sprintf(debug, "<per Default> Processing N: %hhu T: %hhu F: %hhu O: %hhu"
                                    , payload.node_id, payload.type, payload.flags, payload.orderno);
									logger.logmsg(VERBOSEORDER, debug);
                    }
                    if ( order.is_orderno(payload.orderno) ) {
//                        process_payload(&payload);
                        // Order compleate => delete it!
                        order.del_orderno(payload.orderno);
//						do_transmit(address2, &payload);
                        // Check if we still have orders for this node
//                        make_order(payload.node_id, MSGTYPORQ1);
                    } 
                }
			}
			
		} // radio.available
//
// Orderloop: Tell the nodes what they have to do
//
		if ( order.has_order ) {  // go transmitting if its time to do ..
			// Look if we have something to send
			while ( order.get_order_for_transmission(&payload.orderno, &payload.node_id, &payload.type, &payload.flags,
                &payload.data1, &payload.data2, &payload.data3, &payload.data4, 
                &payload.data5, &payload.data6, mymillis() )) {
                    do_transmit(cfg.rf24TXAddress, &payload);
                }
				usleep(50000);
        } else {
            usleep(200000);
        }
//
//  end orderloop
//
	} // while(1)
	return 0;
    free(debug);
}

