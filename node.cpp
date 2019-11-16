#include "node.h"
#include <stdio.h> 
#include <iostream>

Node::Node(void) {
    initial_ptr = NULL;
}

void Node::new_entry(Node::node_t* new_ptr) {
    Node::node_t *search_ptr;
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

