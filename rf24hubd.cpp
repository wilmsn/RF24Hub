#include "rf24hubd.h" 

void send_fhem_cmd(NODE_DATTYPE node_id, uint8_t channel, char* value) {
    char* tn_cmd = alloc_str(verboselevel,"send_fhem_cmd tn_cmd",TELNETBUFFERSIZE,ts(tsbuf));
    char* fhem_dev;
    fhem_dev = sensor.getFhemDevByNodeChannel(node_id, channel); 
	sprintf(tn_cmd,"set %s %s", fhem_dev, value);
    send_fhem_tn(tn_cmd);
    free_str(verboselevel, "send_fhem_cmd tn_cmd", tn_cmd,ts(tsbuf));
}

// send_fhem_tn ==> send a telnet comand to the fhem-host
// usage example: send_fhem_cmd("set device1 on");

void send_fhem_tn(char* tn_cmd) {
    char tn_quit[] =  "\r\nquit\r\n";
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    if ( verboselevel & VERBOSETELNET) {    
        printf("%ssend_fhem_cmd: %s\n", ts(tsbuf), tn_cmd );
    }
    portno = std::stoi(cfg.fhemPortNo);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd != 0) {
        server = gethostbyname(cfg.fhemHostName.c_str());
        if (server != NULL) {
            bzero((char *) &serv_addr, sizeof(serv_addr));
            serv_addr.sin_family = AF_INET;
            bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
            serv_addr.sin_port = htons(portno);
            if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
                printf("%sERROR: send_fhem_cmd: error connecting\n", ts(tsbuf));
            } else {
                n = write(sockfd,tn_cmd,strlen(tn_cmd));
                if (n < 0) {
                    printf("%sERROR: send_fhem_cmd: error writing to socket\n", ts(tsbuf));
                } else {
                    if ( verboselevel & VERBOSETELNET) {
                        printf("%ssend_fhem_cmd: Telnet to %s Port %u successfull Command: %s\n", ts(tsbuf), cfg.fhemHostName.c_str(), portno, tn_cmd);
                    }
                }
                write(sockfd,tn_quit,strlen(tn_quit));
            }
            close(sockfd);
        } else { // server == NULL
            printf("%sERROR: send_fhem_cmd: no such host\n", ts(tsbuf));
        }
    } else { // sockfd == 0
        printf("%sERROR: send_fhem_cmd: error opening socket\n", ts(tsbuf));
	}	
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
        int msgID = msgget(MSGKEYHUB, IPC_CREAT | 0600);
        TnMsg.mtype = 1;
        TnMsg.TnData.tn_socket = 0;
        //sprintf(TnMsg.tntext,"Incoming telnet data from %s:",inet_ntoa(address->sin_addr));
        //sprintf(TnMsg.TnData.tntext,"Incoming telnet data from :");
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
		set			sensor		<sensor#> 	<value>				Sets Sensor to Value (Store in Orderbuffer only)
      for all details have a look at "rf24hub_text.h" 
