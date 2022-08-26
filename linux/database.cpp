#include "database.h"

Database::Database(void) {
    tsbuf = (char*) malloc (TSBUFFERSIZE);
    sql_stmt = (char*) malloc(SQLSTRINGSIZE);
    verboseLevel = 0;
}

void Database::db_check_error(void) {
    if (mysql_errno(db) != 0) {
	printf("%sDB-Fehler: %s\n", ts(tsbuf), mysql_error(db));
    }
}

void Database::setLVVolt(NODE_DATTYPE node_id, float lv_volt) {
    sprintf (sql_stmt, "update node set volt_lv = %f where node_id = %u", lv_volt, node_id);
    do_sql(sql_stmt);
}

void Database::setLVFlag(NODE_DATTYPE node_id, bool lv_flag) {
    sprintf (sql_stmt, "update node set lv_flag = %s where node_id = %u", lv_flag? "y":"n", node_id);
    do_sql(sql_stmt);
}

void Database::sync_sensordata_d(void) {
    sprintf (sql_stmt, "%s", "create table sensordata_max engine = MEMORY as select UNIX_TIMESTAMP(FROM_UNIXTIME(max(utime),'%Y%m%d')) max_ts from sensordata_d");
    do_sql(sql_stmt);
    sprintf (sql_stmt, "%s", "delete from sensordata_d where utime > (select max_ts from sensordata_max)" );
    do_sql(sql_stmt);
    sprintf (sql_stmt, "%s", "insert into sensordata_d(sensor_id, value, utime) select sensor_id as s_id, min(value) as min_val, UNIX_TIMESTAMP(FROM_UNIXTIME(utime,'%Y%m%d'))+21600 as ts from sensordata where utime > (select max_ts from sensordata_max) group by sensor_id, UNIX_TIMESTAMP(FROM_UNIXTIME(utime,'%Y%m%d'))" );
    do_sql(sql_stmt);
    sprintf (sql_stmt, "%s", "insert into sensordata_d(sensor_id, value, utime) select sensor_id as s_id, max(value) as max_val, UNIX_TIMESTAMP(FROM_UNIXTIME(utime,'%Y%m%d'))+64800 as ts from sensordata where utime > (select max_ts from sensordata_max) group by sensor_id, UNIX_TIMESTAMP(FROM_UNIXTIME(utime,'%Y%m%d'))" );
    do_sql(sql_stmt);
    sprintf (sql_stmt, "%s", "drop table sensordata_max" );
    do_sql(sql_stmt);
}

void Database::rebuild_sensordata_d(void) {
    sprintf (sql_stmt, "truncate table sensordata_d");
    do_sql(sql_stmt);
    sprintf (sql_stmt, "%s", "insert into sensordata_d(sensor_id, value, utime) select sensor_id, min(value) as min_val, UNIX_TIMESTAMP(FROM_UNIXTIME(utime,'%Y%m%d'))+21600 from sensordata group by sensor_id, UNIX_TIMESTAMP(FROM_UNIXTIME(utime,'%Y%m%d'))" );
    do_sql(sql_stmt);
    sprintf (sql_stmt, "%s", "insert into sensordata_d(sensor_id, value, utime) select sensor_id, max(value) as max_val, UNIX_TIMESTAMP(FROM_UNIXTIME(utime,'%Y%m%d'))+64800 from sensordata group by sensor_id, UNIX_TIMESTAMP(FROM_UNIXTIME(utime,'%Y%m%d'))" );
    do_sql(sql_stmt);
}

void Database::initSystem(void) {
    // sensordata_d ist eine in Memory Tabelle und muss neu aufgebaut werden!
    rebuild_sensordata_d();
}

