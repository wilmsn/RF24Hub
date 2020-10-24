/**
 * 
 * This module covers all 
 * the communication to the database.
 * Here we use MariaDB
 * 
 * If you want to use a diffent database or even flatfiles
 * to store the data, just replace this module.
 * Make sure that functions and parameters remain
 * unchanged !!!
 * 
 */

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
#include "gateway.h"

using namespace std; 

class Database {

private:

    MYSQL       *db;
    MYSQL_RES   *res;
    MYSQL_ROW   row;
    char*       pEnd;
    char*       tsbuf;
    char*       sql_stmt;
    uint16_t    verboselevel;
    void do_sql(char* stmt);
    void debugPrintSQL(char* sqlstmt);
    void db_check_error(void);

public:

/**
 *  Setzt das Verboselevel
 */
    void setVerbose(uint16_t _verboselevel);
/**
 * connects to the database
 */
    bool connect(string db_hostname, string db_username, string db_password, string db_schema, int db_port);
/**
 * Stores the data of a sensor
 */
    void storeSensorValue(uint32_t mysensor, char* value);
/**
 * Stores the config data of a node
 */
    void storeNodeConfig(NODE_DATTYPE node, uint8_t channel, char* value);
/**
 * Reads the data to initializise the sensor array 
 * from the Database
 */
    void initSensor(Sensor* sensor);
/**
 * Reads the data to initializise the node array 
 * from the Database
 */
    void initNode(Node* node);
/**
 * Reads the data to initializise the node array 
 * from the Database
 */
    void initGateway(Gateway* gateway);
/**
 * Initializise the system 
 * from the Database
 */
    void initSystem(void);
/**
 * Stores/Syncs all the data from node_configdata*_im
 * to node_configdata* to make them persistent
 */
    void sync_config(void);
/**
 * Stores/Syncs all the data from "sensor_im"
 * to "sensor" to make them persistent
 */
    void sync_sensor(void);
/**
 * Stores/Syncs all the data from "sensordata_im"
 * to "sensordata" to make them persistent
 */
    void sync_sensordata(void);
/**
 * Refreshes the table sensordata_d from scratch
 */
    void sync_sensordata_d(void);
/**
 * The constructor
 */
    Database(void);
};

#endif // _DATABASE_H_
