#include "orderbuffer.h"
#include <stdio.h> 
#include <iostream>

OrderBuffer::OrderBuffer(void) {
    initial_ptr = NULL;
}

void OrderBuffer::new_entry(OrderBuffer::orderbuffer_t* new_ptr) {
    orderbuffer_t *search_ptr;
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
    orderbuffer_t *search_ptr, *tmp1_ptr;
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

bool OrderBuffer::find_orderno(uint16_t orderno) {
    int retval = false;
    orderbuffer_t *search_ptr;
    search_ptr = initial_ptr;
    while (search_ptr) {
        if (search_ptr->orderno == orderno ) {
            retval = true;
        }
        search_ptr=search_ptr->next;
    }
    return retval;
}

bool OrderBuffer::del_orderno(uint16_t orderno) {
    int retval = false;
    orderbuffer_t *search_ptr;
    search_ptr = initial_ptr;
    while (search_ptr) {
        if (search_ptr->orderno == orderno ) {
            if (del_entry(search_ptr)) retval = true;
        }
        search_ptr=search_ptr->next;
    }
    return retval;
}

bool OrderBuffer::del_node_channel(uint16_t node, uint8_t channel) {
    int retval = false;
    orderbuffer_t *search_ptr;
    search_ptr = initial_ptr;
    while (search_ptr) {
        if (search_ptr->node == node && search_ptr->channel == channel) {
            if (del_entry(search_ptr)) retval = true;
        }
        search_ptr=search_ptr->next;
    }
    return retval;
}

bool OrderBuffer::node_has_entry(uint16_t node) {
    int retval = false;
    orderbuffer_t *search_ptr;
    search_ptr = initial_ptr;
    while (search_ptr) {
        if (search_ptr->node == node) {
            retval = true;
        }
        search_ptr=search_ptr->next;
    }
    return retval;
}

void OrderBuffer::debug_print_buffer(void) {
    orderbuffer_t *search_ptr;
    search_ptr = initial_ptr;
    while (search_ptr) {
        search_ptr=search_ptr->next;
    }
}


