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
#include "common.h"


class Order {

    
public:
    
    uint8_t orderno;
    bool has_order;
    bool del_orderno(uint8_t);
    bool del_node(uint16_t);
    bool is_orderno(uint8_t orderno);
    uint16_t del_old_entry(uint64_t entrytime);
    void begin(Logger* _logger);
    void add_order(uint16_t node, uint8_t type, bool HB_order, uint32_t data, uint64_t entrytime);
    void modify_order(uint16_t node, uint8_t pos, uint32_t data);
    void modify_orderflags(uint16_t node, uint8_t flags);
    void add_endorder(uint16_t node, uint8_t msg_type, uint64_t entrytime);
    bool get_order_for_transmission(uint8_t* orderno, uint16_t* node, uint8_t* type, uint8_t* flags,
                                    uint32_t* data1, uint32_t* data2, uint32_t* data3, uint32_t* data4, uint32_t* data5, uint32_t* data6,  
                                    uint64_t systime); 
    void print_buffer(int new_tn_in_socket);
    void html_buffer(int new_tn_in_socket);
    Order(void);

private:

    // Structure to handle the orderqueue
    struct order_t {
        uint8_t 	    orderno;   		// the orderno as primary key for our message for the nodes
        uint16_t       	node;   		// the destination node
        bool            HB_order;       // true if this node is a Heartbeat Node
        uint8_t      	type;      		// Becomes networkheader.type
        uint8_t   	    flags;     		// Some flags as part of payload
        uint32_t      	data1;		    // The transportvalue for the Sensor 1 and Value 1
        uint32_t      	data2;		    // The transportvalue for the Sensor 2 and Value 2
        uint32_t      	data3;		    // The transportvalue for the Sensor 3 and Value 3
        uint32_t      	data4;		    // The transportvalue for the Sensor 4 and Value 4
        uint32_t      	data5;		    // The transportvalue for the Sensor 5 and Value 5
        uint32_t      	data6;		    // The transportvalue for the Sensor 6 and Value 6
        uint64_t		entrytime;      // Timestamp for creating of this record
        uint64_t	  	last_send;		// Timestamp for last sending of this record
        order_t         *next;          // poiter to the next record
    };

    Logger*   logger;
    order_t  *initial_ptr;
    void      new_entry(order_t*);
    bool      del_entry(order_t*);
    order_t  *find_node(uint16_t node);
    void debug_print_buffer(uint16_t);    
};

#endif // _ORDER_H_
