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
#include "rf24hub_config.h"
#include "node.h"
#include "sensor.h"
#include "common.h"
#include "log.h"

using namespace std; 

class Database {

private:

    MYSQL       *db;
    MYSQL_RES   *res;
    MYSQL_ROW   row;
    Logger      *logger;
    char*       pEnd;
    void do_sql(char* stmt);
    void db_check_error(void);

public:

    void begin(Logger* _logger);
    bool connect(string db_hostname, string db_username, string db_password, string db_schema, int db_port);
    void storeSensorValue(uint32_t mysensor, float value);
    void storeNodeConfig(uint16_t node, uint8_t channel, float value);
    void initSensor(Sensor* sensor);
    void initNode(Node* node);
    void initSystem(void);
    void sync_sensor(void);
    void sync_sensordata(void);
};

#endif // _DATABASE_H_
