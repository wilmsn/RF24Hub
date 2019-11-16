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
	uint16_t     	channel;
	float        	value;
	orderbuffer_t*  next;          // poiter to the next record
};

    orderbuffer_t* initial_ptr;
    void new_entry(orderbuffer_t*);
    bool del_entry(orderbuffer_t*);
    bool del_orderno(uint16_t orderno);
    bool del_node_channel(uint16_t node, unsigned char channel);
    bool node_has_entry(uint16_t node);
    OrderBuffer(void);

};

#endif // _ORDERBUFFER_H_
