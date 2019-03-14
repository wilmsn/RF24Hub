/*


*/
#ifndef _ORDERQUEUE_H_   
#define _ORDERQUEUE_H_
#include <stdint.h>
#include "rf24hub_config.h"

class ORDERQUEUE {

    
private:
    
struct orderQueue_t {
    orderQueue_t  *next;
};

    orderQueue_t *initial_orderQueue_ptr;
    
public:
    int numOrdersWaiting;

    ORDERQUEUE(void);
    
    int newEntry();
    void listOrders(void);

};

#endif // _ORDERQUEUE_H_
