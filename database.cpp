#include "database.h"

Database::Database(void) {
    tsbuf = (char*) malloc (TSBUFFERSIZE);
    sql_stmt = (char*) malloc(SQLSTRINGSIZE);
    verboselevel = 0;
}

void Database::db_check_error(void) {
	if (mysql_errno(db) != 0) {
		printf("DB-Fehler: %s\n", mysql_error(db));
    }
}

void Database::sync_config(void) {
/*    sprintf (sql_stmt, "insert into node_configdata_history(node_id, channel, value, utime) select node_id, channel, value, utime from node_configdata_history_im where (node_id, channel, value, utime) not in ( select node_id, channel, value, utime from  node_configdata_history )");
    debugPrintSQL(sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error(); */
    sprintf (sql_stmt, "delete from node_configdata where (node_id, channel) in ( select node_id, channel from  node_configdata_im)");
    debugPrintSQL(sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
    mysql_commit(db);
    sprintf (sql_stmt, "insert into node_configdata(node_id, channel, value, utime) select node_id, channel, value, utime from node_configdata_im");
    debugPrintSQL(sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
    mysql_commit(db);
}

void Database::sync_sensor(void) {
	sprintf (sql_stmt, "insert into sensor(sensor_id, sensor_name, add_info, node_id, channel, store_days, fhem_dev, html_show, html_order, value, utime) select sensor_id, sensor_name, add_info, node_id, channel, store_days, fhem_dev, html_show, html_order, value, utime from sensor_im where sensor_id not in ( select sensor_id from sensor ) ");
    debugPrintSQL(sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
	sprintf (sql_stmt, "update sensor a set value = ( select value from sensor_im where sensor_id = a.sensor_id ), utime = ( select utime from sensor_im where sensor_id = a.sensor_id )");
    debugPrintSQL(sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
}

void Database::sync_sensordata(void) {
	sprintf (sql_stmt, "insert into sensordata(sensor_id, utime, value) select sensor_id, utime, value from sensordata_im where (sensor_id,utime) not in (select sensor_id, utime from sensordata)");
    debugPrintSQL(sql_stmt);
    mysql_query(db, sql_stmt);
	db_check_error();
}

void Database::sync_sensordata_d(void) {
    sprintf (sql_stmt, "truncate table sensordata_d");
    debugPrintSQL(sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
	sprintf (sql_stmt, "%s", "insert into sensordata_d(sensor_id, value, utime) select sensor_id, min(value) as min_val, UNIX_TIMESTAMP(FROM_UNIXTIME(utime,'%Y%m%d'))+21600 from sensordata group by sensor_id, UNIX_TIMESTAMP(FROM_UNIXTIME(utime,'%Y%m%d'))" );
    debugPrintSQL(sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
    sprintf (sql_stmt, "%s", "insert into sensordata_d(sensor_id, value, utime) select sensor_id, max(value) as min_val, UNIX_TIMESTAMP(FROM_UNIXTIME(utime,'%Y%m%d'))+64800 from sensordata group by sensor_id, UNIX_TIMESTAMP(FROM_UNIXTIME(utime,'%Y%m%d'))" );
    debugPrintSQL(sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
}

void Database::initSystem(void) {
    // Falls das letzte Programmende ein Chrash war sollen einige "*_im" Tabellen gesichert werden!
    sync_config();
    sync_sensordata();
    sync_sensor();
	sprintf (sql_stmt, "truncate table sensor_im");
    debugPrintSQL(sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
	sprintf (sql_stmt, "insert into sensor_im(sensor_id, sensor_name, add_info, node_id, channel, store_days, fhem_dev, html_show, html_order, value, utime) select sensor_id, sensor_name, add_info, node_id, channel, store_days, fhem_dev, html_show, html_order, value, utime from sensor");
    debugPrintSQL(sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
	sprintf (sql_stmt, "truncate table sensordata_im");
    debugPrintSQL(sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
	sprintf (sql_stmt, "insert into sensordata_im(sensor_id, utime, value) select sensor_id, utime, value from sensordata");
    debugPrintSQL(sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
    sync_config();
	sprintf (sql_stmt, "truncate table node_configdata_im");
    debugPrintSQL(sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
    sprintf (sql_stmt, "insert into node_configdata_im(node_id, channel, value, utime) select node_id, channel, value, utime from node_configdata");
    debugPrintSQL(sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
	sprintf (sql_stmt, "truncate table gateway_im");
    debugPrintSQL(sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
    sprintf (sql_stmt, "insert into gateway_im(gw_name, gw_ip, isactive) select gw_name, gw_ip, isactive from gateway");
    debugPrintSQL(sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
    sync_sensordata_d();
    mysql_commit(db);
}

void Database::initGateway(Gateway* gateway) {
	MYSQL_ROW row;
    char gw_name[40];
    char gw_ip[40];
    uint16_t gw_no;
    bool isactive;
	sprintf (sql_stmt, "select gw_name, gw_ip, gw_no, isactive from gateway");
    debugPrintSQL(sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
	MYSQL_RES *result = mysql_store_result(db);
	db_check_error();
	while ((row = mysql_fetch_row(result))) {
		if ( row[0] != NULL ) sprintf(gw_name,"%s",trim(row[0])); else sprintf(gw_name," ");
		if ( row[1] != NULL ) sprintf(gw_ip,"%s",trim(row[1])); else sprintf(gw_ip," ");
        if ( row[2] != NULL ) gw_no = strtoul(row[2], &pEnd, 10); else gw_no = 0;
		if ( row[3] != NULL ) isactive = strtoul(row[3], &pEnd, 10); else isactive = 0;
        gateway->addGateway(gw_name, gw_ip, gw_no, isactive); 
	}
	mysql_free_result(result);    
}    
    
void Database::initNode(Node* node) {
    NODE_DATTYPE node_id = 0;
    bool myHBnode = false;
    uint32_t pa_utime;
    uint32_t pa_level;
    float u_batt;
    char cmp_y[]="y",cmp_j[]="j"; 
	MYSQL_ROW row;
	sprintf (sql_stmt, "select node_id, heartbeat, pa_utime, pa_level from node");
    debugPrintSQL(sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
	MYSQL_RES *result = mysql_store_result(db);
	db_check_error();
	while ((row = mysql_fetch_row(result))) {
		if ( row[0] != NULL ) node_id = strtoul(row[0], &pEnd,10);
        if ((strcmp(row[1],cmp_y) == 0) || (strcmp(row[1],cmp_j) == 0)) { myHBnode = true; } else { myHBnode = false; }
		if ( row[2] != NULL ) pa_utime = strtoul(row[2], &pEnd, 10); else pa_utime = 1;
		if ( row[3] != NULL ) pa_level = strtoul(row[3], &pEnd, 10); else pa_level = 9;
        node->addNode(node_id, 0, myHBnode, pa_level, pa_utime); 
	}
	mysql_free_result(result);    
	sprintf (sql_stmt, "select node_id, value from sensor where channel = 101");
    debugPrintSQL(sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
	result = mysql_store_result(db);
	db_check_error();
	while ((row = mysql_fetch_row(result))) {
		if ( row[0] != NULL ) node_id = strtoul(row[0], &pEnd,10);
		if ( row[1] != NULL ) u_batt = strtof(row[1], &pEnd); else u_batt = 0;
        node->setVoltage(node_id, u_batt); 
	}
	mysql_free_result(result);    
}

void Database::initSensor(Sensor* sensor) {
    char* fhem_dev = alloc_str(verboselevel,"Database::initSensor fhem_dev",FHEMDEVLENGTH, ts(tsbuf));
    uint32_t     	mysensor;
    NODE_DATTYPE   	node_id;
    uint8_t     	mychannel;
    float           myvalue;
    uint32_t        utime;
	MYSQL_ROW row;
	sprintf (sql_stmt, "select sensor_id, node_id, channel, fhem_dev, value, utime from sensor");
    debugPrintSQL(sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
	MYSQL_RES *result = mysql_store_result(db);
	db_check_error();
	while ((row = mysql_fetch_row(result))) {
		if ( row[0] != NULL ) mysensor = strtoul(row[0], &pEnd,10); else mysensor = 0;
		if ( row[1] != NULL ) node_id = strtoul(row[1], &pEnd,10); else node_id = 0; 
		if ( row[2] != NULL ) mychannel = strtoul(row[2], &pEnd,10); else mychannel = 0;
		if ( row[3] != NULL ) sprintf(fhem_dev,"%s",trim(row[3])); else sprintf(fhem_dev,"not_set");
		if ( row[4] != NULL ) myvalue = strtof(row[4], &pEnd); else myvalue = 0;
		if ( row[5] != NULL ) utime = strtoul(row[5], &pEnd, 10); else utime = 1;
        // ToDo
        sensor->addSensor(mysensor, node_id, mychannel, fhem_dev, utime, myvalue);
	}
	mysql_free_result(result);
    free_str(verboselevel,"Database::initSensor fhem_dev",fhem_dev, ts(tsbuf)); 
}

void Database::do_sql(char *sqlstmt) {
    debugPrintSQL(sqlstmt);
	if (mysql_query(db, sqlstmt) != 0) {
        printf("%s\n", sqlstmt);
    }
    db_check_error();
}

void Database::storeSensorValue(uint32_t mysensor, char* value) {
    sprintf(sql_stmt,"insert into sensordata_im (sensor_ID, utime, value) values (%u, UNIX_TIMESTAMP(), %s)", mysensor, value);
    do_sql(sql_stmt);
    sprintf(sql_stmt,"update sensor_im set value = %s, utime = UNIX_TIMESTAMP() where sensor_id = %u",value ,mysensor);
    do_sql(sql_stmt);
}

void Database::storeNodeConfig(NODE_DATTYPE node_id, uint8_t channel, char* value) {
    if ( channel == REG_PALEVEL ) {
        sprintf(sql_stmt,"update node set pa_level = %s, pa_utime = UNIX_TIMESTAMP() where node_id = %u ", value, node_id);
        do_sql(sql_stmt);
    }
    //sprintf(sql_stmt,"delete from node_configdata_history where node_id = %u and channel = %u and utime > UNIX_TIMESTAMP() - 100 ", node_id, channel);
    //do_sql(sql_stmt);
    //sprintf(sql_stmt,"insert into node_configdata_history_im (node_id, channel, utime, value) values (%u, %u, UNIX_TIMESTAMP(), %s ) ", node_id, channel, value);
    //do_sql(sql_stmt);
    sprintf(sql_stmt,"delete from node_configdata_im where node_id = %u and channel = %u ", node_id, channel, value);
    do_sql(sql_stmt);
    sprintf(sql_stmt,"insert into node_configdata_im (node_id, channel, utime, value) values (%u, %u, UNIX_TIMESTAMP(), %s ) ", node_id, channel, value);
    do_sql(sql_stmt);
}

bool Database::connect(string db_hostname, string db_username, string db_password, string db_schema, int db_port) {
    int mysql_wait_count = 0;
    bool retval = true;
    if ( verboselevel & VERBOSESTARTUP) {
        printf("%sMaria-DB client version: %s\n", ts(tsbuf), mysql_get_client_info());
    }
    db = mysql_init(NULL);
    while (db == NULL) {
        if ( verboselevel & VERBOSESTARTUP) {
            printf("%sWaiting for Database: %d Sek.\n", ts(tsbuf), 20-mysql_wait_count); 
        }
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
            if ( verboselevel & VERBOSESTARTUP) {    
                printf("%sWaiting for Database: %d Sek.\n", ts(tsbuf), 20-mysql_wait_count); 
            }
            if ( mysql_wait_count < 20 ) {
                mysql_wait_count++;
                sleep(1);
            } else {
                fprintf(stderr, "%s\n", mysql_error(db));
                mysql_close(db);
                retval = false;
            }
        }
        if ( verboselevel & VERBOSESTARTUP) {
            printf("%sConnected to host %s with DB %s on port %s\n", ts(tsbuf), db_hostname, mysql_get_server_info(db), db_port);
        }
    }
    return retval;
}

void Database::debugPrintSQL(char* sqlstmt) {
    if ( verboselevel & VERBOSESQL) {    
        printf("%sSQL: %s\n", ts(tsbuf), sqlstmt);
    }
}

void Database::setVerbose(uint16_t _verboselevel) {
    verboselevel = _verboselevel;
}