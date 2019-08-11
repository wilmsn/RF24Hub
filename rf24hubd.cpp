#include "rf24hubd.h" 

/*******************************************************************************************
*
* Configfilehandling
* default place to look at is: DEFAULT_CONFIG_FILE (see rf24hubd.h)
*
********************************************************************************************/

void parse_config (struct config_parameters * parms) {
  char *s, buff[256];
  FILE *fp = fopen (config_file, "r");
  if (fp == NULL) {
    printf( "Configfile %s nicht gefunden!\n", config_file );
    return;
  }
  /* Read next line */
  while ((s = fgets (buff, sizeof buff, fp)) != NULL) {
    /* Skip blank lines and comments */
    if (buff[0] == '\n' || buff[0] == '#')
      continue;
    /* Parse name/value pair from line */
    char name[PARAM_MAXLEN+1], value[PARAM_MAXLEN+1];
    s = strtok (buff, "=");
    if (s==NULL)
      continue;
    else
      strncpy (name, s, PARAM_MAXLEN);
    s = strtok (NULL, "=");
    if (s==NULL)
      continue;
    else
      strncpy (value, s, PARAM_MAXLEN);
    trim (value);
    /* Copy into correct entry in parameters struct */
    if      (strcmp(name, "db_hostname")==0) strcpy (parms->db_hostname, value);
    else if (strcmp(name, "db_port")==0)              parms->db_port = atoi(value);
    else if (strcmp(name, "db_schema")==0)   strcpy (parms->db_schema, value);
    else if (strcmp(name, "db_username")==0) strcpy (parms->db_username, value);
    else if (strcmp(name, "db_password")==0) strcpy (parms->db_password, value);
    else if (strcmp(name, "fhem_hostname")==0) {
                strcpy (parms->fhem_hostname, value);
                fhem_host_set=true;
        }
    else if (strcmp(name, "fhem_port")==0) {
                strcpy(parms->fhem_port, value);
                fhem_port_set=true;
        }
    else if (strcmp(name, "tcp_in_port")==0) {
                strcpy(parms->tcp_in_port, value);
                tcp_in_port_set=true;
        }
    else if (strcmp(name, "udp_in_port")==0) {
                strcpy(parms->udp_in_port, value);
                udp_in_port_set=true;
        }
    else if (strcmp(name, "logfile")==0)     strcpy (parms->logfilename, value);
    else if (strcmp(name, "pidfile")==0)     strcpy (parms->pidfilename, value);
    else if (strcmp(name, "verboselevel")==0) {
                if ( verboselevel == -1 ) parms->verboselevel = atoi(value);
                else parms->verboselevel = verboselevel;
        }
    else if (strcmp(name, "rf24network_channel")==0) parms->rf24network_channel = atoi(value);
    else if (strcmp(name, "rf24network_speed")==0) {
                if (strcmp(value, "RF24_2MBPS")==0) {
                        parms->rf24network_speed = RF24_2MBPS;
                }
                else if (strcmp(value, "RF24_250KBPS")==0) {
                        parms->rf24network_speed = RF24_250KBPS;
                }
                else if (strcmp(value, "RF24_1MBPS")==0) {
                        parms->rf24network_speed = RF24_1MBPS;
                }
                else {
                        printf ("%s: Unknown value for %s ! Use RF24_1MBPS \n", value, name);
                        parms->rf24network_speed = RF24_1MBPS;
                }
    }
        else
      printf ("WARNING: %s/%s: Unknown name/value pair!\n", name, value);
  }
  /* Close file */
  fclose (fp);
}

void print_config (struct config_parameters * parms) {
    printf ("Logfile: %s\n", parms->logfilename);
    printf ("PIDfile: %s\n", parms->pidfilename);
    printf ("Verboselevel: %d\n", parms->verboselevel);
    printf ("DB-Hostname: %s\n", parms->db_hostname);
    printf ("DB-Port: %d\n", parms->db_port);
    printf ("DB-Schema: %s\n", parms->db_schema);
    printf ("DB-Username: %s\n", parms->db_username);
    printf ("DB-Password: %s\n", parms->db_password);
    printf ("FHEM-Hostname: %s\n", parms->fhem_hostname);
    printf ("FHEM-Port: %s\n", parms->fhem_port);
    printf ("TCP-Port (incoming): %s\n", parms->tcp_in_port);
    printf ("UDP-Port (incoming): %s\n", parms->udp_in_port);
//    printf ("Ende print_config\n");
}

void usage(const char *prgname) {
    fprintf(stdout, "%s version %s\n", prgname, prgversion);
    fprintf(stdout, "Usage: %s <option>\n", prgname);
    fprintf(stdout, "with options: \n");
    fprintf(stdout, "   -h or -? or --help \n");
    fprintf(stdout, "           Print help\n");
    fprintf(stdout, "   -d or --daemon\n");
    fprintf(stdout, "         Start as daemon\n");
    fprintf(stdout, "   -c or --configfilename <filename>\n");
    fprintf(stdout, "         Set configfilename\n");
    fprintf(stdout, "   -v or --verbose <verboselevel>\n");
    fprintf(stdout, "         Set verboselevel (1...9)\n");
    fprintf(stdout, "   -s or --scanner <scanlevel>\n");
    fprintf(stdout, "         Set scannlevel (0...9) and scan all channels\n");
    fprintf(stdout, "   -t or --scannchannel <channel>\n");
    fprintf(stdout, "         Scanns the single channel <channel>\n");
    fprintf(stdout, "For clean exit use \"CTRL-C\" or \"kill -15 <pid>\"\n\n");
}

/*
 * trim: get rid of trailing and leading whitespace...
 *       ...including the annoying "\n" from fgets()
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

/*******************************************************************************************
*
* END Configfilehandling
*
********************************************************************************************/

/*******************************************************************************************
*
* Telnethandling
* Used for communication with FHEM
*
********************************************************************************************/
// exec_fhem_cmd ==> send a telnet comand to the fhem-host
// usage example: exec_fhem_cmd("set device1 on");
void exec_fhem_cmd(const char *fhem_cmd) {
/*    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server; */
	char debug[200];
    
	sprintf(debug,"DEBUG: %s\n", fhem_cmd);
	logmsg(VERBOSETELNET,debug);
    
    int sockfd;  
    struct addrinfo hints, *servinfo, *p;
    int rv, n;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // use AF_INET6 to force IPv6
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(parms.fhem_hostname, parms.fhem_port, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            perror("connect");
            close(sockfd);
            continue;
        }

        break; // if we get here, we must have connected successfully
    }

    if (p == NULL) {
        // looped off the end of the list with no connection
        fprintf(stderr, "failed to connect\n");
        exit(2);
	} else {	
		n = write(sockfd,fhem_cmd,strlen(fhem_cmd));
		if (n < 0) {
			sprintf(debug,"ERROR: writing to socket");
			logmsg(VERBOSECRITICAL,debug);
		} else {
			sprintf(debug,"Telnet to %s Port %s CMD: %s successfull",parms.fhem_hostname, parms.fhem_port, fhem_cmd);
			logmsg(VERBOSETELNET,debug);
		}		
	}		 
    close(sockfd);

    freeaddrinfo(servinfo); // all done with this structure
