/*


*/
#ifndef _SENSOR_H_   
#define _SENSOR_H_
#include <stdint.h>
#include "rf24hub_config.h"

class Sensor {

    
//private:
    
    
public:
    
struct sensor_t {
	uint32_t     	sensor;
	uint16_t       	node;   		
	uint8_t     	channel;
    char			s_type;
	char			fhem_dev[FHEMDEVLENGTH];
    uint64_t        last_ts;
    float			last_val;	
	sensor_t         *next;          // poiter to the next record
};

    sensor_t *initial_ptr;
    void new_entry(sensor_t*);
    bool update_last_val(uint16_t node, uint8_t channel, float value, uint64_t mymillis);
    Sensor(void);

};

#endif // _SENSOR_H_