*/

    char cmp_init[]="init",
         cmp_on[]="on",
         cmp_off[]="off",
         cmp_add[]="add",
         cmp_delete[]="delete",
	 cmp_sensor[]="sensor",
	 cmp_node[]="node",
	 cmp_mastered[]="mastered",
	 cmp_unmastered[]="unmastered",
	 cmp_set[]="set",
	 cmp_html[]="html",
	 cmp_order[]="order",
	 cmp_verbose[]="verbose",
         cmp_push[]="push",
         cmp_show[]="show",
         cmp_sync[]="sync",
         cmp_gateway[]="gateway",
         cmp_truncate[]="truncate",
         cmp_logfile[]="logfile";
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
    if ( (strcmp(wort1,cmp_set) == 0) && (strcmp(wort2,cmp_sensor) == 0) && (strlen(wort3) > 0) && (strlen(wort4) > 0) ) {
    // In word3 we may have a) the number of the sensor b) the name of the sensor c) the fhem_dev of a sensor
    // for the processing we need the number of the sensor ==> find it!
        NODE_DATTYPE node_id = 0;
        uint8_t   channel = 0;
        uint32_t  data = 0;
        if ( wort3[0] >= 'A' && wort3[0] <= 'z' ) {
            if (sensor.getNodeChannelByFhemDev(&node_id, &channel, wort3) ) {
		tn_input_ok = true;
            } else {
                if (sensor.getNodeChannelBySensorName(&node_id, &channel, wort3) ) {
                    tn_input_ok = true;
                }
            }
        } else {
            uint32_t sensor_id = strtol(wort3, &pEnd, 10);
            if ( sensor.getNodeChannelBySensorID(&node_id, &channel, sensor_id ) ) {
                tn_input_ok = true;
            }
        }
        if ( node_id > 0 &&  channel > 0 ) {
            data = packTransportValue(channel, wort4);
            orderbuffer.addOrderBuffer(mymillis(),node_id,channel,data);
        } else {
            tn_input_ok = false;
        }
    }
    // push <node> <channel> <value>
    // Pushes a value direct to a channel into a node
    if (( strcmp(wort1,cmp_push) == 0 ) && (strlen(wort2) > 0) && (strlen(wort3) > 0) && (strlen(wort4) > 0) ) {
        NODE_DATTYPE node_id = strtol(wort2, &pEnd, 10);
        uint8_t channel = strtol(wort3, &pEnd, 10);
        if ( node.isValidNode(node_id) ) {
            orderbuffer.addOrderBuffer(mymillis(), node_id, channel, packTransportValue(channel, wort4) );
            tn_input_ok = true;
        }
    }
    // set verbose <new verboselevel>
    // sets the new verboselevel
    if (( strcmp(wort1,cmp_set) == 0 ) && (strcmp(wort2,cmp_verbose) == 0) && (strlen(wort3) > 0) && (strlen(wort4) == 0) ) {
	tn_input_ok = true;
	verboselevel = decodeVerbose(verboselevel, wort3);
	node.setVerbose(verboselevel);
	sensor.setVerbose(verboselevel);
	order.setVerbose(verboselevel);
	orderbuffer.setVerbose(verboselevel);
	database.setVerbose(verboselevel);
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
    // show gateway
    // lists the current gateway status
    if (( strcmp(wort1,cmp_show) == 0 ) && (strcmp(wort2,cmp_gateway) == 0) && (strlen(wort3) == 0) && (strlen(wort4) == 0) ) {
	tn_input_ok = true;
        gateway.printBuffer(tn_socket, false);
    }
    // add gateway
    // adds a gateway in status active to database and system
    if (( strcmp(wort1,cmp_add) == 0 ) && (strcmp(wort2,cmp_gateway) == 0) && (strlen(wort3) > 0) && (strlen(wort4) > 0) ) {
        if ( wort3[0] >= 'A' && wort3[0] <= 'z' ) {
	    uint16_t gw_id = strtol(wort4, &pEnd, 10);
	    if ( gw_id > 0 ) {
		tn_input_ok = true;
		database.addGateway(wort3, gw_id);
		gateway.addGateway(wort3, gw_id, true);
		gateway.printBuffer(tn_socket, false);
	    }
        }
    }
    // delete gateway
    // deletes a gateway from database and system
    if (( strcmp(wort1,cmp_delete) == 0 ) && (strcmp(wort2,cmp_gateway) == 0) && (strlen(wort3) > 0) && (strlen(wort4) == 0) ) {
        uint16_t gw_id = strtol(wort3, &pEnd, 10);
        if ( gw_id > 0 ) {
	    tn_input_ok = true;
	    database.delGateway(gw_id);
	    gateway.delGateway(gw_id);
	    gateway.printBuffer(tn_socket, false);
        }
    }
    // set gateway <GW_NO> on
    // lists the current node- and sensorbuffer
    if (( strcmp(wort1,cmp_set) == 0 ) && (strcmp(wort2,cmp_gateway) == 0) && (strlen(wort3) > 0) && (strcmp(wort4,cmp_on) == 0) ) {
	tn_input_ok = true;
	uint16_t gw_id = (uint16_t)strtoul(wort3, &pEnd, 10);
	database.enableGateway(gw_id);
	gateway.setGateway(gw_id, true);
	gateway.printBuffer(tn_socket, false);
    }
    // set gateway <GW_NO> off
    // lists the current node- and sensorbuffer
    if (( strcmp(wort1,cmp_set) == 0 ) && (strcmp(wort2,cmp_gateway) == 0) && (strlen(wort3) > 0) && (strcmp(wort4,cmp_off) == 0) ) {
	tn_input_ok = true;
	uint16_t gw_id = (uint16_t)strtoul(wort3, &pEnd, 10);
	database.disableGateway(gw_id);
	gateway.setGateway(gw_id, false);
	gateway.printBuffer(tn_socket, false);
    }
    // set node <node_id> <mastered/unmastered>
    // syncronisation of all relevant tables of rf24hubd: stores in memory table data to database hard disk tables 
    if ( (strcmp(wort1,cmp_set) == 0) && (strcmp(wort2,cmp_node) == 0) && (strlen(wort3) > 0) && (strlen(wort4) > 0) ) {
        NODE_DATTYPE node_id = strtol(wort3, &pEnd, 10);
	printf("%u\n", node_id);    
        if (node_id > 0) {
            if (strcmp(wort4,cmp_mastered) == 0) {
                tn_input_ok = true;
                database.updateNodeMastered(node_id, true);
            }
            if (strcmp(wort4,cmp_unmastered) == 0) {
                tn_input_ok = true;
                database.updateNodeMastered(node_id, false);
            }
        }
    }
    // show verbose
    if (( strcmp(wort1,cmp_show) == 0 ) && (strcmp(wort2,cmp_verbose) == 0) && (strlen(wort3) == 0) && (strlen(wort4) == 0) ) {
	tn_input_ok = true;
	sprintf(message,"Active verboselevel: %s\n", printVerbose(verboselevel, buf));
	write(tn_socket , message , strlen(message));
    }
    // html order
    // lists the current order/orderbuffer for html page
    if (( strcmp(wort1,cmp_html) == 0 ) && (strcmp(wort2,cmp_order) == 0) && (strlen(wort3) == 0) && (strlen(wort4) == 0) ) {
	tn_input_ok = true;
        orderbuffer.printBuffer(tn_socket, true);
    }	
    // sync
    // syncronisation of all relevant tables of rf24hubd: stores in memory table data to database hard disk tables 
    if ( (strcmp(wort1,cmp_sync) == 0) && (strlen(wort2) == 0) && (strlen(wort3) == 0) && (strlen(wort4) == 0) ) {
	tn_input_ok = true;
        database.sync_sensordata_d();
    }
    // init
    // initialisation of rf24hubd: reloads data from database
    if ( (strcmp(wort1,cmp_init) == 0) && (strlen(wort2) == 0) && (strlen(wort3) == 0) && (strlen(wort4) == 0) ) {
	tn_input_ok = true;
        exit_system();
        node.cleanup();
        sensor.cleanup();
        gateway.cleanup();
	init_system();
        node.printBuffer(tn_socket, false);
        sensor.printBuffer(tn_socket, false);
        gateway.printBuffer(tn_socket, false);
    }
    // truncate logfile
    // truncation of the logfile for maintenance
    if ( (strcmp(wort1,cmp_truncate) == 0) && (strcmp(wort2,cmp_logfile) == 0) && (strlen(wort3) == 0) && (strlen(wort4) == 0) ) {
        truncate (cfg.hubLogFileName.c_str(), 0);
        printf(message, "Logfile: %s geleert", cfg.hubLogFileName.c_str());
	tn_input_ok = true;
    }
    if ( ! tn_input_ok) {
        for(unsigned int i=0; i<sizeof(tn_usage_txt)/ sizeof(int); i++) {
            sprintf(message,"%s\n",tn_usage_txt[i]);
            write(tn_socket , message , strlen(message));
        }
        sprintf(message,"%s version %s\n", PRGNAME, SWVERSION_STR);
        write(tn_socket , message , strlen(message));
    }		
    free_str(verboselevel,"process_tn_in wort1",wort1,ts(tsbuf));
    free_str(verboselevel,"process_tn_in wort2",wort2,ts(tsbuf));
    free_str(verboselevel,"process_tn_in wort3",wort3,ts(tsbuf));
    free_str(verboselevel,"process_tn_in wort4",wort4,ts(tsbuf));
    free_str(verboselevel,"process_tn_in message", message,ts(tsbuf));
    return tn_input_ok;
}

