#include "rf24hub_common.h"
#include "config.h"
#include "logmsg.h"
#include <getopt.h>
#include <unistd.h>
#include <stdlib.h>
#include <mysql/mysql.h>

bool tn_port_set = false;
bool tn_host_set = false;
bool in_port_set = false;
bool start_daemon = false;
logmode_t logmode;
int verboselevel = 2;
FILE * logfile_ptr;
FILE * pidfile_ptr;
MYSQL     *db;
MYSQL_RES *res;
MYSQL_ROW row;
	uint16_t       	node;   		// the destination node


char config_file[PARAM_MAXLEN_CONFIGFILE];
struct config_parameters parms;
char sql_stmt[200];

void db_check_error(void) {
	if (mysql_errno(db) != 0) {
        char debug[DEBUGSTRINGSIZE];
		sprintf(debug, "DB-Fehler: %s\n", mysql_error(db));
        logmsg(VERBOSECRITICAL, debug);
    }
}

int main(int argc, char* argv[]) {
    char debug[DEBUGSTRINGSIZE];
    processParams(argc, argv);

	// check if started as root
/*	if ( getuid()!=0 ) {
           fprintf(stdout, "rf24hubd has to be startet as user root\n");
          exit(1);
        }
*/
    // check if config file is readable
    parseConfigFile(config_file);
	
    // starts logging
    setPidfile();
    
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
			sleep(1);
			db = mysql_init(NULL);
		} else {
			fprintf(stderr, "%s\n", mysql_error(db));
            mysql_close(db);
			unlink(parms.pidfilename);
            exit(1);
		}
    }
    mysql_wait_count = 0;
    printf("DB Connect with %s %s %s %s %d \n ",parms.db_hostname, parms.db_username, parms.db_password, parms.db_schema, parms.db_port);
    while (mysql_real_connect(db, parms.db_hostname, parms.db_username, parms.db_password, parms.db_schema, parms.db_port, NULL, 0) == NULL) {
		sprintf(debug,"Waiting for Database: %d Sec.", 20-mysql_wait_count);
		logmsg(VERBOSESTARTUP, debug);		
		if ( mysql_wait_count < 20 ) {
			mysql_wait_count++;
			sleep(1);
		} else {
			fprintf(stderr, "%s\n", mysql_error(db));
			mysql_close(db);
			unlink(parms.pidfilename);
			exit(1);
		}
    }
    sprintf(debug, "Connected to host %s with DB %s on port %d", parms.db_hostname, mysql_get_server_info(db), parms.db_port);
    logmsg(VERBOSESTARTUP, debug);
    node = 2;
	sprintf (sql_stmt, "select node_id, sleeptime1, sleeptime2, sleeptime3, sleeptime4, radiomode, voltagefactor from node where node_id = '0%o' LIMIT 1 ",node);
	mysql_query(db, sql_stmt);
	db_check_error();
	MYSQL_RES *result = mysql_store_result(db);
	db_check_error();
	MYSQL_ROW row;
	if ((row = mysql_fetch_row(result))) {
        sprintf(debug,"Node: %s %s %s %s %s %s %s", row[0], row[1], row[2],row[3], row[4], row[5], row[6]);
        logmsg(VERBOSESTARTUP, debug);
/*        
        fill_order_buffer( node, 111, strtof(row[0], &pEnd));
		fill_order_buffer( node, 112, strtof(row[1], &pEnd));
		fill_order_buffer( node, 113, strtof(row[2], &pEnd));
		fill_order_buffer( node, 114, strtof(row[3], &pEnd));
		fill_order_buffer( node, 115, strtof(row[4], &pEnd));
		fill_order_buffer( node, 116, strtof(row[5], &pEnd));
		fill_order_buffer( node, 118, 1.0);
		*/
	}
	mysql_free_result(result);
 

    
    removePidfile();
    return 0;
}

