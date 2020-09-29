#include "rf24hubd.h" 

// do_tn_cmd ==> send a telnet comand to the fhem-host
// usage example: do_tn_cmd("set device1 on");
void do_tn_cmd(NODE_DATTYPE node_id, uint8_t channel, char* value) {
    char* tn_cmd = alloc_str(verboselevel,"do_tn_cmd tn_cmd",TELNETBUFFERSIZE,ts(tsbuf));
    char tn_quit[] =  "\r\nquit\r\n";
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char* fhem_dev;
    fhem_dev = sensor.getFhemDevByNodeChannel(node_id, channel); 
	sprintf(tn_cmd,"set %s %s", fhem_dev, value);
    if ( verboselevel & VERBOSETELNET) {    
        printf("%sdo_tn_cmd: %s\n", ts(tsbuf), tn_cmd );
    }
    portno = std::stoi(cfg.fhemPort);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("%sERROR: do_tn_cmd: error opening socket\n", ts(tsbuf));
	}	
    server = gethostbyname(cfg.fhemHost.c_str());
    if (server == NULL) {
        printf("%sERROR: do_tn_cmd: no such host\n", ts(tsbuf));
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) { 
        printf("%sERROR: do_tn_cmd: error connecting\n", ts(tsbuf));
	} else {	
		n = write(sockfd,tn_cmd,strlen(tn_cmd));
		if (n < 0) {
			printf("%sERROR: do_tn_cmd: error writing to socket\n", ts(tsbuf));
		} else {
            if ( verboselevel & VERBOSETELNET) {    
                printf("%sdo_tn_cmd: Telnet to %s Port %u successfull Command: %s\n", ts(tsbuf), cfg.fhemHost.c_str(), portno, tn_cmd);
            }
		}		
		write(sockfd,tn_quit,strlen(tn_quit));
	}		 
    close(sockfd);
    free_str(verboselevel, "do_tn_cmd tn_cmd", tn_cmd,ts(tsbuf));
}

/* Die Thread-Funktion fuer den Empfang von telnet Daten*/
static void* receive_tn_in (void *arg) {
    struct thread_tn_data *f = (struct thread_tn_data *)arg;
    char* buffer = alloc_str(verboselevel,"receive_tn_in buffer",TELNETBUFFERSIZE,ts(tsbuf));
    char* client_message = alloc_str(verboselevel,"receive_tn_in client_message",DEBUGSTRINGSIZE,ts(tsbuf));
    ssize_t MsgLen;
    TnMsg_t TnMsg;
    int ret;
    // send something like a prompt. perl telnet is waiting for it otherwise we get error
    // use this in perl: my $t = new Net::Telnet (Timeout => 2, Port => 7001, Prompt => '/rf24hub>/');
    sprintf(client_message,"rf24hub> ");
    write(f->tnsocket , client_message , strlen(client_message));
    MsgLen = recv(f->tnsocket, buffer, TELNETBUFFERSIZE, 0);
    if (MsgLen>0) {
        int msgID = msgget(MSGKEY, IPC_CREAT | 0600);
        TnMsg.mtype = 1;
        TnMsg.TnData.tn_socket = 0;
        //sprintf(TnMsg.tntext,"Incoming telnet data from %s:",inet_ntoa(address->sin_addr));
        sprintf(TnMsg.TnData.tntext,"Incoming telnet data from :");
        if (msgID >= 0) msgsnd(msgID, &TnMsg, sizeof(TnMsg), 0);
        TnMsg.mtype = 2;
        TnMsg.TnData.tn_socket = f->tnsocket;
        sprintf(TnMsg.TnData.tntext,"%s", trim(buffer));
        if (msgID >= 0) msgsnd(msgID, &TnMsg, sizeof(TnMsg), 0);
            
        // Wait for a message with processing result            
        ret=msgrcv(msgID, &TnMsg, sizeof(TnMsg), 9, 0);
        if ( ret > 5 ) {
            write(f->tnsocket, TnMsg.TnData.tntext, strlen(TnMsg.TnData.tntext));
        }
        
    }
    //sprintf(buffer,"%s %d\n",PRGNAME,SWVERSION);
    //write(tnsocket , buffer , strlen(buffer));
    sleep(1);
    close (f->tnsocket);
    free(f);
    free_str(verboselevel,"receive_tn_in buffer",buffer,ts(tsbuf));
    free_str(verboselevel,"receive_tn_in client_message",client_message,ts(tsbuf));
    pthread_exit((void *)pthread_self());
}
	
