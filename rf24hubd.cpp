#include "rf24hubd.h" 

/*******************************************************************************************
*
* Telnethandling
* Used for communication with FHEM
*
********************************************************************************************/

// do_tn_cmd ==> send a telnet comand to the fhem-host
// usage example: do_tn_cmd("set device1 on");
void do_tn_cmd(NODE_DATTYPE node_id, uint8_t channel, float value) {
    char* tn_cmd = alloc_str(verboselevel,"do_tn_cmd tn_cmd",TELNETBUFFERSIZE);
    char tn_quit[] =  "\r\nquit\r\n";
    char* buf = alloc_str(verboselevel,"do_tn_cmd buf",TSBUFFERSIZE);
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char* fhem_dev;
    if ( verboselevel & VERBOSETELNET) {    
        cout << ts(buf) << "do_tn_cmd: Node: " << (int)node_id << " Channel: " << (int)channel << " Value: " << value << endl;
    }
    fhem_dev = sensor.getFhemDevByNodeChannel(node_id, channel); 
	sprintf(tn_cmd,"set %s %f", fhem_dev, value);
    if ( verboselevel & VERBOSETELNET) {    
        cout << ts(buf) << "do_tn_cmd: " << tn_cmd << endl;
    }
    portno = std::stoi(cfg.fhemPort);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        cout << ts(buf) << "ERROR: do_tn_cmd: error opening socket" << endl;
	}	
    server = gethostbyname(cfg.fhemHost.c_str());
    if (server == NULL) {
        cout << ts(buf) << "ERROR: do_tn_cmd: no such host" << endl;
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) { 
        cout << ts(buf) << "ERROR: do_tn_cmd: error connecting" << endl;
	} else {	
		n = write(sockfd,tn_cmd,strlen(tn_cmd));
		if (n < 0) {
			cout << ts(buf) << "ERROR: do_tn_cmd: error writing to socket" << endl;
		} else {
            if ( verboselevel & VERBOSETELNET) {    
                cout << ts(buf) << "do_tn_cmd: Telnet to " << cfg.fhemHost << " Port " << portno << " successfull Command: " <<  tn_cmd << endl;
            }
		}		
		write(sockfd,tn_quit,strlen(tn_quit));
	}		 
    free_str(verboselevel, "do_tn_cmd buf", buf);
    close(sockfd);
    free_str(verboselevel, "do_tn_cmd tn_cmd", tn_cmd);
}

void receive_tn_in(int tnsocket, struct sockaddr_in * address) {
    char* buffer = alloc_str(verboselevel,"receive_tn_in buffer",TELNETBUFFERSIZE);
    char* client_message = alloc_str(verboselevel,"receive_tn_in client_message",DEBUGSTRINGSIZE);
    ssize_t MsgLen;
    TnMsg_t TnMsg;
    int ret;
    // send something like a prompt. perl telnet is waiting for it otherwise we get error
    // use this in perl: my $t = new Net::Telnet (Timeout => 2, Port => 7001, Prompt => '/rf24hub>/');
    sprintf(client_message,"rf24hub> ");
    write(tnsocket , client_message , strlen(client_message));
    MsgLen = recv(tnsocket, buffer, TELNETBUFFERSIZE, 0);
    if (MsgLen>0) {
        int msgID = msgget(MSGKEY, IPC_CREAT | 0600);
        TnMsg.mtype = 1;
        TnMsg.tn_socket = 0;
        sprintf(TnMsg.tntext,"Incoming telnet data from %s:",inet_ntoa(address->sin_addr));
        if (msgID >= 0) msgsnd(msgID, &TnMsg, sizeof(TnMsg), 0);
        TnMsg.mtype = 2;
        TnMsg.tn_socket = tnsocket;
        sprintf(TnMsg.tntext,"%s", trim(buffer));
        if (msgID >= 0) msgsnd(msgID, &TnMsg, sizeof(TnMsg), 0);
            
        // Wait for a message with processing result            
        ret=msgrcv(msgID, &TnMsg, sizeof(TnMsg), 9, 0);
        if ( ret > 5 ) {
            write(tnsocket , TnMsg.tntext , strlen(TnMsg.tntext));
        }
        
    }
    sprintf(buffer,"%s %s\n",PRGNAME,PRGVERSION);
    write(tnsocket , buffer , strlen(buffer));
    close (tnsocket);
    free_str(verboselevel,"receive_tn_in buffer",buffer);
    free_str(verboselevel,"receive_tn_in client_message",client_message);
}
	
