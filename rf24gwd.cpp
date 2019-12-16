#include "rf24gwd.h"
#include "zahlenformat.h"

//#define DEBUG

void sighandler(int signal) {
//    exit_system(); 
//	sprintf(debug, "SIGTERM: Shutting down ... ");
    sprintf(debug,"%s","SIGTERM: Shutting down ... ");
	logger.logmsg(VERBOSECRITICAL, debug);
//    unlink(parms.pidfilename);
//	msgctl(msqid, IPC_RMID, NULL);
    exit (0);
}

void error_exit(int myerrno, char* error) {
//    exit_system(); 
//	sprintf(debug, "SIGTERM: Shutting down ... ");
//	logmsg(VERBOSECRITICAL, debug);
    //unlink(parms.pidfilename);
    exit (myerrno);
}    


int main(int argc, char* argv[]) {
	// check if started as root
	if ( getuid()!=0 ) {
		cout << "rf24gateway has to be startet as user root" << endl; 
        exit(1);
    }
    // check for PID file, if exists terminate else create it
    // starts logging

    signal(SIGTERM, sighandler);
    signal(SIGINT, sighandler);

    logger.verboselevel = 9;
    radio.begin();
//    radio.setPALevel(RF24_PA_MIN);
    radio.setPALevel( RF24_PA_MAX ) ;
    radio.setChannel( 10 );
    radio.setAutoAck( true );
    radio.enableDynamicPayloads();
    radio.setDataRate(RF24_1MBPS);
//	radio.setRetries(15,15);
uint8_t  address1[] = { 0xf0, 0xcc, 0xcc, 0xcc, 0xcc};
uint8_t  address2[] = { 0x33, 0xcc, 0xcc, 0xcc, 0xcc};
    radio.openWritingPipe(address2);
    radio.openReadingPipe(1,address1);
    radio.startListening();
    usleep(500);
    sprintf(debug,"%s","starting network ... "); logger.logmsg(VERBOSESTARTUP, debug);
    radio.printDetails();
    while(1) {
//		bool goodSignal = radio.testRPD();
        if ( radio.available() ){  
			radio.read( &payload, sizeof(payload) );
            sprintf(debug,"%s",">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");		logger.logmsg(VERBOSEOTHER, debug);
//			sprintf(debug,"Node %s", goodSignal ? "Strong signal > 64dBm" : "Weak signal < 64dBm" ); logger.logmsg(VERBOSEOTHER, debug);
    	    sprintf(debug,"Node Network_number: %u", payload.network_id); logger.logmsg(VERBOSEOTHER, debug);
            sprintf(debug,"Node Node_number: %u", payload.node_id);		logger.logmsg(VERBOSEOTHER, debug);
            sprintf(debug,"Node Msg_number: %u", payload.msg_id);			logger.logmsg(VERBOSEOTHER, debug);
            sprintf(debug,"Node Msg_type: %u", payload.msg_type);			logger.logmsg(VERBOSEOTHER, debug);
            sprintf(debug,"Node Msg_flags: %04x", payload.msg_flags);		logger.logmsg(VERBOSEOTHER, debug);
            sprintf(debug,"Node OrderNo: %u", payload.orderno);	      	logger.logmsg(VERBOSEOTHER, debug);
            sprintf(debug,"Node Data1: %u:%f", getChannel(payload.data1), getValue_f(payload.data1));
                logger.logmsg(VERBOSEOTHER, debug);
            sprintf(debug,"Node Data2: %u:%f", getChannel(payload.data2), getValue_f(payload.data2));
                logger.logmsg(VERBOSEOTHER, debug);
            sprintf(debug,"Node Data3: %u:%f", getChannel(payload.data3), getValue_f(payload.data3));
                logger.logmsg(VERBOSEOTHER, debug);
            sprintf(debug,"Node Data4: %u:%f", getChannel(payload.data4), getValue_f(payload.data4));
                logger.logmsg(VERBOSEOTHER, debug);
            sprintf(debug,"Node Data5: %u:%f", getChannel(payload.data5), getValue_f(payload.data5));
                logger.logmsg(VERBOSEOTHER, debug);
            sprintf(debug,"Node Data6: %u:%f", getChannel(payload.data6), getValue_f(payload.data6));
                logger.logmsg(VERBOSEOTHER, debug);
            sprintf(debug,"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");			logger.logmsg(VERBOSEOTHER, debug);
            udp_node_data.network_id = payload.network_id;
            udp_node_data.node_id = payload.node_id;
            udp_node_data.msg_id = payload.msg_id;
            udp_node_data.msg_flags = payload.msg_flags;
            udp_node_data.msg_type = payload.msg_type;
            udp_node_data.orderno = payload.orderno;
            udp_node_data.data1 = payload.data1;
            udp_node_data.data2 = payload.data2;
            udp_node_data.data3 = payload.data3;
            udp_node_data.data4 = payload.data4;
            udp_node_data.data5 = payload.data5;
            udp_node_data.data6 = payload.data6;
		}
		radio.stopListening();
		if ( radio.write( &udp_hub_data, sizeof(udp_hub_data) ) ) {
			sprintf(debug,"%s","Writing to Node -> OK"); logger.logmsg(VERBOSERF24, debug);
		} else {
			sprintf(debug,"%s","Writing to Node -> ERROR"); logger.logmsg(VERBOSERF24, debug);
		}					
		radio.startListening();
 		usleep(1000000);
	}
    return 0;
}

 
