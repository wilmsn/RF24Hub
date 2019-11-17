/*


*/
#ifndef _ORDERBUFFER_H_   
#define _ORDERBUFFER_H_
#include <stdint.h>

class OrderBuffer {

    
//private:
public:
    
// Structure to handle the orderqueue
struct orderbuffer_t {
	uint16_t 	    orderno;   	// the orderno as primary key for our message for the nodes
	uint64_t		entrytime;
	uint16_t     	node;
	uint8_t     	channel;
	float        	value;
	orderbuffer_t*  next;          // poiter to the next record
};

    orderbuffer_t* initial_ptr;
    void new_entry(orderbuffer_t*);
    bool del_entry(orderbuffer_t*);
    bool find_orderno(uint16_t);
    bool del_orderno(uint16_t);
    bool del_node_channel(uint16_t, uint8_t);
    bool node_has_entry(uint16_t);
    void debug_print_buffer(void);
    OrderBuffer(void);

};

#endif // _ORDERBUFFER_H_
