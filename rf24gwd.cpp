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
//    radio.setDataRate(RF24_250KBPS);
	radio.setRetries(15,15);
uint8_t  address1[] = { 0xf0, 0xcc, 0xcc, 0xcc, 0xcc};
uint8_t  address2[] = { 0x33, 0xcc, 0xcc, 0xcc, 0xcc};
    radio.openWritingPipe(address2);
    radio.openReadingPipe(1,address1);
    radio.startListening();
    usleep(500);
    sprintf(debug,"%s","starting network ... "); logger.logmsg(VERBOSESTARTUP, debug);
    radio.printDetails();
    while(1) {
        if ( radio.available() ){  
			radio.read( &payload, sizeof(payload) );
            sprintf(debug,"%s",">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");		logger.logmsg(VERBOSEOTHER, debug);
            sprintf(debug,"Node Msg_number: %u", payload.msg_id);			logger.logmsg(VERBOSEOTHER, debug);
            sprintf(debug,"Node Data1: %u", payload.data1);
                logger.logmsg(VERBOSEOTHER, debug);
            sprintf(debug,"<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");			logger.logmsg(VERBOSEOTHER, debug);
		radio.stopListening();
		if ( radio.write( &payload, sizeof(payload) ) ) {
			sprintf(debug,"%s","Writing to Node -> OK"); logger.logmsg(VERBOSERF24, debug);
		} else {
			sprintf(debug,"%s","Writing to Node -> ERROR"); logger.logmsg(VERBOSERF24, debug);
		}					
		radio.startListening();
	}
 	usleep(100);
    }
    return 0;
}

 
