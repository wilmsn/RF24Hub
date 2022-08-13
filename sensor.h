/**
 * @file sensor.h
 * @brief Der Sensor wird als Objekt innerhalb der Klasse Sensor verwaltet.
 * 
 * Zum Programmstart werden alle relevanten Informationen zu den SEnsoren aus der Datenbank geladen um dann im weiteren Programmablauf
 * hier verwaltet zu werden.
 * Wichtig: Wird ein neuer Sensor innerhalb der Datenbank angelegt, ist dieser zunächst für das Programm unbekannt.
 * Zur Synkronisation ist per telnet der Befehl "init" abzusetzen.
*/
#ifndef _SENSOR_H_   
#define _SENSOR_H_
#include <stdint.h>
#include <cstring>
#include <unistd.h>
#include <cstdlib>
#include "rf24_config.h"
#include "rf24hub_config.h"
#include "common.h"

class Sensor {

    
private:

struct sensor_t {
        uint32_t        sensor_id;
        char            sensor_name[FHEMDEVLENGTH];
        NODE_DATTYPE    node_id;   		
        uint8_t         channel;
        uint8_t         datatype;
        char            fhem_dev[FHEMDEVLENGTH];
        uint32_t        last_data;
        uint32_t        last_utime;
        sensor_t*       p_next;
};
sensor_t*     p_initial;
/**************************************************************
 * char buffer zur allgemeinen Ausgabe
 *************************************************************/
char*       buf;
char*       buf1;
/**************************************************************
 * char buffer zur Ausgabe des timestrings ==> ts(tsbuf)
 *************************************************************/
char*       tsbuf;
/**************************************************************
 * Bufferinterner Speicher für den verboselevel
 *************************************************************/
uint16_t    verboselevel;
/**************************************************************
 * fügt einen neuen record zum Buffer hinzu
 *************************************************************/
void        newEntry(sensor_t*);
/**************************************************************
 * löscht den übergebenen record aus dem Buffer
 *************************************************************/
bool        delEntry(sensor_t*);

public:
    
/**
 * Setzt den Verboselevel. Mögliche Level sind in den Makros VERBOSE* in config.h definiert.
 * @param verboselevel Der aktuelle Verboselevel
 */
void setVerbose(uint16_t _verboselevel);

/**
 * Löscht den kompletten Inhalt und leert den Buffer
 */
void cleanup(void);

/**
 * Fügt einen neuen Sensor hinzu
 * @param sensor_id Die Sensor-ID
 * @param node_id Die Node-ID
 * @param channel Der Channel für diesen Sensor  
 * @param datatype Der verwendete Datentyp zur Übertragung der Werte
 * @param fhem_dev Der Devicename innerhalb von FHEM für diesen Sensor. Ist dieser gesetzt wird bei jeder Änderung eine Änderung an FHEM gesendet.
 * @param last_utime Unix-Timestamp des letzten Wertes
 * @param last_data Der letzte Wert des Sensors (Transportwert)
 * @param sensor_name Der Bezeichner dieses Sensors
 */
void addSensor(uint32_t sensor, NODE_DATTYPE node_id, uint8_t channel, uint8_t datatype, char* fhem_dev, uint32_t last_utime, uint32_t last_data, char* sensor_name);

/**
 * Setzt den letzten Wert dieses Sensors in der Form des TransportValues
 * @param sensor_id Die Sensor-ID
 * @param last_data Der letzte Wert des Sensors (Transportwert)
 * @return "true" wenn ein Update erfolgt ist, sonst "false"
 */
bool updateLastVal(uint32_t sensor_id, uint32_t last_data);

/**
 * Findet die Sensor_id durch node_id und channel, 
 * @param node_id Die Node-ID
 * @param channel Der Channel für diesen Sensor  
 * @return Rückgabewert ist die sensor_id
 */
uint32_t getSensorByNodeChannel(NODE_DATTYPE node_id, uint8_t channel);

/**
 * Findet node_id und channel des Sensors durch die sensor_id
 * @param node_id Ein Pointer auf die Node-ID (Nimmt das Ergebnis auf)
 * @param channel Ein Pointer auf den Channel für diesen Sensor (Nimmt das Ergebnis auf)
 * @param sensor_id Die Sensor ID  
 * @return "true" wenn diese Sensor ID gefunden wurde, sonst "false"
 */
bool getNodeChannelBySensorID(NODE_DATTYPE* p_node_id, uint8_t* p_channel, uint32_t sensor_id);

/**
 * Findet node_id und channel des Sensors durch FHEM Device
 * @param node_id Ein Pointer auf die Node-ID (Nimmt das Ergebnis auf)
 * @param channel Ein Pointer auf den Channel für diesen Sensor (Nimmt das Ergebnis auf)
 * @param sensor_name Ein Pointer auf einen String mit der Bezeichnung des Sensors  
 * @return "true" wenn diese Sensorbezeichnung gefunden wurde, sonst "false"
 */
bool getNodeChannelBySensorName(NODE_DATTYPE *p_node_id, uint8_t* p_channel, char* sensor_name);

/**
 * Findet node_id und channel des Sensors durch FHEM Device
 * @param node_id Ein Pointer auf die Node-ID (Nimmt das Ergebnis auf)
 * @param channel Ein Pointer auf den Channel für diesen Sensor (Nimmt das Ergebnis auf)
 * @param fhem_dev Ein Pointer auf einen String mit der FHEM Device  
 */
bool getNodeChannelByFhemDev(NODE_DATTYPE *p_node_id, uint8_t* p_channel, char* fhem_dev);

/**
 * Findet FHEM Device des Sensors durch node_id und channel
 * @param node_id Ein Pointer auf die Node-ID (Nimmt das Ergebnis auf)
 * @param channel Ein Pointer auf den Channel für diesen Sensor (Nimmt das Ergebnis auf)
 * @return Ein Pointer auf einen String mit der Bezeichnung des Sensors  
 */
char* getFhemDevByNodeChannel(NODE_DATTYPE node_id, uint8_t channel);

/**
 * Findet den für diesen Sensor verwendeten Datentypen durch node_id und channel
 * @param node_id Ein Pointer auf die Node-ID (Nimmt das Ergebnis auf)
 * @param channel Ein Pointer auf den Channel für diesen Sensor (Nimmt das Ergebnis auf)
 * @return Der Datentyp dieses Sensors  
 */
int8_t getDataTypeByNodeChannel(NODE_DATTYPE node_id, uint8_t channel);

/**
 * Druckt alle records im Buffer in den out_socket
 * out_socket ist dabei ein gültiger socket file descriptor
 * entweder aus accept für einen socket oder mittels
 * fileno(stdout) für den stdout
 * Der zweite Parameter bestimmt das Format,
 * true => HTML Format; false => Textformat
 * @param tn_socket Das aktuelle Socket zur Ausgabe
 * @param htmlformat "true" liefert die Ausgabe im HTML Format, "false" im Textformat
 */
void printBuffer(int new_tn_in_socket, bool html);

Sensor(void);

};

#endif // _SENSOR_H_
