#include "rf24hubd.h" 

/*******************************************************************************************
*
* Configfilehandling
* default place to look at is: DEFAULT_CONFIG_FILE (see sensorhub.h)
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
    char name[PARAM_MAXLEN], value[PARAM_MAXLEN];
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
// exec_tn_cmd ==> send a telnet comand to the fhem-host
// usage example: exec_tn_cmd("set device1 on");
void exec_tn_cmd(const char *tn_cmd) {
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
	char debug[200];
    
	sprintf(debug,"DEBUG: %s\n", tn_cmd);
	logmsg(7,debug);
    portno = parms.telnet_port;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        sprintf(debug,"ERROR: opening socket");
		logmsg(2,debug);
	}	
    server = gethostbyname(parms.telnet_hostname);
    if (server == NULL) {
        sprintf(debug,"ERROR: no such host\n");
		logmsg(2,debug);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) { 
        sprintf(debug,"ERROR: connecting");
		logmsg(2,debug);
	} else {	
		n = write(sockfd,tn_cmd,strlen(tn_cmd));
		if (n < 0) {
			sprintf(debug,"ERROR: writing to socket");
			logmsg(2,debug);
		} else {
			sprintf(debug,"Telnet to %s Port %d CMD: %s successfull",parms.telnet_hostname, portno, tn_cmd);
			logmsg(7,debug);
		}		
	}		 
    close(sockfd);
}

void prepare_tn_cmd(MYSQL *db,  uint16_t node, uint8_t sensor, float value) {
	char telnet_cmd[200];
	sprintf (sql_stmt, "select fhem_dev from sensor where node_id = '0%o' and sensor_id = %u and fhem_dev is not null LIMIT 1 ", node, sensor);
	mysql_query(db, sql_stmt);
	db_check_error(db);
	MYSQL_RES *result = mysql_store_result(db);
	db_check_error(db);
	MYSQL_ROW row;
	while ((row = mysql_fetch_row(result))) {
		sprintf(telnet_cmd,"set %s %f \n", row[0], value);
		sprintf(debug,"Telnet-CMD: %s\n", telnet_cmd);
		logmsg(8,debug);				
		exec_tn_cmd(telnet_cmd);
	}
}

void fill_telnet_buffer( uint32_t sensor, uint16_t node, uint16_t channel, float value) {
	int i=0;
	while ( telnet_buffer[i].sensor > 0 && i < MAXTELNETBUFFER) i++;
	telnet_buffer[i].sensor=sensor;
	telnet_buffer[i].node=node;
	telnet_buffer[i].channel=channel;
	telnet_buffer[i].value=value;
}

void telnet_buffer2db( MYSQL *db, uint16_t node ) {
	struct tb_t {
	uint16_t     s;
	uint16_t     c;
	float        v;
	};
	tb_t tb[4];
	uint16 flags;
	if ( verboselevel > 8 ) {
		sprintf(debug, "Array> Called with Node: 0%o ", node);
		logmsg(9,debug);				
		for (int i=0; i < MAXTELNETBUFFER; i++) {
			sprintf(debug, "Array> Sensor: %u; Node: 0%o; Channel: %u; Value: %f ", telnet_buffer[i].sensor, telnet_buffer[i].node, telnet_buffer[i].channel, telnet_buffer[i].value);
			logmsg(9,debug);				
		}
	}
	int i=0, m=61;	
	bool finished = false;
	while ( ! finished ) {
		for (int j=0; j<4; j++) { tb[j].s=0; tb[j].c=0; tb[j].v=0; }
		int k=0;
//		bool sql_ready = false;
		while ( k < 4 && i < MAXTELNETBUFFER ) {
			while ( telnet_buffer[i].node != node && i < MAXTELNETBUFFER ) i++;
			if ( telnet_buffer[i].node == node ) {
				tb[k].s = telnet_buffer[i].sensor;
				tb[k].c = telnet_buffer[i].channel;
				tb[k].v = telnet_buffer[i].value;
				k++;
				i++;
			}
			if ( ( k > 3 || i > MAXTELNETBUFFER - 1 ) && tb[0].s > 0 ) {
//printf("**** i: %d k: %d \n",i,k);				
				if ( i > MAXTELNETBUFFER-1 ) { flags = 0x01; finished=true; } else flags = 0x00;
				if (orderno > 50000) orderno=1; else orderno++;
				sprintf(sql_stmt,"insert into jobbuffer(orderno,flags,node_id,n_type,sensor_id1,channel1,value1,sensor_id2,channel2,value2,sensor_id3,channel3,value3,sensor_id4,channel4,value4) values(%u,%u,'0%o',%u, %u,%u,%f,%u,%u,%f,%u,%u,%f,%u,%u,%f)"
				,orderno,flags,node,m,tb[0].s,tb[0].c,tb[0].v,tb[1].s,tb[1].c,tb[1].v,tb[2].s,tb[2].c,tb[2].v,tb[3].s,tb[3].c,tb[3].v);
				do_sql(db, sql_stmt);
				m++;
			}
		}	
	}	
}
	
void process_tn_in(MYSQL *db, int new_socket, char* buffer, char* client_message) {
	char cmp1[10], cmp1a[10], 
		 cmp2[10], 
		 cmp3[10];
	char *wort1, *wort2, *wort3, *wort4;
	uint8_t channel = 0;
	uint16_t node = 0;
	bool tn_input_ok=false;
	char delimiter[] = " ";
	trim(buffer);
	sprintf(debug,"Incoming telnet data: %s ",buffer);
	logmsg(7, debug);
	wort1 = strtok(buffer, delimiter);
	wort2 = strtok(NULL, delimiter);
	wort3 = strtok(NULL, delimiter);
	wort4 = strtok(NULL, delimiter);
    if ( wort4 != NULL ) {
		sprintf(cmp1, "set");
		sprintf(cmp1a, "setlast");
		sprintf(cmp2, "sensor");
		if ( (( strcmp(wort1,cmp1) == 0 ) || ( strcmp(wort1,cmp1a) == 0 )) && (strcmp(wort2,cmp2) == 0) && (wort3 != NULL) && (wort4 != NULL) ) {
			tn_input_ok = true;
			// delete old entries for the same sensor with the same value
			sprintf(sql_stmt,"select node_id, channel from sensor where sensor_id = %s ", wort3);
			mysql_query(db, sql_stmt);
			db_check_error(db);
			MYSQL_RES *result = mysql_store_result(db);
			db_check_error(db);
			MYSQL_ROW row;
			if ((row = mysql_fetch_row(result))) {
				node = getnodeadr(row[0]);
				channel = strtoul(row[1], &pEnd, 10);
			}
			sprintf(debug, "Telnet roh> Sensor: %s Node: %s Channel: %s ", wort3, row[0], row[1]);
			logmsg(9,debug);				
			sprintf(debug, "Telnet> Sensor: %s Node: 0%o Channel: %u ", wort3, node, channel);
			logmsg(9,debug);				
			// just add the sensor to the buffer
			fill_telnet_buffer( strtoul(wort3, &pEnd, 10), node, channel, strtof(wort4, &pEnd));
			if ( strcmp(wort1,cmp1a) == 0 ) {
				telnet_buffer2db(db, node);
			}
			ordersqlrefresh = true;
			sprintf(client_message,"Command received => OK\n");
			write(new_socket , client_message , strlen(client_message));
		}
		sprintf(cmp2, "node");
		sprintf(cmp3, "init");
		if (( strcmp(wort1,cmp1) == 0 ) && (strcmp(wort2,cmp2) == 0) && (wort3 != NULL) && (strcmp(wort4,cmp3) == 0) ) {
			tn_input_ok = true;
			node_init(db, getnodeadr(wort3), 1);
			ordersqlrefresh = true;
			sprintf(client_message,"Command received => OK\n");
			write(new_socket , client_message , strlen(client_message));
		}
	}
	if ( ! tn_input_ok) {
		sprintf(client_message,"Usage:\n");
		write(new_socket , client_message , strlen(client_message));
		sprintf(client_message,"set sensor <sensornumber> <sensorvalue>\n");
		write(new_socket , client_message , strlen(client_message));
		sprintf(client_message,"   Sets the sensor <sensornumber> to the value <sensorvalue>\n");
		write(new_socket , client_message , strlen(client_message));
		sprintf(client_message,"set node <nodenumber> init\n");
		write(new_socket , client_message , strlen(client_message));
		sprintf(client_message,"   Inits the Node <nodenumber>. Use <nodenumber> like '041'\n");
		write(new_socket , client_message , strlen(client_message));
//		sprintf(client_message,"set node <nodenumber> <initvalue>\n");
//		write(new_socket , client_message , strlen(client_message));
//		sprintf(client_message,"   Sends one initialisation parameter to the node\n");
//		write(new_socket , client_message , strlen(client_message));
//		sprintf(client_message,"   valid are: sleeptime1...4, radiomode, voltagefactor\n");
//		write(new_socket , client_message , strlen(client_message));
		sprintf(client_message,"\n");
		write(new_socket , client_message , strlen(client_message));
	} 						
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
uint16_t node_init(MYSQL *db, uint16_t node, uint16_t orderno ) {
	// delete old entries for this node
	sprintf (sql_stmt, "delete from jobbuffer where node_id = '0%o'",node);
    do_sql(db,sql_stmt); 
	sprintf (sql_stmt, "select sleeptime1, sleeptime2, sleeptime3, sleeptime4, radiomode, voltagefactor from node where node_id = '0%o' LIMIT 1 ",node);
	mysql_query(db, sql_stmt);
	db_check_error(db);
	MYSQL_RES *result = mysql_store_result(db);
	db_check_error(db);
	MYSQL_ROW row;
	if ((row = mysql_fetch_row(result))) {
		sprintf(sql_stmt,"insert into jobbuffer(orderno,flags,node_id,n_type,sensor_id1,channel1,value1,sensor_id2,channel2,value2,sensor_id3,channel3,value3,sensor_id4,channel4,value4,priority,utime) values(%u,%u,'0%o',%u, %u,%u,%s,%u,%u,%s,%u,%u,%s,%u,%u,%s,1,unix_timestamp())"
			,orderno++,0x00,node,61,0,111,row[0],0,112,row[1],0,113,row[2],0,114,row[3]);
		do_sql(db, sql_stmt);
		sprintf(sql_stmt,"insert into jobbuffer(orderno,flags,node_id,n_type,sensor_id1,channel1,value1,sensor_id2,channel2,value2,sensor_id3,channel3,value3,sensor_id4,channel4,value4,priority,utime) values(%u,%u,'0%o',%u, %u,%u,%s,%u,%u,%s,%u,%u,%d,%u,%u,%d,1,unix_timestamp())"
			,orderno++,0x01,node,61,0,115,row[4],0,116,row[5],0,118,1,0,0,0);
		do_sql(db, sql_stmt);
	}
	mysql_free_result(result);
	sprintf (sql_stmt, "select sensor_id, node_id, channel, value from sensor where s_type = 'a' and node_id = '0%o' ",node);
	mysql_query(db, sql_stmt);
	db_check_error(db);
	result = mysql_store_result(db);
	db_check_error(db);
	while ((row = mysql_fetch_row(result))) {
		fill_telnet_buffer( strtoul(row[0], &pEnd,10), strtoul(row[1], &pEnd,10), strtoul(row[2], &pEnd,10), strtof(row[3], &pEnd));
//		sprintf(sql_stmt,"insert into jobbuffer(orderno,node_id,channel,value, priority, utime) values (%d,'0%o',%s,'%s',6, unix_timestamp())",orderno++, initnode, row[0], row[1]);
//		do_sql(db, sql_stmt);
	}
	mysql_free_result(result);
	telnet_buffer2db( db, node );
	return orderno;
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

void db_check_error(MYSQL *db) {
	if (mysql_errno(db) != 0) {
		sprintf(debug, "DB-Fehler: %s\n", mysql_error(db));
        logmsg(2, debug);
    }
}

void do_sql(MYSQL *db, char *sqlstmt) {
	if (mysql_query(db, sqlstmt)) {
		sprintf(debug, "%s", mysql_error(db));
		logmsg(1, debug);
	}
    sprintf(debug, "%s", sqlstmt);
	logmsg(7, debug);
}


bool is_jobbuffer_entry(MYSQL *db, uint16_t orderno) {
    MYSQL_ROW row;	
	bool retval=false;
	sprintf(sql_stmt, "select count(*) from jobbuffer where orderno = %u ", orderno );
	logmsg(8,sql_stmt);	
	mysql_query(db, sql_stmt);
	db_check_error(db);
	MYSQL_RES *result = mysql_store_result(db);
	db_check_error(db);
	row = mysql_fetch_row(result);
	retval=(row[0] > 0);
	return retval;
}

void del_jobbuffer_entry(MYSQL *db, uint16_t orderno) {
	char mysql_stmt[150];
	char mydebug[100];
	sprintf(mysql_stmt, " delete from jobbuffer where orderno = %u ", orderno  );
	do_sql(db, mysql_stmt);
	sprintf(mydebug, "Info: del_jobbuffer_entry: orderno %d deleted", orderno);
    logmsg(8, mydebug);               
	ordersqlrefresh=true;
}

void store_sensor_value(MYSQL *db, uint16_t node, uint8_t sensor, float value, bool d1, bool d2) {
	if ( tn_active ) { 
		prepare_tn_cmd(db, node, sensor, value); 
	}
	sprintf(sql_stmt,"insert into sensordata (sensor_ID, utime, value) select sensor_id, UNIX_TIMESTAMP(), %f from sensor where node_id = '0%o' and channel = %u ", value, node, sensor);
	do_sql(db, sql_stmt);
	sprintf(sql_stmt,"update sensor set value= %f, Utime = UNIX_TIMESTAMP(), signal_quality = '%d%d' where node_id = '0%o' and channel = %u ", value, d1, d2, node, sensor);
	do_sql(db, sql_stmt);
}

void process_sensor(MYSQL *db, uint16_t node, uint8_t sensor, float value, bool d1, bool d2) {
			switch (sensor) {
				case 1 ... 99: {
				// Sensor 
					store_sensor_value(db, node, sensor, value, d1, d2);
					sprintf(debug, DEBUGSTR "Value of  %u on Node: %o is %f ", sensor, node, value);
					logmsg(6, debug);       
				}
				break; 
				case 101: {
				// battery voltage
					store_sensor_value(db, node, sensor, value, d1, d2);
					sprintf(debug, DEBUGSTR "Voltage of Node: %o is %f ", node, value);
					logmsg(6, debug);        
					sprintf(sql_stmt,"update node set U_Batt = %f, signal_quality = '%d%d', last_contact = unix_timestamp() where Node_ID = '0%o'", value, d1, d2, node);
					do_sql(db, sql_stmt);
				}
				break; 
				case 111: { // Init Sleeptime 1
					sprintf(debug, DEBUGSTR "Node: %o: Sleeptime1 set to %f ", node, value);
					logmsg(6, debug);        
				}	
				break; 
				case 112: { // Init Sleeptime 2
					sprintf(debug, DEBUGSTR "Node: %o: Sleeptime2 set to %f ", node, value);
					logmsg(6, debug);        
				}
				break; 
				case 113: { // Init Sleeptime 3
					sprintf(debug, DEBUGSTR "Node: %o: Sleeptime3 set to %f ", node, value);
					logmsg(6, debug);        
				}
				break; 				
				case 114: { // Init Sleeptime 4
					sprintf(debug, DEBUGSTR "Node: %o: Sleeptime4 set to %f ", node, value);
					logmsg(6, debug);        
				}
				break; 
				case 115: { // Init Radiobuffer
                    bool radio_always_on = value > 0.5;
					if ( radio_always_on ) sprintf(debug, "Node: %o: Radio allways on", node);
					else sprintf(debug, "Node: %o: Radio allways off", node);
					logmsg(6, debug);        
				}					
				break;  
				case 116: { // Init Voltagefactor
					sprintf(debug, "Node: %o: Set Voltagefactor to: %f.", node, value);
					logmsg(6, debug);        
				}
				break;  
				case 118: {
					sprintf(debug, DEBUGSTR "Node: %o Init finished.", node);
					logmsg(6, debug);        
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
	sprintf(debug, "SIGTERM: Shutting down ... ");
	logmsg(1, debug);
    unlink(parms.pidfilename);
//	msgctl(msqid, IPC_RMID, NULL);
    exit (0);
}

long runtime(long starttime) {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (tv.tv_sec - starttime) *1000 + tv.tv_usec / 1000;
}

void logmsg(int mesgloglevel, char *mymsg){
	if ( logmode == logfile ) {
		if (mesgloglevel <= verboselevel) {
			char buf[3];
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

int main(int argc, char* argv[]) {
    pid_t pid;
	int c;
	long starttime=time(0);
	long sent_time;
	long akt_time;

    akt_time=runtime(starttime);
	sent_time=akt_time;
	orderno = 1;
	logmode = interactive;
	strcpy(config_file,"x");

	/* vars for telnet socket handling */
	int create_socket, new_socket, MsgLen;
	socklen_t addrlen;
	char *buffer =  (char*) malloc (BUF);
	struct sockaddr_in address;
	long save_fd;
	const int y = 1;
	bool wait4message = false;
	
    // processing argc and argv[]
	while (1) {
		static struct option long_options[] = {	
			{"daemon",  no_argument, 0, 'd'},
			{"verbose",  required_argument, 0, 'v'},
            {"configfile",    required_argument, 0, 'c'},
            {"help", no_argument, 0, 'h'},
            {0, 0, 0, 0} 
		};
        /* getopt_long stores the option index here. */
        int option_index = 0;
        c = getopt_long (argc, argv, "?dhv:c:",long_options, &option_index);
        /* Detect the end of the options. */
        if (c == -1) break;
        switch (c) {
            case 'd':
				start_daemon = true;
				logmode = logfile;
            break;
			case 'v':
                verboselevel = (optarg[0] - '0') * 1;
                debugmode=true;
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
                abort ();
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
           fprintf(stdout, "sensorhubd has to be startet as user root\n");
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
//    printf ("Initializing parameters to default values...\n");
//    init_parameters (&parms);
    printf ("Reading config file...\n");
    parse_config (&parms);
    printf ("Startup Parameters:\n");
    print_config (&parms);

    // starts logging
    logfile_ptr = fopen (parms.logfilename,"a");
    if ( logfile_ptr == NULL ) {
        fprintf(stdout,"Could not open %s for writing\n Printig logs to console\n", parms.logfilename );
    } else {
		log2logfile = true;
        fclose(logfile_ptr);
		sprintf(debug, "Start logging to %s", parms.logfilename);
        logmsg(2, debug);
    }
    // open database
    sprintf(debug,"Maria-DB:");
    logmsg(2, debug);
    sprintf(debug,"MySQL client version: %s", mysql_get_client_info());
    logmsg(2, debug);
    MYSQL *db = mysql_init(NULL);
    int mysql_wait_count = 0;
    while (db == NULL) {
		sprintf(debug,"Waiting for Database: %d Sec.", 20-mysql_wait_count);
		logmsg(2, debug);		
		if ( mysql_wait_count < 20 ) {
			mysql_wait_count++;
			delay(1000);
			db = mysql_init(NULL);
		} else {
			fprintf(stderr, "%s\n", mysql_error(db));
            mysql_close(db);
            exit(1);
		}
    }
    mysql_wait_count = 0;
    while (mysql_real_connect(db, parms.db_hostname, parms.db_username, parms.db_password, parms.db_schema, parms.db_port, NULL, 0) == NULL) {
		sprintf(debug,"Waiting for Database: %d Sec.", 20-mysql_wait_count);
		logmsg(2, debug);		
		if ( mysql_wait_count < 20 ) {
			mysql_wait_count++;
			delay(1000);
		} else {
			fprintf(stderr, "%s\n", mysql_error(db));
			mysql_close(db);
			exit(1);
		}
    }
    sprintf(debug, "Connected to host %s with DB %s on port %d", parms.db_hostname, mysql_get_server_info(db), parms.db_port);
    logmsg(2, debug);

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
            // starts sensorhub as a deamon
            // no messages to console!
            debugmode=false;
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
    // check for PID file, set if not exists terminate else create it
    if( access( parms.pidfilename, F_OK ) != -1 ) {
        fprintf(stdout, "PIDFILE: %s exists, terminating\n\n", parms.pidfilename);
        exit(1);
    }
    pid=getpid();
    pidfile_ptr = fopen (parms.pidfilename,"w");
    if (pidfile_ptr==NULL) {
        sprintf(debug,"Can't write PIDFILE: %s! Exit programm ....\n", parms.pidfilename);
        fprintf(stdout, debug);
        exit (1);
    }
    fprintf (pidfile_ptr, "%d", pid );
    fclose(pidfile_ptr);
    sprintf(debug, "sensorhub running with PID: %d", pid);
    logmsg(2, debug);
    if ( tn_port_set && tn_host_set ) {
        tn_active = true;
        sprintf(debug, "telnet session started: Host: %s Port: %d ", parms.telnet_hostname, parms.telnet_port);
        logmsg(2, debug);
    }
	create_socket=0;
	if ( in_port_set ) {
    /* open incoming port for messages */
		if ((create_socket=socket( AF_INET, SOCK_STREAM, 0)) > 0) {
			sprintf (debug,"Socket für eingehende Messages auf Port %i angelegt", parms.incoming_port);
			logmsg(2, debug);
		}
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
		address.sin_port = htons (parms.incoming_port);
		setsockopt( create_socket, SOL_SOCKET, SO_REUSEADDR, &y, sizeof(int) );
		if (bind( create_socket, (struct sockaddr *) &address, sizeof (address)) == 0 ) {
			sprintf (debug,"Binding Socket OK");
			logmsg(2, debug);
		}
		listen (create_socket, 5);
		addrlen = sizeof (struct sockaddr_in);
		save_fd = fcntl( create_socket, F_GETFL );
		save_fd |= O_NONBLOCK;
		fcntl( create_socket, F_SETFL, save_fd );
	}
    sleep(2);
    sprintf(debug, "starting radio on channel ... %d ", parms.rf24network_channel);
    logmsg(2, debug);
    radio.begin();
    delay(5);
    sprintf(debug, "starting network ... ");
    logmsg(2, debug);
    network.begin( parms.rf24network_channel, 0);
    radio.setDataRate(parms.rf24network_speed);
    if (verboselevel > 5) { radio.printDetails(); }
    sprintf(debug,"\%s up and running .... ",PRGNAME);
    logmsg(2, debug);
	
	// Cleanup inside Database
	sprintf(sql_stmt,"delete from jobbuffer");
	do_sql(db, sql_stmt);
    sprintf(debug,"Database cleanup compleate ");
    logmsg(2, debug);
	
	// Init Arrays
	for (int i=0; i < MAXTELNETBUFFER; i++) telnet_buffer[i].sensor=0;
	for (int i=0; i < MAXNODES; i++) order[i].orderno=0;
    
	// Main Loop
    while(1) {
		if (orderno > 50000) orderno = 1;
        /* Handling of incoming messages */
		if ( in_port_set ) {
			char client_message[30];
			if ( ! wait4message ) {  
				new_socket = accept ( create_socket, (struct sockaddr *) &address, &addrlen );
				if (new_socket > 0) {
					wait4message = true;
					// send something like a prompt. perl telnet is waiting for it otherwise we get error
					// use this in perl: my $t = new Net::Telnet (Timeout => 2, Port => 7001, Prompt => '/rf24hub>/');
					sprintf(client_message,"rf24hub> ");
					write(new_socket , client_message , strlen(client_message));
					sprintf (debug,"Client %s ist connected ...", inet_ntoa (address.sin_addr));
					logmsg(6, debug);
				}
			} else {
				save_fd = fcntl( new_socket, F_GETFL );
				save_fd |= O_NONBLOCK;
				fcntl( new_socket, F_SETFL, save_fd );
				/* Process data  */
				sprintf(buffer,"                                                                               ");
				MsgLen = recv(new_socket, buffer, BUF, 0);
				if (MsgLen>0) {
					process_tn_in(db, new_socket, buffer, client_message);
					close (new_socket);
					wait4message = false;
				}
			}	 
		}
		network.update();
		if ( network.available() ) {
//
// Receive loop: react on the message from the nodes
//
            rf24_carrier = radio.testCarrier();
			rf24_rpd = radio.testRPD();
			network.read(rxheader,&payload,sizeof(payload));
			sprintf(debug, DEBUGSTR "Received: Channel: %u from Node: %o to Node: %o Orderno %d (Sensor/Value): (%u/%f) (%u/%f) (%u/%f) (%u/%f) "
						, rxheader.type, rxheader.from_node, rxheader.to_node, payload.orderno
						, payload.channel1, payload.value1, payload.channel2, payload.value2, payload.channel3, payload.value3, payload.channel4, payload.value4);
			logmsg(6, debug);
//			uint16_t sendernode=rxheader.from_node;
			if (is_jobbuffer_entry(db, payload.orderno)) {
				if ( payload.channel1 > 0 ) process_sensor(db, rxheader.from_node, payload.channel1, payload.value1, rf24_carrier, rf24_rpd);
				if ( payload.channel2 > 0 ) process_sensor(db, rxheader.from_node, payload.channel2, payload.value2, rf24_carrier, rf24_rpd);
				if ( payload.channel3 > 0 ) process_sensor(db, rxheader.from_node, payload.channel3, payload.value3, rf24_carrier, rf24_rpd);
				if ( payload.channel4 > 0 ) process_sensor(db, rxheader.from_node, payload.channel4, payload.value4, rf24_carrier, rf24_rpd);
				del_jobbuffer_entry(db, payload.orderno);
			}
			if ( rxheader.type == 119 ) {
					orderno = node_init(db, rxheader.from_node, orderno);
					ordersqlrefresh=true;
			}	
			
		} // network.available
//
// Orderloop: Tell the nodes what they have to do
//
                akt_time=runtime(starttime);
                if ( akt_time > sent_time + 199 ) {  // send every 200 milliseconds
                        sent_time=akt_time;
                        if ( ordersqlrefresh ) {
                                int init_order_ptr = 0;
                        // Do we have old jobs - delete them
                                sprintf (sql_stmt, "delete from jobbuffer where utime < (unix_timestamp() - 1000) ");
                                do_sql(db, sql_stmt);
                        // if we got new jobs refresh the order array first
                                sprintf (sql_stmt, "select distinct node_id from jobbuffer");
                                logmsg(8,sql_stmt);
                                mysql_query(db, sql_stmt);
                                db_check_error(db);
                                MYSQL_RES *result = mysql_store_result(db);
                                db_check_error(db);
                                MYSQL_ROW row;
                                while ( (row = mysql_fetch_row(result)) ) {
									printf("Node_ID : %s\n",row[0]);
                                    sprintf (sql_stmt, "select orderno, node_id, n_type, flags, channel1, value1, channel2, value2, channel3, value3, channel4, value4 from jobbuffer where node_id = '%s' order by priority asc, n_type asc limit 1", row[0]);
                                    logmsg(8,sql_stmt);
                                    mysql_query(db, sql_stmt);
                                    db_check_error(db);
                                    MYSQL_RES *result1 = mysql_store_result(db);
                                    db_check_error(db);
                                    MYSQL_ROW row1;
                                    if ((row1 = mysql_fetch_row(result1))) {
									printf("Node_ID1 : %s\n",row1[1]);
                                        order[init_order_ptr].orderno  = strtoul(row1[0], &pEnd,10);
                                        order[init_order_ptr].to_node  = getnodeadr(row1[1]);
                                        order[init_order_ptr].type  = strtoul(row1[2], &pEnd,10);
										order[init_order_ptr].flags    = strtoul(row1[3], &pEnd,10);
										order[init_order_ptr].channel1  = strtoul(row1[4], &pEnd,10);
										order[init_order_ptr].value1   = strtof(row1[5], &pEnd);
										order[init_order_ptr].channel2  = strtoul(row1[6], &pEnd,10);
										order[init_order_ptr].value2   = strtof(row1[7], &pEnd);
										order[init_order_ptr].channel3  = strtoul(row1[8], &pEnd,10);
										order[init_order_ptr].value3   = strtof(row1[9], &pEnd);
										order[init_order_ptr].channel4  = strtoul(row1[10], &pEnd,10);
										order[init_order_ptr].value4   = strtof(row1[11], &pEnd);
                                        init_order_ptr++;
                                    }
                                }
                                ordersqlrefresh=false;
                                mysql_free_result(result);
                                while ( init_order_ptr < MAXNODES) {
                                        order[init_order_ptr].orderno = 0;
                                        init_order_ptr++;
                                }
                        }
// Look if we have something to send
                        bool do_loop = true;
                        while ( order[order_ptr].orderno == 0 && do_loop ) {
                                if ( order_ptr < MAXNODES - 1 ) {
                                        order_ptr++;
                                } else {
                                        order_ptr=0;
                                        do_loop = false;
                                }
                        }
//						printf(" %u\n", order_ptr);
                        if      ( order[order_ptr].orderno != 0 ) {
                                txheader.from_node = 0;
                                payload.orderno = order[order_ptr].orderno;
                                txheader.to_node  = order[order_ptr].to_node;
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
                                        sprintf(debug, DEBUGSTR "Send: Type: %u from Node: 0%o to Node: 0%o orderno %d (Sensor1/Value) (%u/%f) (%u/%f) (%u/%f) (%u/%f)"
                                                        , txheader.type, txheader.from_node, txheader.to_node, payload.orderno
														, payload.channel1, payload.value1, payload.channel2, payload.value2, payload.channel3, payload.value3, payload.channel4, payload.value4);
                                        logmsg(6, debug);
                                } else {
                                        sprintf(debug, DEBUGSTR "Failed: Type: %u from Node: 0%o to Node: 0%o orderno %d (Sensor1/Value) (%u/%f) (%u/%f) (%u/%f) (%u/%f)"
                                                        , txheader.type, txheader.from_node, txheader.to_node, payload.orderno
														, payload.channel1, payload.value1, payload.channel2, payload.value2, payload.channel3, payload.value3, payload.channel4, payload.value4);
                                        logmsg(6, debug);
                                }
                                order_ptr++;
                        }
                }
                usleep(2000);
//
//  end orderloop
//
        } // while(1)
        return 0;
}

