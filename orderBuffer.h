/*


*/
#ifndef ORDERBUFFER_H   
#define ORDERBUFFER_H
#include <stdint.h>

class ORDERBUFFER {

    
private:
    
struct orderBuffer_t {
	uint16_t 	    orderno;   	// the orderno as primary key for our message for the nodes
	uint64_t		entrytime;
	uint16_t     	node;
	uint16_t     	channel;
	float        	value;
    orderBuffer_t   *next;
};

    orderBuffer_t *initialBuffer_ptr;

    int delEntry(orderBuffer_t *orderBufferEntry);
    int delNodeChannel(uint16_t node, uint16_t channel);
    
public:
    int newOrder(uint16_t node, uint16_t channel, float value);
    int delOldOrder(uint64_t entrytime);
    int delOrderNo(uint16_t orderno);
    int setOrderNo(uint16_t orderno, uint16_t node, uint16_t channel);
    void* listOrder(void * pos, char * msg);
    void listOrderHTML(void);
    void * getInitialBuffer_ptr(void);
    ORDERBUFFER();

};

#endif // ORDERBUFFER_H
