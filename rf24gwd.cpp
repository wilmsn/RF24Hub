#include "rf24gwd.h" 

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
    sprintf(client_message,"rf24gw> ");
    write(f->tnsocket , client_message , strlen(client_message));
    MsgLen = recv(f->tnsocket, buffer, TELNETBUFFERSIZE, 0);
    if (MsgLen>0) {
        int msgID = msgget(MSGKEYGW, IPC_CREAT | 0600);
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

	char 
         //cmp_init[]="init", 
		 //cmp_sensor[]="sensor",
		 cmp_set[]="set",
		 //cmp_setlast[]="setlast",
		 //cmp_html[]="html",
		 //cmp_order[]="order",	 
		 cmp_verbose[]="verbose",	
         //cmp_push[]="push",
         cmp_show[]="show",
         cmp_radio[]="radio",
         //cmp_sync[]="sync",
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
    //char* pEnd;
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
    // set verbose <new verboselevel>
	// sets the new verboselevel
	if (( strcmp(wort1,cmp_set) == 0 ) && (strcmp(wort2,cmp_verbose) == 0) && (strlen(wort3) > 0) && (strlen(wort4) == 0) ) {
//        if ( wort3[0] > '0' && wort3[0] < '9' + 1 ) {
			tn_input_ok = true;
			verboselevel = decodeVerbose(verboselevel, wort3);
//		}	
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
	if ( ! tn_input_ok) {
        //printf("%u \n",sizeof(tn_usage_txt)/ sizeof(int));
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

void exit_system(void) {
}

void init_system(void) {
}

/*******************************************************************************************
*
* All the rest 
*
********************************************************************************************/
void sighandler(int signal) {
    printf("%sSIGTERM: Shutting down ...\n",ts(tsbuf));
    cfg.removePidFile(cfg.gwPidFileName);
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

int main(int argc, char* argv[]) {
    pid_t pid;
    payload_t payload;
    buf = (char*)malloc(TSBUFFERSIZE);
    tsbuf = (char*)malloc(TSBUFFERSIZE);
    char* buf1 = (char*)malloc(20);
	socklen_t tcp_addrlen, udp_addrlen;
    struct sockaddr_in udp_address_in, tcp_address_in;
//    char* buf2 = (char*)malloc(20);
	/* vars for telnet socket handling */
    int udp_sockfd_in, tcp_sockfd_in;
    int new_tn_in_socket = 0;
	int msgID;
    ssize_t UdpMsgLen;
    TnMsg_t LogMsg;
    useconds_t loopSleepTime = LOOPSLEEPTIME_BUSY;
    unsigned int busyIndicator = 0;
    
    // processing argc and argv[]
    cfg.processParams(PRGNAME, argc, argv);

	// check if started as root
	if ( getuid()!=0 ) {
        printf("%s has to be startet as user root!\n",PRGNAME);
        exit(1);
    }
    
    // check for PID file, if exists terminate else create it
    if ( cfg.checkPidFileSet(cfg.gwPidFileName) ) {
         exit(1);
    }
    printf("--------------------------------------------------\n");
    printf("%sStartup Parameters:\n",ts(tsbuf));
    cfg.printConfig_gw();
    sprintf(buf1,"GW:%s",cfg.gwNo.c_str());

    // init SIGTERM and SIGINT handling
    signal(SIGTERM, sighandler);
    signal(SIGINT, sighandler);

    printf("Checking Network ..");
    memset(&udpdata, 0x00, sizeof(udpdata));
    sendUdpMessage(cfg.gwHubHostName.c_str(), cfg.hubUdpPortNo.c_str(), &udpdata); 
    printf(".OK\n");
    
    // run as daemon if started with -d
    if (cfg.startDaemon) {
        logfile_ptr = fopen (cfg.gwLogFileName.c_str(),"a");
        if ( ! logfile_ptr ) {
            fprintf(stderr,"LOGFILE: %s can't open logfile, terminating !!!\n", cfg.gwLogFileName.c_str());
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
            cfg.removePidFile(cfg.gwPidFileName);
            exit(1);
        }

        freopen(cfg.gwLogFileName.c_str(), "a+", stdout); 
        freopen(cfg.gwLogFileName.c_str(), "a+", stderr); 
    }

    cfg.setPidFile(cfg.gwPidFileName);

    // Eingehendes Socket für TCP Messages öffnen
    printf("%sSocket für eingehende TCP Messages (telnet) auf Port %s angelegt\n", ts(tsbuf), cfg.gwTcpPortNo.c_str());
    if ( ! openSocket(cfg.gwTcpPortNo.c_str(), &tcp_address_in, &tcp_sockfd_in, TCP) ) {
        printf("Error opening port !!!!\n");
        cfg.removePidFile(cfg.gwPidFileName);
        exit (0);
    }

    // Eingehendes Socket für UDP Messages öffnen
    printf("%sSocket für eingehende UDP Messages vom Gateway auf Port %s angelegt\n", ts(tsbuf), cfg.gwUdpPortNo.c_str());
    if ( ! openSocket(cfg.gwUdpPortNo.c_str(), &udp_address_in, &udp_sockfd_in, UDP) ) {
        printf("Error: Open socket !!!!!! \n");
        cfg.removePidFile(cfg.gwPidFileName);
        exit (0);
    }

    printf("%sstarting radio on channel ... %d\n", ts(tsbuf), RF24_CHANNEL);
    radio.begin();
    radio.setPALevel( RF24_PA_MAX ) ;
    radio.setChannel( RF24_CHANNEL );
    radio.setDataRate( RF24_SPEED );
    radio.setAutoAck( false );
    radio.disableDynamicPayloads();
    radio.setPayloadSize(32);
    uint8_t  rf24_node2hub[] = RF24_NODE2HUB;
    uint8_t  rf24_hub2node[] = RF24_HUB2NODE;
    radio.openWritingPipe(rf24_hub2node);
    radio.openReadingPipe(1,rf24_node2hub);
    radio.startListening();
    radio.printDetails();

    init_system();
    printf("%s%s up and running\n", ts(tsbuf), PRGNAME); 

    // Main Loop
    while(1) {
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
            msgID = msgget(MSGKEYGW, IPC_CREAT | 0600);
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
		if ( radio.available() ) {
//
// Receive loop: react on the message from the nodes
//
			radio.read(&payload,sizeof(payload));
            udpdata.gw_no = std::stoi(cfg.gwNo);
            memcpy(&udpdata.payload, &payload, sizeof(payload) );
            if ( verboselevel & VERBOSERF24) printPayload(ts(tsbuf), "N>G", &udpdata.payload);
			sendUdpMessage(cfg.gwHubHostName.c_str(), cfg.hubUdpPortNo.c_str(), &udpdata); 
            busyIndicator = 0;
		} // radio.available
//
// Orderloop: Tell the nodes what they have to do
//
    UdpMsgLen = recvfrom ( udp_sockfd_in, &udpdata, sizeof(udpdata), 0, (struct sockaddr *) &udp_address, &udp_addrlen );
    if (UdpMsgLen > 0) {
        memcpy(&payload, &udpdata.payload, sizeof(payload) );
        radio.stopListening();
        radio.flush_tx();
        radio.openWritingPipe(rf24_hub2node);
        radio.write(&payload,sizeof(payload));
        radio.startListening();
        //sprintf(buf1,"Snd:");
        if ( verboselevel & VERBOSERF24) printPayload(ts(tsbuf), "G>N", &payload);
        busyIndicator = 0;
    }
    if (busyIndicator > 10) loopSleepTime = LOOPSLEEPTIME_QUIET; else loopSleepTime = LOOPSLEEPTIME_BUSY;
    usleep(loopSleepTime);
    busyIndicator++;
//
//  end orderloop
//
	} // while(1)
	return 0;
}
