#include "database.h"

Database::Database(void) {
    buf = alloc_str(verboselevel,"Database buf",TSBUFFERSIZE);
    verboselevel = 0;
}

void Database::db_check_error(void) {
	if (mysql_errno(db) != 0) {
        char* buf = alloc_str(verboselevel,"Database::db_check_error buf",TSBUFFERSIZE);
		cout << ts(buf) << "DB-Fehler: " << mysql_error(db) << endl;
        free_str(verboselevel,"Database::db_check_error buf",buf);
    }
}

void Database::sync_sensordata(void) {
    char* sql_stmt = alloc_str(verboselevel,"Database::sync_sensordata sql_stmt",SQLSTRINGSIZE);
	sprintf (sql_stmt, "insert into sensordata(sensor_id, utime, value) select sensor_id, utime, value from sensordata_im where (sensor_id,utime) not in (select sensor_id, utime from sensordata)");
    debugPrintSQL(sql_stmt);
    mysql_query(db, sql_stmt);
	db_check_error();
    free_str(verboselevel,"Database::sync_sensordata sql_stmt",sql_stmt);
}

void Database::sync_sensor(void) {
    char* sql_stmt = alloc_str(verboselevel,"Database::sync_sensor sql_stmt",SQLSTRINGSIZE);
	sprintf (sql_stmt, "update sensor a set value = ( select value from sensor_im where sensor_id = a.sensor_id ), utime = ( select utime from sensor_im where sensor_id = a.sensor_id )");
    debugPrintSQL(sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
    free_str(verboselevel,"Database::sync_sensor sql_stmt",sql_stmt);
}

void Database::initSystem(void) {
    char* sql_stmt = alloc_str(verboselevel,"Database::initSystem sql_stmt",SQLSTRINGSIZE);
    // Falls das letzte Programmende ein Chrash war soll die Tabelle "sensordata_im" gesichert werden!
	sprintf (sql_stmt, "insert into sensordata(sensor_id, utime, value) select sensor_id, utime, value from sensordata_im where (sensor_id,utime) not in (select sensor_id, utime from sensordata)");
    debugPrintSQL(sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
	sprintf (sql_stmt, "truncate table sensor_im");
    debugPrintSQL(sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
	sprintf (sql_stmt, "insert into sensor_im(sensor_id, sensor_name, add_info, node_id, channel, store_days, fhem_dev, html_show, value, utime) select sensor_id, sensor_name, add_info, node_id, channel, store_days, fhem_dev, html_show, value, utime from sensor");
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
    free_str(verboselevel,"Database::initSystem sql_stmt",sql_stmt);
}

void Database::initNode(Node* node) {
    char* sql_stmt = alloc_str(verboselevel,"Database::initNode sql_stmt",SQLSTRINGSIZE);
    NODE_DATTYPE node_id = 0;
    bool myHBnode = false;
    uint32_t pa_utime;
    uint32_t pa_level;
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
    free_str(verboselevel,"Database::initNode sql_stmt",sql_stmt);   
}

void Database::initSensor(Sensor* sensor) {
    char* sql_stmt = alloc_str(verboselevel,"Database::initSensor sql_stmt",SQLSTRINGSIZE);
    char* fhem_dev = alloc_str(verboselevel,"Database::initSensor fhem_dev",FHEMDEVLENGTH);
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
        sensor->addSensor(mysensor, node_id, mychannel, fhem_dev, utime, myvalue, 0 , 0);
	}
	mysql_free_result(result);
    free_str(verboselevel,"Database::initSensor sql_stmt",sql_stmt);
    free_str(verboselevel,"Database::initSensor fhem_dev",fhem_dev); 
}

void Database::do_sql(char *sqlstmt) {
    debugPrintSQL(sqlstmt);
	mysql_query(db, sqlstmt);
    db_check_error();
}

void Database::storeSensorValue(uint32_t mysensor, char* value) {
    char* sql_stmt = alloc_str(verboselevel,"Database::storeSensorValue sql_stmt",SQLSTRINGSIZE);
    sprintf(sql_stmt,"insert into sensordata_im (sensor_ID, utime, value) values (%u, UNIX_TIMESTAMP(), %s)", mysensor, value);
    do_sql(sql_stmt);
    sprintf(sql_stmt,"update sensor_im set value = %s, utime = UNIX_TIMESTAMP() where sensor_id = %u",value ,mysensor);
    do_sql(sql_stmt);
    free_str(verboselevel,"Database::storeSensorValue sql_stmt",sql_stmt);
}

void Database::storeNodeConfig(NODE_DATTYPE node_id, uint8_t channel, char* value) {
    char* sql_stmt = alloc_str(verboselevel,"Database::storeNodeConfig sql_stmt",SQLSTRINGSIZE);
    sprintf(sql_stmt,"update node set pa_level = %s, pa_utime = UNIX_TIMESTAMP() where node_id = %u and 124 = %u",value,node_id,channel);
    do_sql(sql_stmt);
    sprintf(sql_stmt,"delete from node_configdata_history where node_id = %u and channel = %u and utime > UNIX_TIMESTAMP() - 100 ", node_id, channel);
    do_sql(sql_stmt);
    sprintf(sql_stmt,"insert into node_configdata_history (node_id, channel, utime, value) values (%u, %u, UNIX_TIMESTAMP(), %s ) ", node_id, channel, value);
    do_sql(sql_stmt);
//    sprintf(sql_stmt,"delete from node_configdata where node_id = %u and channel = %u ", node_id, channel);
//    do_sql(sql_stmt);
    sprintf(sql_stmt,"insert into node_configdata (node_id, channel, utime, value) values (%u, %u, UNIX_TIMESTAMP(), %f ) ON DUPLICATE KEY UPDATE value = %s, utime = UNIX_TIMESTAMP()", node_id, channel, value, value);
    do_sql(sql_stmt);
    free_str(verboselevel,"Database::storeNodeConfig sql_stmt",sql_stmt);
}

bool Database::connect(string db_hostname, string db_username, string db_password, string db_schema, int db_port) {
    int mysql_wait_count = 0;
    bool retval = true;
    char buf[] = TSBUFFERSTRING;
    if ( verboselevel & VERBOSESTARTUP) {    
        cout << ts(buf) << "Maria-DB client version: " << mysql_get_client_info() << endl;
    }
    db = mysql_init(NULL);
    while (db == NULL) {
        if ( verboselevel & VERBOSESTARTUP) {    
            cout << ts(buf) << "Waiting for Database: " << 20-mysql_wait_count << "Sek," << endl;
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
                cout << ts(buf) << "Waiting for Database: " << 20-mysql_wait_count << "Sek," << endl;
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
            cout << ts(buf) << "Connected to host " << db_hostname << " with DB " << mysql_get_server_info(db) << " on port " << db_port << endl;
        }
    }
    return retval;
}

void Database::debugPrintSQL(char* sqlstmt) {
    if ( verboselevel & VERBOSESQL) {    
        char buf[] = TSBUFFERSTRING;
        cout << ts(buf) << "SQL: " << sqlstmt << endl;
    }
}

void Database::setVerbose(uint16_t _verboselevel) {
    verboselevel = _verboselevel;
}
