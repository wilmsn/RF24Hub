#include "DB-mariaDB.h"

/*
 * Constructor
 */
DB::DB(CONFIG * cfg_ptr) {
    cfg = cfg_ptr;
}

int DB::init(char db_hostname[PARAM_MAXLEN_DB_HOSTNAME], int db_port, char db_schema[PARAM_MAXLEN_DB_SCHEMA], char db_username[PARAM_MAXLEN_DB_USERNAME], char db_password[PARAM_MAXLEN_DB_PASSWORD])
{
    string debug;
    int mysql_wait_count = 0;
	db = mysql_init(NULL);
    mysql_wait_count = 0;
    int retcode = 1;
    while (mysql_real_connect(db, db_hostname, db_username, db_password, db_schema, db_port, NULL, 0) == NULL) {
		if ( mysql_wait_count < 20 ) {
			mysql_wait_count++;
			sleep(1000);
		} else {
            retcode = 0;
        }
    }
    debug = "DB Server Version: ";
    debug += mysql_get_server_info(db);
    cfg->logmsg(VERBOSESTARTUP, debug);
    debug = "DB Client Version: ";
    debug += mysql_get_client_info();
    cfg->logmsg(VERBOSESTARTUP, debug);
    
    return retcode;
}

void DB::initSystem(void) {
    sprintf (sql_stmt, "truncate table sensor_im");
	do_sql(sql_stmt);
	sprintf (sql_stmt, "insert into sensor_im(sensor_id, sensor_name, add_info, node_id, channel, value, utime, store_days, fhem_dev, signal_quality, s_type, html_show) select sensor_id, sensor_name, add_info, node_id, channel, value, utime, store_days, fhem_dev, signal_quality, s_type, html_show from sensor");
	do_sql(sql_stmt);
	sprintf (sql_stmt, "truncate table sensordata_im");
	do_sql(sql_stmt);
	sprintf (sql_stmt, "insert into sensordata_im(sensor_id, utime, value) select sensor_id, utime, value from sensordata where utime > UNIX_TIMESTAMP(subdate(current_date, 2))");
	do_sql(sql_stmt);
}

void DB::do_sql(char* sqlstmt) {
    string debug;
   	if (mysql_query(db, sqlstmt)) {
        debug = "ERROR (SQL below): ";
        debug += mysql_error(db);
		cfg->logmsg(VERBOSECRITICAL, debug);
	}
    debug = "SQL Stmt: ";
    debug += sql_stmt;
	cfg->logmsg(VERBOSESQL, debug);
}
/*
int DB::findSensor(char* sensor, uint16_t * node, uint16_t * channel) {
    sprintf (sql_stmt, "select node_id, channel from sensor_im where fhem_dev = '%s' LIMIT 1 ", sensor);
	mysql_query(db, sql_stmt);
	db_check_error();
	MYSQL_RES *result = mysql_store_result(db);
	db_check_error();
	MYSQL_ROW row;
	if ((row = mysql_fetch_row(result))) {
		*node = stoul (row[0],nullptr,0);;
		*channel = stoul (row[1],nullptr,0);
	}
	mysql_free_result(result);
}
*/
void DB::db_check_error(void) {
    string debug;
    if (mysql_errno(db) != 0) {
		debug = "DB-Fehler: ";
        debug += mysql_error(db);
        cfg->logmsg(VERBOSECRITICAL, debug);
    }
}


void DB::fillSensorBuffer(SENSORBUFFER * sb) {
    char fhem_dev[100];
    sprintf (sql_stmt, "select sensor_id, node_id, channel, fhem_dev from sensor order by sensor_id asc");
	mysql_query(db, sql_stmt);
	db_check_error();
	MYSQL_RES *result = mysql_store_result(db);
	db_check_error();
	MYSQL_ROW row;
	while ((row = mysql_fetch_row(result))) {
        if ( row[3] != NULL ) sprintf(fhem_dev,"%s",row[3]); else sprintf(fhem_dev,"not_set");
		sb->newSensor(stoul(row[0],nullptr,0),stoul(row[1],nullptr,0),stoul(row[2],nullptr,0),fhem_dev);
        printf("##>>>>>, %u\n",stoul(row[0],nullptr,0));
	}
	printf("####n");
	mysql_free_result(result);
    
}
