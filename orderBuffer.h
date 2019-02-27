/*


*/
#ifndef _ORDERBUFFER_H_   
#define _ORDERBUFFER_H_
#include <stdint.h>

class orderBuffer {

    
private:
    
struct orderBuffer_t {
	uint16_t 	    orderno;   	// the orderno as primary key for our message for the nodes
	uint64_t		entrytime;
	uint16_t     	node;
	uint16_t     	channel;
	float        	value;
    orderBuffer_t   *next;
};

    orderBuffer_t *initial_orderBuffer_ptr;

    int delEntry(orderBuffer_t *orderBufferEntry);
    int delNodeChannel(uint16_t node, uint16_t channel);
    
public:
    int newOrder(uint16_t node, uint16_t channel, float value, uint64_t entrytime);
    int delOldOrder(uint64_t entrytime);
    int delOrderNo(uint16_t orderno);
    int setOrderNo(uint16_t orderno, uint16_t node, uint16_t channel);
    void listOrder(void);
    void listOrderHTML(void);
    orderBuffer(void);

};

#endif // _ORDERBUFFER_H_
