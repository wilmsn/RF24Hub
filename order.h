/*


*/
#ifndef _ORDER_H_   
#define _ORDER_H_
#include <stdint.h>
#include <cstring>
#include <unistd.h>
#include "log.h"
#include "rf24hub_config.h"


class Order {

    
private:
    Logger* logger;
    
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
    bool has_order;
    void new_entry(order_t*);
    bool del_orderno(uint16_t);
    bool del_entry(order_t*);
    bool del_node(uint16_t);
    bool find_orderno(uint16_t orderno);
    uint16_t del_old_entry(uint64_t entrytime);
    void debug_print_buffer(void);
    void begin(Logger* _logger);
    void add_order(uint16_t orderno, uint16_t node, uint8_t type, unsigned int flags,
                   uint8_t channel1, float value1, uint8_t channel2, float value2, 
                   uint8_t channel3, float value3, uint8_t channel4, float value4, 
                   uint64_t entrytime);
    bool get_order_for_transmission(uint16_t* orderno, uint16_t* node, unsigned char* type, uint16_t* flags,
                                    uint8_t* channel1, float* value1, uint8_t* channel2, float* value2, 
                                    uint8_t* channel3, float* value3, uint8_t* channel4, float* value4, 
                                    uint64_t systime); 
    void print_buffer(int new_tn_in_socket);
    void html_buffer(int new_tn_in_socket);
    Order(void);

};

#endif // _ORDER_H_
