#include "database.h"

/*

  char db_hostname[PARAM_MAXLEN_HOSTNAME];
  int db_port;
  char db_schema[PARAM_MAXLEN_DB_SCHEMA];
  char db_username[PARAM_MAXLEN_DB_USERNAME];
  char db_password[PARAM_MAXLEN_DB_PASSWORD];
  char telnet_hostname[PARAM_MAXLEN_HOSTNAME];
  int telnet_port;
  int incoming_port;
  rf24_datarate_e rf24network_speed;
  uint8_t rf24network_channel;

*/  
  
void Database::begin(Logger* _logger) {
    logger = _logger;
}

void Database::db_check_error(void) {
	if (mysql_errno(db) != 0) {
        char *debug =  (char*) malloc (DEBUGSTRINGSIZE);
		sprintf(debug, "DB-Fehler: %s\n", mysql_error(db));
        logger->logmsg(VERBOSECRITICAL, debug);
        free(debug);
    }
}

void Database::fillNode(Node* node) {
    char *sql_stmt =  (char*) malloc (SQLSTRINGSIZE);
    uint16_t mynode = 0;
    bool myHBnode = false;
    char cmp_y[]="y",cmp_j[]="j"; 
	MYSQL_ROW row;
	sprintf (sql_stmt, "select node_id, heartbeat from node");
	logger->logmsg(VERBOSESQL, sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
	MYSQL_RES *result = mysql_store_result(db);
	db_check_error();
	while ((row = mysql_fetch_row(result))) {
		if ( row[0] != NULL ) mynode = strtoul(row[0], &pEnd,10);
        if ((strcmp(row[1],cmp_y) == 0) || (strcmp(row[1],cmp_j) == 0)) { myHBnode = true; } else { myHBnode = false; }
        node->add_node(mynode, 0, myHBnode); 
	}
	mysql_free_result(result);    
    free(sql_stmt);   
}

void Database::fillSensor(Sensor* sensor) {
    char *sql_stmt =  (char*) malloc (SQLSTRINGSIZE);
    char *fhem_dev =  (char*) malloc (FHEMDEVLENGTH);
    uint32_t     	mysensor;
    uint16_t       	mynode;
    uint8_t     	mychannel;
	MYSQL_ROW row;
	sprintf (sql_stmt, "select sensor_id, node_id, channel, fhem_dev from sensor");
	logger->logmsg(VERBOSESQL, sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
	MYSQL_RES *result = mysql_store_result(db);
	db_check_error();
	while ((row = mysql_fetch_row(result))) {
		if ( row[0] != NULL ) mysensor = strtoul(row[0], &pEnd,10); else mysensor = 0;
		if ( row[1] != NULL ) mynode = strtoul(row[1], &pEnd,10); else mynode = 0; 
		if ( row[2] != NULL ) mychannel = strtoul(row[2], &pEnd,10); else mychannel = 0;
		if ( row[3] != NULL ) sprintf(fhem_dev,"%s",row[3]); else sprintf(fhem_dev,"not_set");
        sensor->add_sensor(mysensor, mynode, mychannel, fhem_dev);
	}
	mysql_free_result(result);
    free(sql_stmt);
    free(fhem_dev); 
}

void Database::do_sql(char *sqlstmt) {
    char *debug =  (char*) malloc (DEBUGSTRINGSIZE);
    sprintf(debug, "%s", sqlstmt);
	logger->logmsg(VERBOSESQL, debug);
	if (mysql_query(db, sqlstmt)) {
		sprintf(debug, "%s", mysql_error(db));
		logger->logmsg(VERBOSECRITICAL, debug);
	}
	free(debug);
}

void Database::storeSensorValue(uint16_t node, uint8_t channel, float value) {
    char *debug =  (char*) malloc (DEBUGSTRINGSIZE);
    char *sql_stmt =  (char*) malloc (SQLSTRINGSIZE);
	sprintf(debug, "database.storeSensorValue: Node: %u Channel: %u Value: %f ", node, channel, value);
	logger->logmsg(VERBOSEORDER, debug);        
//    if ( sensor.update_last_val(node, channel, value, mymillis() )) {    
//        if ( tn_active ) { 
//            do_tn_cmd(node, channel, value); 
//        }
    sprintf(sql_stmt,"insert into sensordata_im (sensor_ID, utime, value) select sensor_id, UNIX_TIMESTAMP(), %f from sensor where node_id = %u and channel = %u ", value, node, channel);
    do_sql(sql_stmt);
//    sprintf(sql_stmt,"update sensor_im set value= %f, utime = UNIX_TIMESTAMP(), signal_quality = '%d%d' where node_id = '0%o' and channel = %u ", value, d1, d2, node, channel);
//    do_sql(sql_stmt);
    free(debug);  
    free(sql_stmt);
}

void Database::storeNodeConfig(uint16_t node, uint8_t channel, float value) {
    char *debug =  (char*) malloc (DEBUGSTRINGSIZE);
    char *sql_stmt =  (char*) malloc (SQLSTRINGSIZE);
	sprintf(debug, "database.storeNodeConfig: Node: %u Channel: %u Value: %f ", node, channel, value);
	logger->logmsg(VERBOSEORDER, debug);        
    sprintf(sql_stmt,"insert into node.confidata (node_id, channel, utime, value) values (%u, %u, UNIX_TIMESTAMP(), %f ) ", node, channel, value);
    do_sql(sql_stmt);
//    sprintf(sql_stmt,"update sensor_im set value= %f, utime = UNIX_TIMESTAMP(), signal_quality = '%d%d' where node_id = '0%o' and channel = %u ", value, d1, d2, node, channel);
//    do_sql(sql_stmt);
    free(debug);    
    free(sql_stmt);
}

bool Database::connect(string db_hostname, string db_username, string db_password, string db_schema, int db_port) {
    int mysql_wait_count = 0;
    bool retval = true;
    char *debug =  (char*) malloc (DEBUGSTRINGSIZE);
    sprintf(debug,"Maria-DB:");
    logger->logmsg(VERBOSESTARTUP, debug);
    sprintf(debug,"MySQL client version: %s", mysql_get_client_info());
    logger->logmsg(VERBOSESTARTUP, debug);
    db = mysql_init(NULL);
    while (db == NULL) {
		sprintf(debug,"Waiting for Database: %d Sec.", 20-mysql_wait_count);
		logger->logmsg(VERBOSESTARTUP, debug);		
		if ( mysql_wait_count < 20 ) {
			mysql_wait_count++;
			sleep(1);
			db = mysql_init(NULL);
		} else {
			fprintf(stderr, "%s\n", mysql_error(db));
            mysql_close(db);
			retval = false;
		}
    }
    if ( retval ) {
        mysql_wait_count = 0;
        while (mysql_real_connect(db, db_hostname.c_str(), db_username.c_str(), db_password.c_str(), db_schema.c_str(), db_port, NULL, 0) == NULL) {
            sprintf(debug,"Waiting for Database: %d Sec.", 20-mysql_wait_count);
            logger->logmsg(VERBOSESTARTUP, debug);		
            if ( mysql_wait_count < 20 ) {
                mysql_wait_count++;
                sleep(1);
            } else {
                fprintf(stderr, "%s\n", mysql_error(db));
                mysql_close(db);
                retval = false;
            }
        }
        sprintf(debug, "Connected to host %s with DB %s on port %d", db_hostname.c_str(), mysql_get_server_info(db), db_port);
        logger->logmsg(VERBOSESTARTUP, debug);
    }
printf("####db: %p\n",db);   
    sprintf(debug,"insert into sensordata(sensor_id, utime, value) values (1, UNIX_TIMESTAMP(), 22)");
    mysql_query(db, debug);
    free(debug);
    return retval;
}
