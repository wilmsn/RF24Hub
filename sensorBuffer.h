/*


*/
#ifndef _SENSORBUFFER_H_   
#define _SENSORBUFFER_H_
#include <stdint.h>
#include "rf24hub_config.h"

class sensorBuffer {

    
private:
    
struct sensorBuffer_t {
	uint32_t     	sensor;
	uint16_t       	node;   		// the destination node
	uint16_t     	channel;
    char			s_type;
	char			*fhem_dev;
    float			last_val;	
    sensorBuffer_t  *next;
};

    sensorBuffer_t *initial_sensorBuffer_ptr;

    int delEntry(sensorBuffer_t *sensorBufferEntry);
    
public:
    int newSensor(uint32_t sensor, uint16_t node, uint16_t channel, char s_type, char* fhem_dev, float last_val);
    void listSensor(void);
    sensorBuffer(void);

};

#endif // _SENSORBUFFER_H_
