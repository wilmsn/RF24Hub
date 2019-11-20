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
    else if (strcmp(name, "telnet_hostname")==0) {
                strcpy (parms->telnet_hostname, value);
                tn_host_set=true;
        }
    else if (strcmp(name, "telnet_port")==0) {
                parms->telnet_port = atoi(value);
                tn_port_set=true;
        }
    else if (strcmp(name, "incoming_port")==0) {
                parms->incoming_port = atoi(value);
                in_port_set=true;
        }
    else if (strcmp(name, "logfile")==0)     strcpy (parms->logfilename, value);
    else if (strcmp(name, "pidfile")==0)     strcpy (parms->pidfilename, value);
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
    printf ("DB-Hostname: %s\n", parms->db_hostname);
    printf ("DB-Port: %d\n", parms->db_port);
    printf ("DB-Schema: %s\n", parms->db_schema);
    printf ("DB-Username: %s\n", parms->db_username);
    printf ("DB-Password: %s\n", parms->db_password);
    printf ("Telnet-Hostname: %s\n", parms->telnet_hostname);
    printf ("Telnet-Port: %d\n", parms->telnet_port);
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
// do_tn_cmd ==> send a telnet comand to the fhem-host
// usage example: do_tn_cmd("set device1 on");
void do_tn_cmd(uint16_t node, uint8_t channel, float value) {
    char *tn_cmd =  (char*) malloc (TELNETBUFFERSIZE);
//	char tn_cmd[200];
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    Sensor::sensor_t *sensor_ptr;
    sensor_ptr=sensor.initial_ptr;
	while (sensor_ptr) {
		if ( sensor_ptr->node == node && sensor_ptr->channel == channel ) {
			sprintf(tn_cmd,"set %s %f\n", sensor_ptr->fhem_dev, value);
		}
		sensor_ptr=sensor_ptr->next;
	}
	sprintf(debug,"do_tn_cmd: %s", tn_cmd);
	logger.logmsg(VERBOSETELNET,debug);
    portno = parms.telnet_port;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        sprintf(debug,"do_tn_cmd: error opening socket");
		logger.logmsg(VERBOSECRITICAL,debug);
	}	
    server = gethostbyname(parms.telnet_hostname);
    if (server == NULL) {
        sprintf(debug,"do_tn_cmd: no such host");
		logger.logmsg(VERBOSECRITICAL,debug);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) { 
        sprintf(debug,"do_tn_cmd: error connecting");
		logger.logmsg(VERBOSECRITICAL,debug);
	} else {	
		n = write(sockfd,tn_cmd,strlen(tn_cmd));
		if (n < 0) {
			sprintf(debug,"do_tn_cmd: error writing to socket");
			logger.logmsg(VERBOSECRITICAL,debug);
		} else {
			sprintf(debug,"do_tn_cmd: Telnet to %s Port %d CMD: %s successfull",parms.telnet_hostname, portno, tn_cmd);
			logger.logmsg(VERBOSETELNET,debug);
		}		
	}		 
    close(sockfd);
    free(tn_cmd);
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
	logger.logmsg(VERBOSETELNET, debug);
    sprintf(buffer,"                                 ");
    MsgLen = recv(new_tn_in_socket, buffer, TELNETBUFFERSIZE, 0);
//    sprintf(client_message,"%s",buffer);
//    write(new_tn_in_socket , client_message , strlen(client_message));
//    char msglen_str[10];
//    sprintf(msglen_str,"%ld",MsgLen);
//	sprintf (debug,"Buffer: %s MsgLen: %d ", trim(buffer), MsgLen);
//	logger.logmsg(VERBOSETELNET, debug);

