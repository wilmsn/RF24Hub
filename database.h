/*************************************************
 * 
 * This module covers all 
 * the communication to the database.
 * Here we use MariaDB
 * 
 ************************************************/

#ifndef _DATABASE_H_   
#define _DATABASE_H_
#include <stdint.h>
#include <cstring>
#include <unistd.h>
#include <stdio.h> 
#include <stdlib.h>
#include <time.h> 
#include <iostream>
#include <mariadb/mysql.h>
#include "rf24_config.h"
#include "rf24hub_config.h"
#include "common.h"
#include "node.h"
#include "sensor.h"

using namespace std; 

class Database {

private:

    MYSQL       *db;
    MYSQL_RES   *res;
    MYSQL_ROW   row;
    char*       pEnd;
    char*       buf;
    char*       sql_stmt;
    uint16_t    verboselevel;
    void do_sql(char* stmt);
    void debugPrintSQL(char* sqlstmt);
    void db_check_error(void);

public:

/**************************************************************
 *  Setzt das Verboselevel
 *************************************************************/    
void setVerbose(uint16_t _verboselevel);
    bool connect(string db_hostname, string db_username, string db_password, string db_schema, int db_port);
    void storeSensorValue(uint32_t mysensor, char* value);
    void storeNodeConfig(NODE_DATTYPE node, uint8_t channel, char* value);
    void initSensor(Sensor* sensor);
    void initNode(Node* node);
    void initSystem(void);
    void sync_sensor(void);
    void sync_sensordata(void);
    Database(void);
};

#endif // _DATABASE_H_