/*    
    if (sockfd < 1) {
        sprintf(debug,"ERROR: opening socket");
		logmsg(VERBOSECRITICAL,debug);
	}	
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) { 
        sprintf(debug,"ERROR: connecting");
		logmsg(VERBOSECRITICAL,debug);
	} else {	
		n = write(sockfd,fhem_cmd,strlen(fhem_cmd));
		if (n < 0) {
			sprintf(debug,"ERROR: writing to socket");
			logmsg(VERBOSECRITICAL,debug);
		} else {
			sprintf(debug,"Telnet to %s Port %d CMD: %s successfull",parms.fhem_hostname, portno, fhem_cmd);
			logmsg(VERBOSETELNET,debug);
		}		
	}		 
    close(sockfd);
*/    
}

void prepare_fhem_cmd(uint16_t node, uint8_t channel, float value) {
	char telnet_cmd[200];
	for(int i=0; i<SENSORARRAYSIZE; i++) {
		if ( sensor[i].node == node && sensor[i].channel == channel ) {
			sprintf(telnet_cmd,"set %s %f \n", sensor[i].fhem_dev, value);
		}
	}
	sprintf(debug,"Telnet-CMD: %s\n", telnet_cmd);
	logmsg(VERBOSETELNET,debug);				
	exec_fhem_cmd(telnet_cmd);
}
	
void receive_tn_in(int new_tn_in_socket, struct sockaddr_in * address) {
    char *buffer =  (char*) malloc (TELNETBUFFERSIZE);
    char *client_message =  (char*) malloc (TELNETBUFFERSIZE);
    ssize_t MsgLen;
    // send something like a prompt. perl telnet is waiting for it otherwise we get error
    // use this in perl: my $t = new Net::Telnet (Timeout => 2, Port => 7001, Prompt => '/rf24hub>/');
    sprintf(client_message,"rf24hub> ");
    write(new_tn_in_socket , client_message , strlen(client_message));
	sprintf (debug,"Client %s ist connected ...", inet_ntoa (address->sin_addr));
	logmsg(VERBOSETELNET, debug);
    sprintf(buffer,"                                 ");
    MsgLen = recv(new_tn_in_socket, buffer, TELNETBUFFERSIZE, 0);
//    sprintf(client_message,"%s",buffer);
//    write(new_tn_in_socket , client_message , strlen(client_message));
//    char msglen_str[10];
//    sprintf(msglen_str,"%ld",MsgLen);
	sprintf (debug,"Buffer: %s MsgLen: %d ", trim(buffer), MsgLen);
	logmsg(VERBOSETELNET, debug);

//    debug = "Buffer: \"";
//    debug += trim(buffer);
//    debug += "\" Msglen: ";
//    debug += msglen_str;
//    cfg.logmsg(VERBOSETELNET, debug);
    if (MsgLen>0) {
        process_tn_in(new_tn_in_socket, buffer, client_message);
    }
    close (new_tn_in_socket);
    free(buffer);
    free(client_message);
    //                 exit(0);
}
	
