#include "rf24gwd.h" 


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
    cfg.removePidFile(cfg.gw_pidFileName);
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

/*
void sniffer(void) {
    payload_t payload;
    radio.flush_tx();
    radio.setCRCLength(RF24_CRC_16);
    while (true) {
        if ( radio.available() ) {
            if ( radio.isValid() ) {
                radio.read(&payload,sizeof(payload));
                printPayload(0xffff, "Rec", " ", &payload);
            } else {
                radio.flush_rx();
            }
        } else {  
            radio.flush_rx();
            usleep(10000);
        }
    }
}
*/
void printPayload(char* msg_header, payload_t* mypayload) {
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
        printf("%s%s: N:%u T:%u m:%u F:0x%02X O:%u H:%u (%u/%s)(%u/%s)(%u/%s)(%u/%s)(%u/%s)(%u/%s)\n",
               ts(tsbuf), msg_header, mypayload->node_id, mypayload->msg_type, mypayload->msg_id, mypayload->msg_flags, mypayload->orderno, mypayload->heartbeatno,
               getChannel(mypayload->data1), vbuf1,
               getChannel(mypayload->data2), vbuf2,
               getChannel(mypayload->data3), vbuf3,
               getChannel(mypayload->data4), vbuf4,
               getChannel(mypayload->data5), vbuf5,
               getChannel(mypayload->data6), vbuf6);   
        free_str(verboselevel,"vbuf1",vbuf1,ts(tsbuf));
        free_str(verboselevel,"vbuf2",vbuf2,ts(tsbuf));
        free_str(verboselevel,"vbuf3",vbuf3,ts(tsbuf));
        free_str(verboselevel,"vbuf4",vbuf4,ts(tsbuf));
        free_str(verboselevel,"vbuf5",vbuf5,ts(tsbuf));
        free_str(verboselevel,"vbuf6",vbuf6,ts(tsbuf));
}

int main(int argc, char* argv[]) {
    pid_t pid;
    payload_t payload;
    char* buf1 = (char*)malloc(20);
//    char* buf2 = (char*)malloc(20);
    tsbuf = (char*)malloc(TSBUFFERSIZE);
	/* vars for telnet socket handling */
    struct sockaddr_in udp_address_in;
    int udp_sockfd_in;
    socklen_t len;
    ssize_t UdpMsgLen;

    // processing argc and argv[]
    cfg.processParams(PRGNAME, argc, argv);

	// check if started as root
	if ( getuid()!=0 ) {
        printf("%s has to be startet as user root!\n",PRGNAME);
        exit(1);
    }
    
    // check for PID file, if exists terminate else create it
    if ( cfg.checkPidFileSet(cfg.gw_pidFileName) ) {
         exit(1);
    } else {
         cfg.setPidFile(cfg.gw_pidFileName);
    }
    printf("--------------------------------------------------\n");
    printf("%sStartup Parameters:\n",ts(tsbuf));
    cfg.printConfig_gw();
    sprintf(buf1,"GW:%s",cfg.gw_gwID.c_str());
//    sprintf(buf2,"");
    // init SIGTERM and SIGINT handling
    signal(SIGTERM, sighandler);
    signal(SIGINT, sighandler);

    // run as daemon if started with -d
    if (cfg.startDaemon) {
        logfile_ptr = fopen (cfg.gw_logFileName.c_str(),"a");
        if ( ! logfile_ptr ) {
            fprintf(stderr,"LOGFILE: %s can't open logfile, terminating !!!\n", cfg.gw_logFileName.c_str());
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
            cfg.removePidFile(cfg.gw_pidFileName);
            exit(1);
        }

        freopen(cfg.gw_logFileName.c_str(), "a+", stdout); 
        freopen(cfg.gw_logFileName.c_str(), "a+", stderr); 
    }
    
/*    if ( cfg.startSniffer ) {
        radio.begin();
        radio.setPALevel( RF24_PA_MAX ) ;
        radio.setChannel( RF24_CHANNEL );
        radio.setDataRate( RF24_SPEED );
        radio.setAutoAck( false );
        radio.disableDynamicPayloads();
        radio.setPayloadSize(32);
        uint8_t  rf24_node2hub[] = RF24_NODE2HUB;
        uint8_t  rf24_hub2node[] = RF24_HUB2NODE;
        radio.openReadingPipe(0,rf24_node2hub);
        radio.openReadingPipe(1,rf24_hub2node);
        radio.startListening();
        radio.printDetails();
        printf("\n");
        printf("%ssniffing on radio on channel ... %d\n", ts(tsbuf), RF24_CHANNEL);
        sniffer();
    }
*/
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

    // Eingehendes Socket für UDP Messages öffnen
    printf("%sSocket für eingehende UDP Messages vom Gateway auf Port %s angelegt\n", ts(tsbuf), cfg.udp_gwPortno.c_str());
    openSocket(NULL, cfg.udp_gwPortno.c_str(), &udp_address_in, &udp_sockfd_in, UDP);
    //printf("opensocket: %s:%s\n",cfg.gw_hubHostname.c_str(), cfg.udp_hubPortno.c_str());
    //openSocket(cfg.gw_hubHostname.c_str(), cfg.udp_hubPortno.c_str(),&udp_address,&udp_sockfd,UDP);

    init_system();
    printf("%s%s up and running\n", ts(tsbuf), PRGNAME); 

    // Main Loop
    while(1) {
		if ( radio.isValid() && radio.available() ) {
//
// Receive loop: react on the message from the nodes
//
			radio.read(&payload,sizeof(payload));
            udpdata.gwno = std::stoi(cfg.gw_gwID);;
            udpdata.payload.node_id = payload.node_id;
            udpdata.payload.msg_id = payload.msg_id;
            udpdata.payload.msg_type = payload.msg_type;
            udpdata.payload.msg_flags = payload.msg_flags;
            udpdata.payload.orderno = payload.orderno;
            udpdata.payload.heartbeatno = payload.heartbeatno;
            udpdata.payload.data1 = payload.data1;
            udpdata.payload.data2 = payload.data2;
            udpdata.payload.data3 = payload.data3;
            udpdata.payload.data4 = payload.data4;
            udpdata.payload.data5 = payload.data5;
            udpdata.payload.data6 = payload.data6;
            printPayload(buf1, &udpdata.payload);
			sendUdpMessage(cfg.gw_hubHostname.c_str(), cfg.udp_hubPortno.c_str(), &udpdata); 
		} // radio.available
//
// Orderloop: Tell the nodes what they have to do
//
    UdpMsgLen = recvfrom ( udp_sockfd_in, &udpdata, sizeof(udpdata), 0, (struct sockaddr *) &udp_address, &len );
    if (UdpMsgLen > 0) {
        memcpy(&payload, &udpdata.payload, sizeof(payload) );
        //sprintf(buf1,"Snd:");
        printPayload("Snd:",&payload);
    }
//
//  end orderloop
//
	} // while(1)
	return 0;
}

