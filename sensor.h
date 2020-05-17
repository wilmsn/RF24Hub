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
        NODE_DATTYPE   	node_id;   		
        uint8_t     	channel;
        char			fhem_dev[FHEMDEVLENGTH];
        uint64_t        last_ts;
        float			last_val;
        uint32_t        last_val_utime;
        sensor_t*       p_next;
};
sensor_t*     p_initial;
/**************************************************************
 * char buffer zur Ausgabe des timestrings ==> ts(buf)
 *************************************************************/
char*       buf;
/**************************************************************
 * Bufferinterner Speicher für den verboselevel
 *************************************************************/
uint16_t    verboselevel;
/**************************************************************
 * fügt einen neuen record zum Buffer hinzu
 *************************************************************/
void    newEntry(sensor_t*);
/**************************************************************
 * löscht den übergebenen record aus dem Buffer
 *************************************************************/
bool    delEntry(sensor_t*);
 
public:
    
/**************************************************************
 *  Setzt das Verboselevel
 *************************************************************/    
void setVerbose(uint16_t _verboselevel);
/**************************************************************
 *  Löscht den kompletten Inhalt und leert den Buffer
 *************************************************************/    
void cleanup(void);
/**************************************************************
 *  Fügt einen neuen Sensor hinzu
 *************************************************************/    
void addSensor(uint32_t sensor, NODE_DATTYPE node_id, uint8_t channel, char* fhem_dev, uint32_t last_val_utime, float last_val);
/**************************************************************
 *  Setzt value und last_ts auf den übergebenen Wert, 
 *  Rückgabewert ist die sensor_id
 *************************************************************/
uint32_t updateLastVal(NODE_DATTYPE node_id, uint8_t channel, float value, uint64_t mymillis);
/**************************************************************
 *  Findet node_id und channel des Sensors durch die sensor_id
 *************************************************************/
bool getNodeChannelBySensorID(NODE_DATTYPE* p_node_id, uint8_t* p_channel, uint32_t sensor_id);
/**************************************************************
 *  Findet node_id und channel des Sensors durch FHEM Device
 *************************************************************/
bool getNodeChannelByFhemDev(NODE_DATTYPE *p_node_id, uint8_t* p_channel, char* fhem_dev);
/**************************************************************
 *  Findet FHEM Device des Sensors durch node_id und channel
 *************************************************************/
char* getFhemDevByNodeChannel(NODE_DATTYPE node_id, uint8_t channel);
/**************************************************************
 *  Druckt den Inhalt des Buffers auf StdIO
 *************************************************************/
void printBuffer(void);
/**************************************************************
 *  Druckt den Inhalt des Buffers auf Telnet Socket
 *************************************************************/
void printBuffer2tn(int new_tn_in_socket);

Sensor(void);

};

#endif // _SENSOR_H_