void process_tn_in(int new_tn_in_socket, char* buffer, char* client_message) {
/* Messages can llook like this:
       <word1		word2		word3		word4 				function>
		init													Reinitialization of rf24hubd (reads actual values from database)
		list		order										Lists open orders in textform
		html 		order										Lists open orders in HTML form
		set			sensor		<sensor#> 	<value>				Sets Sensor to Value (Store in Orderbuffer only)
					node		<node#>		init				Initialize this Node (Send Initdata to the Node) 
					verbose		<value>							Sets Verboselevel to new Value (1...9)
		setlast		sensor		<sensor#> 	<value>				Sets Sensor to Value and starts sending (Store in Orderbuffer and transfer all requests for this Node to Order)
			
*/
			
	char cmp_init[]="init", 
		 cmp_sensor[]="sensor",
		 cmp_set[]="set",
		 cmp_setlast[]="setlast",
		 cmp_node[]="node",
		 cmp_list[]="list",
		 cmp_html[]="html",
		 cmp_order[]="order",	 
		 cmp_verbose[]="verbose";	 
	char *wort1a, *wort2a, *wort3a, *wort4a;
	char *wort1, *wort2, *wort3, *wort4;
	uint16_t node = 0;
	uint32_t akt_sensor = 0;
	bool tn_input_ok=false;
	char delimiter[] = " ";
	trim(buffer);
	sprintf(debug,"Incoming telnet data: %s ",buffer);
	logmsg(VERBOSETELNET, debug);
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
	// set/setlast sensor <sensor> <value>
	// sets a sensor to a value, setlast starts the request over air
	if ( (( strcmp(wort1,cmp_set) == 0 ) || ( strcmp(wort1,cmp_setlast) == 0 )) && (strcmp(wort2,cmp_sensor) == 0) && (strlen(wort3) > 1) && (strlen(wort4) > 0) ) {
		tn_input_ok = true;
		// In word3 we may have a) the number of the sensor b) the name of the sensor c) the fhem_dev of a sensor
		// for the processing we need the number of the sensor ==> find it!
		for (int i = 0; i < SENSORARRAYSIZE; i++) {
			if ( (sensor[i].sensor > 0) && ((strcmp(wort3,sensor[i].fhem_dev) == 0) || ( sensor[i].sensor == strtoul(wort3, &pEnd, 10)) ) ) {
				sprintf(debug, "Sensor found: %u Node: 0%o Channel: %u FHEM: %s", 
								sensor[i].sensor,
								sensor[i].node,
								sensor[i].channel,
								sensor[i].fhem_dev);
				logmsg(VERBOSETELNET, debug);
				akt_sensor = sensor[i].sensor;
			}
		}		
		// just add the sensor to the buffer
		node = set_sensor( akt_sensor, strtof(wort4, &pEnd));
		if ( node == 0 ) {
			sprintf(debug,"Sensor (%s) not in cache ==> running initialisation!",wort3);
			logmsg(VERBOSETELNET, debug);
			init_system();
		} else {
			if ( strcmp(wort1,cmp_setlast) == 0 ) {
				get_order(node);
				print_order_buffer();
			}
		}
	}
    // set node <node> init
	// sends the init sequence to a node
	if (( strcmp(wort1,cmp_set) == 0 ) && (strcmp(wort2,cmp_node) == 0) && (strlen(wort3) > 1) && (strcmp(wort4,cmp_init) == 0) ) {
		tn_input_ok = true;
		init_node(getnodeadr(wort3));
	}
    // set verbose <new verboselevel>
	// sets the new verboselevel
	if (( strcmp(wort1,cmp_set) == 0 ) && (strcmp(wort2,cmp_verbose) == 0) && (wort3 != NULL) && (wort4 == 0) ) {
        if ( wort3[0] > '0' && wort3[0] < '9' + 1 ) {
			tn_input_ok = true;
			verboselevel = (wort3[0] - '0') * 1;
		}	
	}
    // list order
	// lists the current orderbuffer
	if (( strcmp(wort1,cmp_list) == 0 ) && (strcmp(wort2,cmp_order) == 0) && (strlen(wort3) == 0) && (strlen(wort4) == 0) ) {
		tn_input_ok = true;
		sprintf(client_message,"----- Orderbuffer(max(%d): ------\n", (int)ORDERBUFFERLENGTH); 
		write(new_tn_in_socket , client_message , strlen(client_message));
		for (int i=0; i < ORDERBUFFERLENGTH -1; i++) {
			if ( order_buffer[i].node > 0 ) {
				sprintf(client_message,"order_buffer[%d]\t Onr:\t%u,\tentry:\t%llu (%d sec.),\tnode:\t0%o,\tchannel:\t%u\tval:\t%f\n", 
					i,order_buffer[i].orderno, order_buffer[i].entrytime, (int)(order_buffer[i].entrytime - mymillis())/1000, order_buffer[i].node,
					order_buffer[i].channel, order_buffer[i].value );
				write(new_tn_in_socket , client_message , strlen(client_message));
			}				
		}
		sprintf(client_message,"----- Order(max %d): ------\n", (int)ORDERLENGTH); 
		write(new_tn_in_socket , client_message , strlen(client_message));
		for (int i=0; i < ORDERLENGTH -1; i++) {
			if ( order[i].node > 0 ) {
				sprintf(client_message,"order[%d]\t Onr:\t%u,\tentry:\t%llu (%d sec.),\tnode:\t0%o,\ttype:\t%u\tflags:\t%u\t(channel/Value)\t(%u/%f)\t(%u/%f)\t(%u/%f)\t(%u/%f)\n", 
					i,order[i].orderno, order[i].entrytime, (int)(order[i].entrytime - mymillis())/1000, order[i].node, order[i].type, order[i].flags
					,order[i].channel1, order[i].value1
					,order[i].channel2, order[i].value2
					,order[i].channel3, order[i].value3
					,order[i].channel4, order[i].value4);
				write(new_tn_in_socket , client_message , strlen(client_message));
			}				
		}
	}	
    // html order
	// lists the current order/orderbuffer for html page
	if (( strcmp(wort1,cmp_html) == 0 ) && (strcmp(wort2,cmp_order) == 0) && (strlen(wort3) == 0) && (strlen(wort4) == 0) ) {
		tn_input_ok = true;
		sprintf(client_message,"\n<center><big>Orderbuffer</big><table><tr><th>OrderNo</th><th>EntryTime</th><th>Node</th><th>Channel</th><th>Value</th></tr>\n"); 
		write(new_tn_in_socket , client_message , strlen(client_message));
		for (int i=0; i < ORDERBUFFERLENGTH -1; i++) {
			if ( order_buffer[i].node > 0 ) {
				sprintf(client_message,"<tr><td>%u</td><td>%llu (%d sec.)</td><td>0%o</td><td>%u</td><td>%f</td></tr>\n", 
					order_buffer[i].orderno, order_buffer[i].entrytime, (int)(order_buffer[i].entrytime - mymillis())/1000, order_buffer[i].node,
					order_buffer[i].channel, order_buffer[i].value );
				write(new_tn_in_socket , client_message , strlen(client_message));
			}				
		}
		sprintf(client_message,"</table><br><big>Order</big><br><table><tr><th>OrderNo</th><th>EntryTime</th><th>Node</th><th>Type</th><th>Flags</th><th>Channel</th><th>Value</th></tr>\n"); 
		write(new_tn_in_socket , client_message , strlen(client_message));
		for (int i=0; i < ORDERLENGTH -1; i++) {
			if ( order[i].node > 0 ) {
				sprintf(client_message,"<tr><td>%u</td><td>%llu (%d sec.)</td><td>0%o</td><td>%u</td><td>%u</td><td>%u<br>%u<br>%u<br>%u</td><td>%f<br>%f<br>%f<br>%f</td></tr>\n", 
					order[i].orderno, order[i].entrytime, (int)(order[i].entrytime - mymillis())/1000, order[i].node, order[i].type, order[i].flags
					,order[i].channel1, order[i].channel2, order[i].channel3, order[i].channel4
					,order[i].value1, order[i].value2, order[i].value3, order[i].value4);
				write(new_tn_in_socket , client_message , strlen(client_message));
			}				
		}
		sprintf(client_message,"</table></center>\n"); 
		write(new_tn_in_socket , client_message , strlen(client_message));
    }	
    // init
	// initialisation of rf24hubd: reloads data from database
	if ( (strcmp(wort1,cmp_init) == 0) && (strlen(wort2) == 0) && (strlen(wort3) == 0) && (strlen(wort4) == 0) ) {
		tn_input_ok = true;
		init_system();
	}
	if ( ! tn_input_ok) {
		sprintf(client_message,"Usage:\n");
		write(new_tn_in_socket , client_message , strlen(client_message));
		sprintf(client_message,"set[last] sensor <sensornumber> <sensorvalue>\n");
		write(new_tn_in_socket , client_message , strlen(client_message));
		sprintf(client_message,"   Sets the sensor <sensornumber> to the value <sensorvalue>\n");
		write(new_tn_in_socket , client_message , strlen(client_message));
		sprintf(client_message,"   'set' stores only; 'setlast' executes all settings for this node\n");
		write(new_tn_in_socket , client_message , strlen(client_message));
		sprintf(client_message,"set node <nodenumber> init\n");
		write(new_tn_in_socket , client_message , strlen(client_message));
		sprintf(client_message,"   Inits the Node <nodenumber>. Use <nodenumber> like '041'\n");
		write(new_tn_in_socket , client_message , strlen(client_message));
		sprintf(client_message,"init \n");
		write(new_tn_in_socket , client_message , strlen(client_message));
		sprintf(client_message,"   Reinitialisation of rf24hub (all open ordes will be deleted)\n");
		write(new_tn_in_socket , client_message , strlen(client_message));
		sprintf(client_message,"list order\n");
		write(new_tn_in_socket , client_message , strlen(client_message));
		sprintf(client_message,"   lists the content of the order queue\n");
		write(new_tn_in_socket , client_message , strlen(client_message));
		sprintf(client_message,"set verbose <verboselevel>\n");
		write(new_tn_in_socket , client_message , strlen(client_message));
		sprintf(client_message,"   sets a new verboselevel <1..9> are valid levels\n");
		write(new_tn_in_socket , client_message , strlen(client_message));
		sprintf(client_message,"\n");
		write(new_tn_in_socket , client_message , strlen(client_message));
		sprintf(client_message,"%s version %s\n", PRGNAME, PRGVERSION);
		write(new_tn_in_socket , client_message , strlen(client_message));
	} else {
		sprintf(client_message,"Command received => OK\n");
		write(new_tn_in_socket , client_message , strlen(client_message));
	}		
	free(wort1);
    free(wort2);
    free(wort3);
    free(wort4);
}
	
/*******************************************************************************************
*
* END Telnethandling
*
********************************************************************************************/
/*******************************************************************************************
*
* Nodehandling 
* Used for communication with the nodes
*
********************************************************************************************/
void init_node(uint16_t node ) {
	// delete old entries for this node
	sprintf(debug,"Init of Node: 0%o", node);
	logmsg(VERBOSEOTHER,debug);
	sprintf (sql_stmt, "select sleeptime1, sleeptime2, sleeptime3, sleeptime4, radiomode, voltagefactor from node where node_id = '0%o' LIMIT 1 ",node);
	mysql_query(db, sql_stmt);
	db_check_error();
	MYSQL_RES *result = mysql_store_result(db);
	db_check_error();
	MYSQL_ROW row;
	if ((row = mysql_fetch_row(result))) {
		fill_order_buffer( node, 111, strtof(row[0], &pEnd));
		fill_order_buffer( node, 112, strtof(row[1], &pEnd));
		fill_order_buffer( node, 113, strtof(row[2], &pEnd));
		fill_order_buffer( node, 114, strtof(row[3], &pEnd));
		fill_order_buffer( node, 115, strtof(row[4], &pEnd));
		fill_order_buffer( node, 116, strtof(row[5], &pEnd));
		fill_order_buffer( node, 118, 1.0);
	}
	mysql_free_result(result);
    for (int i=0; i < SENSORARRAYSIZE; i++) {
		if (sensor[i].node == node && sensor[i].s_type == 'a') {
			fill_order_buffer( node, sensor[i].channel, sensor[i].last_val);
		}
	}
	get_order(node);
}