void make_order(NODE_DATTYPE node_id, uint8_t msg_type) {
    void* ret_ptr;
    uint32_t data;
    if ( node.isMasteredNode(node_id) ) {
      order.delByNode(node_id);
      ret_ptr = orderbuffer.findOrder4Node(node_id, NULL, &data);
      if (ret_ptr) {
        order.addOrder(node_id, msg_type, data, mymillis());
        ret_ptr = orderbuffer.findOrder4Node(node_id, ret_ptr, &data);
        if (ret_ptr) {
          order.modifyOrder(node_id, 2, data);
          ret_ptr = orderbuffer.findOrder4Node(node_id, ret_ptr, &data);
          if (ret_ptr) {
            order.modifyOrder(node_id, 3, data);
            ret_ptr = orderbuffer.findOrder4Node(node_id, ret_ptr, &data);
            if (ret_ptr) {
              order.modifyOrder(node_id, 4, data);
              ret_ptr = orderbuffer.findOrder4Node(node_id, ret_ptr, &data);
              if (ret_ptr) {
                order.modifyOrder(node_id, 5, data);
                ret_ptr = orderbuffer.findOrder4Node(node_id, ret_ptr, &data);
                if (ret_ptr) {
                  order.modifyOrder(node_id, 6, data);
                  ret_ptr = orderbuffer.findOrder4Node(node_id, ret_ptr, &data);
                  if (ret_ptr) {
                    order.modifyOrderFlags(node_id, PAYLOAD_FLAG_EMPTY );
                  } else {
                    order.modifyOrderFlags(node_id, PAYLOAD_FLAG_LASTMESSAGE );
                  }
                } else {
                  order.modifyOrderFlags(node_id, PAYLOAD_FLAG_LASTMESSAGE );
                }
              } else {
                order.modifyOrderFlags(node_id, PAYLOAD_FLAG_LASTMESSAGE );
              }
            } else {
              order.modifyOrderFlags(node_id, PAYLOAD_FLAG_LASTMESSAGE );
            }
          } else {
            order.modifyOrderFlags(node_id, PAYLOAD_FLAG_LASTMESSAGE );
          }
        } else {
          order.modifyOrderFlags(node_id, PAYLOAD_FLAG_LASTMESSAGE );
        }
      }
    }
}