/* Die Thread-Funktion fuer den Speicherung der Sensor Daten*/

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
         cmp_sync[]="sync",
         cmp_config[]="config";
	char *wort1a, *wort2a, *wort3a, *wort4a;
	char *wort1, *wort2, *wort3, *wort4;
    char* message = alloc_str(verboselevel,"process_tn_in message",120,ts(tsbuf));
	bool tn_input_ok = false;
	char delimiter[] = " ";
	//trim(buffer);
	wort1a = strtok(inbuffer, delimiter);
	wort2a = strtok(NULL, delimiter);
	wort3a = strtok(NULL, delimiter);
	wort4a = strtok(NULL, delimiter);
    char* pEnd;
    if (wort1a) { 
        wort1 = alloc_str(verboselevel,"process_tn_in wort1",strlen(wort1a)+1,ts(tsbuf));
        strcpy(wort1, wort1a);
    } else { 
        wort1 = alloc_str(verboselevel,"process_tn_in wort1",1,ts(tsbuf));
        wort1[0] = '\0';
    }
    if (wort2a) { 
        wort2 = alloc_str(verboselevel,"process_tn_in wort2",strlen(wort2a)+1,ts(tsbuf));
        strcpy(wort2, wort2a);
    } else { 
        wort2 = alloc_str(verboselevel,"process_tn_in wort2",1,ts(tsbuf));
        wort2[0] = '\0';
    }
    if (wort3a) { 
        wort3 = alloc_str(verboselevel,"process_tn_in wort3",strlen(wort3a)+1,ts(tsbuf));
        strcpy(wort3, wort3a);
    } else { 
        wort3 = alloc_str(verboselevel,"process_tn_in wort3",1,ts(tsbuf));
        wort3[0] = '\0';
    }
    if (wort4a) { 
        wort4 = alloc_str(verboselevel,"process_tn_in wort4",strlen(wort4a)+1,ts(tsbuf));
        strcpy(wort4, wort4a);
    } else { 
        wort4 = alloc_str(verboselevel,"process_tn_in wort4",1,ts(tsbuf));
        wort4[0] = '\0';
    }
