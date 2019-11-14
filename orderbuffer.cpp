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

bool OrderBuffer::del_orderno(uint16_t orderno) {
    int retval = false;
    OrderBuffer::orderbuffer_t *search_ptr, *last_ptr;
    search_ptr = initial_ptr;
    while (search_ptr) {
        if (search_ptr->orderno != orderno ) {
            last_ptr = search_ptr;
            search_ptr=search_ptr->next;
        } else {
            if (search_ptr == initial_ptr) {
                initial_ptr=search_ptr->next;
            } else {
                last_ptr->next=search_ptr->next;
            }
            delete search_ptr;
            retval = true;
        }
    }
    return retval;
}
