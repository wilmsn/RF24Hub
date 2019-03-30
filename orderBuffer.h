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

struct sendBuffer_t {
	uint16_t 	    orderno;   	// the orderno as primary key for our message for the nodes
	uint64_t		entrytime;
	uint16_t     	node;
	uint16_t     	channel1;
	float        	value1;
	uint16_t     	channel2;
	float        	value2;
	uint16_t     	channel3;
	float        	value3;
	uint16_t     	channel4;
	float        	value4;
    sendBuffer_t   *next;
};

    sendBuffer_t    *iniSB_ptr;
    sendBuffer_t    *aktSB_ptr;
    orderBuffer_t   *iniOB_ptr;
    uint16_t        orderno;

    int delEntry(sendBuffer_t *sendBufferEntry);


    int delEntry(orderBuffer_t *orderBufferEntry);
    int delNodeChannel(uint16_t node, uint16_t channel);
    
public:
/*
 * Places a new order into the system
 * A new order will just be stored and NOT send to the node
 */
    int newOrder(uint16_t node, uint16_t channel, float value);
/*
 * Start sending all stored orders for this node
 */
    int send2Node(uint16_t node);
/*
 * Deletes old orders, older than the given entrytime
 */
    int delOldOrder(uint64_t entrytime);
/*
 * Delete a single ordernumber
 * ????? needed ??????
 */
    int delOrderNo(uint16_t orderno);
/*
 * list the content of the two buffers a) orderbuffer b) sendBuffer 
 * For the first record: pos = NULL
 * For the next record use the returnvalue of the last call
 * If the returnvalue is NULL than there 
 *
 */    
    void listOrder(int telnet_ptr, char msg_type);
 //   void* listOrderHTML(void);
 //   void * getInitialBuffer_ptr(void);
    ORDERBUFFER();

};

#endif // ORDERBUFFER_H
