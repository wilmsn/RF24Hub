#include "rf24test.h"
#include "zahlenformat.h"

void sighandler(int signal) {
    sprintf(debug,"%s","SIGTERM: Shutting down ... ");
	logger.logmsg(VERBOSECRITICAL, debug);
    exit (0);
}

void error_exit(int myerrno, char* error) {
    exit (myerrno);
}    


int main(int argc, char* argv[]) {
	// check if started as root
	if ( getuid()!=0 ) {
		cout << "rf24gateway has to be startet as user root" << endl; 
        exit(1);
    }
    float val1=1; 
    float val2=1; 
    float val3=1; 
    float val4=1; 
    // check for PID file, if exists terminate else create it
    // starts logging

    signal(SIGTERM, sighandler);
    signal(SIGINT, sighandler);

    logger.verboselevel = 9;
    radio.begin();
//    radio.setPALevel(RF24_PA_MIN);
    radio.setPALevel( RF24_PA_MAX ) ;
    radio.setChannel( 91 );
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
            switch ( payload.type) {
                case 51: {
                    sprintf(debug,"%s",">>>>>>>>>>>>>>>>in>>>>>>>>>>>>>>>>>>>>>");		logger.logmsg(VERBOSEOTHER, debug);
                    sprintf(debug,"Node: %u", payload.node_id);			logger.logmsg(VERBOSEOTHER, debug);
                    sprintf(debug,"Data: (C/V): (%u/%g)(%u/%g)(%u/%g)(%u/%g)(%u/%g)(%u/%g)"
                            , getChannel(payload.data1), getValue_f(payload.data1)
                            , getChannel(payload.data2), getValue_f(payload.data2)
                            , getChannel(payload.data3), getValue_f(payload.data3)
                            , getChannel(payload.data4), getValue_f(payload.data4)
                            , getChannel(payload.data5), getValue_f(payload.data5)
                            , getChannel(payload.data6), getValue_f(payload.data6) );
                    logger.logmsg(VERBOSEOTHER, debug);
                    sprintf(debug,"<<<<<<<<<<<<<<<<out<<<<<<<<<<<<<<<<<<<");			logger.logmsg(VERBOSEOTHER, debug);
                    payload.type = 71;
                    payload.data3=calcTransportValue_f(21,val1);
                    payload.data4=calcTransportValue_f(21,val2);
                    payload.data5=calcTransportValue_f(21,val3);
                    payload.data6=calcTransportValue_f(21,val4);
                    sprintf(debug,"Data: (C/V): (%u/%g)(%u/%g)(%u/%g)(%u/%g)(%u/%g)(%u/%g)"
                            , getChannel(payload.data1), getValue_f(payload.data1)
                            , getChannel(payload.data2), getValue_f(payload.data2)
                            , getChannel(payload.data3), getValue_f(payload.data3)
                            , getChannel(payload.data4), getValue_f(payload.data4)
                            , getChannel(payload.data5), getValue_f(payload.data5)
                            , getChannel(payload.data6), getValue_f(payload.data6) );
                    logger.logmsg(VERBOSEOTHER, debug);
                    radio.stopListening();
                    if ( radio.write( &payload, sizeof(payload) ) ) {
                        sprintf(debug,"%s","Writing to Node -> OK"); logger.logmsg(VERBOSERF24, debug);
                    } else {
                        sprintf(debug,"%s","Writing to Node -> ERROR"); logger.logmsg(VERBOSERF24, debug);
                    }					
                    radio.startListening();
                    val1++; val2+=0.021; val3+=2; val4+=0.44;
                    if (val1 > 1000) { val1=1; val2=1; val3=1; val4=1; }
                }
                break;
                case 81:
                    sprintf(debug,"+++++Quittung erhalten++++++");
                    logger.logmsg(VERBOSEOTHER, debug);
                    break;
            }
        }
        usleep(100);
    }
    return 0;
}

 