//    debug = "Buffer: \"";
//    debug += trim(buffer);
//    debug += "\" Msglen: ";
//    debug += msglen_str;
//    cfg.logmsg(VERBOSETELNET, debug);
    if (MsgLen>0) {
        process_tn_in(new_tn_in_socket, buffer, client_message);
    } else {
        sprintf (debug,"Nicht verarbeitete telnet message: %s MsgLen: %d ", trim(buffer), MsgLen);
        logger.logmsg(VERBOSETELNET, debug);
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
		push		<node>      <channel>   <value>	            Pushes a value direct to a channel into a node 
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
		 cmp_verbose[]="verbose",	
         cmp_push[]="push";
	char *wort1a, *wort2a, *wort3a, *wort4a;
	char *wort1, *wort2, *wort3, *wort4;
	bool tn_input_ok=false;
	char delimiter[] = " ";
	trim(buffer);
	sprintf(debug,"Incoming telnet data: %s ",buffer);
	logger.logmsg(VERBOSETELNET, debug);
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
	if ( (( strcmp(wort1,cmp_set) == 0 ) || ( strcmp(wort1,cmp_setlast) == 0 )) && (strcmp(wort2,cmp_sensor) == 0) && (strlen(wort3) > 0) && (strlen(wort4) > 0) ) {
		tn_input_ok = true;
		// In word3 we may have a) the number of the sensor b) the name of the sensor c) the fhem_dev of a sensor
		// for the processing we need the number of the sensor ==> find it!
        uint16_t mynode = 0;
        uint8_t mychannel = 0;
        sensor.find_node_chanel(&mynode, &mychannel, wort3, strtoul(wort3, &pEnd, 10));
        orderbuffer.add_orderbuffer(0,mymillis(),mynode,mychannel,strtof(wort4, &pEnd));
		if ( strcmp(wort1,cmp_setlast) == 0 && ! node.is_HB_node(mynode) ) {
			get_order(mynode);
			//print_orderbuffer();
		}
    }
	// push <node> <channel> <value>
	// Pushes a value direct to a channel into a node
	if (( strcmp(wort1,cmp_push) == 0 ) && (strlen(wort2) > 1) && (strlen(wort3) > 0) && (strlen(wort4) > 0) ) {
		tn_input_ok = true;
        orderbuffer.add_orderbuffer(0,mymillis(),getnodeadr(wort2), strtol(wort3, &pEnd, 10), strtof(wort4, &pEnd));
    }
    // set node <node> init
	// sends the init sequence to a node
	if (( strcmp(wort1,cmp_set) == 0 ) && (strcmp(wort2,cmp_node) == 0) && (strlen(wort3) > 1) && (strcmp(wort4,cmp_init) == 0) ) {
		tn_input_ok = true;
		init_node(getnodeadr(wort3));
	}
    // set verbose <new verboselevel>
	// sets the new verboselevel
	if (( strcmp(wort1,cmp_set) == 0 ) && (strcmp(wort2,cmp_verbose) == 0) && (strlen(wort3) == 1) && (strlen(wort4) == 0) ) {
        if ( wort3[0] > '0' && wort3[0] < '9' + 1 ) {
			tn_input_ok = true;
			verboselevel = (wort3[0] - '0') * 1;
		}	
	}
    // list order
	// lists the current orderbuffer
	if (( strcmp(wort1,cmp_list) == 0 ) && (strcmp(wort2,cmp_order) == 0) && (strlen(wort3) == 0) && (strlen(wort4) == 0) ) {
		tn_input_ok = true;
        orderbuffer.print_buffer(new_tn_in_socket);
        order.print_buffer(new_tn_in_socket);
	}	
    // list sensor
	// lists the current node- and sensorbuffer
	if (( strcmp(wort1,cmp_list) == 0 ) && (strcmp(wort2,cmp_sensor) == 0) && (strlen(wort3) == 0) && (strlen(wort4) == 0) ) {
		tn_input_ok = true;
        node.print_buffer(new_tn_in_socket);
        sensor.print_buffer(new_tn_in_socket);
	}	
	// html order
	// lists the current order/orderbuffer for html page
	if (( strcmp(wort1,cmp_html) == 0 ) && (strcmp(wort2,cmp_order) == 0) && (strlen(wort3) == 0) && (strlen(wort4) == 0) ) {
		tn_input_ok = true;
        orderbuffer.html_buffer(new_tn_in_socket);
        order.html_buffer(new_tn_in_socket);
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
		sprintf(client_message,"push <node> <channel> <sensorvalue>\n");
		write(new_tn_in_socket , client_message , strlen(client_message));
		sprintf(client_message,"   Pushes a value direct to a channel inside a node\n");
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
		sprintf(client_message,"list sensor\n");
		write(new_tn_in_socket , client_message , strlen(client_message));
		sprintf(client_message,"   lists the nodes and sensors in system\n");
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
void init_node(uint16_t mynode ) {
	// delete old entries for this node
    Sensor::sensor_t *sensor_ptr;
	sprintf(debug,"Init of Node: 0%o", mynode);
	logger.logmsg(VERBOSEOTHER,debug);
	sprintf (sql_stmt, "select sleeptime1, sleeptime2, sleeptime3, sleeptime4, radiomode, voltagefactor from node where node_id = '0%o' LIMIT 1 ",mynode);
	mysql_query(db, sql_stmt);
	db_check_error();
	MYSQL_RES *result = mysql_store_result(db);
	db_check_error();
	MYSQL_ROW row;
	if ((row = mysql_fetch_row(result))) {
		orderbuffer.add_orderbuffer(0, mymillis(), mynode, 111, strtof(row[0], &pEnd));
		orderbuffer.add_orderbuffer(0, mymillis(), mynode, 112, strtof(row[1], &pEnd));
		orderbuffer.add_orderbuffer(0, mymillis(), mynode, 113, strtof(row[2], &pEnd));
		orderbuffer.add_orderbuffer(0, mymillis(), mynode, 114, strtof(row[3], &pEnd));
		orderbuffer.add_orderbuffer(0, mymillis(), mynode, 115, strtof(row[4], &pEnd));
		orderbuffer.add_orderbuffer(0, mymillis(), mynode, 116, strtof(row[5], &pEnd));
		orderbuffer.add_orderbuffer(0, mymillis(), mynode, 118, 1.0);
	}
	mysql_free_result(result);
    sensor_ptr=sensor.initial_ptr;
    while (sensor_ptr) {
		if (sensor_ptr->node == mynode && sensor_ptr->s_type == 'a') {
			orderbuffer.add_orderbuffer(0, mymillis(), mynode, sensor_ptr->channel, sensor_ptr->last_val);
		}
		sensor_ptr=sensor_ptr->next;
	}
	if ( ! node.is_HB_node(mynode) ) get_order(mynode);
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

bool get_order(uint16_t node) {
	int j = 0;
    bool retval = false;
	orderno++;
	order_waiting = true;
    OrderBuffer::orderbuffer_t* orderbuffer_ptr;
    sprintf(debug, "get_order: node: 0%o orderno: %u", node, orderno);
	logger.logmsg(VERBOSEORDER,debug);
//if we have an old order for this node => delete it!
    order.del_node(node);
    orderbuffer_ptr = orderbuffer.initial_ptr;
    Order::order_t* neworder_ptr = new Order::order_t;
    while (orderbuffer_ptr) {
        if (node == orderbuffer_ptr->node) {
			sprintf(debug, "get_order: j is: %d ", j);
			logger.logmsg(VERBOSEORDER,debug);
			if ( j < 4 ) orderbuffer_ptr->orderno = orderno;
			if (j == 0) {
				neworder_ptr->orderno = orderno;
				neworder_ptr->node = node;
				neworder_ptr->type = 61;
				neworder_ptr->entrytime = mymillis();
				neworder_ptr->flags = 0x00;
				neworder_ptr->channel1 = orderbuffer_ptr->channel;
				neworder_ptr->value1 = orderbuffer_ptr->value;				
			}
			if (j == 1) {
				neworder_ptr->channel2 = orderbuffer_ptr->channel;
				neworder_ptr->value2 = orderbuffer_ptr->value;				
			}
			if (j == 2) {
				neworder_ptr->channel3 = orderbuffer_ptr->channel;
				neworder_ptr->value3 = orderbuffer_ptr->value;				
			}
			if (j == 3) {
				neworder_ptr->channel4 = orderbuffer_ptr->channel;
				neworder_ptr->value4 = orderbuffer_ptr->value;	
			}
			j++;
		}
		orderbuffer_ptr=orderbuffer_ptr->next;
	}
	if (j < 5) neworder_ptr->flags = 0x01;
    neworder_ptr->next=NULL;
    if (j > 0) {
        order.new_entry(neworder_ptr);
        //print_order();
        retval = true;
    } else {
        delete neworder_ptr;
        retval = false;
    }
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
        logger.logmsg(VERBOSECRITICAL, debug);
    }
}

void do_sql(char *sqlstmt) {
	if (mysql_query(db, sqlstmt)) {
		sprintf(debug, "%s", mysql_error(db));
		logger.logmsg(VERBOSECRITICAL, debug);
	}
    sprintf(debug, "%s", sqlstmt);
	logger.logmsg(VERBOSESQL, debug);
}

void exit_system(void) {
    // Save data from sensordata_im and sensor_im to persistant tables
	sprintf (sql_stmt, "update sensor a set value = ( select value from sensor_im where sensor_id = a.sensor_id ), utime = ( select utime from sensor_im where sensor_id = a.sensor_id )");
	logger.logmsg(VERBOSESQL, sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
	sprintf (sql_stmt, "insert into sensordata(sensor_id, utime, value) select sensor_id, utime, value from sensordata_im where (sensor_id,utime) not in (select sensor_id, utime from sensordata)");
	logger.logmsg(VERBOSESQL, sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
}

void init_system(void) {
	char cmp_s[]="s",cmp_a[]="a",cmp_y[]="y",cmp_j[]="j"; 
    // Copy sensordata and sensor to memorytable since yesterday
	sprintf (sql_stmt, "truncate table sensor_im");
	logger.logmsg(VERBOSESQL, sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
	sprintf (sql_stmt, "insert into sensor_im(sensor_id, sensor_name, add_info, node_id, channel, value, utime, store_days, fhem_dev, signal_quality, s_type, html_show) select sensor_id, sensor_name, add_info, node_id, channel, value, utime, store_days, fhem_dev, signal_quality, s_type, html_show from sensor");
	logger.logmsg(VERBOSESQL, sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
	sprintf (sql_stmt, "truncate table sensordata_im");
	logger.logmsg(VERBOSESQL, sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
	sprintf (sql_stmt, "insert into sensordata_im(sensor_id, utime, value) select sensor_id, utime, value from sensordata where utime > UNIX_TIMESTAMP(subdate(current_date, 2))");
	logger.logmsg(VERBOSESQL, sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
	// END sensordata to memorytable
	sprintf (sql_stmt, "select sensor_id, node_id, channel, value, fhem_dev, s_type from sensor");
	logger.logmsg(VERBOSESQL, sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
	MYSQL_RES *result = mysql_store_result(db);
	db_check_error();
	MYSQL_ROW row;
	while ((row = mysql_fetch_row(result))) {
        Sensor::sensor_t* newsensor_ptr = new Sensor::sensor_t;
		if ( row[0] != NULL ) newsensor_ptr->sensor = strtoul(row[0], &pEnd,10); else newsensor_ptr->sensor = 0;
		if ( row[1] != NULL ) newsensor_ptr->node = getnodeadr(row[1]); else newsensor_ptr->node = 0; 
		if ( row[2] != NULL ) newsensor_ptr->channel = strtoul(row[2], &pEnd,10); else newsensor_ptr->channel = 0;
		if ( row[3] != NULL ) newsensor_ptr->last_val = strtof(row[3], &pEnd); else newsensor_ptr->last_val = 0;
		if ( row[4] != NULL ) sprintf(newsensor_ptr->fhem_dev,"%s",row[4]); else sprintf(newsensor_ptr->fhem_dev,"not_set");
        if ( row[5] != NULL ) {
            if (strcmp(row[5],cmp_s) == 0) newsensor_ptr->s_type = 's';
            if (strcmp(row[5],cmp_a) == 0) newsensor_ptr->s_type = 'a';
        }
        newsensor_ptr->last_ts = 0;
        newsensor_ptr->next=NULL;
        sensor.new_entry(newsensor_ptr);
	}
	mysql_free_result(result);
	sprintf (sql_stmt, "select node_id, u_batt, heartbeat from node");
	logger.logmsg(VERBOSESQL, sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
	result = mysql_store_result(db);
	db_check_error();
	while ((row = mysql_fetch_row(result))) {
        uint16_t new_node = 0;
        float new_u_batt = 0;
        bool new_HB_node = false;
		if ( row[0] != NULL ) new_node = getnodeadr(row[0]);
		if ( row[1] != NULL ) new_u_batt = strtof(row[1], &pEnd); else new_u_batt = 0;
        if ( row[5] != NULL ) if ((strcmp(row[2],cmp_y) == 0) || (strcmp(row[2],cmp_j) == 0)) new_HB_node = true; 
        if (new_node > 0) node.add_node(new_node, new_u_batt, new_HB_node); 
	}
	mysql_free_result(result);    
//	print_sensor();
}

void store_sensor_value(uint16_t node, uint8_t channel, float value, bool d1, bool d2) {
//    if ( sensor.update_last_val(node, channel, value, mymillis() )) {    
        if ( tn_active ) { 
            do_tn_cmd(node, channel, value); 
        }
        sprintf(sql_stmt,"insert into sensordata_im (sensor_ID, utime, value) select sensor_id, UNIX_TIMESTAMP(), %f from sensor_im where node_id = '0%o' and channel = %u ", value, node, channel);
        do_sql(sql_stmt);
        sprintf(sql_stmt,"update sensor_im set value= %f, utime = UNIX_TIMESTAMP(), signal_quality = '%d%d' where node_id = '0%o' and channel = %u ", value, d1, d2, node, channel);
        do_sql(sql_stmt);
//    }
}

void process_sensor(uint16_t node, uint8_t channel, float value, bool d1, bool d2) {
	switch (channel) {
		case 1 ... 99: {
		// Sensor or Actor
			sprintf(debug, "Value of  %u on Node: 0%o is %f ", channel, node, value);
			logger.logmsg(VERBOSECONFIG, debug);       
			store_sensor_value(node, channel, value, d1, d2);
		}
		break; 
		case 101: {
		// battery voltage
			sprintf(debug, "Voltage of Node: 0%o is %f ", node, value);
			logger.logmsg(VERBOSECONFIG, debug);        
			sprintf(sql_stmt,"update node set u_batt = %f, signal_quality = '%d%d', last_contact = unix_timestamp() where node_id = '0%o'", value, d1, d2, node);
// TODO
//			do_sql(sql_stmt);
			store_sensor_value(node, channel, value, d1, d2);
		}
		break; 
		case 111: { // Init Sleeptime 1
			sprintf(debug, "Node: 0%o: Sleeptime1 set to %f ", node, value);
			logger.logmsg(VERBOSECONFIG, debug);        
		}	
		break; 
		case 112: { // Init Sleeptime 2
			sprintf(debug, "Node: 0%o: Sleeptime2 set to %f ", node, value);
			logger.logmsg(VERBOSECONFIG, debug);        
		}
		break; 
		case 113: { // Init Sleeptime 3
			sprintf(debug, "Node: 0%o: Sleeptime3 set to %f ", node, value);
			logger.logmsg(VERBOSECONFIG, debug);        
		}
		break; 				
		case 114: { // Init Sleeptime 4
			sprintf(debug, "Node: 0%o: Sleeptime4 set to %f ", node, value);
			logger.logmsg(VERBOSECONFIG, debug);        
		}
		break; 
		case 115: { // Init Radiobuffer
			bool radio_always_on = value > 0.5;
			if ( radio_always_on ) sprintf(debug, "Node: %o: Radio allways on", node);
			else sprintf(debug, "Node: 0%o: Radio sleeps", node);
			logger.logmsg(VERBOSECONFIG, debug);        
		}					
		break;  
		case 116: { // Init Voltagefactor
			sprintf(debug, "Node: 0%o: Set Voltagefactor to: %f.", node, value);
			logger.logmsg(VERBOSECONFIG, debug);        
		}
		break;  
		case 118: {
			sprintf(debug, "Node: 0%o Init finished.", node);
			logger.logmsg(VERBOSECONFIG, debug);        
		}
		break; 
		default: { 
			sprintf(debug, "Message dropped!!!! Node: 0%o Channel: %u Value: %f ", node, channel, value);
			logger.logmsg(VERBOSECONFIG, debug);        
		}
	}	
	orderbuffer.del_node_channel(node, channel);
}	

/*******************************************************************************************
*
* All the rest 
*
********************************************************************************************/
void sighandler(int signal) {
	sprintf(debug, "SIGTERM: Cleanup system ... saving *_im tables ...");
	logger.logmsg(VERBOSECRITICAL, debug);
    exit_system(); 
	sprintf(debug, "SIGTERM: Shutting down ... ");
	logger.logmsg(VERBOSECRITICAL, debug);
    unlink(parms.pidfilename);
//	msgctl(msqid, IPC_RMID, NULL);
    exit (0);
}

uint64_t mymillis(void) {
	struct timeval tv;
	uint64_t timebuf;
	gettimeofday(&tv, NULL);
	timebuf = ((tv.tv_sec & 0x000FFFFF) * 1000 + (tv.tv_usec / 1000)) - start_time;
	sprintf(debug, "Mymillis: -----> %llu", timebuf );
	logger.logmsg(VERBOSEOTHER, debug);
	return timebuf;
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
	int c;
	start_time = mymillis();
	orderno = 1;
	logger.set_logmode('i');
	strcpy(config_file,"x");

	/* vars for telnet socket handling */
	int tn_in_socket, new_tn_in_socket;
	socklen_t addrlen;
//	char *buffer =  (char*) malloc (BUF);
	struct sockaddr_in address;
	long save_fd;
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
				logger.set_logmode('l');
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
    logger.verboselevel = verboselevel;
    logfile_ptr = fopen (parms.logfilename,"a");
    if ( logfile_ptr == NULL ) {
        fprintf(stdout,"Could not open %s for writing\n Printig logs to console\n", parms.logfilename );
    } else {
		log2logfile = true;
        fclose(logfile_ptr);
        logger.set_logfile(parms.logfilename);
		sprintf(debug, "Start logging to %s", parms.logfilename);
        logger.logmsg(VERBOSESTARTUP, debug);
    }
    node.begin(&logger);
    sensor.begin(&logger);
    order.begin(&logger);
    orderbuffer.begin(&logger);
    // open database
    sprintf(debug,"Maria-DB:");
    logger.logmsg(VERBOSESTARTUP, debug);
    sprintf(debug,"MySQL client version: %s", mysql_get_client_info());
    logger.logmsg(VERBOSESTARTUP, debug);
    db = mysql_init(NULL);
    int mysql_wait_count = 0;
    while (db == NULL) {
		sprintf(debug,"Waiting for Database: %d Sec.", 20-mysql_wait_count);
		logger.logmsg(VERBOSESTARTUP, debug);		
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
		logger.logmsg(VERBOSESTARTUP, debug);		
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
    logger.logmsg(VERBOSESTARTUP, debug);

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
                logger.logmsg(2,debug);
            } else if (pid > 0) {
                // Parentprozess -> exit and return to shell
                // write a message to the console
                sprintf(debug, "Starting rf24hubd as daemon...");
                logger.logmsg(2,debug);
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
    logger.logmsg(VERBOSESTARTUP, debug);
    if ( tn_port_set && tn_host_set ) {
        tn_active = true;
        sprintf(debug, "telnet session started: Host: %s Port: %d ", parms.telnet_hostname, parms.telnet_port);
        logger.logmsg(VERBOSESTARTUP, debug);
    }
	tn_in_socket=0;
	if ( in_port_set ) {
    /* open incoming port for messages */
		if ((tn_in_socket=socket( AF_INET, SOCK_STREAM, 0)) > 0) {
			sprintf (debug,"Socket für eingehende Messages auf Port %i angelegt", parms.incoming_port);
			logger.logmsg(VERBOSESTARTUP, debug);
		}
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
		address.sin_port = htons (parms.incoming_port);
		setsockopt( tn_in_socket, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(int) );
		if (bind( tn_in_socket, (struct sockaddr *) &address, sizeof (address)) == 0 ) {
			sprintf (debug,"Binding Socket OK");
			logger.logmsg(VERBOSESTARTUP, debug);
		}
		listen (tn_in_socket, 5);
		addrlen = sizeof (struct sockaddr_in);
		save_fd = fcntl( tn_in_socket, F_GETFL );
		save_fd |= O_NONBLOCK;
		fcntl( tn_in_socket, F_SETFL, save_fd );
	}
    sleep(2);
    sprintf(debug, "starting radio on channel ... %d ", parms.rf24network_channel);
    logger.logmsg(VERBOSESTARTUP, debug);
    radio.begin();
//	radio.setRetries(1,15);
    delay(5);
    sprintf(debug, "starting network ... ");
    logger.logmsg(VERBOSESTARTUP, debug);
    network.begin( parms.rf24network_channel, 0);
    radio.setDataRate(parms.rf24network_speed);
    if (verboselevel >= VERBOSECONFIG) { radio.printDetails(); }
    sprintf(debug,"\%s up and running .... ",PRGNAME);
    logger.logmsg(VERBOSESTARTUP, debug);
	
	// Init Arrays
    init_system();
    
	// Main Loop
    while(1) {
		if (orderno > 50000) orderno = 1;
        /* Handling of incoming messages */
		if ( in_port_set ) {
//			char client_message[30];
            new_tn_in_socket = accept ( tn_in_socket, (struct sockaddr *) &address, &addrlen );
            if (new_tn_in_socket > 0) {
                //receive_tn_in(new_tn_in_socket, &address);
//printf("######1");
                thread t2(receive_tn_in, new_tn_in_socket, &address);
                t2.detach();
                //close (new_tn_in_socket);
            }
            
/*            
			if ( ! wait4message ) {  
				new_tn_in_socket = accept ( tn_in_socket, (struct sockaddr *) &address, &addrlen );
				if (new_tn_in_socket > 0) {
					wait4message = true;
					// send something like a prompt. perl telnet is waiting for it otherwise we get error
					// use this in perl: my $t = new Net::Telnet (Timeout => 2, Port => 7001, Prompt => '/rf24hub>/');
					sprintf(client_message,"rf24hub> ");
					write(new_tn_in_socket , client_message , strlen(client_message));
					sprintf (debug,"Client %s ist connected ...", inet_ntoa (address.sin_addr));
					logmsg(VERBOSECONFIG, debug);
				}
			} else {
				save_fd = fcntl( new_tn_in_socket, F_GETFL );
				save_fd |= O_NONBLOCK;
				fcntl( new_tn_in_socket, F_SETFL, save_fd );
				// Process data  //
				sprintf(buffer,"                                                                               ");
				MsgLen = recv(new_tn_in_socket, buffer, BUF, 0);
				if (MsgLen>0) {
					process_tn_in(new_tn_in_socket, buffer, client_message);
					close (new_tn_in_socket);
					wait4message = false;
				}
			}	 
			*/
		}
		network.update();
		if ( network.available() ) {
//
// Receive loop: react on the message from the nodes
//
//            rf24_carrier = radio.testCarrier();
			rf24_rpd = radio.testRPD();
//            network.peek(rxheader);
//            
			network.read(rxheader,&payload,sizeof(payload));
			sprintf(debug, "Received: Type: %u from Node: %o to Node: %o Orderno %d (C/V): (%u/%f) (%u/%f) (%u/%f) (%u/%f)"
						, rxheader.type, rxheader.from_node, rxheader.to_node, payload.orderno
						, payload.channel1, payload.value1, payload.channel2, payload.value2, payload.channel3, payload.value3, payload.channel4, payload.value4);
			logger.logmsg(VERBOSERF24, debug);
			switch ( rxheader.type ) {
                case 51: // heartbeatnode!!
                    if (node.is_new_HB(rxheader.from_node, mymillis())) {
                        if ( payload.channel1 > 0 ) 
                            process_sensor(rxheader.from_node, payload.channel1, payload.value1, rf24_carrier, rf24_rpd);
                        if ( payload.channel2 > 0 ) 
                            process_sensor(rxheader.from_node, payload.channel2, payload.value2, rf24_carrier, rf24_rpd);
                        if ( payload.channel3 > 0 ) 
                            process_sensor(rxheader.from_node, payload.channel3, payload.value3, rf24_carrier, rf24_rpd);
                        if ( payload.channel4 > 0 ) 
                            process_sensor(rxheader.from_node, payload.channel4, payload.value4, rf24_carrier, rf24_rpd);
//                    order.del_orderno(payload.orderno);
                        if ( orderbuffer.node_has_entry(rxheader.from_node) ) {
                            sprintf(debug, "Entries for Heartbeat Node found, sending them");
                            logger.logmsg(VERBOSEORDER, debug);
                            get_order(rxheader.from_node);                    
                        } else {
                            sprintf(debug, "No Entries for Heartbeat Node found, sending Endmessage");
                            logger.logmsg(VERBOSEORDER, debug);
                            txheader.from_node = 0;
                            txheader.to_node  = rxheader.from_node;
                            txheader.type = 52;
                            payload.orderno=0;
                            payload.flags=0x01;
                            payload.channel1=0;
                            payload.value1=0;
                            payload.channel2=0;
                            payload.value2=0;
                            payload.channel3=0;
                            payload.value3=0;
                            payload.channel4=0;
                            payload.value4=0;
							if (network.write(txheader,&payload,sizeof(payload))) {
								if ( verboselevel >= VERBOSERF24  ) {
									sprintf(debug, "Send: Type: %u from Node: 0%o to Node: 0%o orderno %d (Channel/Value) (%u/%f) (%u/%f) (%u/%f) (%u/%f)"
											, txheader.type, txheader.from_node, txheader.to_node, payload.orderno
											, payload.channel1, payload.value1, payload.channel2, payload.value2, payload.channel3, payload.value3, payload.channel4, payload.value4);
									logger.logmsg(VERBOSERF24, debug);
								}
							} else {
								if ( verboselevel >= VERBOSERF24 ) {
									sprintf(debug, "Failed: Type: %u from Node: 0%o to Node: 0%o orderno %d (Channel/Value) (%u/%f) (%u/%f) (%u/%f) (%u/%f)"
												, txheader.type, txheader.from_node, txheader.to_node, payload.orderno
												, payload.channel1, payload.value1, payload.channel2, payload.value2, payload.channel3, payload.value3, payload.channel4, payload.value4);
									logger.logmsg(VERBOSERF24, debug);
								}
							}
                        }
                    }
                break;    
                case 119:
					init_node(rxheader.from_node);
                break;
                default:	
				if ( orderbuffer.find_orderno(payload.orderno) || order.find_orderno(payload.orderno) ) {
					if ( payload.channel1 > 0 ) process_sensor(rxheader.from_node, payload.channel1, payload.value1, rf24_carrier, rf24_rpd);
					if ( payload.channel2 > 0 ) process_sensor(rxheader.from_node, payload.channel2, payload.value2, rf24_carrier, rf24_rpd);
					if ( payload.channel3 > 0 ) process_sensor(rxheader.from_node, payload.channel3, payload.value3, rf24_carrier, rf24_rpd);
					if ( payload.channel4 > 0 ) process_sensor(rxheader.from_node, payload.channel4, payload.value4, rf24_carrier, rf24_rpd);
                    // Order compleate => delete it!
					order.del_orderno(payload.orderno);
                    // Check if we still have orders for this node
                    get_order(rxheader.from_node);
				}
			}
			
		} // network.available
//
// Orderloop: Tell the nodes what they have to do
//
		if ( order.has_order ) {  // go transmitting if its time to do ..
			// Look if we have something to send
			while ( order.get_order_for_transmission(&payload.orderno, &txheader.to_node, &txheader.type, &payload.flags,
                &payload.channel1, &payload.value1, &payload.channel2, &payload.value2, 
                &payload.channel3, &payload.value3, &payload.channel4, &payload.value4, mymillis() )) {
					txheader.from_node = 0;
					if (network.write(txheader,&payload,sizeof(payload))) {
						if ( verboselevel >= VERBOSERF24  ) {
							sprintf(debug, "Send: Type: %u from Node: 0%o to Node: 0%o orderno %d (Channel/Value) (%u/%f) (%u/%f) (%u/%f) (%u/%f)"
									, txheader.type, txheader.from_node, txheader.to_node, payload.orderno
									, payload.channel1, payload.value1, payload.channel2, payload.value2, payload.channel3, payload.value3, payload.channel4, payload.value4);
							logger.logmsg(VERBOSERF24, debug);
						}
					} else {
						if ( verboselevel >= VERBOSERF24 ) {
							sprintf(debug, "Failed: Type: %u from Node: 0%o to Node: 0%o orderno %d (Channel/Value) (%u/%f) (%u/%f) (%u/%f) (%u/%f)"
									, txheader.type, txheader.from_node, txheader.to_node, payload.orderno
									, payload.channel1, payload.value1, payload.channel2, payload.value2, payload.channel3, payload.value3, payload.channel4, payload.value4);
							logger.logmsg(VERBOSERF24, debug);
                        }
                    }
				}
				usleep(2000);
        } else {
            usleep(200000);
        }
//
//  end orderloop
//
	} // while(1)
	return 0;
}

