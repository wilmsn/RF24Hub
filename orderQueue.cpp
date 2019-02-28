#include "orderQueue.h"
#include <string.h>
#include <stdio.h> 
#include <iostream>

/*
 * Constructor
 * Hier wird ein Pointer für den Start der verketteten Liste angelegt.
 */
orderQueue::orderQueue(void)
{
    initial_orderQueue_ptr = NULL;
    numOrdersWaiting = 0;
}

int orderQueue::newEntry()
{
    orderQueue::orderQueue_t *akt_orderQueue_ptr, *new_orderQueue_ptr;
    new_orderQueue_ptr = new orderQueue::orderQueue_t;
    if (! orderQueue::initial_orderQueue_ptr) {
        orderQueue::initial_orderQueue_ptr = new_orderQueue_ptr;
        akt_orderQueue_ptr = new_orderQueue_ptr;
    } else {
        akt_orderQueue_ptr = orderQueue::initial_orderQueue_ptr;
        while ( akt_orderQueue_ptr->next ) akt_orderQueue_ptr = akt_orderQueue_ptr->next;
        akt_orderQueue_ptr->next = new_orderQueue_ptr;
        akt_orderQueue_ptr = new_orderQueue_ptr;
    }

    orderQueue::numOrdersWaiting++; 
    return 1;
}

int orderQueue::ordersWaiting(void)
{
    return numOrdersWaiting;
}
