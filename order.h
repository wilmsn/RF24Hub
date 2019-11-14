/*


*/
#ifndef _ORDER_H_   
#define _ORDER_H_
#include <stdint.h>

class Order {

    
//private:
public:
    
// Structure to handle the orderqueue
struct order_t {
	uint16_t 	    orderno;   		// the orderno as primary key for our message for the nodes
	uint16_t       	node;   		// the destination node
	unsigned char  	type;      		// Becomes networkheader.type
	unsigned int   	flags;     		// Some flags as part of payload
	unsigned char  	channel1;		// The channel for the sensor 1
	float		    value1;    		// the information that is send to sensor 1
	unsigned char  	channel2;  		// The channel for the sensor 2
	float		    value2;   	 	// the information that is send to sensor 2
	unsigned char  	channel3;  		// The channel for the sensor 3
	float		    value3;    		// the information that is send to sensor 3
	unsigned char  	channel4;  		// The channel for the sensor 1
	float		    value4;    		// the information that is send to sensor 4
	uint64_t		entrytime;
	uint64_t	  	last_send;		// Timestamp for last sending of this record
	order_t         *next;          // poiter to the next record
};

    order_t *initial_ptr;
    void new_entry(order_t*);
    bool del_orderno(uint16_t orderno);
    bool del_entry(order_t* my_ptr);
    Order(void);

};

#endif // _ORDER_H_
