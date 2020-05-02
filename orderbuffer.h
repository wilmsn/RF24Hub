/*


*/
#ifndef _ORDERBUFFER_H_   
#define _ORDERBUFFER_H_
#include <stdio.h> 
#include <iostream>
#include <stdint.h>
#include <cstring>
#include <unistd.h>
#include "common.h"
#include "buffer.h"
#include "rf24hub_config.h"
//#include "log.h"

extern uint16_t verboselevel;    

class OrderBuffer : public Buffer {

// Structure to handle the orderqueue
    
    
private:

struct orderbuffer_t {
        uint64_t		entrytime;
        uint8_t     	node_id;
        uint8_t     	channel;
        float        	value;
};
/*********************************************
 * Gibt einen Zeiger auf die grundlegende
 * Bufferstruktur zurück.
 * Um an die Daten des lokalen struct zu kommen
 * muss eine Umwandlung mit 
 *    (orderbuffer_t *)getDataPtr(p_result);
 * durchgeführt werden!
 ********************************************/
void* findNode(uint8_t node_id);

public:
    
    void addOrderBuffer(uint64_t millis, uint8_t node_id, uint8_t channel, float value);
    void* findOrder4Node(uint8_t node_id, void* last_ptr, uint8_t* channel, float* value);
    bool delByNodeChannel(uint8_t node_id, uint8_t channel);
    bool delByNode(uint8_t node_id);
    bool nodeHasEntry(uint8_t node_id);
    void printBuffer2tn(int new_tn_in_socket);
    void htmlBuffer2tn(int new_tn_in_socket);
    void printBuffer(uint16_t debuglevel);    
    OrderBuffer(void);

};

#endif // _ORDERBUFFER_H_
