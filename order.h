/*


*/
#ifndef _ORDER_H_   
#define _ORDER_H_
#include <stdint.h>
#include "rf24hub_config.h"


class Order {

    
//private:
public:
    
// Structure to handle the orderqueue
struct order_t {
	uint16_t 	    orderno;   		// the orderno as primary key for our message for the nodes
	uint16_t       	node;   		// the destination node
	uint8_t      	type;      		// Becomes networkheader.type
	unsigned int   	flags;     		// Some flags as part of payload
	uint8_t      	channel1;		// The channel for the sensor 1
	float		    value1;    		// the information that is send to sensor 1
	uint8_t        	channel2;  		// The channel for the sensor 2
	float		    value2;   	 	// the information that is send to sensor 2
	uint8_t      	channel3;  		// The channel for the sensor 3
	float		    value3;    		// the information that is send to sensor 3
	uint8_t      	channel4;  		// The channel for the sensor 1
	float		    value4;    		// the information that is send to sensor 4
	uint64_t		entrytime;
	uint64_t	  	last_send;		// Timestamp for last sending of this record
	order_t         *next;          // poiter to the next record
};

    order_t *initial_ptr;
    void new_entry(order_t*);
    bool del_orderno(uint16_t);
    bool del_entry(order_t*);
    bool del_node(uint16_t);
    bool find_orderno(uint16_t orderno);
    void debug_print_buffer(void);
    Order(void);

};

#endif // _ORDER_H_
