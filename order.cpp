#include "order.h"
#include <stdio.h> 
#include <iostream>

Order::Order(void) {
    initial_ptr = NULL;
}

void Order::new_entry(order_t* new_ptr) {
    order_t *search_ptr;
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

bool Order::del_entry(order_t* my_ptr) {
    bool retval = false;
    order_t *search_ptr, *tmp1_ptr;
    search_ptr = initial_ptr;
    tmp1_ptr = initial_ptr;
    while (search_ptr) {
        if (search_ptr == my_ptr ) {
            if (search_ptr == initial_ptr) {
                if (initial_ptr->next) { 
                    tmp1_ptr=initial_ptr->next;
                    delete initial_ptr;
                    initial_ptr=tmp1_ptr;
                } else {
                    delete initial_ptr;
                    initial_ptr = NULL;
                }
            } else            {
                tmp1_ptr->next=search_ptr->next;
                delete search_ptr;
            }
            search_ptr = NULL;
            retval = true;
        } else {
            tmp1_ptr = search_ptr;
            search_ptr=search_ptr->next;
        }
    }
    return retval;
}

bool Order::del_orderno(uint16_t orderno) {
     int retval = false;
    order_t *search_ptr;
    search_ptr = initial_ptr;
    while (search_ptr) {
        if (search_ptr->orderno == orderno ) {
            if (del_entry(search_ptr)) retval = true;
        }
        search_ptr=search_ptr->next;
    }
    return retval;
}

bool Order::del_node(uint16_t node) {
    bool retval = false;
    order_t *search_ptr;
    search_ptr = Order::initial_ptr;
    while (search_ptr) {
        if (search_ptr->node == node ) {
            retval = Order::del_entry(search_ptr);
        }
        search_ptr=search_ptr->next;
    }
    return retval;
}
    
bool Order::find_orderno(uint16_t orderno) {
    int retval = false;
    order_t *search_ptr;
    search_ptr = initial_ptr;
    while (search_ptr) {
        if (search_ptr->orderno == orderno ) {
            retval = true;
        }
        search_ptr=search_ptr->next;
    }
    return retval;
}
    
void Order::debug_print_buffer(void) {
    order_t *search_ptr;
    search_ptr = initial_ptr;
    while (search_ptr) {
        search_ptr=search_ptr->next;
    }
}