void Database::initGateway(GatewayClass* gatewayClass) {
    if ( connect() ) {
	MYSQL_ROW row;
	char gw_name[40];
	uint16_t gw_no;
	bool isactive;
	sprintf (sql_stmt, "select gw_name, gw_no, isactive from gateway");
	debugPrintSQL(sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
	MYSQL_RES *result = mysql_store_result(db);
	db_check_error();
	while ((row = mysql_fetch_row(result))) {
	    if ( row[0] != NULL ) sprintf(gw_name,"%s",trim(row[0])); else sprintf(gw_name," ");
	    if ( row[1] != NULL ) gw_no = strtoul(row[1], &pEnd, 10); else gw_no = 0;
	    if ( row[2] != NULL ) isactive = ( row[2][0] == 'y' || row[2][0] == 'j' || row[2][0] == '1'); else isactive = false;
	    gatewayClass->addGateway(gw_name, gw_no, isactive); 
	}
	mysql_free_result(result);    
	disconnect();
    }
}    
    
unsigned long Database::getBeginOfDay(){
    unsigned long retval = 0;
    if ( connect() ) {
	MYSQL_ROW row;
	sprintf (sql_stmt, "%s", "select UNIX_TIMESTAMP(FROM_UNIXTIME(UNIX_TIMESTAMP(),'%Y%m%d'))");
	debugPrintSQL(sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
	MYSQL_RES *result = mysql_store_result(db);
	db_check_error();
	while ((row = mysql_fetch_row(result))) {
	    if ( row[0] != NULL ) retval = strtoul(row[0], &pEnd, 10); else retval = 0;
	}
	mysql_free_result(result);
	disconnect();
    }
    return retval;    
}

void Database::addGateway(char* gw_hostname, uint16_t gw_no){
    sprintf (sql_stmt, "insert into gateway(gw_name, gw_no, isActive) values('%s', %u, 'j')", gw_hostname, gw_no);
    do_sql(sql_stmt);
}

void Database::delGateway(uint16_t gw_no){
    sprintf (sql_stmt, "delete from gateway where gw_no = %u", gw_no);
    do_sql(sql_stmt);
}

void Database::enableGateway(uint16_t gw_no){
    sprintf (sql_stmt, "update gateway set isActive = 'y' where gw_no = %u", gw_no);
    do_sql(sql_stmt);
}

void Database::disableGateway(uint16_t gw_no){
    sprintf (sql_stmt, "update gateway set isActive = 'n' where gw_no = %u", gw_no);
    do_sql(sql_stmt);
}

void Database::initNode(NodeClass* nodeClass) {
    if ( connect() ) {
	NODE_DATTYPE node_id = 0;
	char node_name[NODENAMESIZE];
	node_name[0] = 0;
	bool isMastered = false;
	uint32_t pa_level;
	uint32_t rec_level;
	float lv_volt = 0.0;
	bool lv_flag = false;
	char cmp_y[]="y",cmp_j[]="j"; 
	MYSQL_ROW row;
	sprintf (sql_stmt, "select node_id, node_name, mastered, pa_level, rec_level, volt_lv, lv_flag from node");
	debugPrintSQL(sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
	MYSQL_RES *result = mysql_store_result(db);
	db_check_error();
	while ((row = mysql_fetch_row(result))) {
	    if ( row[0] != NULL ) node_id = strtoul(row[0], &pEnd,10);
	    if ( row[1] != NULL ) sprintf(node_name, "%s", row[1]);
	    if ((strcmp(row[2],cmp_y) == 0) || (strcmp(row[2],cmp_j) == 0)) { isMastered = true; } else { isMastered = false; }
	    if ( row[3] != NULL ) pa_level = strtoul(row[3], &pEnd, 10); else pa_level = 9;
	    if ( row[4] != NULL ) rec_level = strtoul(row[4], &pEnd, 10); else rec_level = 9;
	    if ( row[5] != NULL ) lv_volt = strtof(row[5], &pEnd);
	    if ( row[6][0] == 'y' ) lv_flag = true; 
	    nodeClass->addNode(node_id, node_name, isMastered, pa_level, rec_level, lv_volt, lv_flag); 
	}
	mysql_free_result(result);    
	disconnect();
    }
}

void Database::initSensor(SensorClass* sensorClass) {
    if ( connect() ) {
	char* fhem_dev = alloc_str(verboseLevel,"Database::initSensor fhem_dev",FHEMDEVLENGTH, ts(tsbuf));
	char* sensor_name = alloc_str(verboseLevel,"Database::initSensor sensor_name",FHEMDEVLENGTH, ts(tsbuf));
	uint32_t     	mysensor;
	NODE_DATTYPE   	node_id;
	uint8_t     	mychannel;
	int8_t     	mydatatype;
	MYSQL_ROW row;
	sprintf (sql_stmt, "select sensor_id, node_id, channel, datatype, fhem_dev, sensor_name from sensor");
	debugPrintSQL(sql_stmt);
	mysql_query(db, sql_stmt);
	db_check_error();
	MYSQL_RES *result = mysql_store_result(db);
	db_check_error();
	while ((row = mysql_fetch_row(result))) {
	    if ( row[0] != NULL ) mysensor = strtoul(row[0], &pEnd,10); else mysensor = 0;
	    if ( row[1] != NULL ) node_id = strtoul(row[1], &pEnd,10); else node_id = 0; 
	    if ( row[2] != NULL ) mychannel = strtoul(row[2], &pEnd,10); else mychannel = 0;
	    if ( row[3] != NULL ) mydatatype = strtoul(row[3], &pEnd,10); else mydatatype = -1;
	    if ( row[4] != NULL ) sprintf(fhem_dev,"%s",trim(row[4])); else sprintf(fhem_dev,"not_set");
	    if ( row[5] != NULL ) sprintf(sensor_name,"%s",trim(row[5]));
	    // ToDo
	    sensorClass->addSensor(mysensor, node_id, mychannel, mydatatype, fhem_dev, 0, 0, sensor_name);
	}
	mysql_free_result(result);
	free_str(verboseLevel,"Database::initSensor fhem_dev",fhem_dev, ts(tsbuf)); 
	free_str(verboseLevel,"Database::initSensor sensor_name",sensor_name, ts(tsbuf)); 
	disconnect();
    }
}

void Database::do_sql(char *sqlstmt) {
    if ( connect() ) {
	debugPrintSQL(sqlstmt);
	    if (mysql_query(db, sqlstmt) != 0) {
	    printf("%s\n", sqlstmt);
	}
	db_check_error();
	mysql_commit(db);
	disconnect();
    }
}

void Database::storeSensorValue(uint32_t sensor_id, char* value) {
    sprintf(sql_stmt,"insert into sensordata (sensor_ID, utime, value) values (%u, UNIX_TIMESTAMP(), %s)", sensor_id, value);
    do_sql(sql_stmt);
}

void Database::storeNodeConfig(NODE_DATTYPE node_id, uint8_t channel, char* value) {
    if ( channel == REG_PALEVEL ) {
        sprintf(sql_stmt,"update node set pa_level = %s where node_id = %u ", value, node_id);
        do_sql(sql_stmt);
    }
    if ( channel == REG_RECLEVEL ) {
        sprintf(sql_stmt,"update node set rec_level = %s where node_id = %u ", value, node_id);
        do_sql(sql_stmt);
    }
    sprintf(sql_stmt,"delete from node_configdata where node_id = %u and channel = %u ", node_id, channel);
    do_sql(sql_stmt);
    sprintf(sql_stmt,"insert into node_configdata (node_id, channel, utime, value) values (%u, %u, UNIX_TIMESTAMP(), %s ) ", node_id, channel, value);
    do_sql(sql_stmt);
}

void Database::updateNodeMastered(NODE_DATTYPE node_id, bool isMastered) {
    if (isMastered) {
        sprintf(sql_stmt,"update node set mastered = 'y' where node_id = %u ", node_id);
    } else {
        sprintf(sql_stmt,"update node set mastered = 'n' where node_id = %u ", node_id);
    }
    do_sql(sql_stmt);
}

bool Database::connect(string _db_hostname, string _db_username, string _db_password, string _db_schema, int _db_port) {
    snprintf(db_hostname, DB_HOSTNAME_SIZE, "%s", _db_hostname.c_str());
    snprintf(db_username, DB_USERNAME_SIZE, "%s", _db_username.c_str());
    snprintf(db_password, DB_PASSWORD_SIZE, "%s", _db_password.c_str());
    snprintf(db_schema, DB_SCHEMA_SIZE, "%s", _db_schema.c_str());
    db_port = _db_port;
    if ( verboseLevel & VERBOSESTARTUP) {
        printf("%sMaria-DB client version: %s\n", ts(tsbuf), mysql_get_client_info());
    }
    bool retval = connect();
    if (retval) {
	disconnect();
        printf("Init finished sucessfull\n");
    }
    return retval;
}

bool Database::connect() {
    bool retval = true;
    db = mysql_init(NULL);
    if (db == NULL) {
	sleep(1);
	db = mysql_init(NULL);
	if (db == NULL) {
	    printf("%sERROR getting DB-Handle \n", ts(tsbuf));
	    retval = false;
	}
    }
    if ( retval ) {
        if (mysql_real_connect(db, db_hostname, db_username, db_password, db_schema, db_port, NULL, 0) == NULL) {
	    sleep(1);
	    if (mysql_real_connect(db, db_hostname, db_username, db_password, db_schema, db_port, NULL, 0) == NULL) {
		printf("%sERROR connecting database: %s\n", ts(tsbuf), mysql_error(db));
                mysql_close(db);
                retval = false;
            }
        }
    }
    return retval;
}

bool Database::disconnect() {
    mysql_close(db);
    db = NULL;
    return true;
}

void Database::debugPrintSQL(char* sqlstmt) {
    if ( verboseLevel & VERBOSESQL) {    
        printf("%sSQL: %s\n", ts(tsbuf), sqlstmt);
    }
}

void Database::setVerbose(uint16_t _verboseLevel) {
    verboseLevel = _verboseLevel;
}
