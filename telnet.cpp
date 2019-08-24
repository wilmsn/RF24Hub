#include "telnet.h"

void openSocket(const char* port, struct sockaddr_in *address, int* handle, sockType_t sockType ) {
    int in_socket;
    int rv;
    long save_fd;
	const int y = 1;
    //*handle = 99;
    /* open incoming port for messages */
    struct addrinfo hints, *servinfo, *p;
    memset(&hints, 0, sizeof hints);
    //	hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_family = AF_INET6; // set to AF_INET to force IPv4
    if ( sockType == TCP ) {
		hints.ai_socktype = SOCK_STREAM;
	} else {
		hints.ai_socktype = SOCK_DGRAM;
	}		
    hints.ai_flags = AI_PASSIVE; // use my IP
    if ((rv = getaddrinfo(NULL, port, &hints, &servinfo)) != 0) {
        //return (int)-1;
    }
	// loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((in_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
                continue;
        }
        if (bind(in_socket, p->ai_addr, p->ai_addrlen) == -1) {
           close(in_socket);
           continue;
        }
        break;
    }
    if (p == NULL) {
        //return (int)-1;
    }
    freeaddrinfo(servinfo);
	setsockopt( in_socket, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(int) );
	listen (in_socket, 5);
	save_fd = fcntl( in_socket, F_GETFL );
	save_fd |= O_NONBLOCK;
	fcntl( in_socket, F_SETFL, save_fd );
    *handle = in_socket;
}


/*    
void receive_tn_in(int new_tn_in_socket, struct sockaddr_in * address) {
    char *buffer =  (char*) malloc (TELNETBUFFERSIZE);
    char *client_message =  (char*) malloc (TELNETBUFFERSIZE);
    ssize_t MsgLen;
 cout << "Anfang receiveTelnetMessage" << endl;
    // send something like a prompt. perl telnet is waiting for it otherwise we get error
    // use this in perl: my $t = new Net::Telnet (Timeout => 2, Port => 7001, Prompt => '/rf24hub>/');
    sprintf(client_message,"rf24hub> ");
    write(tn_socket , client_message , strlen(client_message));
 cout << "Client %s ist connected ..." << inet_ntoa (address->sin_addr) << endl;
//	cfg.logmsg(VERBOSETELNET, debug);
    sprintf(buffer,"                                 ");
    MsgLen = recv(tn_socket, buffer, TELNETBUFFERSIZE, 0);
//    sprintf(client_message,"%s",buffer);
//    write(tn_socket , client_message , strlen(client_message));
//    char msglen_str[10];
//    sprintf(msglen_str,"%ld",MsgLen);
//	sprintf (debug,"Buffer: %s MsgLen: %d ", cfg.trim(buffer), MsgLen);
//	cfg.logmsg(VERBOSETELNET, debug);

//    debug = "Buffer: \"";
//    debug += trim(buffer);
//    debug += "\" Msglen: ";
//    debug += msglen_str;
//    cfg.logmsg(VERBOSETELNET, debug);
    if (MsgLen>0) {
        //process_tn_in(tn_socket, buffer, client_message);
//        cout << "TN Message" << client_message << endl;
    }
 cout << "vor close receiveTelnetMessage" << endl;
    close (tn_socket);
    free(buffer);
    free(client_message);
 cout << "ende receiveTelnetMessage" << endl;
    //                 exit(0);
}
*/
void receiveTelnetMessage(int new_tn_in_socket, struct sockaddr_in * address) {
    char *buffer =  (char*) malloc (TELNETBUFFERSIZE);
    char *client_message =  (char*) malloc (TELNETBUFFERSIZE);
    ssize_t MsgLen;
    // send something like a prompt. perl telnet is waiting for it otherwise we get error
    // use this in perl: my $t = new Net::Telnet (Timeout => 2, Port => 7001, Prompt => '/rf24hub>/');
    sprintf(client_message,"rf24hub> ");
    write(new_tn_in_socket , client_message , strlen(client_message));
//	cout << "Client " <<  inet_ntoa (address->sin_addr) << "ist connected ..." << endl;
    memset(buffer,0,sizeof(buffer));
    MsgLen = recv(new_tn_in_socket, buffer, TELNETBUFFERSIZE, 0);
    debug = "Telnet Data: ";
    debug += cfg.trim(buffer);
	cfg.logmsg(VERBOSETELNET, debug);
//    cout << "Msglen: " << MsgLen << " Buffer: " << buffer << endl;


//    write(new_tn_in_socket , client_message , strlen(client_message));
//    char msglen_str[10];
//    sprintf(msglen_str,"%ld",MsgLen);
//	sprintf (debug,"Buffer: %s MsgLen: %d ", cfg.trim(buffer), MsgLen);
//	cfg.logmsg(VERBOSETELNET, debug);

//    debug = "Buffer: \"";
//    debug += trim(buffer);
//    debug += "\" Msglen: ";
//    debug += msglen_str;
//    cfg.logmsg(VERBOSETELNET, debug);
    if (MsgLen>0) {
//        process_tn_in(new_tn_in_socket, buffer, client_message);
    }
    close (new_tn_in_socket);
    free(buffer);
    free(client_message);
    //                 exit(0);
}