uint16_t getnodeadr(char *node) {
	int mynodeadr = 0;
	bool err = false;
	char t[5];
	for ( int i = 0; (node[i] > 0) && (! err); i++ ) {
		if ( mynodeadr > 0 ) mynodeadr = (mynodeadr << 3);
		sprintf(t,"%c",node[i]); 
		mynodeadr = mynodeadr + atoi(t);
		err = (node[i] == '6' || node[i] == '7' || node[i] == '8' || node[i] == '9' || (( i > 0 ) && ( node[i] == '0' ))); 
	}
	if (err) mynodeadr = 0;
	return mynodeadr;
}


void init_order(unsigned int element) {
	order[element].orderno = 0;
	order[element].node = 0;
	order[element].type = 0;
	order[element].flags = 0;
	order[element].channel1 = 0;
	order[element].value1 = 0;
	order[element].channel2 = 0;
	order[element].value2 = 0;
	order[element].channel3 = 0;
	order[element].value3 = 0;
	order[element].channel4 = 0;
	order[element].value4 = 0;
	order[element].entrytime = 0;
	order[element].last_send = 0;
}

void print_order(void) {
	if ( verboselevel > 8 ) {
		sprintf(debug,"======= Content of Order: ==========");
		logmsg(VERBOSEOTHER,debug);
		for (int i=0; i < ORDERLENGTH -1; i++) {
			if ( order[i].orderno > 0 ) {
				sprintf(debug, "order[%d] = Onr:\t%u,\tnode:\t0%o,\ttype:\t%u\tflags:\t%u (%u/%f) (%u/%f) (%u/%f) (%u/%f) Entry: %llu Last_Send: %llu", 
					i,
					order[i].orderno,
					order[i].node,
					order[i].type,
					order[i].flags,
					order[i].channel1,
					order[i].value1,
					order[i].channel2,
					order[i].value2,
					order[i].channel3,
					order[i].value3,
					order[i].channel4,
					order[i].value4,
					order[i].entrytime,
					order[i].last_send );
				logmsg(VERBOSEOTHER,debug);
			}
		}
		sprintf(debug,"====================================");
		logmsg(VERBOSEOTHER,debug);
	}	
}

void init_order_buffer(unsigned int element) {
	order_buffer[element].orderno = 0;
	order_buffer[element].entrytime = 0;
	order_buffer[element].node = 0;
	order_buffer[element].channel = 0;
	order_buffer[element].value = 0;
}

void fill_order_buffer( uint16_t node, uint16_t channel, float value) {
	int i=0;
	while ( (order_buffer[i].node > 0) && !((order_buffer[i].node == node) && (order_buffer[i].channel == channel ))  && ( i < ORDERBUFFERLENGTH -1 ) ) {
		i++;
	}
	order_buffer[i].orderno = 0;
	order_buffer[i].entrytime = mymillis();
	order_buffer[i].node = node;
	order_buffer[i].channel = channel;
	order_buffer[i].value = value;
}

void print_order_buffer(void) {
	if ( verboselevel > 8 ) {
		sprintf(debug,"======= Content of Order_Buffer: ==========");
		logmsg(VERBOSEOTHER,debug);
		for (int i=0; i < ORDERBUFFERLENGTH -1; i++) {
			if ( order_buffer[i].node > 0 ) {
				sprintf(debug,"order_buffer[%d]\t Onr: %u,\tentry:\t%llu,\tnode:\t0%o,\tchannel:\t%u\tval:\t%f", 
					i,
					order_buffer[i].orderno,
					order_buffer[i].entrytime,
					order_buffer[i].node,
					order_buffer[i].channel,
					order_buffer[i].value );
				logmsg(VERBOSEOTHER,debug);
			}				
		}
		sprintf(debug,"===========================================");
		logmsg(VERBOSEOTHER,debug);
	}	
}

bool is_valid_orderno(uint16_t myorderno) {
	bool retval = false;
	for (int i=0; i < ORDERBUFFERLENGTH -1; i++) {
		if ( myorderno == order_buffer[i].orderno ) retval = true;
	}
	return retval;
}

bool delete_orderno(uint16_t myorderno) {
	bool retval = false;
	uint16_t node = 0;
	if ( myorderno > 0 ) {
		for (int i=0; i < ORDERBUFFERLENGTH -1; i++) {
			if ( myorderno == order_buffer[i].orderno ) {
				retval = true;
				init_order_buffer(i);
			}
		}
		for (int i=0; i < ORDERLENGTH -1; i++) {
			if ( myorderno == order[i].orderno ) {
				retval = true;
				node = order[i].node;
				init_order(i);
			}
		}
		if (node > 0) get_order(node);
	}
	return retval;
}

void get_order(uint16_t node) {
    unsigned int order_ptr=0;
	int j = 0;
	orderno++;
	order_waiting = true;
	sprintf(debug, "get_order: node: 0%o orderno: %u", node, orderno);
	logmsg(VERBOSEOTHER,debug);
	// if we have old orders for this node ==> delete them! 
	for (int i=0; i < ORDERLENGTH -1; i++) {
		if ( order[i].node == node ) {
			order[i].node = 0;
			order[i].orderno = 0;
		}
	}
	//look for the first free position in order[]
	sprintf(debug, "get_order: order_ptr is: %u; order[order_ptr].orderno is %u", order_ptr, order[order_ptr].orderno);
	logmsg(VERBOSEOTHER,debug);	
	while ( order_ptr < ORDERLENGTH -1 && order[order_ptr].orderno > 0) {
		order_ptr++; 
		sprintf(debug, "get_order: order_ptr is: %u; order[order_ptr].orderno is %u", order_ptr, order[order_ptr].orderno);
		logmsg(VERBOSEOTHER,debug);
	}		
	sprintf(debug, "get_order: order_ptr is: %u", order_ptr);
	logmsg(VERBOSEOTHER,debug);
	//collect the data for this order
	for (int i=0; i < ORDERBUFFERLENGTH -1; i++) {
		if (node == order_buffer[i].node) {
			sprintf(debug, "get_order: j is: %d order_ptr is: %u", j, order_ptr);
			logmsg(VERBOSEOTHER,debug);
			if ( j < 4 ) order_buffer[i].orderno = orderno;
			if (j == 0) {
				order[order_ptr].orderno = orderno;
				order[order_ptr].node = node;
				order[order_ptr].type = 61;
				order[order_ptr].entrytime = mymillis();
				order[order_ptr].flags = 0x00;
				order[order_ptr].channel1 = order_buffer[i].channel;
				order[order_ptr].value1 = order_buffer[i].value;				
			}
			if (j == 1) {
				order[order_ptr].channel2 = order_buffer[i].channel;
				order[order_ptr].value2 = order_buffer[i].value;				
			}
			if (j == 2) {
				order[order_ptr].channel3 = order_buffer[i].channel;
				order[order_ptr].value3 = order_buffer[i].value;				
			}
			if (j == 3) {
				order[order_ptr].channel4 = order_buffer[i].channel;
				order[order_ptr].value4 = order_buffer[i].value;	
			}
			j++;
		}
	}
	if (j < 5) order[order_ptr].flags = 0x01;
	print_order();
}	

uint16_t set_sensor(uint32_t mysensor, float value) {
	int i = 0;
	uint16_t node = 0;
	while ( (sensor[i].sensor != mysensor) && (i < SENSORARRAYSIZE - 1) ) i++;
	if ( i < SENSORARRAYSIZE - 1 ) {
		fill_order_buffer( sensor[i].node, sensor[i].channel, value);
		node = sensor[i].node;
	}
	return node;
}

