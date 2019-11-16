#include "order.h"
#include <stdio.h> 
#include <iostream>

Order::Order(void) {
    initial_ptr = NULL;
}

void Order::new_entry(Order::order_t* new_ptr) {
    Order::order_t *search_ptr;
    if (initial_ptr) {
        search_ptr = initial_ptr;
        while (search_ptr->next) {
            search_ptr = search_ptr->next;
        }
        search_ptr->next = new_ptr;
    } else {
        initial_ptr = new_ptr;
    }
}

bool Order::del_orderno(uint16_t orderno) {
    bool retval = false;
    Order::order_t *akt_ptr, *last_ptr;
    akt_ptr = Order::initial_ptr;
    while (akt_ptr) {
        if (akt_ptr->orderno != orderno ) {
            last_ptr = akt_ptr;
            akt_ptr=akt_ptr->next;
        } else {
            if (akt_ptr == initial_ptr) {
                initial_ptr=akt_ptr->next;
            } else            {
                last_ptr->next=akt_ptr->next;
            }
            delete akt_ptr;
            retval = true;
        }
    }
    return retval;
}

bool Order::del_entry(Order::order_t* my_ptr) {
    bool retval = false;
    Order::order_t *akt_ptr, *last_ptr;
    akt_ptr = Order::initial_ptr;
    while (akt_ptr) {
        if (akt_ptr == my_ptr ) {
            if (akt_ptr == initial_ptr) {
                if (initial_ptr->next) { 
                    initial_ptr=akt_ptr->next;
                } else {
                    initial_ptr = NULL;
                }
            } else            {
                last_ptr->next=akt_ptr->next;
            }
            delete akt_ptr;
            akt_ptr = NULL;
            retval = true;
        } else {
            last_ptr = akt_ptr;
            akt_ptr=akt_ptr->next;
        }
    }
    return retval;
}

bool Order::del_node(uint16_t node) {
    bool retval = false;
    Order::order_t *akt_ptr, *last_ptr;
    akt_ptr = Order::initial_ptr;
    while (akt_ptr) {
        if (akt_ptr->node == node ) {
            retval = Order::del_entry(akt_ptr);
        } else {
            last_ptr = akt_ptr;
        }
        akt_ptr=akt_ptr->next;
    }
    return retval;
}
    
    
