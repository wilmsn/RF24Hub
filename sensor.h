/*


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

//char* buf;
struct sensor_t {
        uint32_t     	sensor_id;
        char		sensor_name[FHEMDEVLENGTH];
        NODE_DATTYPE   	node_id;   		
        uint8_t     	channel;
	uint8_t         datatype;
        char		fhem_dev[FHEMDEVLENGTH];
        uint32_t    	last_data;
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
 * Variable für den Schlüssel zur Ver-/Entschlüsselung der
 * Transportdaten
 *************************************************************/
uint32_t    mykey;
/**************************************************************
 * fügt einen neuen record zum Buffer hinzu
 *************************************************************/
void    newEntry(sensor_t*);
/**************************************************************
 * löscht den übergebenen record aus dem Buffer
 *************************************************************/
bool    delEntry(sensor_t*);

public:
    
/**
 *  Setzt das Verboselevel
 */
void setVerbose(uint16_t _verboselevel);
/**
 *  Setzt den Key
 */
void setKey(uint32_t _key);
/**
 *  Löscht den kompletten Inhalt und leert den Buffer
 */
void cleanup(void);
/**
 *  Fügt einen neuen Sensor hinzu
 */
void addSensor(uint32_t sensor, NODE_DATTYPE node_id, uint8_t channel, uint8_t datatype, char* fhem_dev, uint32_t last_utime, uint32_t last_data, char* sensor_name);
/**
 *  Setzt den letzten Wert dieses Sensors in der Form des TransportValues
 *  Rückgabewert: true wenn ein update erfolgt ist.
 */
bool updateLastVal(uint32_t sensor_id, uint32_t last_data);
/**
 *  Findet die Sensor_id durch node_id und channel, 
 *  Rückgabewert ist die sensor_id
 */
uint32_t getSensorByNodeChannel(NODE_DATTYPE node_id, uint8_t channel);
/**
 *  TRUE wenn es sich um ein Systemregister handelt
 */
bool isSystemRegister(uint8_t channel);
/**
 *  Findet node_id und channel des Sensors durch die sensor_id
 */
bool getNodeChannelBySensorID(NODE_DATTYPE* p_node_id, uint8_t* p_channel, uint32_t sensor_id);
/**
 *  Findet node_id und channel des Sensors durch FHEM Device
 */
bool getNodeChannelBySensorName(NODE_DATTYPE *p_node_id, uint8_t* p_channel, char* sensor_name);
/**
 *  Findet node_id und channel des Sensors durch FHEM Device
 */
bool getNodeChannelByFhemDev(NODE_DATTYPE *p_node_id, uint8_t* p_channel, char* fhem_dev);
/**
 *  Findet FHEM Device des Sensors durch node_id und channel
 */
char* getFhemDevByNodeChannel(NODE_DATTYPE node_id, uint8_t channel);
/**
 *  Findet FHEM Device des Sensors durch node_id und channel
 */
int8_t getDataTypeByNodeChannel(NODE_DATTYPE node_id, uint8_t channel);
/**
 * Druckt alle records im Buffer in den out_socket
 * out_socket ist dabei ein gültiger socket file descriptor
 * entweder aus accept für einen socket oder mittels
 * fileno(stdout) für den stdout
 * Der zweite Parameter bestimmt das Format,
 * true => HTML Format; false => Textformat
 */
void printBuffer(int new_tn_in_socket, bool html);

Sensor(void);

};

#endif // _SENSOR_H_
