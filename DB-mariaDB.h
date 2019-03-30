/*


*/
#ifndef DB_MARIADB_H   
#define DB_MARIADB_H
#include <stdint.h>
#include <mariadb/mysql.h>
#include "rf24hub_common.h"
#include "config.h"
#include "sensorBuffer.h"
#include <stdio.h> 
#include <iostream>
#include <unistd.h>

using namespace std;

class DB {

    
private:
    
MYSQL     *db;
MYSQL_RES *res;
MYSQL_ROW row;
CONFIG    *cfg;
char* pEnd;

char sql_stmt[SQLSTRINGSIZE];

void do_sql(char* sqlstmt);

void db_check_error(void);
    
public:
    DB(CONFIG * cfg_ptr);

    int init(char db_hostname[PARAM_MAXLEN_DB_HOSTNAME], int db_port, char db_schema[PARAM_MAXLEN_DB_SCHEMA], char db_username[PARAM_MAXLEN_DB_USERNAME], char db_password[PARAM_MAXLEN_DB_PASSWORD]);
    
    void initSystem(void);
    
//    int findSensor(char* sensor, uint16_t * node, uint16_t * channel);
    
    void fillSensorBuffer(SENSORBUFFER * sb);
    
};

#endif // DB_MARIADB_H