//printf("+++++Wort1..4: %s %s %s %s\n",wort1, wort2, wort3, wort4);
    // set/setlast sensor <sensor> <value>
	// sets a sensor to a value, setlast starts the request over air
	if ( (( strcmp(wort1,cmp_set) == 0 ) || ( strcmp(wort1,cmp_setlast) == 0 )) && (strcmp(wort2,cmp_sensor) == 0) && (strlen(wort3) > 0) && (strlen(wort4) > 0) ) {
		// In word3 we may have a) the number of the sensor b) the name of the sensor c) the fhem_dev of a sensor
		// for the processing we need the number of the sensor ==> find it!
        NODE_DATTYPE mynode = 0;
        uint8_t   mychannel = 0;
        uint32_t  mydata = 0;
        if ( wort3[0] >= 'A' && wort3[0] <= 'z' ) {
            if (sensor.getNodeChannelByFhemDev(&mynode, &mychannel, wort3) ) {
                //orderbuffer.addOrderBuffer(mymillis(),mynode,mychannel,myvalue);
          		tn_input_ok = true;
            }
        } else {
            uint32_t mysensor = strtol(wort3, &pEnd, 10);
            if ( sensor.getNodeChannelBySensorID(&mynode, &mychannel, mysensor ) ) {
                //orderbuffer.addOrderBuffer(mymillis(),mynode,mychannel,myvalue);
                tn_input_ok = true;
            }
        }
        mydata = packData(mychannel, wort4); 
        orderbuffer.addOrderBuffer(mymillis(),mynode,mychannel,mydata);
        //ToDo
        
        
        
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
        orderbuffer.addOrderBuffer(mymillis(), mynode, mychannel, packData(mychannel, wort4) );
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
            database.setVerbose(verboselevel);
//		}	
	}
    // show order
	// lists the current orderbuffer
	if (( strcmp(wort1,cmp_show) == 0 ) && (strcmp(wort2,cmp_order) == 0) && (strlen(wort3) == 0) && (strlen(wort4) == 0) ) {
		tn_input_ok = true;
        orderbuffer.printBuffer(tn_socket, false);
        order.printBuffer(tn_socket, false);
	}	
    // show sensor
	// lists the current node- and sensorbuffer
	if (( strcmp(wort1,cmp_show) == 0 ) && (strcmp(wort2,cmp_sensor) == 0) && (strlen(wort3) == 0) && (strlen(wort4) == 0) ) {
		tn_input_ok = true;
        node.printBuffer(tn_socket, false);
        sensor.printBuffer(tn_socket, false);
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
        orderbuffer.printBuffer(tn_socket, true);
        order.printBuffer(tn_socket, true);
    }	
    // sync
	// syncronisation of all relevant tables of rf24hubd: stores in memory table data to database hard disk tables 
	if ( (strcmp(wort1,cmp_sync) == 0) && (strlen(wort2) == 0) && (strlen(wort3) == 0) && (strlen(wort4) == 0) ) {
		tn_input_ok = true;
        database.sync_sensor();
        database.sync_sensordata();
        database.sync_sensordata_d();
    }
    // init
	// initialisation of rf24hubd: reloads data from database
	if ( (strcmp(wort1,cmp_init) == 0) && (strlen(wort2) == 0) && (strlen(wort3) == 0) && (strlen(wort4) == 0) ) {
		tn_input_ok = true;
        exit_system();
        node.cleanup();
        sensor.cleanup();
		init_system();
        node.printBuffer(tn_socket, false);
        sensor.printBuffer(tn_socket, false);
	}
	if ( ! tn_input_ok) {
        //printf("%u \n",sizeof(tn_usage_txt)/ sizeof(int));
        for(unsigned int i=0; i<sizeof(tn_usage_txt)/ sizeof(int); i++) {
            sprintf(message,"%s\n",tn_usage_txt[i]);
            write(tn_socket , message , strlen(message));
        }
	}		
	free_str(verboselevel,"process_tn_in wort1",wort1,ts(tsbuf));
    free_str(verboselevel,"process_tn_in wort2",wort2,ts(tsbuf));
    free_str(verboselevel,"process_tn_in wort3",wort3,ts(tsbuf));
    free_str(verboselevel,"process_tn_in wort4",wort4,ts(tsbuf));
    free_str(verboselevel,"process_tn_in message", message,ts(tsbuf));
    return tn_input_ok;
}

