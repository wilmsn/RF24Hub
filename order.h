/*


*/
#ifndef _ORDER_H_   
#define _ORDER_H_
#include <stdint.h>
#include <cstring>
#include <unistd.h>
#include <stdio.h> 
#include <iostream>
#include "log.h"
#include "rf24hub_config.h"


class Order {

    
public:
    
    uint16_t orderno;
    bool has_order;
    bool del_orderno(uint16_t);
    bool del_node(uint16_t);
    bool is_orderno(uint16_t orderno);
    uint16_t del_old_entry(uint64_t entrytime);
    void debug_print_buffer(int);
    void begin(Logger* _logger);
    void add_order(uint16_t node, uint8_t type, bool HB_order, uint8_t channel1, float value1, uint64_t entrytime);
    void modify_order(uint16_t node, uint8_t pos, uint8_t channel, float value);
    void modify_orderflags(uint16_t orderno, uint16_t flags);
    void add_endorder(uint16_t node, uint64_t entrytime);
    bool get_order_for_transmission(uint16_t* orderno, uint16_t* node, unsigned char* type, uint16_t* flags,
                                    uint8_t* channel1, float* value1, uint8_t* channel2, float* value2, 
                                    uint8_t* channel3, float* value3, uint8_t* channel4, float* value4, 
                                    uint64_t systime); 
    void print_buffer(int new_tn_in_socket);
    void html_buffer(int new_tn_in_socket);
    Order(void);

private:

    // Structure to handle the orderqueue
    struct order_t {
        uint16_t 	    orderno;   		// the orderno as primary key for our message for the nodes
        uint16_t       	node;   		// the destination node
        bool            HB_order;       // true if this node is a Heartbeat Node
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
        uint64_t		entrytime;      // Timestamp for creating of this record
        uint64_t	  	last_send;		// Timestamp for last sending of this record
        order_t         *next;          // poiter to the next record
    };

    Logger*   logger;
    order_t  *initial_ptr;
    void      new_entry(order_t*);
    bool      del_entry(order_t*);
    order_t  *find_node(uint16_t node);
    
};

#endif // _ORDER_H_