uint16_t get_sensor(uint32_t mysensor) {
	return set_sensor(mysensor, 0);
}

bool node_is_next(const uint16_t node) {
  /*
  Make sure that childs will not addressed until a parent needs to be addressed
  */
	bool retval;
  retval = true;
/* temporary disabled 
  for(int i=0; i<ORDERLENGTH -1; i++) {
		if ( order[i].node != 0 ) {
      if ( node != order[i].node ) {
        if ((node & order[i].node) == order[i].node) {
			    retval = false;
      		sprintf(debug, "node_is_next=false: i:%d node:%x order[%d].node:%x\n", i,node, i, order[i].node);
          logmsg(9, debug);
        }
      }
		}			
	} */
	return retval;
}
/*******************************************************************************************
*
* END Nodehandling 
*
********************************************************************************************/
/*******************************************************************************************
*
* Databasehandling 
* Used for communication with MariaDB
*
********************************************************************************************/

void db_check_error(void) {
	if (mysql_errno(db) != 0) {
		sprintf(debug, "DB-Fehler: %s\n", mysql_error(db));
        logmsg(VERBOSECRITICAL, debug);
    }
}

void do_sql(char *sqlstmt) {
	if (mysql_query(db, sqlstmt)) {
		sprintf(debug, "%s", mysql_error(db));
		logmsg(VERBOSECRITICAL, debug);
	}
    sprintf(debug, "%s", sqlstmt);
	logmsg(VERBOSESQL, debug);
}

void print_sensor(void) {
	sprintf(debug,"Sensor Array:");
	logmsg(VERBOSEOTHER, debug);
	for (int i = 0; i < SENSORARRAYSIZE; i++) {
		if (sensor[i].sensor > 0 ) {
			sprintf(debug, "Sensor: %u Node: 0%o Channel: %u Type: %c Value: %f FHEM: %s",
				sensor[i].sensor,
				sensor[i].node,
				sensor[i].channel,
				sensor[i].s_type,
				sensor[i].last_val,
				sensor[i].fhem_dev);
			logmsg(VERBOSEOTHER, debug);
		}
	}
}

