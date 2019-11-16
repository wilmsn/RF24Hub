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
	uint16_t       	node;   		// the destination node
	uint16_t     	channel;
    char			s_type;
	char			fhem_dev[FHEMDEVLENGTH];
    float			last_val;	
	sensor_t         *next;          // poiter to the next record
};

    sensor_t *initial_ptr;
    void new_entry(sensor_t*);
//    bool del_entry(order_t* my_ptr);
    Sensor(void);

};

#endif // _SENSOR_H_
