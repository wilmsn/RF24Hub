/*


*/
#ifndef _ORDERBUFFER_H_   
#define _ORDERBUFFER_H_
#include <stdio.h> 
#include <iostream>
#include <stdint.h>
#include <cstring>
#include <unistd.h>
#include "rf24_config.h"
#include "common.h"
#include "dataformat.h"

extern uint16_t verboselevel;   

class OrderBuffer {

// Structure to handle the orderqueue
    
    
private:

    struct orderbuffer_t {
        uint64_t		entrytime;
        uint8_t     	node_id;
        uint8_t     	channel;
        float        	value;
        orderbuffer_t*  p_next;          // poiter to the p_next record
    };

    orderbuffer_t* p_initial;
    void newEntry(orderbuffer_t*);
    bool delEntry(orderbuffer_t*);
    orderbuffer_t* findNode(uint8_t node_id);

public:
    
    void* findOrder4Node(uint8_t node_id, void* p_last, uint8_t* channel, float* value);
    void addOrderBuffer(uint64_t millis, uint8_t node_id, uint8_t channel, float value);
    bool delByNodeChannel(uint8_t node_id, uint8_t channel);
    bool delByNode(uint8_t node_id);
    bool nodeHasEntry(uint8_t node_id);
    void printBuffer2tn(int new_tn_in_socket);
    void htmlBuffer2tn(int new_tn_in_socket);
    void printBuffer(uint16_t debuglevel);    
    OrderBuffer(void);

};

#endif // _ORDERBUFFER_H_
