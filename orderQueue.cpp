#include "orderQueue.h"
#include <string.h>
#include <stdio.h> 
#include <iostream>

/*
 * Constructor
 * Hier wird ein Pointer für den Start der verketteten Liste angelegt.
 */
ORDERQUEUE::ORDERQUEUE(void)
{
    initial_orderQueue_ptr = NULL;
    numOrdersWaiting = 0;
}

int ORDERQUEUE::newEntry()
{
    orderQueue_t *akt_orderQueue_ptr, *new_orderQueue_ptr;
    new_orderQueue_ptr = new orderQueue_t;
    if (! initial_orderQueue_ptr) {
        initial_orderQueue_ptr = new_orderQueue_ptr;
        akt_orderQueue_ptr = new_orderQueue_ptr;
    } else {
        akt_orderQueue_ptr = initial_orderQueue_ptr;
        while ( akt_orderQueue_ptr->next ) akt_orderQueue_ptr = akt_orderQueue_ptr->next;
        akt_orderQueue_ptr->next = new_orderQueue_ptr;
        akt_orderQueue_ptr = new_orderQueue_ptr;
    }

    numOrdersWaiting++; 
    return 1;
}

