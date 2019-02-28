/*


*/
#ifndef _ORDERQUEUE_H_   
#define _ORDERQUEUE_H_
#include <stdint.h>
#include "rf24hub_config.h"

class orderQueue {

    
private:
    
struct orderQueue_t {
    orderQueue_t  *next;
};

    orderQueue_t *initial_orderQueue_ptr;
    int numOrdersWaiting;
    
public:
    int newEntry();
    void listOrders(void);
    int ordersWaiting(void);
    orderQueue(void);

};

#endif // _ORDERQUEUE_H_