bool process_tn_in( char* inbuffer, int tn_socket) {
/* Messages can look like this:
       <word1		word2		word3		word4 				function>
		init													Reinitialization of rf24hubd (reads actual values from database)
		show		order										Lists open orders in textform
                    sensor                                      Lists sensors and nodes
                    radio       config                          Prints radio config to log
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
		 cmp_html[]="html",
		 cmp_order[]="order",	 
		 cmp_verbose[]="verbose",	
         cmp_push[]="push",
         cmp_show[]="show",
         cmp_radio[]="radio",
         cmp_config[]="config";
	char *wort1a, *wort2a, *wort3a, *wort4a;
	char *wort1, *wort2, *wort3, *wort4;
    char* buf = alloc_str(verboselevel,"process_tn_in buf",120);
    char* message = alloc_str(verboselevel,"process_tn_in message",120);
	bool tn_input_ok = false;
	char delimiter[] = " ";
	//trim(buffer);
	wort1a = strtok(inbuffer, delimiter);
	wort2a = strtok(NULL, delimiter);
	wort3a = strtok(NULL, delimiter);
	wort4a = strtok(NULL, delimiter);
    char* pEnd;
    if (wort1a) { 
        wort1 = alloc_str(verboselevel,"process_tn_in wort1",strlen(wort1a)+1);
        strcpy(wort1, wort1a);
    } else { 
        wort1 = alloc_str(verboselevel,"process_tn_in wort1",1);
        wort1[0] = '\0';
    }
    if (wort2a) { 
        wort2 = alloc_str(verboselevel,"process_tn_in wort2",strlen(wort2a)+1);
        strcpy(wort2, wort2a);
    } else { 
        wort2 = alloc_str(verboselevel,"process_tn_in wort2",1);
        wort2[0] = '\0';
    }
    if (wort3a) { 
        wort3 = alloc_str(verboselevel,"process_tn_in wort3",strlen(wort3a)+1);
        strcpy(wort3, wort3a);
    } else { 
        wort3 = alloc_str(verboselevel,"process_tn_in wort3",1);
        wort3[0] = '\0';
    }
    if (wort4a) { 
        wort4 = alloc_str(verboselevel,"process_tn_in wort4",strlen(wort4a)+1);
        strcpy(wort4, wort4a);
    } else { 
        wort4 = alloc_str(verboselevel,"process_tn_in wort4",1);
        wort4[0] = '\0';
    }
//printf("+++++Wort1..4: %s %s %s %s\n",wort1, wort2, wort3, wort4);
    // set/setlast sensor <sensor> <value>
	// sets a sensor to a value, setlast starts the request over air
	if ( (( strcmp(wort1,cmp_set) == 0 ) || ( strcmp(wort1,cmp_setlast) == 0 )) && (strcmp(wort2,cmp_sensor) == 0) && (strlen(wort3) > 0) && (strlen(wort4) > 0) ) {
		// In word3 we may have a) the number of the sensor b) the name of the sensor c) the fhem_dev of a sensor
		// for the processing we need the number of the sensor ==> find it!
        NODE_DATTYPE mynode = 0;
        uint8_t mychannel = 0;
        float   myvalue = strtof(wort4, &pEnd);
        if ( wort3[0] >= 'A' && wort3[0] <= 'z' ) {
            if (sensor.getNodeChannelByFhemDev(&mynode, &mychannel, wort3) ) {
                orderbuffer.addOrderBuffer(mymillis(),mynode,mychannel,myvalue);
          		tn_input_ok = true;
            }
        } else {
            uint32_t mysensor = strtol(wort3, &pEnd, 10);
            if ( sensor.getNodeChannelBySensorID(&mynode, &mychannel, mysensor ) ) {
                orderbuffer.addOrderBuffer(mymillis(),mynode,mychannel,myvalue);
                tn_input_ok = true;
            }
        }
		if ( tn_input_ok && strcmp(wort1,cmp_setlast) == 0 && ! node.isHBNode(mynode) ) {
			make_order(mynode, PAYLOAD_TYPE_DAT);
        } 
    }
	// push <node> <channel> <value>
	// Pushes a value direct to a channel into a node
	if (( strcmp(wort1,cmp_push) == 0 ) && (strlen(wort2) > 0) && (strlen(wort3) > 0) && (strlen(wort4) > 0) ) {
		tn_input_ok = true;
        NODE_DATTYPE mynode = strtol(wort2, &pEnd, 10);
        uint8_t mychannel = strtol(wort3, &pEnd, 10);
        float   myvalue = strtof(wort4, &pEnd);
        orderbuffer.addOrderBuffer(mymillis(),mynode,mychannel,myvalue);
        if ( ! node.isHBNode(mynode) ) {
			make_order(mynode, PAYLOAD_TYPE_DAT);
        }
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
			verboselevel = decodeVerbose(verboselevel, wort3);
            node.setVerbose(verboselevel);
            sensor.setVerbose(verboselevel);
            order.setVerbose(verboselevel);
            orderbuffer.setVerbose(verboselevel);
//		}	
	}
    // show order
	// lists the current orderbuffer
	if (( strcmp(wort1,cmp_show) == 0 ) && (strcmp(wort2,cmp_order) == 0) && (strlen(wort3) == 0) && (strlen(wort4) == 0) ) {
		tn_input_ok = true;
        orderbuffer.printBuffer2tn(tn_socket);
        order.printBuffer2tn(tn_socket);
	}	
    // show sensor
	// lists the current node- and sensorbuffer
	if (( strcmp(wort1,cmp_show) == 0 ) && (strcmp(wort2,cmp_sensor) == 0) && (strlen(wort3) == 0) && (strlen(wort4) == 0) ) {
		tn_input_ok = true;
        node.printBuffer2tn(tn_socket);
        sensor.printBuffer2tn(tn_socket);
	}	
    // show radio config
	if (( strcmp(wort1,cmp_show) == 0 ) && (strcmp(wort2,cmp_radio) == 0) && (strcmp(wort3,cmp_config) == 0) && (strlen(wort4) == 0) ) {
		tn_input_ok = true;
		radio.printDetails();
	}
    // show verbose
	if (( strcmp(wort1,cmp_show) == 0 ) && (strcmp(wort2,cmp_verbose) == 0) && (strlen(wort3) == 0) && (strlen(wort4) == 0) ) {
		tn_input_ok = true;
		sprintf(message,"Active verboselevel: %s\n",printVerbose(verboselevel, buf));
		write(tn_socket , message , strlen(message));
    }
	// html order
	// lists the current order/orderbuffer for html page
	if (( strcmp(wort1,cmp_html) == 0 ) && (strcmp(wort2,cmp_order) == 0) && (strlen(wort3) == 0) && (strlen(wort4) == 0) ) {
		tn_input_ok = true;
        orderbuffer.htmlBuffer2tn(tn_socket);
        order.htmlBuffer2tn(tn_socket);
    }	
    // init
	// initialisation of rf24hubd: reloads data from database
	if ( (strcmp(wort1,cmp_init) == 0) && (strlen(wort2) == 0) && (strlen(wort3) == 0) && (strlen(wort4) == 0) ) {
		tn_input_ok = true;
        exit_system();
        node.cleanup();
        sensor.cleanup();
		init_system();
        node.printBuffer2tn(tn_socket);
        sensor.printBuffer2tn(tn_socket);
	}
	if ( ! tn_input_ok) {
        //printf("%u \n",sizeof(tn_usage_txt)/ sizeof(int));
        for(unsigned int i=0; i<sizeof(tn_usage_txt)/ sizeof(int); i++) {
            sprintf(message,"%s\n",tn_usage_txt[i]);
            write(tn_socket , message , strlen(message));
        }
	}		
	free_str(verboselevel,"process_tn_in wort1",wort1);
    free_str(verboselevel,"process_tn_in wort2",wort2);
    free_str(verboselevel,"process_tn_in wort3",wort3);
    free_str(verboselevel,"process_tn_in wort4",wort4);
    free_str(verboselevel,"process_tn_in buf", buf);
    free_str(verboselevel,"process_tn_in message", message);
    return tn_input_ok;
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

void make_order(NODE_DATTYPE mynode, uint8_t mytype) {
    uint8_t channel; 
    float value;
    void* ret_ptr;
    uint32_t data;
    order.delByNode(mynode);
    ret_ptr = orderbuffer.findOrder4Node(mynode, NULL, &channel, &value);
    data = calcTransportValue_f(channel, value);
    if (ret_ptr) {
        order.addOrder(mynode, mytype, node.isHBNode(mynode), data, mymillis());
        ret_ptr = orderbuffer.findOrder4Node(mynode, ret_ptr, &channel, &value);
        data = calcTransportValue_f(channel, value);
        if (ret_ptr) {
            order.modifyOrder(mynode, 2, data);
            ret_ptr = orderbuffer.findOrder4Node(mynode, ret_ptr, &channel, &value);
            data = calcTransportValue_f(channel, value);
            if (ret_ptr) {
                order.modifyOrder(mynode, 3, data);
                ret_ptr = orderbuffer.findOrder4Node(mynode, ret_ptr, &channel, &value);
                data = calcTransportValue_f(channel, value);
                if (ret_ptr) {
                    order.modifyOrder(mynode, 4, data);
                    ret_ptr = orderbuffer.findOrder4Node(mynode, ret_ptr, &channel, &value);
                    data = calcTransportValue_f(channel, value);
                    if (ret_ptr) {
                        order.modifyOrder(mynode, 5, data);
                        ret_ptr = orderbuffer.findOrder4Node(mynode, ret_ptr, &channel, &value);
                        data = calcTransportValue_f(channel, value);
                        if (ret_ptr) {
                            order.modifyOrder(mynode, 4, data);
                            ret_ptr = orderbuffer.findOrder4Node(mynode, ret_ptr, &channel, &value);
                            if (ret_ptr) {
                                order.modifyOrderFlags(mynode, PAYLOAD_FLAG_EMPTY );
                            } else {
                                order.modifyOrderFlags(mynode, PAYLOAD_FLAG_LASTMESSAGE );
                            }                        
                        } else {
                            order.modifyOrderFlags(mynode, PAYLOAD_FLAG_LASTMESSAGE );
                        }                        
                    } else {
                        order.modifyOrderFlags(mynode, PAYLOAD_FLAG_LASTMESSAGE );
                    }                        
                } else {
                    order.modifyOrderFlags(mynode, PAYLOAD_FLAG_LASTMESSAGE );
                }                        
            } else {
                order.modifyOrderFlags(mynode, PAYLOAD_FLAG_LASTMESSAGE );
            }                        
        } else {
            order.modifyOrderFlags(mynode, PAYLOAD_FLAG_LASTMESSAGE );
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
    database.sync_sensordata();
    database.sync_sensor();
}

void init_system(void) {
    database.initSystem();
    database.initSensor(&sensor);
    database.initNode(&node);
    node.printBuffer();
    sensor.printBuffer();
}

void store_sensor_value(NODE_DATTYPE node_id, uint8_t channel, float value) {
    uint32_t sensor_id = sensor.updateLastVal(node_id, channel, value, mymillis() );
    if ( sensor_id > 0 ) {    
        database.storeSensorValue(sensor_id, value);
        if ( cfg.incomingPortSet ) { 
            do_tn_cmd(node_id, channel, value); 
        }
    }
}

void process_sensor(NODE_DATTYPE node_id, uint32_t mydata) {
    char* buf = alloc_str(verboselevel,"process_sensor buf",TSBUFFERSIZE);
    uint8_t channel = getChannel(mydata);
    float value = getValue_f(mydata);
	switch (channel) {
		case 1 ... 99: {
		// Sensor or Actor
            if ( verboselevel & VERBOSECONFIG) {    
                cout << ts(buf) << "Value of Node: " << (int)node_id << " Channel: " << (int)channel << " is " << value << endl;
            }
			store_sensor_value(node_id, channel, value);
		}
		break; 
		case 101: {
		// battery voltage
            if ( verboselevel & VERBOSECONFIG) {    
                cout << ts(buf) << "Voltage of Node: " << (int)node_id << " is " << value << "V" << endl;
            }
// TODO
//			do_sql(sql_stmt);
			store_sensor_value(node_id, channel, value);
            node.setVoltage(node_id, value);
		}
		break; 
		case 102 ... 127: { // System settings
            if ( verboselevel & VERBOSECONFIG) {    
                cout << ts(buf) << "Value of Node: " << (int)node_id << " Channel: " << (int)channel << " is " << value << endl;
            }
            database.storeNodeConfig(node_id, channel, value);
		}	
		break; 
		default: { 
            if ( verboselevel & VERBOSECONFIG) {    
                cout << ts(buf) << "Message dropped!!!! Node: " << (int)node_id << " Channel: " << (int)channel << " Value: " << value << endl;
            }
		}
	}	
	orderbuffer.delByNodeChannel(node_id, channel);
    free_str(verboselevel, "process_sensor buf", buf);
}	

/*******************************************************************************************
*
* All the rest 
*
********************************************************************************************/
void sighandler(int signal) {
    char* buf = alloc_str(verboselevel,"sighandler buf",TSBUFFERSIZE);
	cout << ts(buf) << "SIGTERM: Cleanup system ... saving *_im tables ..." << endl;
    exit_system(); 
	cout << ts(buf) << "SIGTERM: Shutting down ... " << endl;
    cfg.removePidFile();
//	msgctl(msqid, IPC_RMID, NULL);
    free_str(verboselevel, "sighandler buf", buf);
    exit (0);
}


void channelscanner (uint8_t channel) {
  int values=0;
  cout << "Scanning channel " << channel << ":" << endl;
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
  cout << endl << endl << "1000 passes: Detect " << values << " times a carrier" << endl;
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
	if ( verboselevel & loglevel  ) {        
        char* buf = alloc_str(verboselevel,"debug_print_payload buf",TSBUFFERSIZE);
        cout << ts(buf) << msg_header << ": N:" << (int)mypayload->node_id << " T:" <<  (int)mypayload->msg_type << " M:" <<
        (int)mypayload->msg_id << " F:" << hex << setfill('0') << setw(2) << (int)mypayload->msg_flags << dec << 
        " O:" << (int)mypayload->orderno << " " <<
        "(" << (int)getChannel(mypayload->data1) << "/" << getValue_f(mypayload->data1) << ")" <<
        "(" << (int)getChannel(mypayload->data2) << "/" << getValue_f(mypayload->data2) << ")" <<
        "(" << (int)getChannel(mypayload->data3) << "/" << getValue_f(mypayload->data3) << ")" <<
        "(" << (int)getChannel(mypayload->data4) << "/" << getValue_f(mypayload->data4) << ")" <<
        "(" << (int)getChannel(mypayload->data5) << "/" << getValue_f(mypayload->data5) << ")" <<
        "(" << (int)getChannel(mypayload->data6) << "/" << getValue_f(mypayload->data6) << ")" <<
        result << endl;   
        free_str(verboselevel, "debug_print_payload buf", buf);
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

int main(int argc, char* argv[]) {
    pid_t pid;
    payload_t payload;
    char* buf = alloc_str(verboselevel,"main buf",TSBUFFERSIZE);
	/* vars for telnet socket handling */
	int tn_in_socket = 0;
    int new_tn_in_socket = 0;
	socklen_t addrlen;
	struct sockaddr_in address;
	long save_fd;
	const int y = 1;
	int msgID;
    TnMsg_t LogMsg;
	
	// check if started as root
	if ( getuid()!=0 ) {
		cout << ts(buf) << PRGNAME << " has to be startet as user root" << endl; 
        exit(1);
    }
    
    // processing argc and argv[]
    cfg.processParams(argc, argv);

    // check for PID file, if exists terminate else create it
    if ( cfg.checkPidFileSet() ) {
         exit(1);
    } else {
         cfg.setPidFile();
    }
    cout << "--------------------------------------------------" << endl;
    cout << ts(buf) << "Startup Parameters:" << endl; 
    cfg.printConfig();

    // init SIGTERM and SIGINT handling
    signal(SIGTERM, sighandler);
    signal(SIGINT, sighandler);

    // run as daemon if started with -d
    if (cfg.startDaemon) {
        pid = fork ();
        if (pid == 0) {
        // Child prozess
            chdir ("/");
            umask (0);
        } else if (pid > 0) {
       // Parentprozess -> exit and return to shell
            cout << ts(buf) << "Starting " << PRGNAME << " as daemon..." << endl;
            // and exit
            exit(0);
        } else {
            // nagativ is an error
            cout << ts(buf) << "Fork ERROR ... exiting" << endl; 
            cfg.removePidFile();
            exit(1);
        }

        freopen(cfg.logFileName.c_str(), "a+", stdout); 
        freopen(cfg.logFileName.c_str(), "a+", stderr); 
    }
    
    // connect database
    database.connect(cfg.dbHostName, cfg.dbUserName, cfg.dbPassWord, cfg.dbSchema, std::stoi(cfg.dbPort));

    if ( cfg.fhemPortSet && cfg.fhemHostSet ) {
        cout << ts(buf) << "telnet session to FHEM started: Host: " << cfg.fhemHost << " Port: " << cfg.fhemPort << endl;
    }

    if ( cfg.incomingPortSet ) {
    /* open incoming port for messages */
		if ((tn_in_socket=socket( AF_INET, SOCK_STREAM, 0)) > 0) {
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = INADDR_ANY;
            address.sin_port = htons ( std::stoi ( cfg.incomingPort ) );
            setsockopt( tn_in_socket, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(int) );
            if (bind( tn_in_socket, (struct sockaddr *) &address, sizeof (address)) == 0 ) {
                cout << ts(buf) << "Socket für eingehende Messages auf Port " << cfg.incomingPort << " angelegt" << endl;
            }
            listen (tn_in_socket, 5);
            addrlen = sizeof (struct sockaddr_in);
            save_fd = fcntl( tn_in_socket, F_GETFL );
            save_fd |= O_NONBLOCK;
            fcntl( tn_in_socket, F_SETFL, save_fd );
        } else {
            cout << ts(buf) << "Error opening Socket " << cfg.incomingPort << endl;
            exit(1);
        }            
	}
    sleep(2);
    cout << ts(buf) << "starting radio on channel ... " << RF24_CHANNEL << endl;
    radio.begin();
    radio.setPALevel( RF24_PA_MAX ) ;
    radio.setChannel( RF24_CHANNEL );
//    radio.setAutoAck(0,0);
//    radio.setAutoAck( true );
    radio.enableDynamicPayloads();
    radio.setDataRate( RF24_SPEED );
	radio.setRetries(15,5);
    uint8_t  rf24_node2hub[] = RF24_NODE2HUB;
    uint8_t  rf24_hub2node[] = RF24_HUB2NODE;
    radio.openWritingPipe(rf24_hub2node);
    radio.openReadingPipe(1,rf24_node2hub);
    radio.startListening();
    radio.printDetails();
    cout << ts(buf) << PRGNAME << " up and running .... " << endl;

    // Init Arrays
    node.setVerbose(verboselevel);
    sensor.setVerbose(verboselevel);
    order.setVerbose(verboselevel);
    orderbuffer.setVerbose(verboselevel);
    init_system();

    // Main Loop
    while(1) {
        //print messages if we have
        //if (textbuffer.hasMessage) textbuffer.printMessage();
        // sync sensordata_im to sensordata
        if ( (unsigned)time(NULL) == ((unsigned)time(NULL) & 0b11111111111111111111000000000000 ) ) {
           database.sync_sensordata();   
        }
        /* Handling of incoming messages */
		if ( cfg.incomingPortSet ) {
            new_tn_in_socket = accept ( tn_in_socket, (struct sockaddr *) &address, &addrlen );
            if (new_tn_in_socket > 0) {
                thread t2(receive_tn_in, new_tn_in_socket, &address);
                t2.detach();
            }
            // Read the messagequeue
            msgID = msgget(MSGKEY, IPC_CREAT | 0600);
            if (msgID >= 0) {
                int ret;
                // Logmessages
                ret=msgrcv(msgID, &LogMsg, sizeof(LogMsg), 1, IPC_NOWAIT);
                if ( (ret > 5) && (verboselevel & VERBOSETELNET)) {
                    printf("%s%s\n", ts(buf), LogMsg.tntext);
                }
                // store into orderbuffer and make order
                ret=msgrcv(msgID, &LogMsg, sizeof(LogMsg), 2, IPC_NOWAIT);
                if ( ret > 5 ) {
                    if ( verboselevel & VERBOSETELNET ) {
                        printf("%s%s\n", ts(buf), LogMsg.tntext);
                    }
                    char* tnstr = alloc_str(verboselevel,"main tnstr",DEBUGSTRINGSIZE);
                    strcpy(tnstr,LogMsg.tntext);
                    bool result = process_tn_in(tnstr,LogMsg.tn_socket);
                    free_str(verboselevel,"main tnstr",tnstr);
                    LogMsg.mtype = 9;
                    if ( result ) {
                        sprintf(LogMsg.tntext,"Command received => OK\n");
                    } else {
                        sprintf(LogMsg.tntext,"Command received => Error\n");
                    }
                    if (msgID >= 0) msgsnd(msgID, &LogMsg, sizeof(LogMsg), 0);
                }
            }
		}
		if ( radio.isValid() && radio.available() ) {
//
// Receive loop: react on the message from the nodes
//
			radio.read(&payload,sizeof(payload));
            debug_print_payload(VERBOSERF24, "Rec", " ", &payload);
			switch ( payload.msg_type ) {
                case PAYLOAD_TYPE_INIT: { // Init message from a node!!
                    process_payload(&payload);
                }
                break;    
                case PAYLOAD_TYPE_HB: { // heartbeat message!!
                    if (node.isNewHB(payload.node_id, mymillis())) {  // Got a new Heaqrtbeat -> process it!
                        process_payload(&payload);
                        if ( orderbuffer.nodeHasEntry(payload.node_id) ) {  // WE have orders for this node
                            make_order(payload.node_id, PAYLOAD_TYPE_DAT);                    
                            if ( verboselevel & VERBOSEORDER ) {
                                cout << ts(buf) << "Entries for Heartbeat Node found, sending them" << endl;
                            }
                        } else {
                            make_order(payload.node_id, PAYLOAD_TYPE_HB_RESP);                    
                            if ( verboselevel & VERBOSEORDER) {
                                cout << ts(buf) << "No Entries for Heartbeat Node found, sending Endmessage" << endl;
                            }
                            order.addOrder(payload.node_id, PAYLOAD_TYPE_HB_RESP, true, 0, mymillis());    
                            order.modifyOrderFlags(payload.node_id, PAYLOAD_FLAG_LASTMESSAGE);
                        }
                    }
                }
                break;    
                case PAYLOAD_TYPE_HB_STOP: { // Quittung für einen Heatbeatresponse!!
                    order.delByOrderNo(payload.orderno);  // Nachricht ist angekommen => löschen
                }
                break;
                case PAYLOAD_TYPE_DATRESP: { // Quittung für eine Nachricht vom Typ PAYLOAD_TYPE_DATNOR !!
                    if ( order.isOrderNo(payload.orderno) ) {
                        process_payload(&payload);
                        order.delByOrderNo(payload.orderno);  // Nachricht ist angekommen => löschen
                        if ( orderbuffer.nodeHasEntry(payload.node_id) ) {  // WE have orders for this node
                            make_order(payload.node_id, PAYLOAD_TYPE_DAT);                    
                            if ( verboselevel & VERBOSEORDER ) {
                                cout << ts(buf) << "Entries for Heartbeat Node found, sending them" << endl;
                            }
                        }
                        order.addOrder(payload.node_id, PAYLOAD_TYPE_DATSTOP, true, 0, mymillis());    
                        order.modifyOrderFlags(payload.node_id, PAYLOAD_FLAG_LASTMESSAGE);
                    }
                }
                break;
                case PAYLOAD_TYPE_DATSTOP: { // Quittung für einen Heatbeatresponse!!
                    order.delByOrderNo(payload.orderno);  // Nachricht ist angekommen => löschen
                }
                break;
                case PAYLOAD_TYPE_PING_POW_MIN: {
                    node.setPaLevel(payload.node_id, 0);
                    database.storeNodeConfig(payload.node_id, 118, 0);
                }
                break;
                case PAYLOAD_TYPE_PING_POW_LOW: {
                    node.setPaLevel(payload.node_id, 1);
                    database.storeNodeConfig(payload.node_id, 118, 1);
                }
                break;
                case PAYLOAD_TYPE_PING_POW_HIGH: {
                    node.setPaLevel(payload.node_id, 2);
                    database.storeNodeConfig(payload.node_id, 118, 2);
                }
                break;
                case PAYLOAD_TYPE_PING_POW_MAX: {
                    node.setPaLevel(payload.node_id, 3);
                    database.storeNodeConfig(payload.node_id, 118, 3);
                }
                break;
                default: {	
                    if ( verboselevel & VERBOSEORDER) {
                        cout << ts(buf) << "Processing Node: " << (int)payload.node_id << " Type: " << (int)payload.msg_type << " Orderno: " << (int)payload.orderno << endl;
                    }
                    if ( order.isOrderNo(payload.orderno) ) {
                        process_payload(&payload);
                        order.delByOrderNo(payload.orderno);
                        if ( orderbuffer.nodeHasEntry(payload.node_id) ) {  // WE have orders for this node
                            make_order(payload.node_id, PAYLOAD_TYPE_DAT);                    
                            if ( verboselevel & VERBOSEORDER ) {
                                cout << ts(buf) << "Entries for Heartbeat Node found, sending them" << endl;
                            }
                        }
                    }
                }
			}
			
		} // radio.available
//
// Orderloop: Tell the nodes what they have to do
//
		if ( order.hasEntry() ) {  // go transmitting if its time to do ..
			// Look if we have something to send
			while ( order.getOrderForTransmission(&payload, mymillis() ) ) {
                    radio.stopListening();
                    radio.openWritingPipe(rf24_hub2node);
					if (radio.write(&payload,sizeof(payload))) {
                        radio.startListening();
                        debug_print_payload(VERBOSERF24, "Snd", "OK", &payload);
					} else {
                        radio.startListening();
                        debug_print_payload(VERBOSERF24, "Snd", "Fail", &payload);
                    }
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
}

