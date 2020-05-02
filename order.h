/*


*/
#ifndef _ORDER_H_   
#define _ORDER_H_
#include <stdint.h>
#include <cstring>
#include <unistd.h>
#include <stdio.h> 
#include <iostream>
#include "rf24hub_config.h"
#include "rf24_config.h"
#include "common.h"
#include "buffer.h"
#include "dataformat.h"

extern uint16_t verboselevel;   

class Order : public Buffer{
    
private:

    // Structure to handle the orderqueue
    struct order_t {
        uint8_t                node_id;   		// the destination node
        uint8_t                msg_id;          // ==> payload.msg_id
        uint8_t                msg_type;      	// ==> payload.msg_type
        uint8_t   	           msg_flags;     	// ==> payload.msg_flags
        uint8_t 	           orderno;   		// ==> payload.orderno
        uint32_t      	       data1;		    // The transportvalue for the Sensor 1 and Value 1
        uint32_t      	       data2;		    // The transportvalue for the Sensor 2 and Value 2
        uint32_t      	       data3;		    // The transportvalue for the Sensor 3 and Value 3
        uint32_t      	       data4;		    // The transportvalue for the Sensor 4 and Value 4
        uint32_t      	       data5;		    // The transportvalue for the Sensor 5 and Value 5
        uint32_t      	       data6;		    // The transportvalue for the Sensor 6 and Value 6
        bool                   HB_order;        // true if this node is a Heartbeat Node
        uint64_t		       entrytime;       // Timestamp for creating of this record
        uint64_t	  	       last_send;		// Timestamp for last sending of this record
    };

/*********************************************
 * find*: (gilt für beide Funktionen!)
 * Gibt einen Zeiger auf die grundlegende
 * Bufferstruktur zurück.
 * Um an die Daten des lokalen struct zu kommen
 * muss eine Umwandlung mit 
 *    (order_t *)getDataPtr(p_result);
 * durchgeführt werden!
 ********************************************/
void* findOrderNo(uint8_t orderno); 
void* findNode(uint8_t node_id); 

public:
    
    uint8_t orderno;
    bool hasOrder;

    bool isOrderNo(uint8_t orderno);
    bool delByOrderNo(uint8_t orderno);
    bool delByNode(uint8_t node_id);
//    uint8_t delByTime(uint64_t entrytime);
    void addOrder(uint8_t node_id, uint8_t msg_type, bool HB_order, uint32_t data, uint64_t entrytime);
    void modifyOrder(uint8_t node_id, uint8_t pos, uint32_t data);
    void modifyOrderFlags(uint8_t node_id, uint8_t msg_flags);
    void addEndOrder(uint8_t node_id, uint8_t msg_type, uint64_t entrytime);
    bool getOrderForTransmission(payload_t* payload, uint64_t mytime); 
    void printBuffer2tn(int new_tn_in_socket);
    void htmlBuffer2tn(int new_tn_in_socket);
    void printBuffer(uint16_t verboselevel);    
    Order(void);


};

#endif // _ORDER_H_
