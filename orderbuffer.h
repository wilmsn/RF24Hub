/*


*/
#ifndef _ORDERBUFFER_H_   
#define _ORDERBUFFER_H_
#include "rf24hub_config.h"
#include <stdint.h>
#include <cstring>
#include <unistd.h>
#include "log.h"

class OrderBuffer {

// Structure to handle the orderqueue
    
    
private:
    Logger* logger;

public:
    
struct orderbuffer_t {
	uint16_t 	    orderno;   	// the orderno as primary key for our message for the nodes
	uint64_t		entrytime;
	uint16_t     	node;
	uint8_t     	channel;
	float        	value;
	orderbuffer_t*  next;          // poiter to the next record
};

    void new_entry(orderbuffer_t*);
    bool del_entry(orderbuffer_t*);
    void debug_print_buffer(void);

    orderbuffer_t* initial_ptr;
    void add_orderbuffer(uint32_t orderno, uint64_t millis, uint16_t node, uint8_t channel, float value);
    bool find_orderno(uint16_t);
    bool del_orderno(uint16_t);
    bool del_node_channel(uint16_t, uint8_t);
    bool del_node(uint16_t);
    bool node_has_entry(uint16_t);
    void begin(Logger* _logger);
    void print_buffer(int new_tn_in_socket);
    void html_buffer(int new_tn_in_socket);
    OrderBuffer(void);

};

#endif // _ORDERBUFFER_H_