void exit_system(void) {
    // Save data from sensordata_im and sensor_im to persistant tables
    database.sync_config();
}

void init_system(void) {
    database.initSystem();
    database.initSensor(&sensor);
    database.initNode(&node);
    database.initGateway(&gateway);
    node.printBuffer(fileno(stdout), false);
    sensor.printBuffer(fileno(stdout), false);
    gateway.printBuffer(fileno(stdout), false);
}

void process_sensor(NODE_DATTYPE node_id, uint32_t mydata) {
    uint8_t channel = getChannel(mydata);
    switch (channel) {
        case 1 ... 60: {
	    // Sensor or Actor that gets or delivers a number
            uint32_t sensor_id = sensor.getSensorByNodeChannel(node_id, channel);
            if ( sensor_id > 0 ) { 
                buf = unpackTransportValue(mydata, buf);
                if ( verboselevel & VERBOSECONFIG) {    
                    printf("%sValue of Node: %u Data: %u ==> Channel: %u is %s\n", ts(tsbuf), node_id, mydata, channel, buf);
                }
                sensor.updateLastVal(sensor_id, mydata);
                database.storeSensorValue(sensor_id, buf);
                send_fhem_cmd(node_id, channel, buf);
            }
        }
        break; 
        case 61 ... 80: {
	    // Sensor or Actor that gets or delivers a character set
            uint32_t sensor_id = sensor.getSensorByNodeChannel(node_id, channel);
            if ( sensor_id > 0 ) { 
                buf = unpackTransportValue(mydata, buf);
                if ( verboselevel & VERBOSECONFIG) {    
                    printf("%sValue of Node: %u Data: %u ==> Channel: %u is %s\n", ts(tsbuf), node_id, mydata, channel, buf);
                }
                sensor.updateLastVal(sensor_id, mydata);
                //database.storeSensorValue(sensor_id, mydata); // No need to store this
                //send_fhem_cmd(node_id, channel, buf);
            }
        }
        break; 
        case 101: {
	    // battery voltage
            uint32_t sensor_id = sensor.getSensorByNodeChannel(node_id, channel);
            if ( sensor_id > 0 ) { 
                buf = unpackTransportValue(mydata, buf);
                if ( verboselevel & VERBOSECONFIG) {    
                    printf("%sVoltage of Node: %u is %sV\n", ts(tsbuf), node_id, buf);
                }
                sensor.updateLastVal(sensor_id, mydata);
                node.setVoltage(node_id, strtof(unpackTransportValue(mydata,buf),NULL));
                database.storeSensorValue(sensor_id, buf);
                send_fhem_cmd(node_id, channel,buf);
            }
        }
        break; 
        case 102 ... 106: 
        case 108 ... 110: 
        case 112 ... 125: 
        {
	    // Node config register
            buf = unpackTransportValue(mydata, buf);
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
    cfg.removePidFile(cfg.hubPidFileName);
    exit (0);
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
    char *buf1 = (char*)malloc(TSBUFFERSIZE);
    tsbuf = (char*)malloc(TSBUFFERSIZE);
    char *gw_name=(char*)malloc(40);
    uint16_t gw_no;
    int new_tn_in_socket = 0;
    socklen_t tcp_addrlen, udp_addrlen;
    struct sockaddr_in udp_address_in, tcp_address_in;
    int udp_sockfd_in, tcp_sockfd_in;
    int msgID;
    TnMsg_t LogMsg;
    ssize_t UdpMsgLen;
    unsigned long lastDBsync = 0;

    tcp_addrlen = sizeof(tcp_address_in);
    udp_addrlen = sizeof(udp_address_in);
	
    // processing argc and argv[]
    cfg.processParams(PRGNAME, argc, argv);

	// check if started as root
	if ( getuid()!=0 ) {
        printf("%s has to be startet as user root!\n",PRGNAME);
        exit(1);
    }
    
    // check for PID file, if exists terminate else create it
    if ( cfg.checkPidFileSet(cfg.hubPidFileName) ) {
         exit(1);
    }
    printf("--------------------------------------------------\n");
    printf("%sStartup Parameters:\n",ts(tsbuf));
    cfg.printConfig_hub();
    cfg.printConfig_db();

    // init SIGTERM and SIGINT handling
    signal(SIGTERM, sighandler);
    signal(SIGINT, sighandler);

    // run as daemon if started with -d
    if (cfg.startDaemon) {
        logfile_ptr = fopen (cfg.hubLogFileName.c_str(),"a");
        if ( ! logfile_ptr ) {
            fprintf(stderr,"LOGFILE: %s can't open logfile, terminating !!!\n", cfg.hubLogFileName.c_str());
            exit(1);
        }
        fclose( logfile_ptr );
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
            cfg.removePidFile(cfg.hubPidFileName);
            exit(1);
        }

        freopen(cfg.hubLogFileName.c_str(), "a+", stdout); 
        freopen(cfg.hubLogFileName.c_str(), "a+", stderr); 
    }
    
    cfg.setPidFile(cfg.hubPidFileName);

    // connect database
    database.connect(cfg.dbHostName, cfg.dbUserName, cfg.dbPassWord, cfg.dbSchema, std::stoi(cfg.dbPortNo));

    if ( cfg.fhemPortSet && cfg.fhemHostSet ) {
        printf("%stelnet session to FHEM started: Host: %s Port: %s\n",ts(tsbuf), cfg.fhemHostName.c_str(), cfg.fhemPortNo.c_str());
    }

    // Eingehendes Socket für TCP Messages öffnen
    printf("%sSocket für eingehende TCP Messages (telnet) auf Port %s angelegt\n", ts(tsbuf), cfg.hubTcpPortNo.c_str());
    if ( ! openSocket(cfg.hubTcpPortNo.c_str(), &tcp_address_in, &tcp_sockfd_in, TCP) ) {
        printf("Error opening port !!!!\n");
        cfg.removePidFile(cfg.hubPidFileName);
        exit (0);
    }

    // Eingehendes Socket für UDP Messages öffnen
    printf("%sSocket für eingehende UDP Messages vom Gateway auf Port %s angelegt\n", ts(tsbuf), cfg.hubUdpPortNo.c_str());
    if ( ! openSocket(cfg.hubUdpPortNo.c_str(), &udp_address_in, &udp_sockfd_in, UDP) ) {
        printf("Error opening port !!!!\n");
        cfg.removePidFile(cfg.hubPidFileName);
        exit (0);
    }
    
    // Init Arrays
    node.setVerbose(verboselevel);
    sensor.setVerbose(verboselevel);
    order.setVerbose(verboselevel);
    orderbuffer.setVerbose(verboselevel);
    database.setVerbose(verboselevel);
    gateway.setVerbose(verboselevel);
    init_system();
    printf("%s%s up and running\n", ts(tsbuf),PRGNAME); 

    // Main Loop
    while(1) {
        // sync sensordata to sensordata_d every day
        if ( utime() - lastDBsync > DBSYNCINTERVAL ) {
           database.sync_sensordata_d();   
           lastDBsync = database.getBeginOfDay();
        }
        /* Handling of incoming messages */
		if ( cfg.hubTcpPortSet ) {
            new_tn_in_socket = accept ( tcp_sockfd_in, (struct sockaddr *) &tcp_address_in, &tcp_addrlen );
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
            msgID = msgget(MSGKEYHUB, IPC_CREAT | 0600);
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
// Verarbeitung von UDP Daten Eingang
    UdpMsgLen = recvfrom ( udp_sockfd_in, &udpdata, sizeof(udpdata), 0, (struct sockaddr *) &udp_address_in, &udp_addrlen );
    if (UdpMsgLen > 0) {
        memcpy(&payload, &udpdata.payload, sizeof(payload) );
        //sprintf(gw_,"%s",inet_ntoa(udp_address_in.sin_addr));
        if ( verboselevel & VERBOSEORDER ) {
            printf ("%sUDP Message from: %s \n",ts(tsbuf), inet_ntoa(udp_address_in.sin_addr));
            sprintf(buf1,"G:%u>H", udpdata.gw_no);
            printPayload(ts(tsbuf), buf1, &payload);
        }
        if ( gateway.isGateway(udpdata.gw_no) ) {
            if (payload.msg_flags & PAYLOAD_FLAG_NEEDHELP ) {
                char* tn_buf;
                tn_buf = (char*)malloc(100);
                sprintf(tn_buf,"set %s_Status 1", node.getNodeName(payload.node_id));
                send_fhem_tn(tn_buf);
                free(tn_buf);
            }
            switch ( payload.msg_type ) {
                case PAYLOAD_TYPE_INIT: { // Init message from a node!!
                    if ( node.isNewHB(payload.node_id, payload.heartbeatno, time(0)) ) {
                        process_payload(&payload);
                    } else {
                        // nothing to do here !!!   
                    }
                }
                break;
                case PAYLOAD_TYPE_ESP: { // Message from ESP Node - will not be answered
                    process_payload(&payload);                    
                }
                break;
                case PAYLOAD_TYPE_HB: { // heartbeat message!!
                   if ( node.isNewHB(payload.node_id, payload.heartbeatno, time(0)) ) {  // Got a new Heaqrtbeat -> process it!
                        database.lowVoltage(payload.node_id, payload.msg_flags & PAYLOAD_FLAG_NEEDHELP);
                        process_payload(&payload);
                        if ( node.isMasteredNode(payload.node_id) ) {
                            if ( orderbuffer.nodeHasEntry(payload.node_id) ) {  // WE have orders for this node
                                make_order(payload.node_id, PAYLOAD_TYPE_DAT);
                                if ( verboselevel & VERBOSEORDER ) {
                                    printf("%sEntries for Heartbeat Node found, sending them\n",ts(tsbuf));
                                }
                            } else {
                                if ( node.isMasteredNode(payload.node_id) ) {
                                    order.addOrder(payload.node_id, PAYLOAD_TYPE_HB_RESP, 0, mymillis());
                                    order.modifyOrderFlags(payload.node_id, PAYLOAD_FLAG_LASTMESSAGE);
                                }
                            }
                        }
                    } else {
                        // reset stop counter (TTL) for message to send
                        if ( node.isMasteredNode(payload.node_id) ) order.adjustEntryTime(payload.node_id, mymillis());
                    }
                }
                break;    
                case PAYLOAD_TYPE_DATRESP: { // Quittung für eine Nachricht vom Typ PAYLOAD_TYPE_DATNOR !!
                    if ( node.isMasteredNode(payload.node_id) ) {
                        if ( order.isOrderNo(payload.orderno) ) {
                            process_payload(&payload);
                            order.delByOrderNo(payload.orderno);  // Nachricht ist angekommen => löschen
                            if ( orderbuffer.nodeHasEntry(payload.node_id) ) {  // WE have orders for this node
                                make_order(payload.node_id, PAYLOAD_TYPE_DAT);
                                if ( verboselevel & VERBOSEORDER ) {
                                    printf("%sEntries for Heartbeat Node found, sending them\n",ts(tsbuf));
                                }
                            }
                            order.addOrder(payload.node_id, PAYLOAD_TYPE_DATSTOP, 0, mymillis());
                            order.modifyOrderFlags(payload.node_id, PAYLOAD_FLAG_LASTMESSAGE);
                        }
                    }
                }
                break;
                case PAYLOAD_TYPE_DATSTOP: { // Quittung für einen Heatbeatresponse!!
                    if ( node.isMasteredNode(payload.node_id) )
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
                    if ( node.isNewHB(payload.node_id, payload.heartbeatno, time(0)) ) {  // Got a new Heaqrtbeat -> process it!
                        sprintf(buf,"%u",node.getPaLevel(payload.node_id));
                        database.storeNodeConfig(payload.node_id, REG_PALEVEL, buf);
                    } else {
                     // nothing to do here !!!   
                    }
                }
                break;
                default: {	
                    if ( node.isMasteredNode(payload.node_id) ) {
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
			}
        }  // gateway.isGateway
    } // UDP Message > 0
//
// Orderloop: Tell the nodes what they have to do
//
		if ( order.hasEntry() ) {  // go transmitting if its time to do ..
			// Look if we have something to send
			while ( order.getOrderForTransmission(&payload, mymillis() ) ) {
                // Hier UDP Sender
                void* p_rec = NULL;
                p_rec = gateway.getGateway(p_rec, gw_name, &gw_no);
                while ( p_rec ) {
                    if ( verboselevel & VERBOSERF24) {
                        sprintf(buf1,"H>G:%u", gw_no);
                        printPayload(ts(tsbuf), buf1, &payload);
                    }
                    udpdata.gw_no=0;
                    memcpy(&udpdata.payload, &payload, sizeof(payload) );
                    sendUdpMessage(gw_name, cfg.gwUdpPortNo.c_str(), &udpdata);
                    p_rec = gateway.getGateway(p_rec, gw_name, &gw_no);
                }
            }
 			usleep(20000);
        } else {
            usleep(200000);
        }
//
//  end orderloop
//
	} // while(1)
	return 0;
}

