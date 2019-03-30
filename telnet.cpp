#include "telnet.h"
#include "config.h"
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
//#define logmsg cfg.logmsg

/*
 * trim: get rid of trailing and leading whitespace...
 */
char * trim (char * s) {
  /* Initialize start, end pointers */
  char *s1 = s, *s2 = &s[strlen (s) - 1];
  /* Trim and delimit right side */
  while ( (isspace (*s2)) && (s2 >= s1) )
    s2--;
  *(s2+1) = '\0';
  /* Trim left side */
  while ( (isspace (*s1)) && (s1 < s2) )
    s1++;
  /* Copy finished string */
  strcpy (s, s1);
  return s;
}


void exec_tn_cmd(const char *tn_cmd) {
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    
	cfg.logmsg(VERBOSETELNET,tn_cmd);
    portno = cfg.parms.telnet_port;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
		cfg.logmsg(VERBOSECRITICAL,"ERROR: opening socket");
	}	
    server = gethostbyname(cfg.parms.telnet_hostname);
    if (server == NULL) {
		cfg.logmsg(VERBOSECRITICAL,"ERROR: no such host");
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) { 
		cfg.logmsg(VERBOSECRITICAL,"ERROR: connecting");
	} else {	
		n = write(sockfd,tn_cmd,strlen(tn_cmd));
		if (n < 0) {
			cfg.logmsg(VERBOSECRITICAL,"ERROR: writing to socket");
		} else {
			string debug = "Telnet to ";
            debug += cfg.parms.telnet_hostname;
            debug += " Port ";
            debug += portno;
            debug += " CMD: ";
            debug += tn_cmd;
            debug += "successfull";
			cfg.logmsg(VERBOSETELNET,debug);
		}		
	}		 
    close(sockfd);
}

void prepare_tn_cmd(uint16_t node, uint8_t channel, float value) {
	char telnet_cmd[200];
    sprintf(telnet_cmd,"set %s %f \n", "fhe_dev_test", 0.0);
	std::string debug="Telnet-CMD: ";
    debug += telnet_cmd;
	cfg.logmsg(VERBOSETELNET,debug);				
	exec_tn_cmd(telnet_cmd);
}
	
void receive_tn_in(int new_tn_in_socket, struct sockaddr_in * address) {
    char *buffer =  (char*) malloc (TELNETBUFFERSIZE);
    char *client_message =  (char*) malloc (TELNETBUFFERSIZE);
    ssize_t MsgLen;
    // send something like a prompt. perl telnet is waiting for it otherwise we get error
    // use this in perl: my $t = new Net::Telnet (Timeout => 2, Port => 7001, Prompt => '/rf24hub>/');
    sprintf(client_message,"rf24hub> ");
    write(new_tn_in_socket , client_message , strlen(client_message));
    string debug = "Client ";
    debug += inet_ntoa (address->sin_addr);
    debug += " ist connected ..."; 
    cfg.logmsg(VERBOSETELNET, debug);
    sprintf(buffer,"                                 ");
    MsgLen = recv(new_tn_in_socket, buffer, TELNETBUFFERSIZE, 0);
//    sprintf(client_message,"%s",buffer);
//    write(new_tn_in_socket , client_message , strlen(client_message));
    char msglen_str[10];
    sprintf(msglen_str,"%ld",MsgLen);
    debug = "Buffer: \"";
    debug += trim(buffer);
    debug += "\" Msglen: ";
    debug += msglen_str;
    cfg.logmsg(VERBOSETELNET, debug);
    if (MsgLen>0) {
        process_tn_in(new_tn_in_socket, buffer, client_message);
    }
    close (new_tn_in_socket);
    free(buffer);
    free(client_message);
    //                 exit(0);
}
	