void exit_system(void) {
    // Save data from sensordata_im and sensor_im to persistant tables
	sprintf(debug, "SIGTERM: Cleanup system ... saving *_im tables ...");
	logmsg(VERBOSECRITICAL, debug);
	sprintf (sql_stmt, "update sensor a set value = ( select value from sensor_im where sensor_id = a.sensor_id ), utime = ( select utime from sensor_im where sensor_id = a.sensor_id )");
	logmsg(VERBOSESQL, sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
	sprintf (sql_stmt, "insert into sensordata(sensor_id, utime, value) select sensor_id, utime, value from sensordata_im where (sensor_id,utime) not in (select sensor_id, utime from sensordata)");
	logmsg(VERBOSESQL, sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
}

void init_system(void) {
	int i = 0;
	char cmp_s[]="s",cmp_a[]="a";
	for (int i=0; i<SENSORARRAYSIZE; i++) {
		sensor[i].sensor = 0;
		sensor[i].node = 0;
		sensor[i].channel = 0;
		sensor[i].last_val = 0;
	}
    // Copy sensordata and sensor to memorytable since yesterday
	sprintf (sql_stmt, "truncate table sensor_im");
	logmsg(VERBOSESQL, sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
	sprintf (sql_stmt, "insert into sensor_im(sensor_id, sensor_name, add_info, node_id, channel, value, utime, store_days, fhem_dev, signal_quality, s_type, html_show) select sensor_id, sensor_name, add_info, node_id, channel, value, utime, store_days, fhem_dev, signal_quality, s_type, html_show from sensor");
	logmsg(VERBOSESQL, sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
	sprintf (sql_stmt, "truncate table sensordata_im");
	logmsg(VERBOSESQL, sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
	sprintf (sql_stmt, "insert into sensordata_im(sensor_id, utime, value) select sensor_id, utime, value from sensordata where utime > UNIX_TIMESTAMP(subdate(current_date, 2))");
	logmsg(VERBOSESQL, sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
	// END sensordata to memorytable
	sprintf (sql_stmt, "select sensor_id, node_id, channel, value, fhem_dev, s_type from sensor where sensor_id is not null and node_id is not null and channel is not null");
	logmsg(VERBOSESQL, sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
	MYSQL_RES *result = mysql_store_result(db);
	db_check_error();
	MYSQL_ROW row;
	while ((row = mysql_fetch_row(result))) {
		if ( row[0] != NULL ) sensor[i].sensor = strtoul(row[0], &pEnd,10);
		if ( row[1] != NULL ) sensor[i].node = getnodeadr(row[1]);
		if ( row[2] != NULL ) sensor[i].channel = strtoul(row[2], &pEnd,10);
		if ( row[3] != NULL ) sensor[i].last_val = strtof(row[3], &pEnd); else sensor[i].last_val = 0;
		if ( row[4] != NULL ) sprintf(sensor[i].fhem_dev,"%s",row[4]); else sprintf(sensor[i].fhem_dev,"not_set");
		if (strcmp(row[5],cmp_s) == 0) sensor[i].s_type = 's';
		if (strcmp(row[5],cmp_a) == 0) sensor[i].s_type = 'a';
		i++;
	}
	mysql_free_result(result);	
	print_sensor();
	for (unsigned int i=0; i<ORDERLENGTH -1; i++) init_order(i);
	for (unsigned int i=0; i<ORDERBUFFERLENGTH -1; i++) init_order_buffer(i);
}

void store_sensor_value(uint16_t node, uint8_t channel, float value, bool d1, bool d2) {
	if ( fhem_active ) { 
		prepare_fhem_cmd(node, channel, value); 
	}
	sprintf(sql_stmt,"insert into sensordata_im (sensor_ID, utime, value) select sensor_id, UNIX_TIMESTAMP(), %f from sensor_im where node_id = '0%o' and channel = %u ", value, node, channel);
	do_sql(sql_stmt);
	sprintf(sql_stmt,"update sensor_im set value= %f, utime = UNIX_TIMESTAMP(), signal_quality = '%d%d' where node_id = '0%o' and channel = %u ", value, d1, d2, node, channel);
	do_sql(sql_stmt);
	for(int i=0; i<SENSORARRAYSIZE; i++) {
		if ( sensor[i].node == node && sensor[i].channel == channel ) {
			sensor[i].last_val = value;
		}
	}
}

void process_sensor(uint16_t node, uint8_t channel, float value, bool d1, bool d2) {
	switch (channel) {
		case 1 ... 99: {
		// Sensor or Actor
			store_sensor_value(node, channel, value, d1, d2);
			sprintf(debug, DEBUGSTR "Value of  %u on Node: %o is %f ", channel, node, value);
			logmsg(VERBOSECONFIG, debug);       
		}
		break; 
		case 101: {
		// battery voltage
			store_sensor_value(node, channel, value, d1, d2);
			sprintf(debug, DEBUGSTR "Voltage of Node: %o is %f ", node, value);
			logmsg(VERBOSECONFIG, debug);        
			sprintf(sql_stmt,"update node set u_batt = %f, signal_quality = '%d%d', last_contact = unix_timestamp() where node_id = '0%o'", value, d1, d2, node);
			do_sql(sql_stmt);
		}
		break; 
		case 111: { // Init Sleeptime 1
			sprintf(debug, DEBUGSTR "Node: %o: Sleeptime1 set to %f ", node, value);
			logmsg(VERBOSECONFIG, debug);        
		}	
		break; 
		case 112: { // Init Sleeptime 2
			sprintf(debug, DEBUGSTR "Node: %o: Sleeptime2 set to %f ", node, value);
			logmsg(VERBOSECONFIG, debug);        
		}
		break; 
		case 113: { // Init Sleeptime 3
			sprintf(debug, DEBUGSTR "Node: %o: Sleeptime3 set to %f ", node, value);
			logmsg(VERBOSECONFIG, debug);        
		}
		break; 				
		case 114: { // Init Sleeptime 4
			sprintf(debug, DEBUGSTR "Node: %o: Sleeptime4 set to %f ", node, value);
			logmsg(VERBOSECONFIG, debug);        
		}
		break; 
		case 115: { // Init Radiobuffer
			bool radio_always_on = value > 0.5;
			if ( radio_always_on ) sprintf(debug, "Node: %o: Radio allways on", node);
			else sprintf(debug, "Node: %o: Radio sleeps", node);
			logmsg(VERBOSECONFIG, debug);        
		}					
		break;  
		case 116: { // Init Voltagefactor
			sprintf(debug, "Node: %o: Set Voltagefactor to: %f.", node, value);
			logmsg(VERBOSECONFIG, debug);        
		}
		break;  
		case 118: {
			sprintf(debug, DEBUGSTR "Node: %o Init finished.", node);
			logmsg(VERBOSECONFIG, debug);        
		}
		break; 
		default: { 
		// nothing right now
		}
	}	
}	
/*******************************************************************************************
*
* All the rest 
*
********************************************************************************************/
void sighandler(int signal) {
    exit_system(); 
	sprintf(debug, "SIGTERM: Shutting down ... ");
	logmsg(VERBOSECRITICAL, debug);
    unlink(parms.pidfilename);
//	msgctl(msqid, IPC_RMID, NULL);
    exit (0);
}

void error_exit(int myerrno, char* error) {
    exit_system(); 
	sprintf(debug, "SIGTERM: Shutting down ... ");
	logmsg(VERBOSECRITICAL, debug);
    unlink(parms.pidfilename);
    exit (myerrno);
}    

uint64_t mymillis(void) {
	struct timeval tv;
	uint64_t timebuf;
	gettimeofday(&tv, NULL);
	timebuf = ((tv.tv_sec & 0x000FFFFF) * 1000 + (tv.tv_usec / 1000)) - start_time;
	return timebuf;
}

void logmsg(int mesgloglevel, char *mymsg){
	if ( logmode == logfile ) {
		if (mesgloglevel <= verboselevel) {
			char buf[20];
			logfile_ptr = fopen (parms.logfilename,"a");
			if ( logfile_ptr != NULL ) {
				time_t now = time(0);
				tm *ltm = localtime(&now);
				fprintf (logfile_ptr, "rf24hubd: %d.", ltm->tm_year + 1900 );
				if ( ltm->tm_mon + 1 < 10) sprintf(buf,"0%d",ltm->tm_mon + 1); else sprintf(buf,"%d",ltm->tm_mon + 1);
				fprintf (logfile_ptr, "%s.", buf );
				if ( ltm->tm_mday < 10) sprintf(buf,"0%d",ltm->tm_mday); else sprintf(buf,"%d",ltm->tm_mday);
				fprintf (logfile_ptr, "%s ", buf );
				if ( ltm->tm_hour < 10) sprintf(buf," %d",ltm->tm_hour); else sprintf(buf,"%d",ltm->tm_hour);
				fprintf (logfile_ptr, "%s:", buf );
				if ( ltm->tm_min < 10) sprintf(buf,"0%d",ltm->tm_min); else sprintf(buf,"%d",ltm->tm_min);
				fprintf (logfile_ptr, "%s:", buf );
				if ( ltm->tm_sec < 10) sprintf(buf,"0%d",ltm->tm_sec); else sprintf(buf,"%d",ltm->tm_sec);
				fprintf (logfile_ptr, "%s : %s \n", buf, mymsg );
				fclose (logfile_ptr);
			}
		}	
    } else {  // logmode == interactive 
		if (mesgloglevel <= verboselevel) {
			fprintf(stdout, "%s\n", mymsg); 
		}
//	} else { // log via systemlog
//		if (mesgloglevel <= verboselevel) {
//			openlog ( "sensorhubd", LOG_PID | LOG_CONS| LOG_NDELAY, LOG_LOCAL0 );
//			syslog( LOG_NOTICE, "%s\n", mymsg);
//			closelog();
//		}
	}
}

void channelscanner (uint8_t channel) {
  int values=0;
  printf("Scanning channel %u: \n",channel);
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
  printf("\n\n 1000 passes: Detect %d times a carrier \n", values);
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
	int c, rv, numbytes;
	uint64_t akt_time, del_time, next_time;
	start_time = 0;
	start_time = mymillis();
    akt_time = mymillis();
	del_time = akt_time;
	next_time = akt_time;
	orderno = 1;
	logmode = interactive;
	strcpy(config_file,"x");
    struct addrinfo hints, *servinfo, *p;
    
	/* vars for telnet socket handling */
	int new_tn_in_socket;
	socklen_t addrlen;
//	char *buffer =  (char*) malloc (BUF);
	struct sockaddr_in address;
	long tcp_save_fd, udp_save_fd;
	const int y = 1;
//	bool wait4message = false;
	
    // processing argc and argv[]
	while (1) {
		static struct option long_options[] = {	
			{"daemon",  no_argument, 0, 'd'},
			{"verbose",  required_argument, 0, 'v'},
            		{"configfile",    required_argument, 0, 'c'},
			{"scanner", required_argument, 0, 's'},
                        {"scannchannel", required_argument, 0, 't'},
            		{"help", no_argument, 0, 'h'},
            		{0, 0, 0, 0} 
		};
        /* getopt_long stores the option index here. */
        int option_index = 0;
        c = getopt_long (argc, argv, "?dht:s:v:c:",long_options, &option_index);
        /* Detect the end of the options. */
        if (c == -1) break;
        switch (c) {
            case 't':
                      uint8_t channel;
                      if ( optarg[0] ) {
                        channel=optarg[0]-'0';
                        if ( optarg[1] ) {
                          channel=channel*10+optarg[1]-'0';
                          if ( optarg[2] ) {
                            channel=channel*10+optarg[2]-'0';
                          }
                        }
                        if (channel < 126) {
                          channelscanner(channel); 
                        } else {
                          printf("Error Channel must be in 0 ... 125\n");
                        }
                      } else {
                        printf("Error Channel required\n");
                      }
                      exit(0);
                      break;

            case 's':
                      if (optarg[0] && ! optarg[1]) {
                         scanner(optarg[0]);
                      } else {
                         usage(argv[0]);
                      }
                      exit(0);
                      break;
            case 'd':
				start_daemon = true;
				logmode = logfile;
            break;
			case 'v':
                verboselevel = (optarg[0] - '0') * 1;
            break;
            case 'c':
                strcpy(config_file, optarg);
            break;
            case 'h':
            case '?':
                usage(argv[0]);
                exit (0);
            break;
            default:
                usage (argv[0]);
                exit (0);
        }
    }
    /* Print any remaining command line arguments (not options). */
    if (optind < argc) {
        printf ("non-option ARGV-elements: ");
        while (optind < argc) printf ("%s ", argv[optind++]);
        putchar ('\n');
    }
    // END processing argc and argv[]

	// check if started as root
	if ( getuid()!=0 ) {
           fprintf(stdout, "rf24hubd has to be startet as user root\n");
          exit(1);
        }

    // check if config file is readable
    if ( strcmp(config_file,"x") == 0 ) strcpy(config_file,DEFAULT_CONFIG_FILE);
    if (fopen (config_file, "r") == NULL) {
        fprintf(stdout, "Config file: \"%s\" not found, terminating\n\n", config_file);
        exit(1);
    }
    // Reading and processing and printing config file
    printf ("Reading configuration from %s\n",config_file);
    printf ("Reading config file...\n");
    parse_config (&parms);
    printf ("Startup Parameters:\n");
    print_config (&parms);
    // check for PID file, if exists terminate else create it
    if( access( parms.pidfilename, F_OK ) != -1 ) {
        fprintf(stderr, "PIDFILE: %s exists, terminating\n\n", parms.pidfilename);
        exit(1);
    }
	
    // starts logging
    logfile_ptr = fopen (parms.logfilename,"a");
    if ( logfile_ptr == NULL ) {
        fprintf(stdout,"Could not open %s for writing\n Printig logs to console\n", parms.logfilename );
    } else {
		log2logfile = true;
        fclose(logfile_ptr);
		sprintf(debug, "Start logging to %s", parms.logfilename);
        logmsg(VERBOSESTARTUP, debug);
    }

    // open database
    sprintf(debug,"Maria-DB:");
    logmsg(VERBOSESTARTUP, debug);
    sprintf(debug,"MySQL client version: %s", mysql_get_client_info());
    logmsg(VERBOSESTARTUP, debug);
    db = mysql_init(NULL);
    int mysql_wait_count = 0;
    while (db == NULL) {
		sprintf(debug,"Waiting for Database: %d Sec.", 20-mysql_wait_count);
		logmsg(VERBOSESTARTUP, debug);		
		if ( mysql_wait_count < 20 ) {
			mysql_wait_count++;
			delay(1000);
			db = mysql_init(NULL);
		} else {
			fprintf(stderr, "%s\n", mysql_error(db));
            mysql_close(db);
			unlink(parms.pidfilename);
            exit(1);
		}
    }
    mysql_wait_count = 0;
    while (mysql_real_connect(db, parms.db_hostname, parms.db_username, parms.db_password, parms.db_schema, parms.db_port, NULL, 0) == NULL) {
		sprintf(debug,"Waiting for Database: %d Sec.", 20-mysql_wait_count);
		logmsg(VERBOSESTARTUP, debug);		
		if ( mysql_wait_count < 20 ) {
			mysql_wait_count++;
			delay(1000);
		} else {
			fprintf(stderr, "%s\n", mysql_error(db));
			mysql_close(db);
			unlink(parms.pidfilename);
			exit(1);
		}
    }
    sprintf(debug, "Connected to host %s with DB %s on port %d", parms.db_hostname, mysql_get_server_info(db), parms.db_port);
    logmsg(VERBOSESTARTUP, debug);

    // init SIGTERM and SIGINT handling
    signal(SIGTERM, sighandler);
    signal(SIGINT, sighandler);

    // run as daemon if started with -d
    if (start_daemon) {
        // make sure that we have a logfile
        if ( ! log2logfile ) {
            fprintf(stdout,"Logfile is needed if runs as deamon ... exiting\n");
            unlink(parms.pidfilename);
            exit(1);
        } else {
            // starts rf24hubd as a deamon
            // no messages to console!
            pid = fork ();
            if (pid == 0) {
                // Child prozess
                chdir ("/");
                umask (0);
                sprintf(debug, "Starting up ....");
                logmsg(2,debug);
            } else if (pid > 0) {
                // Parentprozess -> exit and return to shell
                // write a message to the console
                sprintf(debug, "Starting rf24hubd as daemon...");
                logmsg(2,debug);
                fprintf(stdout, debug);
                fprintf(stdout, "\n");
                // and exit
                exit (0);
            } else {
                // nagativ is an error
                unlink(parms.pidfilename);
                exit (1);
            }
        }
    }
    // get pid and write it to pidfile
    pid=getpid();
    pidfile_ptr = fopen (parms.pidfilename,"w");
    if (pidfile_ptr==NULL) {
        sprintf(debug,"Can't write PIDFILE: %s! Exit programm ....\n", parms.pidfilename);
        fprintf(stderr, debug);
        exit (1);
    }
    fprintf (pidfile_ptr, "%d", pid );
    fclose(pidfile_ptr);
    sprintf(debug, "%s running with PID: %d", PRGNAME, pid);
    logmsg(VERBOSESTARTUP, debug);
    if ( fhem_port_set && fhem_host_set ) {
        fhem_active = true;
        sprintf(debug, "telnet session started: Host: %s Port: %s ", parms.fhem_hostname, parms.fhem_port);
        logmsg(VERBOSESTARTUP, debug);
    }
	if ( tcp_in_port_set ) {
        // open TCP Socket
        memset(&hints, 0, sizeof hints);
        hints.ai_family =  	AF_INET6; //AF_INET; //AF_INET6; //AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE; // use my IP
        if ((rv = getaddrinfo(NULL, parms.tcp_in_port, &hints, &servinfo)) != 0) {
            sprintf(debug, "Abort ERROR: getaddrinfo: %s\n", gai_strerror(rv));
            error_exit( 1, debug);
        }
        // loop through all the results and bind to the first we can
        for(p = servinfo; p != NULL; p = p->ai_next) {
            if ((tcp_sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
                perror("server: socket");
                continue;
            }
            if (setsockopt(tcp_sockfd, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(int)) == -1) {
                sprintf(debug,"setsockopt");
                error_exit(1,debug);
            }
            if (bind(tcp_sockfd, p->ai_addr, p->ai_addrlen) == -1) {
                close(tcp_sockfd);
                perror("server: bind");
                continue;
            }
            break;
        }
        if (p == NULL)  {
            sprintf(debug, "server: failed to bind\n");
            error_exit( 2,debug);;
        }
        freeaddrinfo(servinfo); // all done with this structure
        if (listen(tcp_sockfd, 10) == -1) {
            sprintf(debug,"listen");
            error_exit(1,debug);
        }
        tcp_save_fd = fcntl( tcp_sockfd, F_GETFL );
        tcp_save_fd |= O_NONBLOCK;
        fcntl( tcp_sockfd, F_SETFL, tcp_save_fd );
        printf("server: waiting for tcp connections on %s ...\n", parms.tcp_in_port);
    }
    sleep(2);
	if ( udp_in_port_set ) {
        // open UDP Socket
        memset(&hints, 0, sizeof hints);
        hints.ai_family =  	AF_INET6; //AF_INET; //AF_INET6; //AF_UNSPEC;
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_flags = AI_PASSIVE; // use my IP
        if ((rv = getaddrinfo(NULL, parms.udp_in_port, &hints, &servinfo)) != 0) {
            sprintf(debug, "getaddrinfo: %s\n", gai_strerror(rv));
            error_exit( 1,debug);
        }
        // loop through all the results and bind to the first we can
        for(p = servinfo; p != NULL; p = p->ai_next) {
            if ((udp_sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
                perror("server: socket");
                continue;
            }
            if (bind(udp_sockfd, p->ai_addr, p->ai_addrlen) == -1) {
                close(udp_sockfd);
                perror("server: bind");
                continue;
            }
            break;
        }
        if (p == NULL)  {
            sprintf(debug, "server: failed to bind\n");
            error_exit( 2,debug);
        }
        freeaddrinfo(servinfo); // all done with this structure
        udp_save_fd = fcntl( udp_sockfd, F_GETFL );
        udp_save_fd |= O_NONBLOCK;
        fcntl( udp_sockfd, F_SETFL, udp_save_fd );
        printf("server: waiting for UDP connections on %s ...\n", parms.udp_in_port);
    }    
    sprintf(debug, "starting radio on channel ... %d ", parms.rf24network_channel);
    logmsg(VERBOSESTARTUP, debug);
    radio.begin();
//	radio.setRetries(1,15);
    delay(5);
    sprintf(debug, "starting network ... ");
    logmsg(VERBOSESTARTUP, debug);
    network.begin( parms.rf24network_channel, 0);
    radio.setDataRate(parms.rf24network_speed);
    if (verboselevel >= VERBOSECONFIG) { radio.printDetails(); }
    sprintf(debug,"\%s up and running .... ",PRGNAME);
    logmsg(VERBOSESTARTUP, debug);
	
	// Init Arrays
    init_system();
    
	// Main Loop
    while(1) {
		if (orderno > 50000) orderno = 1;
        /* Handling of incoming messages */
		if ( tcp_in_port_set ) {
            new_tn_in_socket = accept ( tcp_sockfd, (struct sockaddr *) &address, &addrlen );
            if (new_tn_in_socket > 0) {
                //receive_tn_in(new_tn_in_socket, &address);
                thread t2(receive_tn_in, new_tn_in_socket, &address);
                t2.detach();
                //close (new_tn_in_socket);
            }
        }
        if ( udp_in_port_set ) {
            numbytes = recvfrom(udp_sockfd, &udp_data, sizeof(udp_data), 0, (struct sockaddr *)&client_addr,  &addr_len);
            if (numbytes >0) {
                printf("listener: packet is %d bytes long\n", numbytes);
                printf("listener: packet received \n");
                printf("Network_number: %u \n",udp_data.network_id);
                printf("Msg_number: %u \n",udp_data.msg_id);
                printf("Sensor_id: %u \n",udp_data.sensor_id);
                printf("Sensor_value: %f \n",udp_data.value);
            }
        }
		network.update();
		if ( network.available() ) {
//
// Receive loop: react on the message from the nodes
//
//            rf24_carrier = radio.testCarrier();
			rf24_rpd = radio.testRPD();
			network.read(rxheader,&payload,sizeof(payload));
			sprintf(debug, DEBUGSTR "Received: Type: %u from Node: %o to Node: %o Orderno %d (Channel/Value): (%u/%f) (%u/%f) (%u/%f) (%u/%f) "
						, rxheader.type, rxheader.from_node, rxheader.to_node, payload.orderno
						, payload.channel1, payload.value1, payload.channel2, payload.value2, payload.channel3, payload.value3, payload.channel4, payload.value4);
			logmsg(VERBOSERF24, debug);
			if ( rxheader.type == 119 ) {
					init_node(rxheader.from_node);
			} else {	
				if (is_valid_orderno(payload.orderno)) {
					if ( payload.channel1 > 0 ) process_sensor(rxheader.from_node, payload.channel1, payload.value1, rf24_carrier, rf24_rpd);
					if ( payload.channel2 > 0 ) process_sensor(rxheader.from_node, payload.channel2, payload.value2, rf24_carrier, rf24_rpd);
					if ( payload.channel3 > 0 ) process_sensor(rxheader.from_node, payload.channel3, payload.value3, rf24_carrier, rf24_rpd);
					if ( payload.channel4 > 0 ) process_sensor(rxheader.from_node, payload.channel4, payload.value4, rf24_carrier, rf24_rpd);
					delete_orderno(payload.orderno);
				}
			}
			
		} // network.available
//
// Orderloop: Tell the nodes what they have to do
//
		akt_time=mymillis();
		if ( akt_time > del_time + DELETEINTERVAL ) {
// Cleanup old entries
			for(int i=0; i<ORDERLENGTH -1; i++) {
				if ((order[i].orderno > 0) && (order[i].entrytime + KEEPINBUFFERTIME < akt_time) ) {
					if ( verboselevel > 4 ) {
						sprintf(debug, "Deleted order[%d] OrderNo: %u for Node: 0%o ", i, order[i].orderno, order[i].node);
						logmsg(VERBOSEOTHER, debug);
					}
					init_order(i);		
				}					
			}
			del_time = akt_time;
		}
		if ( order_waiting && akt_time > next_time ) {  // go transmitting if its time to do ..
			next_time = akt_time + SENDINTERVAL;
			// Look if we have something to send
			order_waiting = false;
			for ( int order_ptr=0; order_ptr<ORDERLENGTH -1; order_ptr++) {
				if (order[order_ptr].orderno != 0) {
					order_waiting = true;
					// this orders are ready to send
					if ( akt_time > order[order_ptr].last_send + SENDINTERVAL ) {
						if ( node_is_next(order[order_ptr].node) ) {
							txheader.from_node = 0;
							payload.orderno = order[order_ptr].orderno;
							txheader.to_node  = order[order_ptr].node;
							payload.flags = order[order_ptr].flags;
							txheader.type  = order[order_ptr].type;
							payload.channel1  = order[order_ptr].channel1;
							payload.value1  = order[order_ptr].value1;
							payload.channel2  = order[order_ptr].channel2;
							payload.value2  = order[order_ptr].value2;
							payload.channel3  = order[order_ptr].channel3;
							payload.value3  = order[order_ptr].value3;
							payload.channel4  = order[order_ptr].channel4;
							payload.value4  = order[order_ptr].value4;
							if (network.write(txheader,&payload,sizeof(payload))) {
								if ( verboselevel >= VERBOSERF24  ) {
									sprintf(debug, DEBUGSTR "Send: Type: %u from Node: 0%o to Node: 0%o orderno %d (Channel/Value) (%u/%f) (%u/%f) (%u/%f) (%u/%f)"
											, txheader.type, txheader.from_node, txheader.to_node, payload.orderno
											, payload.channel1, payload.value1, payload.channel2, payload.value2, payload.channel3, payload.value3, payload.channel4, payload.value4);
									logmsg(VERBOSERF24, debug);
								}
							} else {
								if ( verboselevel >= VERBOSERF24 ) {
									sprintf(debug, DEBUGSTR "Failed: Type: %u from Node: 0%o to Node: 0%o orderno %d (Channel/Value) (%u/%f) (%u/%f) (%u/%f) (%u/%f)"
												, txheader.type, txheader.from_node, txheader.to_node, payload.orderno
												, payload.channel1, payload.value1, payload.channel2, payload.value2, payload.channel3, payload.value3, payload.channel4, payload.value4);
									logmsg(VERBOSERF24, debug);
								}
							}
							order[order_ptr].last_send = akt_time;
						} else {
							if ( verboselevel > 4 ) {
								sprintf(debug,"Node 0%o blocked!!!!!",order[order_ptr].node);
								logmsg(VERBOSEOTHER, debug);
							}
						}
					} else {
						if ( next_time > order[order_ptr].last_send + SENDINTERVAL ) next_time = order[order_ptr].last_send + SENDINTERVAL;
					}
				}	
			}
		}
		// Order seems to be empty if akt_time still greater than next_time
		if ( akt_time > next_time ) {
			next_time = akt_time + SENDINTERVAL;
				if ( verboselevel >= VERBOSEOTHER ) {
					sprintf(debug,"Order empty increment next_send by %d ms", SENDINTERVAL);
					logmsg(VERBOSEOTHER, debug);
				}
		}
		if ( order_waiting ) usleep(2000); else usleep(200000);
//
//  end orderloop
//
	} // while(1)
	return 0;
}

