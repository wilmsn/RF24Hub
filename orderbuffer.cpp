#include "orderbuffer.h"
#include <stdio.h> 
#include <iostream>

OrderBuffer::OrderBuffer(void) {
    initial_ptr = NULL;
}

void OrderBuffer::new_entry(OrderBuffer::orderbuffer_t* new_ptr) {
    OrderBuffer::orderbuffer_t *search_ptr;
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

bool OrderBuffer::del_entry(OrderBuffer::orderbuffer_t* my_ptr) {
    bool retval = false;
    OrderBuffer::orderbuffer_t *akt_ptr, *last_ptr;
    akt_ptr = OrderBuffer::initial_ptr;
    last_ptr = OrderBuffer::initial_ptr;
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

bool OrderBuffer::del_orderno(uint16_t orderno) {
    int retval = false;
    OrderBuffer::orderbuffer_t *search_ptr, *last_ptr;
    search_ptr = initial_ptr;
    while (search_ptr) {
        if (search_ptr->orderno == orderno ) {
            retval = OrderBuffer::del_entry(search_ptr);
        } else {
            last_ptr = search_ptr;
        }
        search_ptr=search_ptr->next;
    }
    return retval;
}

bool OrderBuffer::del_node_channel(uint16_t node, unsigned char channel) {
    int retval = false;
    OrderBuffer::orderbuffer_t *search_ptr, *last_ptr;
    search_ptr = initial_ptr;
    while (search_ptr) {
        if (search_ptr->node == node && search_ptr->channel == channel) {
            retval = OrderBuffer::del_entry(search_ptr);
        } else {
            last_ptr = search_ptr;
        }
        search_ptr=search_ptr->next;
    }
    return retval;
}

bool OrderBuffer::node_has_entry(uint16_t node) {
    int retval = false;
    OrderBuffer::orderbuffer_t *search_ptr, *last_ptr;
    search_ptr = initial_ptr;
    while (search_ptr) {
        if (search_ptr->node == node) {
            retval = true;
        }
        search_ptr=search_ptr->next;
    }
    return retval;
}