void process_tn_in(int new_tn_in_socket, char* buffer, char* msg) {
/* Messages can llook like this:
       <word1		word2		word3		word4 				function>
		init													Reinitialization of rf24hubd (reads actual values from database)
		list		order		text							Lists open orders in textform
                                html                            Lists open orders in HTML form
		set			sensor		<sensor#> 	<value>				Sets Sensor to Value (Store in Orderbuffer only)
					node		<node#>		init				Initialize this Node (Send Initdata to the Node) 
					verbose		<value>							Sets Verboselevel to new Value (1...9)
		setlast		sensor		<sensor#> 	<value>				Sets Sensor to Value and starts sending (Store in Orderbuffer and transfer all requests for this Node to Order)
			
*/
			
	char cmp_init[]="init", 
		 cmp_sensor[]="sensor",
		 cmp_system[]="system",
		 cmp_set[]="set",
		 cmp_node[]="node",
		 cmp_list[]="list",
		 cmp_html[]="html",
         cmp_text[]="text",
		 cmp_order[]="order",	 
		 cmp_verbose[]="verbose";	 
	char *wort1a, *wort2a, *wort3a, *wort4a;
    char *wort1, *wort2, *wort3, *wort4;
	uint16_t node = 0;
	uint32_t akt_sensor = 0;
	bool tn_input_ok=false;
	char delimiter[] = " ";
	cfg.trim(buffer);
	string debug="Incoming telnet data: ";
    debug += buffer;
	cfg.logmsg(VERBOSETELNET, debug);
	wort1a = strtok(buffer, delimiter);
	wort2a = strtok(NULL, delimiter);
	wort3a = strtok(NULL, delimiter);
	wort4a = strtok(NULL, delimiter);
    if (wort1a) { 
        wort1 = static_cast<char*>(malloc(strlen(wort1a)+1));
        strcpy(wort1, wort1a);
    } else { 
        wort1 = static_cast<char*>(malloc(1));
        wort1[0] = '\0';
    }
    if (wort2a) { 
        wort2 = static_cast<char*>(malloc(strlen(wort2a)+1));
        strcpy(wort2, wort2a);
    } else { 
        wort2 = static_cast<char*>(malloc(1));
        wort2[0] = '\0';
    }
    if (wort3a) { 
        wort3 = static_cast<char*>(malloc(strlen(wort3a)+1));
        strcpy(wort3, wort3a);
    } else { 
        wort3 = static_cast<char*>(malloc(1));
        wort3[0] = '\0';
    }
    if (wort4a) { 
        wort4 = static_cast<char*>(malloc(strlen(wort4a)+1));
        strcpy(wort4, wort4a);
    } else { 
        wort4 = static_cast<char*>(malloc(1));
        wort4[0] = '\0';
    }
//    char *client_message =  (char*) malloc (1024);
	// set/setlast sensor <sensor> <value>
	// sets a sensor to a value, setlast starts the request over air
	if ( ( strcmp(wort1,cmp_set) == 0 ) && (strcmp(wort2,cmp_sensor) == 0) && (strlen(wort3)>5) && (strlen(wort4)>0) ) {
		tn_input_ok = true;
        uint16_t my_node;   
        uint16_t my_channel;
        // In word3 we may have a) the number of the sensor b) the fhem_dev of a sensor
		// for the processing we need the number of the sensor ==> find it!
        if (sensorbuffer.findSensor(&my_node, &my_channel, wort3)) {
		// just add the sensor to the buffer
            orderbuffer.newOrder(my_node, my_channel, strtof(wort4,NULL));
        } else {
            debug = "Sensor ";
            debug += wort3;
            debug += "not found!";
            cfg.logmsg(VERBOSECRITICAL,debug);
        }
	}
    // set node <node> init
	// sends the init sequence to a node
	else if (( strcmp(wort1,cmp_set) == 0 ) && (strcmp(wort2,cmp_node) == 0) && (strlen(wort3) > 1) && (strcmp(wort2,cmp_init) == 0) ) {
		tn_input_ok = true;
//		init_node(getnodeadr(wort3));
	}
    // set verbose <new verboselevel>
	// sets the new verboselevel
	else if (( strcmp(wort1,cmp_set) == 0 ) && (strcmp(wort2,cmp_verbose) == 0) && (strlen(wort3) == 1) && (strlen(wort4) == 0) ) {
        if ( wort3[0] > '0' && wort3[0] < '9' + 1 ) {
			tn_input_ok = true;
			cfg.verboselevel = (wort3[0] - '0') * 1;
		}	
	}
    // list order
	// lists the current orderbuffer
	else if (( strcmp(wort1,cmp_list) == 0 ) && (strcmp(wort2,cmp_order) == 0) && (strcmp(wort3,cmp_text) == 0) && (strlen(wort4) == 0) ) {
		tn_input_ok = true;
/*        strcpy(msg,"----- Orderbuffer: ------\n"); 
		write(new_tn_in_socket , msg , strlen(msg));
        void * my_pos = orderbuffer.getInitialBuffer_ptr();
        while (my_pos) {
            my_pos = orderbuffer.listOrder(my_pos, msg, 't');
            write(new_tn_in_socket , msg , strlen(msg));
        }
*/        
        orderbuffer.listOrder(new_tn_in_socket, 't');
    }	
    // html order
	// lists the current order/orderbuffer for html page
	else if (( strcmp(wort1,cmp_list) == 0 ) && (strcmp(wort2,cmp_order) == 0) && (strcmp(wort3,cmp_html) == 0) && (strlen(wort4) == 0) ) {
		tn_input_ok = true;
/*        sprintf(msg,"\n<center><big>Orderbuffer</big><table><tr><th>OrderNo</th><th>EntryTime</th><th>Node</th><th>Channel</th><th>Value</th></tr>\n"); 
		write(new_tn_in_socket , msg , strlen(msg));
        void * my_pos = orderbuffer.getInitialBuffer_ptr();
        while (my_pos) {
            my_pos = orderbuffer.listOrder(my_pos, msg, 'h');
            write(new_tn_in_socket , msg , strlen(msg));
        }
		sprintf(msg,"</table></center>\n"); 
		write(new_tn_in_socket , msg , strlen(msg));
*/
        orderbuffer.listOrder(new_tn_in_socket, 'h');
    }	

    // init
	// initialisation of rf24hubd: reloads data from database
	else if (( strcmp(wort1,cmp_init) == 0 ) && ( strcmp(wort2,cmp_system) == 0 ) && (strlen(wort3) == 0) && (strlen(wort4) == 0) ) {
		tn_input_ok = true;
		//db.initSystem();
        db.fillSensorBuffer(&sensorbuffer);
	}
	if ( ! tn_input_ok) {
		sprintf(msg,"Usage:\n");
		write(new_tn_in_socket , msg , strlen(msg));
		sprintf(msg,"set sensor <sensornumber> <sensorvalue>\n");
		write(new_tn_in_socket , msg , strlen(msg));
		sprintf(msg,"   Sets the sensor <sensornumber> to the value <sensorvalue>\n");
		write(new_tn_in_socket , msg , strlen(msg));
		sprintf(msg,"   'set' stores only, no message is send to the node\n");
		write(new_tn_in_socket , msg , strlen(msg));
		sprintf(msg,"send node <nodenumber>\n");
		write(new_tn_in_socket , msg , strlen(msg));
		sprintf(msg,"   Send all stored requests t the Node <nodenumber>. Use <nodenumber> like '041'\n");
		write(new_tn_in_socket , msg , strlen(msg));
		sprintf(msg,"set node <nodenumber> init\n");
		write(new_tn_in_socket , msg , strlen(msg));
		sprintf(msg,"   Inits the Node <nodenumber>. Use <nodenumber> like '041'\n");
		write(new_tn_in_socket , msg , strlen(msg));
		sprintf(msg,"init system\n");
		write(new_tn_in_socket , msg , strlen(msg));
		sprintf(msg,"   Reinitialisation of rf24hub (all open ordes will be deleted)\n");
		write(new_tn_in_socket , msg , strlen(msg));
		sprintf(msg,"list order <text / html>\n");
		write(new_tn_in_socket , msg , strlen(msg));
		sprintf(msg,"   lists the content of the order queue as text or html\n");
		write(new_tn_in_socket , msg , strlen(msg));
		sprintf(msg,"set verbose <verboselevel>\n");
		write(new_tn_in_socket , msg , strlen(msg));
		sprintf(msg,"   sets a new verboselevel <1..9> are valid levels\n");
		write(new_tn_in_socket , msg , strlen(msg));
		sprintf(msg,"\n");
		write(new_tn_in_socket , msg , strlen(msg));
		sprintf(msg,"%s version %s\n", PRGNAME, PRGVERSION);
		write(new_tn_in_socket , msg , strlen(msg));
	} else {
		sprintf(msg,"Command received => OK\n");
		write(new_tn_in_socket , msg , strlen(msg));
	}		
	free(wort1);
    free(wort2);
    free(wort3);
    free(wort4);
}
