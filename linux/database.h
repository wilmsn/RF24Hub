/**
 * @file database.h
 * @brief Covers all database stuff
 * 
 * This module covers all the communication to the database.
 * Here we use MariaDB
 * 
 * If you want to use a diffent database or even flatfiles
 * to store the data, just replace this module.
 * Make sure that functions and parameters remain unchanged !!!
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
#include "nodeclass.h"
#include "sensorclass.h"
#include "gatewayclass.h"
#define DB_HOSTNAME_SIZE 30
#define DB_USERNAME_SIZE 20
#define DB_PASSWORD_SIZE 20
#define DB_SCHEMA_SIZE 20

using namespace std; 

class Database {

private:

    struct sqlStore_t {
        uint64_t            entrytime;
        char*               sqlstmt;
        sqlStore_t*         p_next;
    };
    sqlStore_t*     p_initial;
    
//    MYSQL*      db;
//    MYSQL_RES*  res;
//    MYSQL_ROW   row;
    char*       pEnd;
    char*       tsbuf;
    char*       sql_stmt;
    uint16_t    verboseLevel;
    char*	db_hostname;
    char*	db_username;
    char*	db_password;
    char*	db_schema;
    int  	db_port;
    void do_sql(char* stmt);
    void debugPrintSQL(char* sqlstmt);
    void db_check_error(MYSQL* mydb);
    

    
public:

/**
 * connects to the database
 */
    MYSQL* connect(void);

    bool disconnect(MYSQL* mydb);

    void storeSQL(char* sqlstmt);
   
    char* getSQL(uint64_t* timestamp);
   
    void delSQL(uint64_t timestamp);

    void exec_sql(char* stmt);

/**
 * Setzt den Verboselevel. Mögliche Level sind in den Makros VERBOSE* in config.h definiert.
 * @param verboseLevel Der aktuelle Verboselevel
 */
    void setVerbose(uint16_t _verboseLevel);
/**
 * connects to the database
 * @param db_hostname Der Hostname der Datenbank mit Domaine
 * @param db_username Der Username zum Zugriff auf die Datenbank. Muss innerhalb der Datenbank alle benötigten Rechte haben.
 * @param db_password Das Datenbankpasswort für den Datenbanknutzer
 * @param db_schema Das Datenbankschema
 * @param db_port Der Datenbankport
 * @return "true" bei erfolgreicher Anmeldung, sonst "false"
 */
//    bool connect(string db_hostname, string db_username, string db_password, string db_schema, int db_port);

/**
 * Sets Voltage of a Node where the Low Voltage Handling starts
 * @param node_id Die Node ID
 * @param volt_LV Die Spannung deren Unterschreitung das LowVoltageLevel startet
 */
   void setLVVolt(NODE_DATTYPE node_id, float lv_volt);
   
   
/**
 * Sets (true) or resets(false) the low Voltage State of a Node
 * @param node_id Die Node ID
 * @param LV_Flag Der Flag gibt an ob der Node im kritischen Spannungsbereich (true) ist oder nicht
 */
   void setLVFlag(NODE_DATTYPE node_id, bool lv_flag);

/**
 * Stores the last data(Transportvalue) of a sensor into the table sensordata
 * @param sensor_id Die Sensor ID
 * @param value Der Sensorwert als Transportvalue
 */
   void storeSensorValue(uint32_t sensor_id, char* value);
    
/**
 * Stores the config data of a node (Registervalue)
 * @param node_id Die Node ID
 * @param channel Der Channel
 * @param value Der Registerwert als String
 */
    void storeNodeConfig(NODE_DATTYPE node_id, uint8_t channel, char* value);

/**
 * Reads the data to initializise the sensor array 
 * from the Database
 * @param sensor Ein Pointer auf das Sensor Objekt
 */
    void initSensor(SensorClass* sensorClass);
    
/**
 * Reads the data to initializise the node array 
 * from the Database
 * @param node Ein Pointer auf das Node Objekt
 */
    void initNode(NodeClass* nodeClass);
    
/**
 * Reads the data to initializise the gateway array 
 * from the Database
 * @param gateway Ein Pointer auf das gateway Objekt
 */
    void initGateway(GatewayClass* gatewayClass);
    
/**
 * Initializise the system 
 * from the Database
 */
    void initSystem(void);

/**
 * Refreshes the table sensordata_d from scratch
 */
    void rebuild_sensordata_d(void);
    
/**
 * Refreshes the table sensordata_d 
 */
    void sync_sensordata_d(void);
    
/**
 * Gets the begin of this day as unix time
 */    
    unsigned long getBeginOfDay();

/**
 * Adds a gateay to the database
 * @param gw_hostname Der Hostname mit Domaine des Gateways
 * @param gw_no Die Gatewaynummer
 */
    void addGateway(char* gw_hostname, uint16_t gw_no);

/**
 * Deletes a gateay from the database
 * @param gw_no Die Gatewaynummer
 */
    void delGateway(uint16_t gw_no);

/**
 * Enables a gateay in the database
 * @param gw_no Die Gatewaynummer
 */
    void enableGateway(uint16_t gw_no);

/**
 * Disables a gateay in the database
 * @param gw_no Die Gatewaynummer
 */
    void disableGateway(uint16_t gw_no);

/**
 * Update the mastered flag of a node
 * @param node_id Die Node ID
 * @param isMastered "true" wenn der Node durch dieses System verwaltet wird, sonst "false"
 */
    void updateNodeMastered(NODE_DATTYPE node_id, bool isMastered);

    
    void set_var(const char* _db_hostname, const char* _db_username, const char* _db_password, const char* _db_schema, int _db_port);

/**
 * The constructor
 */
   Database();
};

#endif // _DATABASE_H_
