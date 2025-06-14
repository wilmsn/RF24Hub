#include "rf24hubd.h" 

void send_fhem_cmd(NODE_DATTYPE node_id, uint8_t channel, char* value) {
    char* tn_cmd = alloc_str(verboseLevel,"send_fhem_cmd tn_cmd",TELNETBUFFERSIZE,ts(tsbuf));
    char* fhem_dev;
    fhem_dev = sensorClass.getFhemDevByNodeChannel(node_id, channel); 
	sprintf(tn_cmd,"set %s %s", fhem_dev, value);
    send_fhem_tn(tn_cmd);
    free_str(verboseLevel, "send_fhem_cmd tn_cmd", tn_cmd,ts(tsbuf));
}

// send_fhem_tn ==> send a telnet comand to the fhem-host
// usage example: send_fhem_cmd("set device1 on");

void send_fhem_tn(char* tn_cmd) {
    char tn_quit[] = "\r\nquit\r\n";
    int sockfd, portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;
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
                printf("%sERROR: send_fhem_tn: error connecting\n", ts(tsbuf));
            } else {
                if (write(sockfd,tn_cmd,strlen(tn_cmd)) < 1) {
                    printf("%sERROR: send_fhem_tn: error writing to socket for telnet command\n", ts(tsbuf));
                } else {
                    if ( verboseLevel & VERBOSETELNET) {
                        printf("%ssend_fhem_tn: Telnet to %s Port %u successfull; Command: %s\n", ts(tsbuf),
                               cfg.fhemHostName.c_str(), portno, tn_cmd);
                    }
                }
                if (write(sockfd,tn_quit,strlen(tn_quit)) < 1) {
                // TODO: Need a message here?
                    printf("%sERROR: send_fhem_tn: error write socket for telnet quit command\n", ts(tsbuf));
                }
            }
            close(sockfd);
        } else { // server == NULL
            printf("%sERROR: send_fhem_tn: no such host\n", ts(tsbuf));
        }
    } else { // sockfd == 0
        printf("%sERROR: send_fhem_tn: error opening socket\n", ts(tsbuf));
	}	
}

/* Die Thread-Funktion fuer den Empfang von telnet Daten*/
static void* receive_tn_in (void *arg) {
    struct ThreadTnData_t *f = (struct ThreadTnData_t *)arg;
    char* buffer = alloc_str(verboseLevel,"receive_tn_in buffer",TELNETBUFFERSIZE,ts(tsbuf));
    char* client_message = alloc_str(verboseLevel,"receive_tn_in client_message",DEBUGSTRINGSIZE,ts(tsbuf));
    TnMsg_t TnMsg;
    // send something like a prompt. perl telnet is waiting for it otherwise we get error
    // use this in perl: my $t = new Net::Telnet (Timeout => 2, Port => 7001, Prompt => '/rf24hub>/');
    sprintf(client_message,"rf24hub> ");
    if (write(f->tnsocket , client_message , strlen(client_message)) < 1 ) {
        printf("%sERROR: receive_tn_in: write socket for prompt\n", ts(tsbuf));
    }
    if (recv(f->tnsocket, buffer, TELNETBUFFERSIZE, 0) > 0) {
        int msgID = msgget(MSGKEYHUB, IPC_CREAT | 0600);
//        TnMsg.mtype = 1;
//        TnMsg.TnData.tn_socket = 0;
//        if (msgID >= 0) msgsnd(msgID, &TnMsg, sizeof(TnMsg), 0);
        TnMsg.mtype = 2;
        TnMsg.TnData.tn_socket = f->tnsocket;
        sprintf(TnMsg.TnData.address,"%s",f->address);
        sprintf(TnMsg.TnData.tntext,"%s", trim(buffer));
        if (msgID >= 0) msgsnd(msgID, &TnMsg, sizeof(TnMsg), 0);
        // Wait for a message with processing result            
        if (msgrcv(msgID, &TnMsg, sizeof(TnMsg), 9, 0) > 5) {
            int strSize = DEBUGSTRINGSIZE+5;
            char tntext[strSize];
            snprintf(tntext,strSize,"%s\n",TnMsg.TnData.tntext);
            if (write(f->tnsocket, tntext, strlen(tntext)) < 1) {
                printf("%sERROR: receive_tn_in: write socket for response\n", ts(tsbuf));
            }
        }
    }
    sleep(1);
    close (f->tnsocket);
    free(f);
    free_str(verboseLevel,"receive_tn_in buffer",buffer,ts(tsbuf));
    free_str(verboseLevel,"receive_tn_in client_message",client_message,ts(tsbuf));
    pthread_exit((void *)pthread_self());
}
	