void make_order(NODE_DATTYPE mynode, uint8_t mytype) {
    void* ret_ptr;
    uint32_t data;
    order.delByNode(mynode);
    ret_ptr = orderbuffer.findOrder4Node(mynode, NULL, &data);
    if (ret_ptr) {
        order.addOrder(mynode, mytype, node.isHBNode(mynode), data, mymillis());
        ret_ptr = orderbuffer.findOrder4Node(mynode, ret_ptr, &data);
        if (ret_ptr) {
            order.modifyOrder(mynode, 2, data);
            ret_ptr = orderbuffer.findOrder4Node(mynode, ret_ptr, &data);
            if (ret_ptr) {
                order.modifyOrder(mynode, 3, data);
                ret_ptr = orderbuffer.findOrder4Node(mynode, ret_ptr, &data);
                if (ret_ptr) {
                    order.modifyOrder(mynode, 4, data);
                    ret_ptr = orderbuffer.findOrder4Node(mynode, ret_ptr, &data);
                    if (ret_ptr) {
                        order.modifyOrder(mynode, 5, data);
                        ret_ptr = orderbuffer.findOrder4Node(mynode, ret_ptr, &data);
                        if (ret_ptr) {
                            order.modifyOrder(mynode, 6, data);
                            ret_ptr = orderbuffer.findOrder4Node(mynode, ret_ptr, &data);
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

void exit_system(void) {
    // Save data from sensordata_im and sensor_im to persistant tables
    database.sync_sensordata();
    database.sync_sensor();
    database.sync_config();
}

void init_system(void) {
    database.initSystem();
    database.initSensor(&sensor);
    database.initNode(&node);
    node.printBuffer(fileno(stdout), false);
    sensor.printBuffer(fileno(stdout), false);
}

void process_sensor(NODE_DATTYPE node_id, uint32_t mydata) {
    uint8_t channel = getChannel(mydata);
    switch (channel) {
        case 1 ... 99: {
                // Sensor or Actor any type
            uint32_t sensor_id = sensor.getSensorByNodeChannel(node_id, channel);
            if ( sensor_id > 0 ) { 
                buf = unpackData(mydata, buf);
                if ( verboselevel & VERBOSECONFIG) {    
                    printf("%sValue of Node: %u Data: %u ==> Channel: %u is %s\n", ts(tsbuf), node_id, mydata, channel, buf);
                }
                sensor.updateLastVal(sensor_id, mydata, mymillis());
                database.storeSensorValue(sensor_id, buf);
                do_tn_cmd(node_id, channel, buf);
            }
        }
        break; 
        case 101: {
                // battery voltage
            uint32_t sensor_id = sensor.getSensorByNodeChannel(node_id, channel);
            if ( sensor_id > 0 ) { 
                buf = unpackData(mydata, buf);
                if ( verboselevel & VERBOSECONFIG) {    
                    printf("%sVoltage of Node: %u is %sV\n", ts(tsbuf), node_id, buf);
                }
                sensor.updateLastVal(sensor_id, mydata, mymillis());
                node.setVoltage(node_id, strtof(unpackData(mydata,buf),NULL));
                database.storeSensorValue(sensor_id, buf);
                do_tn_cmd(node_id, channel,buf);
            }
        }
        break; 
        case 102 ... 125: {
                // Node config register
            buf = unpackData(mydata, buf);
            if ( verboselevel & VERBOSECONFIG) {    
                printf("%sConfigregister of Node: %u Channel: %u is %s\n", ts(tsbuf), node_id, channel, buf);
            }
            database.storeNodeConfig(node_id, channel, buf);
        }	
        break; 
    }
	orderbuffer.delByNodeChannel(node_id, channel);
}	

/*******************************************************************************************
*
* All the rest 
*
********************************************************************************************/
void sighandler(int signal) {
    printf("%sSIGTERM: Cleanup system ... saving *_im tables ...\n",ts(tsbuf));
    exit_system(); 
    printf("%sSIGTERM: Shutting down ...\n",ts(tsbuf));
    cfg.removePidFile();
//	msgctl(msqid, IPC_RMID, NULL);
    exit (0);
}


void channelscanner (uint8_t channel) {
  int values=0;
  printf("Scanning channel: %u\n", channel);
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
  printf("1000 passes: Detect %d times a carrier\n", values);
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

void printPayload(uint16_t loglevel, const char* msg_header, const char* result, payload_t* mypayload) {
	if ( verboselevel & loglevel  ) {   
        char* vbuf1 = alloc_str(verboselevel,"vbuf1",10,ts(tsbuf));
        char* vbuf2 = alloc_str(verboselevel,"vbuf2",10,ts(tsbuf));
        char* vbuf3 = alloc_str(verboselevel,"vbuf3",10,ts(tsbuf));
        char* vbuf4 = alloc_str(verboselevel,"vbuf4",10,ts(tsbuf));
        char* vbuf5 = alloc_str(verboselevel,"vbuf5",10,ts(tsbuf));
        char* vbuf6 = alloc_str(verboselevel,"vbuf6",10,ts(tsbuf));
        vbuf1=unpackData(mypayload->data1, vbuf1);
        vbuf2=unpackData(mypayload->data2, vbuf2);
        vbuf3=unpackData(mypayload->data3, vbuf3);
        vbuf4=unpackData(mypayload->data4, vbuf4);
        vbuf5=unpackData(mypayload->data5, vbuf5);
        vbuf6=unpackData(mypayload->data6, vbuf6);
        printf("%s%s: N:%u T:%u m:%u F:0x%02X O:%u H:%u (%u/%s)(%u/%s)(%u/%s)(%u/%s)(%u/%s)(%u/%s)%s\n",
               ts(tsbuf), msg_header, mypayload->node_id, mypayload->msg_type, mypayload->msg_id, mypayload->msg_flags, mypayload->orderno, mypayload->heartbeatno,
               getChannel(mypayload->data1), vbuf1,
               getChannel(mypayload->data2), vbuf2,
               getChannel(mypayload->data3), vbuf3,
               getChannel(mypayload->data4), vbuf4,
               getChannel(mypayload->data5), vbuf5,
               getChannel(mypayload->data6), vbuf6,
               result);   
        free_str(verboselevel,"vbuf1",vbuf1,ts(tsbuf));
        free_str(verboselevel,"vbuf2",vbuf2,ts(tsbuf));
        free_str(verboselevel,"vbuf3",vbuf3,ts(tsbuf));
        free_str(verboselevel,"vbuf4",vbuf4,ts(tsbuf));
        free_str(verboselevel,"vbuf5",vbuf5,ts(tsbuf));
        free_str(verboselevel,"vbuf6",vbuf6,ts(tsbuf));
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
    buf = (char*)malloc(TSBUFFERSIZE);
    tsbuf = (char*)malloc(TSBUFFERSIZE);
	/* vars for telnet socket handling */
	int tn_in_socket = 0;
    int new_tn_in_socket = 0;
	socklen_t addrlen;
	struct sockaddr_in address;
	long save_fd;
	const int y = 1;
	int msgID;
    TnMsg_t LogMsg;
    time_t lastDBsync = time(0);
	
    // processing argc and argv[]
    cfg.processParams(argc, argv);

	// check if started as root
	if ( getuid()!=0 ) {
        printf("%s has to be startet as user root!\n",PRGNAME);
        exit(1);
    }
    
    // check for PID file, if exists terminate else create it
    if ( cfg.checkPidFileSet() ) {
         exit(1);
    } else {
         cfg.setPidFile();
    }
    printf("--------------------------------------------------\n");
    printf("%sStartup Parameters:\n",ts(tsbuf));
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
            printf("%sStarting %s as daemon...\n",ts(tsbuf),PRGNAME);
            // and exit
            exit(0);
        } else {
            // nagativ is an error
            printf("%sFork ERROR ... exiting\n",ts(tsbuf));
            cfg.removePidFile();
            exit(1);
        }

        freopen(cfg.logFileName.c_str(), "a+", stdout); 
        freopen(cfg.logFileName.c_str(), "a+", stderr); 
    }
    
    // connect database
    database.connect(cfg.dbHostName, cfg.dbUserName, cfg.dbPassWord, cfg.dbSchema, std::stoi(cfg.dbPort));

    if ( cfg.fhemPortSet && cfg.fhemHostSet ) {
        printf("%stelnet session to FHEM started: Host: %s Port: %s\n",ts(tsbuf), cfg.fhemHost.c_str(), cfg.fhemPort.c_str());
    }

    if ( cfg.incomingPortSet ) {
    /* open incoming port for messages */
		if ((tn_in_socket=socket( AF_INET, SOCK_STREAM, 0)) > 0) {
            address.sin_family = AF_INET;
            address.sin_addr.s_addr = INADDR_ANY;
            address.sin_port = htons ( std::stoi ( cfg.incomingPort ) );
            setsockopt( tn_in_socket, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(int) );
            if (bind( tn_in_socket, (struct sockaddr *) &address, sizeof (address)) == 0 ) {
                printf("%sSocket für eingehende Messages auf Port %s angelegt\n", ts(tsbuf), cfg.incomingPort.c_str());
            }
            listen (tn_in_socket, 5);
            addrlen = sizeof (struct sockaddr_in);
            save_fd = fcntl( tn_in_socket, F_GETFL );
            save_fd |= O_NONBLOCK;
            fcntl( tn_in_socket, F_SETFL, save_fd );
        } else {
            printf("%sError opening Socket %s\n", ts(tsbuf), cfg.incomingPort.c_str());
            exit(1);
        }            
	}
    sleep(2);
    printf("%sstarting radio on channel ... %d\n", ts(tsbuf), RF24_CHANNEL);
    radio.begin();
    radio.setPALevel( RF24_PA_MAX ) ;
    radio.setChannel( RF24_CHANNEL );
//    radio.setAutoAck(0,0);
//    radio.setAutoAck( true );
    radio.enableDynamicPayloads();
    radio.setDataRate( RF24_SPEED );
//	radio.setRetries(15,5);
    radio.setRetries(0,0);
    uint8_t  rf24_node2hub[] = RF24_NODE2HUB;
    uint8_t  rf24_hub2node[] = RF24_HUB2NODE;
    radio.openWritingPipe(rf24_hub2node);
    radio.openReadingPipe(1,rf24_node2hub);
    radio.startListening();
    radio.printDetails();

    // Init Arrays
    node.setVerbose(verboselevel);
    sensor.setVerbose(verboselevel);
    order.setVerbose(verboselevel);
    orderbuffer.setVerbose(verboselevel);
    database.setVerbose(verboselevel);
    init_system();
    printf("%s%s up and running\n", ts(tsbuf),PRGNAME); 

    // Main Loop
    while(1) {
        // sync sensordata_im to sensordata
        if ( time(0) - lastDBsync > DBSYNCINTERVAL ) {
           database.sync_sensordata();   
           lastDBsync = time(0);
        }
        /* Handling of incoming messages */
		if ( cfg.incomingPortSet ) {
            new_tn_in_socket = accept ( tn_in_socket, (struct sockaddr *) &address, &addrlen );
            if (new_tn_in_socket > 0) {
                pthread_t a_thread;
                int ret;
                struct thread_tn_data *f;
                f = (struct thread_tn_data *)malloc(sizeof(struct thread_tn_data));
                f->tnsocket = new_tn_in_socket;
                ret = pthread_create(&a_thread, NULL, &receive_tn_in, f);
                if (ret == 0) {
                    pthread_detach(a_thread);
                }                
            }
            // Read the messagequeue
            msgID = msgget(MSGKEY, IPC_CREAT | 0600);
            if (msgID >= 0) {
                int ret;
                // Logmessages
                ret=msgrcv(msgID, &LogMsg, sizeof(LogMsg), 1, IPC_NOWAIT);
                if ( (ret > 5) && (verboselevel & VERBOSETELNET)) {
                    printf("%s%s\n", ts(tsbuf), LogMsg.TnData.tntext);
                }
                // store into orderbuffer and make order
                ret=msgrcv(msgID, &LogMsg, sizeof(LogMsg), 2, IPC_NOWAIT);
                if ( ret > 5 ) {
                    if ( verboselevel & VERBOSETELNET ) {
                        printf("%s%s\n", ts(tsbuf), LogMsg.TnData.tntext);
                    }
                    char* tnstr = alloc_str(verboselevel,"main tnstr",DEBUGSTRINGSIZE,ts(tsbuf));
                    strcpy(tnstr, LogMsg.TnData.tntext);
                    bool result = process_tn_in(tnstr,LogMsg.TnData.tn_socket);
                    free_str(verboselevel,"main tnstr",tnstr,ts(tsbuf));
                    LogMsg.mtype = 9;
                    if ( result ) {
                        sprintf(LogMsg.TnData.tntext,"Command received => OK\n");
                    } else {
                        sprintf(LogMsg.TnData.tntext,"Command received => Error\n");
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
            printPayload(VERBOSERF24, "Rec", " ", &payload);
			switch ( payload.msg_type ) {
                case PAYLOAD_TYPE_INIT: { // Init message from a node!!
                    if (node.isNewHB(payload.node_id, payload.heartbeatno, mymillis())) process_payload(&payload);
                }
                break;    
                case PAYLOAD_TYPE_HB: { // heartbeat message!!
                    if (node.isNewHB(payload.node_id, payload.heartbeatno, mymillis())) {  // Got a new Heaqrtbeat -> process it!
                        process_payload(&payload);
                        if ( orderbuffer.nodeHasEntry(payload.node_id) ) {  // WE have orders for this node
                            make_order(payload.node_id, PAYLOAD_TYPE_DAT);                    
                            if ( verboselevel & VERBOSEORDER ) {
                                printf("%sEntries for Heartbeat Node found, sending them\n",ts(tsbuf));
                            }
                        } else {
                            order.addOrder(payload.node_id, PAYLOAD_TYPE_HB_RESP, true, 0, mymillis());    
                            order.modifyOrderFlags(payload.node_id, PAYLOAD_FLAG_LASTMESSAGE);
                        }
                    }
                }
                break;    
                case PAYLOAD_TYPE_DATRESP: { // Quittung für eine Nachricht vom Typ PAYLOAD_TYPE_DATNOR !!
                    if ( order.isOrderNo(payload.orderno) ) {
                        process_payload(&payload);
                        order.delByOrderNo(payload.orderno);  // Nachricht ist angekommen => löschen
                        if ( orderbuffer.nodeHasEntry(payload.node_id) ) {  // WE have orders for this node
                            make_order(payload.node_id, PAYLOAD_TYPE_DAT);                    
                            if ( verboselevel & VERBOSEORDER ) {
                                printf("%sEntries for Heartbeat Node found, sending them\n",ts(tsbuf));
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
                    node.setPaLevel(payload.node_id, 1);
                }
                break;
                case PAYLOAD_TYPE_PING_POW_LOW: {
                    node.setPaLevel(payload.node_id, 2);
                }
                break;
                case PAYLOAD_TYPE_PING_POW_HIGH: {
                    node.setPaLevel(payload.node_id, 3);
                }
                break;
                case PAYLOAD_TYPE_PING_POW_MAX: {
                    node.setPaLevel(payload.node_id, 4);
                }
                break;
                case PAYLOAD_TYPE_PING_END: {
                    if (node.isNewHB(payload.node_id, payload.heartbeatno, mymillis())) {  // Got a new Heaqrtbeat -> process it!
                        sprintf(buf,"%u",node.getPaLevel(payload.node_id));
                        database.storeNodeConfig(payload.node_id, REG_PALEVEL, buf);
                    }
                }
                break;
                default: {	
                    if ( verboselevel & VERBOSEORDER) {
                        printf("%sProcessing Node:%u Type:%u Orderno: %u\n", ts(tsbuf), payload.node_id, payload.msg_type, payload.orderno);
                    }
                    if ( order.isOrderNo(payload.orderno) ) {
                        process_payload(&payload);
                        order.delByOrderNo(payload.orderno);
                        if ( orderbuffer.nodeHasEntry(payload.node_id) ) {  // WE have orders for this node
                            make_order(payload.node_id, PAYLOAD_TYPE_DAT);                    
                            if ( verboselevel & VERBOSEORDER ) {
                                printf("%sEntries for Heartbeat Node found, sending them\n", ts(tsbuf));
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
                    radio.flush_tx();
                    radio.openWritingPipe(rf24_hub2node);
					if (radio.write(&payload,sizeof(payload))) {
                        radio.startListening();
                        printPayload(VERBOSERF24, "Snd", "OK", &payload);
					} else {
                        radio.startListening();
                        printPayload(VERBOSERF24, "Snd", "Fail", &payload);
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

