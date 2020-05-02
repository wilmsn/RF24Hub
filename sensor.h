/*


*/
#ifndef _SENSOR_H_   
#define _SENSOR_H_
#include <stdint.h>
#include <cstring>
#include <unistd.h>
#include <cstdlib>
#include "common.h"
#include "buffer.h"
#include "rf24hub_config.h"

extern uint16_t verboselevel;   

class Sensor : public Buffer {

    
private:

struct sensor_t {
        uint32_t     	sensor_id;
        uint8_t       	node_id;   		
        uint8_t     	channel;
        char			fhem_dev[FHEMDEVLENGTH];
        uint64_t        last_ts;
        float			last_val;	
};
 
/*************************************************************
 * Findet einen Sensor anhand seiner Nummer
 ************************************************************/    
void* findSensor(uint32_t);

public:
    
/**************************************************************
 *  Löscht den kompletten Inhalt und leert den Buffer
 *************************************************************/    
void cleanup(void);
/**************************************************************
 *  Fügt einen neuen Sensor hinzu
 *************************************************************/    
void addSensor(uint32_t sensor, uint8_t node_id, uint8_t	channel, char* fhem_dev, uint64_t last_ts, float last_val);
/**************************************************************
 *  Setzt value und last_ts auf den übergebenen Wert
 *************************************************************/
bool updateLastVal(uint32_t sensor_id, float value, uint64_t mymillis);
/**************************************************************
 *  Findet node_id und channel des Sensors durch die sensor_id
 *************************************************************/
uint32_t getSensorByNodeChannel(uint8_t node_id, uint8_t channel);
/**************************************************************
 *  Findet node_id und channel des Sensors durch die sensor_id
 *************************************************************/
bool getNodeChannelBySensorID(uint8_t* node_id, uint8_t* channel, uint32_t sensor_id);
/**************************************************************
 *  Findet node_id und channel des Sensors durch FHEM Device
 *************************************************************/
bool getNodeChannelByFhemDev(uint8_t* node_id, uint8_t* channel, char* fhem_dev);
/**************************************************************
 *  Findet FHEM Device des Sensors durch node_id und channel
 *************************************************************/
char* getFhemDevByNodeChannel(uint8_t node_id, uint8_t channel);
/**************************************************************
 *  Druckt den Inhalt des Buffers auf StdIO
 *************************************************************/
void printBuffer(uint16_t debuglevel);
/**************************************************************
 *  Druckt den Inhalt des Buffers auf Telnet Socket
 *************************************************************/
void printBuffer2tn(int new_tn_in_socket);

Sensor(void);

};

#endif // _SENSOR_H_