/* Die Thread-Funktion fuer die Abarbeitung der angesammelten SQL Statements*/
static void* exec_sql (void *arg) {
    struct db_data_t *db_data = (struct db_data_t *)arg;
    db_data->p_database->exec_sql(db_data->sql);
    free(db_data->sql);
    free(db_data);
    pthread_exit((void *)pthread_self());
}

/* Die Thread-Funktion fuer den Speicherung der Sensor Daten*/

int process_tn_in( char* inbuffer, int tn_socket, char* tn_address) {
/* Messages can look like this:
    word1		word2		word3		word4 				function
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
    char* message = alloc_str(verboseLevel,"process_tn_in message",120,ts(tsbuf));
    bool tn_input_ok = false;
    bool sensor_ok = true;
    char delimiter[] = " ";
    int retval = 0;
	//trim(buffer);
    wort1a = strtok(inbuffer, delimiter);
    wort2a = strtok(NULL, delimiter);
    wort3a = strtok(NULL, delimiter);
    wort4a = strtok(NULL, delimiter);
    char* pEnd;
    if (wort1a) {
        wort1 = alloc_str(verboseLevel,"process_tn_in wort1",strlen(wort1a)+1,ts(tsbuf));
        strcpy(wort1, wort1a);
    } else {
        wort1 = alloc_str(verboseLevel,"process_tn_in wort1",1,ts(tsbuf));
        wort1[0] = '\0';
    }
    if (wort2a) {
        wort2 = alloc_str(verboseLevel,"process_tn_in wort2",strlen(wort2a)+1,ts(tsbuf));
        strcpy(wort2, wort2a);
    } else {
        wort2 = alloc_str(verboseLevel,"process_tn_in wort2",1,ts(tsbuf));
        wort2[0] = '\0';
    }
    if (wort3a) {
        wort3 = alloc_str(verboseLevel,"process_tn_in wort3",strlen(wort3a)+1,ts(tsbuf));
        strcpy(wort3, wort3a);
    } else {
        wort3 = alloc_str(verboseLevel,"process_tn_in wort3",1,ts(tsbuf));
        wort3[0] = '\0';
    }
    if (wort4a) {
        wort4 = alloc_str(verboseLevel,"process_tn_in wort4",strlen(wort4a)+1,ts(tsbuf));
        strcpy(wort4, wort4a);
    } else {
        wort4 = alloc_str(verboseLevel,"process_tn_in wort4",1,ts(tsbuf));
        wort4[0] = '\0';
    }
    if ( (strcmp(wort1,cmp_set) == 0) && (strcmp(wort2,cmp_sensor) == 0) && (strlen(wort3) > 0) && (strlen(wort4) > 0) ) {
    // In word3 we may have a) the number of the sensor b) the name of the sensor c) the fhem_dev of a sensor
    // for the processing we need the number of the sensor ==> find it!
        NODE_DATTYPE node_id = 0;
        uint8_t channel = 0;
        tn_input_ok = true;
        sensor_ok = false;
        if ( wort3[0] >= 'A' && wort3[0] <= 'z' ) {
            if (sensorClass.getNodeChannelByFhemDev(&node_id, &channel, wort3) ) {
                sensor_ok = true;
            } else {
                if (sensorClass.getNodeChannelBySensorName(&node_id, &channel, wort3) ) {
                    sensor_ok = true;
                }
            }
        } else {
            uint32_t sensor_id = strtol(wort3, &pEnd, 10);
            if ( sensorClass.getNodeChannelBySensorID(&node_id, &channel, sensor_id ) ) {
                sensor_ok = true;
            }
        }
        if ( node_id > 0 &&  channel > 0 ) {
            switch ( sensorClass.getDataTypeByNodeChannel(node_id, channel) ) {
                case 0: {
                    float val_f = strtof(wort4, &pEnd);
                    orderbuffer.addOrderBuffer(mymillis(), node_id, channel, calcTransportValue(channel, val_f) );
                }
                break;
                case 1: {
                    orderbuffer.addOrderBuffer(mymillis(), node_id, channel, calcTransportValue(channel,
                        (int16_t)strtol(wort4, &pEnd, 10)) );
                }
                break;
                case 2: {
                    orderbuffer.addOrderBuffer(mymillis(), node_id, channel, calcTransportValue(channel,
                        (uint16_t)strtoul(wort4, &pEnd, 10)) );
                }
                break;
                default:
                    sensor_ok = false;
            }
        }
    }
    // push <node> <channel> <value>
    // Pushes a value direct to a channel into a node
    if (( strcmp(wort1,cmp_push) == 0 ) && (strlen(wort2) > 0) && (strlen(wort3) > 0) && (strlen(wort4) > 0) ) {
        NODE_DATTYPE node_id = strtol(wort2, &pEnd, 10);
        uint8_t channel = strtol(wort3, &pEnd, 10);
        if ( nodeClass.isValidNode(node_id) ) {
            orderbuffer.addOrderBuffer(mymillis(), node_id, channel, calcTransportValue(channel, wort4) );
            tn_input_ok = true;
        }
    }
    // set verbose <new verboseLevel>
    // sets the new verboseLevel
    if (( strcmp(wort1,cmp_set) == 0 ) && (strcmp(wort2,cmp_verbose) == 0) && (strlen(wort3) > 0) && (strlen(wort4) == 0) ) {
        tn_input_ok = true;
        verboseLevel = decodeVerbose(verboseLevel, wort3);
        nodeClass.setVerbose(verboseLevel);
        sensorClass.setVerbose(verboseLevel);
        order.setVerbose(verboseLevel);
        orderbuffer.setVerbose(verboseLevel);
        database.setVerbose(verboseLevel);
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
        sensorClass.printBuffer(tn_socket, false);
    }
    // show sensor
    // lists the current node- and sensorbuffer
    if (( strcmp(wort1,cmp_show) == 0 ) && (strcmp(wort2,cmp_node) == 0) && (strlen(wort3) == 0) && (strlen(wort4) == 0) ) {
        tn_input_ok = true;
        nodeClass.printBuffer(tn_socket, false);
    }
    // show gateway
    // lists the current gateway status
    if (( strcmp(wort1,cmp_show) == 0 ) && (strcmp(wort2,cmp_gateway) == 0) && (strlen(wort3) == 0) && (strlen(wort4) == 0) ) {
        tn_input_ok = true;
        gatewayClass.printBuffer(tn_socket, false);
    }
    // add gateway
    // adds a gateway in status active to database and system
    if (( strcmp(wort1,cmp_add) == 0 ) && (strcmp(wort2,cmp_gateway) == 0) && (strlen(wort3) > 0) && (strlen(wort4) > 0) ) {
        if ( wort3[0] >= 'A' && wort3[0] <= 'z' ) {
            uint16_t gw_id = strtol(wort4, &pEnd, 10);
            if ( gw_id > 0 ) {
                tn_input_ok = true;
                database.addGateway(wort3, gw_id);
                gatewayClass.addGateway(wort3, gw_id, true);
                gatewayClass.printBuffer(tn_socket, false);
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
            gatewayClass.delGateway(gw_id);
            gatewayClass.printBuffer(tn_socket, false);
        }
    }
    // set gateway <GW_NO> on
    // lists the current node- and sensorbuffer
    if (( strcmp(wort1,cmp_set) == 0 ) && (strcmp(wort2,cmp_gateway) == 0) && (strlen(wort3) > 0) && (strcmp(wort4,cmp_on) == 0) ) {
        tn_input_ok = true;
        uint16_t gw_id = (uint16_t)strtoul(wort3, &pEnd, 10);
        database.enableGateway(gw_id);
        gatewayClass.setGateway(gw_id, true);
        gatewayClass.printBuffer(tn_socket, false);
    }
    // set gateway <GW_NO> off
    // lists the current node- and sensorbuffer
    if (( strcmp(wort1,cmp_set) == 0 ) && (strcmp(wort2,cmp_gateway) == 0) && (strlen(wort3) > 0) && (strcmp(wort4,cmp_off) == 0) ) {
        tn_input_ok = true;
        uint16_t gw_id = (uint16_t)strtoul(wort3, &pEnd, 10);
        database.disableGateway(gw_id);
        gatewayClass.setGateway(gw_id, false);
        gatewayClass.printBuffer(tn_socket, false);
    }
    // set node <node_id> <mastered/unmastered>
    if ( (strcmp(wort1,cmp_set) == 0) && (strcmp(wort2,cmp_node) == 0) && (strlen(wort3) > 0) && (strlen(wort4) > 0) ) {
        NODE_DATTYPE node_id = strtol(wort3, &pEnd, 10);
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
        sprintf(message,"Active verboseLevel: %s\n", printVerbose(verboseLevel, buf));
        if ( write(tn_socket , message , strlen(message)) < 1 ) {
            // TODO: Need a message here?
            printf("#3 Error in write socket länge < 1\n");
        }
    }
    // html order
    // lists the current order/orderbuffer for html page
    if (( strcmp(wort1,cmp_html) == 0 ) && (strcmp(wort2,cmp_order) == 0) && (strlen(wort3) == 0) && (strlen(wort4) == 0) ) {
        tn_input_ok = true;
        orderbuffer.printBuffer(tn_socket, true);
    }	
    // init
    // initialisation of rf24hubd: reloads data from database
    if ( (strcmp(wort1,cmp_init) == 0) && (strlen(wort2) == 0) && (strlen(wort3) == 0) && (strlen(wort4) == 0) ) {
        tn_input_ok = true;
        exit_system();
        nodeClass.cleanup();
        sensorClass.cleanup();
        gatewayClass.cleanup();
        init_system();
        nodeClass.printBuffer(tn_socket, false);
        sensorClass.printBuffer(tn_socket, false);
        gatewayClass.printBuffer(tn_socket, false);
    }
    // sync
    if ( (strcmp(wort1,cmp_sync) == 0) && (strlen(wort2) == 0) && (strlen(wort3) == 0) && (strlen(wort4) == 0) ) {
        tn_input_ok = true;
        // syncs all nodes to the system
        nodeClass.cleanup();
        database.initNode(&nodeClass);
        // syncs all sensors to the system
        sensorClass.cleanup();
        database.initSensor(&sensorClass);        
        // syncs all relevant database tables
        database.sync_sensordata_d();        
    }
    // truncate logfile
    // truncation of the logfile for maintenance
    if ( (strcmp(wort1,cmp_truncate) == 0) && (strcmp(wort2,cmp_logfile) == 0) && (strlen(wort3) == 0) && (strlen(wort4) == 0) ) {
        if( truncate (cfg.hubLogFileName.c_str(), 0))
        printf(message, "Logfile: %s geleert", cfg.hubLogFileName.c_str());
        tn_input_ok = true;
    }
    if ( ! tn_input_ok) {
        retval = 1;
        bool cont = true;
        printf("%sTN-Input: %s from %s\n",ts(tsbuf),inbuffer, tn_address);
        for(unsigned int i=0; i < tn_usage_size; i++) {
            sprintf(message,"%s\n",tn_usage_txt[i]);
            if ( strlen(message) > 1 && cont ) {
                if (write(tn_socket , message , strlen(message)) < 1) {
                    //TODO 
                    printf("%s #4 Error in write socket länge < 1\n",ts(tsbuf));
                    cont = false;
                }
            }
        }
        sprintf(message,"%s version %s\n", PRGNAME, SWVERSION_STR);
        if ( strlen(message) > 1 && cont ) {
            if (write(tn_socket , message , strlen(message)) < 1) {
                // TODO
                printf("%s #5 Error in write socket länge < 1\n",ts(tsbuf));
            }
        }
    }
    if ( ! sensor_ok ) retval = 2;
    free_str(verboseLevel,"process_tn_in wort1",wort1,ts(tsbuf));
    free_str(verboseLevel,"process_tn_in wort2",wort2,ts(tsbuf));
    free_str(verboseLevel,"process_tn_in wort3",wort3,ts(tsbuf));
    free_str(verboseLevel,"process_tn_in wort4",wort4,ts(tsbuf));
    free_str(verboseLevel,"process_tn_in message", message,ts(tsbuf));
    return retval;
}

void make_order(NODE_DATTYPE node_id, uint8_t msg_type) {
    void* ret_ptr;
    uint32_t data;
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

void exit_system(void) {
    // Nothing to do right now
}

void init_system(void) {
    database.initSystem();
    database.initSensor(&sensorClass);
    database.initNode(&nodeClass);
    database.initGateway(&gatewayClass);
    nodeClass.printBuffer(fileno(stdout), false);
    sensorClass.printBuffer(fileno(stdout), false);
    gatewayClass.printBuffer(fileno(stdout), false);
}

void process_sensor(NODE_DATTYPE node_id, uint32_t mydata) {
    uint8_t channel = getChannel(mydata);
    switch (channel) {
        case SENSOR_CHANNEL_FIRST ... SENSOR_CHANNEL_LAST: {
	    // Sensor or Actor that gets or delivers a number
            uint32_t sensor_id = sensorClass.getSensorByNodeChannel(node_id, channel);
            if ( sensor_id > 0 ) {
                buf = unpackTransportValue(mydata, buf);
                float val;
                getValue(mydata, &val);
                if ( channel == SENSOR_BATT ) {
                    nodeClass.setVoltage(node_id, val);
                    if ( verboseLevel & VERBOSECONFIG) {
                        printf("%sVoltage of Node: %u is %.2fV\n", ts(tsbuf), node_id, val);
                    }
                } else {
                    if ( verboseLevel & VERBOSECONFIG) {
                        printf("%sValue of Node: %u Data: %u ==> Channel: %u is %.2f\n", ts(tsbuf), node_id, mydata, channel, val);
                    }
                }
                sensorClass.updateLastVal(sensor_id, mydata);
                database.storeSensorValue(sensor_id, buf);
                send_fhem_cmd(node_id, channel, buf);
            }
        }
        break; 
// Register mit Sonderbedandlung: Durchlaufen erst die Sonderbedandlung dann die normale Registerbehandlung
        case REG_NOSTORE_FIRST ... REG_NOSTORE_LAST:
	    // do nothing
        break;
        case REG_NORMAL_FIRST ... REG_NORMAL_LAST: 
        case REG_READONLY_FIRST ... REG_READONLY_LAST: 
        {
        // Node config register
            buf = unpackTransportValue(mydata, buf);
            if ( verboseLevel & VERBOSECONFIG) {
                printf("%sConfigregister of Node: %u Channel: %u is %s\n", ts(tsbuf), node_id, channel, buf);
            }
            database.storeNodeConfig(node_id, channel, buf);
            if ( channel == REG_VOLT_LV ) {
                float volt_lv;
                getValue(mydata, &volt_lv);
                nodeClass.setLVVolt(node_id, volt_lv);
                database.setLVVolt(node_id, volt_lv);
            }
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
    process_sensor(mypayload->node_id, mypayload->data1);
    process_sensor(mypayload->node_id, mypayload->data2);
    process_sensor(mypayload->node_id, mypayload->data3);
    process_sensor(mypayload->node_id, mypayload->data4);
    process_sensor(mypayload->node_id, mypayload->data5);
    process_sensor(mypayload->node_id, mypayload->data6);
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
            if (chdir ("/"));
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

        if(freopen(cfg.hubLogFileName.c_str(), "a+", stdout)); 
        if(freopen(cfg.hubLogFileName.c_str(), "a+", stderr)); 
    }
    
    cfg.setPidFile(cfg.hubPidFileName);

    // init database Class
    database.set_var(cfg.dbHostName.c_str(), cfg.dbUserName.c_str(), cfg.dbPassWord.c_str(), cfg.dbSchema.c_str(), std::stoi(cfg.dbPortNo));

    if ( cfg.fhemPortSet && cfg.fhemHostSet ) {
        printf("%stelnet session to FHEM started: Host: %s Port: %s\n",ts(tsbuf), cfg.fhemHostName.c_str(), cfg.fhemPortNo.c_str());
    }

    // Eingehendes Socket für TCP Messages öffnen
    if ( ! openSocket(cfg.hubTcpPortNo.c_str(), &tcp_address_in, &tcp_sockfd_in, TCP) ) {
        printf("#6 Error opening port !!!!\n");
        cfg.removePidFile(cfg.hubPidFileName);
        exit (0);
    } else {
        printf("%sSocket für eingehende TCP Messages (telnet) auf Port %s angelegt\n", ts(tsbuf), cfg.hubTcpPortNo.c_str());
    }    

    // Eingehendes Socket für UDP Messages öffnen
    if ( ! openSocket(cfg.hubUdpPortNo.c_str(), &udp_address_in, &udp_sockfd_in, UDP) ) {
        printf("#7 Error opening port !!!!\n");
        cfg.removePidFile(cfg.hubPidFileName);
        exit (0);
    } else {
        printf("%sSocket für eingehende UDP Messages vom Gateway auf Port %s angelegt\n", ts(tsbuf), cfg.hubUdpPortNo.c_str());
    }
    
    // Init Arrays
    printf("%s Setting up tables - this may take a while ...\n", ts(tsbuf)); 
    nodeClass.setVerbose(verboseLevel);
    sensorClass.setVerbose(verboseLevel);
    order.setVerbose(verboseLevel);
    orderbuffer.setVerbose(verboseLevel);
    database.setVerbose(verboseLevel);
    gatewayClass.setVerbose(verboseLevel);
    init_system();
    printf("%s%s up and running\n", ts(tsbuf),PRGNAME); 

    // Main Loop
    while(1) {
        // DB_SYNC:
        // sync sensordata to sensordata_d every day
        if ( time(NULL) - lastDBsync > DBSYNCINTERVAL ) {
           database.sync_sensordata_d();   
           lastDBsync = database.getBeginOfDay();
        }
        // DB_TREAD
        // Processing of stored SQL statements
        pthread_t db_thread;
        uint64_t myts;
        if ( database.testDB() ) {
            char* mysql = database.getSQL(&myts);
            if ( myts > 0 ) {
                char* sqlstmt = (char*)malloc(strlen(mysql)+1);
                sprintf(sqlstmt,"%s",mysql);
                database.delSQL(myts);
                struct db_data_t *db_data;
                db_data = (struct db_data_t *)malloc(sizeof(struct db_data_t));
                db_data->sql = sqlstmt;
                db_data->p_database = &database;
                if ( pthread_create(&db_thread, NULL, &exec_sql, db_data) == 0) {
                    pthread_detach(db_thread);
                }
            }
        }
        // TN_THREAD
        // Handling of incoming telnet messages
        if ( cfg.hubTcpPortSet ) {
            // Hier wird geprüft ob neue Telnet Verbindungen anstehen.
            // Wenn ja wird ein Thread für die neue Verbindung geöffnet.
            new_tn_in_socket = accept ( tcp_sockfd_in, (struct sockaddr *) &tcp_address_in, &tcp_addrlen );
            if (new_tn_in_socket > 0) {
                pthread_t tn_thread;
                struct ThreadTnData_t *f;
                f = (struct ThreadTnData_t *)malloc(sizeof(struct ThreadTnData_t));
                f->tnsocket = new_tn_in_socket;
                snprintf(f->address, 19, "%s", inet_ntoa(tcp_address_in.sin_addr));
                if ( pthread_create(&tn_thread, NULL, &receive_tn_in, f) == 0) {
                    pthread_detach(tn_thread);
                }
            }
            // IPC_EXEC_TELNET_CMD
            // Die im Thread empfangene Telnetnachricht wird als IPC-Message zurückgegeben.
            // Die Verarbeitung des empfangenen Befehls erfolgt im Hauptprogramm!
            // Der Thread selber nimmt keine Veränderung an den Programmkomponenten vor!
            msgID = msgget(MSGKEYHUB, IPC_CREAT | 0600);
            if (msgID >= 0) {
                // Eine Nachricht vom mtype = 2 enthält den Telnetbefehl
                // ==> process, store into orderbuffer and make order
                if (msgrcv(msgID, &LogMsg, sizeof(LogMsg), 2, IPC_NOWAIT) > 5 ) {
                    if ( verboseLevel & VERBOSETELNET ) {
                        printf("%sTelnet message from %s: %s\n", ts(tsbuf), LogMsg.TnData.address, LogMsg.TnData.tntext);
                    }
                    char* tnstr = alloc_str(verboseLevel,"main tnstr",DEBUGSTRINGSIZE,ts(tsbuf));
                    strcpy(tnstr, LogMsg.TnData.tntext);
                    int result = process_tn_in(tnstr,LogMsg.TnData.tn_socket,LogMsg.TnData.address);
                    free_str(verboseLevel,"main tnstr",tnstr,ts(tsbuf));
                    // Das Ergebnis der Verarbeitung wird als mtype = 9 an den Telnetthread zurückgeschickt.
                    // Dieser beendet sich daraufhin.
                    LogMsg.mtype = 9;
                    switch ( result ) {
                        case 1:
                            sprintf(LogMsg.TnData.tntext,"Command received => Syntax Error");
                        case 2:
                            sprintf(LogMsg.TnData.tntext,"Command received => Sensor unknown");
                        break;
                        default:
                        sprintf(LogMsg.TnData.tntext,"Command received => OK");
                    }
                    if (msgID >= 0) msgsnd(msgID, &LogMsg, sizeof(LogMsg), 0);
                }
            }
        }
        // UDP_EXEC_DATA
        // Verarbeitung von UDP Daten Eingang
        UdpMsgLen = recvfrom ( udp_sockfd_in, &udpdata, sizeof(udpdata), 0, (struct sockaddr *) &udp_address_in, &udp_addrlen );
        if (UdpMsgLen > 0) {
            memcpy(&payload, &udpdata.payload, sizeof(payload) );
            if ( gatewayClass.isGateway(udpdata.gw_no) && nodeClass.isValidNode(payload.node_id) && (udpdata.utime + 30) > time(0)) {
                if ( verboseLevel & VERBOSERF24 ) {
                    printf ("%sUDP Message from: %s \n",ts(tsbuf), inet_ntoa(udp_address_in.sin_addr));
                    sprintf(buf1,"[%lu][%lu]G:%u>H", time(0), udpdata.utime, udpdata.gw_no);
                    printPayload(ts(tsbuf), buf1, &payload);
                }
                if (nodeClass.setLVFlag(payload.node_id, payload.msg_flags & PAYLOAD_FLAG_LOWVOLTAGE )) {
                    char* tn_buf;
                    tn_buf = (char*)malloc(100);
                    sprintf(tn_buf,"set %s_Status 1", nodeClass.getNodeName(payload.node_id));
                    send_fhem_tn(tn_buf);
                    free(tn_buf);
                    database.setLVFlag(payload.node_id, payload.msg_flags & PAYLOAD_FLAG_LOWVOLTAGE);
                }
                switch ( payload.msg_type ) {
                    case PAYLOAD_TYPE_INIT: { // Init message from a node!!
                        if ( nodeClass.isNewHB(payload.node_id, payload.heartbeatno, time(0)) ) {
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
                    case PAYLOAD_TYPE_HB: { // Initial heartbeat message!!
                        if ( nodeClass.isNewHB(payload.node_id, payload.heartbeatno, time(0)) ) {
                        // Got a new Heaqrtbeat for a mastered Node-> process it!
                            process_payload(&payload);
                            if ( orderbuffer.nodeHasEntry(payload.node_id) ) {
                                // WE have orders for this node
                                make_order(payload.node_id, PAYLOAD_TYPE_DAT);
                                if ( verboseLevel & VERBOSEORDER ) {
                                    printf("%sEntries for Node found, sending them\n",ts(tsbuf));
                                }
                            } else {
                                order.addOrder(payload.node_id, PAYLOAD_TYPE_HB_RESP, 0, mymillis());
                                order.modifyOrderFlags(payload.node_id, PAYLOAD_FLAG_LASTMESSAGE);
                            }
                        } else {
                        // reset stop counter (TTL) for message to send
                            order.adjustEntryTime(payload.node_id, mymillis());
                        }
                    }
                    break;
                    case PAYLOAD_TYPE_HB_F: { // Followup heartbeat message!!
                        if ( nodeClass.isCurHB(payload.node_id, payload.heartbeatno) ) {
                        // Got a current Heaqrtbeat for a mastered Node-> process it!
                            process_payload(&payload);
                            order.addOrder(payload.node_id, PAYLOAD_TYPE_HB_F_RESP, 0, mymillis());
                            order.modifyOrderFlags(payload.node_id, PAYLOAD_FLAG_LASTMESSAGE);
                        } else {
                        // reset stop counter (TTL) for message to send
                            order.adjustEntryTime(payload.node_id, mymillis());
                        }
                    }
                    break;
                    case PAYLOAD_TYPE_DATRESP: { // Quittung für eine Nachricht vom Typ PAYLOAD_TYPE_DATNOR !!
                        if ( order.isOrderNo(payload.orderno) ) {
                            process_payload(&payload);
                            order.delByOrderNo(payload.orderno);  // Nachricht ist angekommen => löschen
                            if ( orderbuffer.nodeHasEntry(payload.node_id) ) {
                                // WE have orders for thisnode
                                make_order(payload.node_id, PAYLOAD_TYPE_DAT);
                                if ( verboseLevel & VERBOSEORDER ) {
                                    printf("%sEntries for Heartbeat Node found, sending them\n",ts(tsbuf));
                                }
                            }
                            order.addOrder(payload.node_id, PAYLOAD_TYPE_DATSTOP, 0, mymillis());
                            order.modifyOrderFlags(payload.node_id, PAYLOAD_FLAG_LASTMESSAGE);
                        }
                    }
                    break;
                    case PAYLOAD_TYPE_DATSTOP: { 
                        // Quittung für einen Heatbeatresponse!!
                        order.delByOrderNo(payload.orderno);  // Nachricht ist angekommen => löschen
                    }
                    break;
                    case PAYLOAD_TYPE_PING_POW_MIN: {
                        nodeClass.setRecLevel(payload.node_id, 0);
                    }
                    break;
                    case PAYLOAD_TYPE_PING_POW_LOW: {
                        nodeClass.setRecLevel(payload.node_id, 1);
                    }
                    break;
                    case PAYLOAD_TYPE_PING_POW_HIGH: {
                        nodeClass.setRecLevel(payload.node_id, 2);
                    }
                    break;
                    case PAYLOAD_TYPE_PING_POW_MAX: {
                        nodeClass.setRecLevel(payload.node_id, 3);
                    }
                    break;
                    case PAYLOAD_TYPE_PING_END: {
                        if ( nodeClass.isNewHB(payload.node_id, payload.heartbeatno, time(0)) ) {  
                            // Got a new Heaqrtbeat -> process it!
                            sprintf(buf,"%u",nodeClass.getRecLevel(payload.node_id));
                            database.storeNodeConfig(payload.node_id, REG_RECLEVEL, buf);
                        } else {
                        // nothing to do here !!!   
                        }
                    }
                    break;
                    default: {	
                        if ( verboseLevel & VERBOSEORDER) {
                            printf("%sProcessing Node:%u Type:%u Orderno: %u\n", ts(tsbuf), payload.node_id, payload.msg_type,  payload.orderno);
                        }
                        if ( order.isOrderNo(payload.orderno) ) {
                            process_payload(&payload);
                            order.delByOrderNo(payload.orderno);
                            if ( orderbuffer.nodeHasEntry(payload.node_id) ) {
                                // WE have orders for this node
                                make_order(payload.node_id, PAYLOAD_TYPE_DAT);
                                if ( verboseLevel & VERBOSEORDER ) {
                                    printf("%sEntries for Heartbeat Node found, sending them\n", ts(tsbuf));
                                }
                            }
                        }
                    }
                }
            } else { // isGateway && isValidNode
                if ( verboseLevel & VERBOSERF24 ) {
                    printf ("%sUDP Message from: %s for Node %u dropped\n",ts(tsbuf), inet_ntoa(udp_address_in.sin_addr),
                        udpdata.payload.node_id);
                }
            }
        } // UDP Message > 0
        // ORDERLOOP
        // Tell the nodes what they have to do
        if ( order.hasEntry() ) {  // go transmitting if its time to do ..
            // Look if we have something to send
            while ( order.getOrderForTransmission(&payload, mymillis() ) ) {
                // Hier UDP Sender
                void* p_rec = NULL;
                p_rec = gatewayClass.getGateway(p_rec, gw_name, &gw_no);
                while ( p_rec ) {
                    if ( verboseLevel & VERBOSERF24) {
                        sprintf(buf1,"H>G:%u", gw_no);
                        printPayload(ts(tsbuf), buf1, &payload);
                    }
                    udpdata.gw_no=0;
                    memcpy(&udpdata.payload, &payload, sizeof(payload) );
                    sendUdpMessage(gw_name, cfg.gwUdpPortNo.c_str(), &udpdata);
                    p_rec = gatewayClass.getGateway(p_rec, gw_name, &gw_no);
                }
            }
            usleep(20000);
        } else {
            usleep(200000);
        }
    } // while(1)
    return 0;
}

