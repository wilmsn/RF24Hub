#include "order.h"

Order::Order(void) {
    initial_ptr = NULL;
    has_order = false;
}

void Order::new_entry(order_t* new_ptr) {
    order_t *search_ptr;
    new_ptr->next = NULL;
    char *debug =  (char*) malloc (DEBUGSTRINGSIZE);
    if (logger->verboselevel >= VERBOSEORDER) {
        sprintf(debug,"Order: new_entry %p", new_ptr); 
        logger->logmsg(VERBOSEORDER, debug);    
        sprintf(debug,"Bestand vorher"); 
        logger->logmsg(VERBOSEORDER, debug);
        debug_print_buffer();
    }
    if (initial_ptr) {
        search_ptr = initial_ptr;
        while (search_ptr->next) {
            search_ptr = search_ptr->next;
        }
        search_ptr->next = new_ptr;
    } else {
        initial_ptr = new_ptr;
    }
    if (logger->verboselevel >= VERBOSEORDER) {
        sprintf(debug,"Bestand nachher"); 
        logger->logmsg(VERBOSEORDER, debug);
        debug_print_buffer();
    }
    has_order = true;
    free(debug);
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
    if ( ! initial_ptr) has_order = false;
    return retval;
}

bool Order::del_orderno(uint16_t orderno) {
    bool retval = false;
    order_t *search_ptr;
    char *debug =  (char*) malloc (DEBUGSTRINGSIZE);
    if (logger->verboselevel >= VERBOSEORDER) {
        sprintf(debug,"Order: del_orderno %u", orderno); 
        logger->logmsg(VERBOSEORDER, debug);    
        sprintf(debug,"Bestand vorher"); 
        logger->logmsg(VERBOSEORDER, debug);
        debug_print_buffer();
    }
    search_ptr = initial_ptr;
    while (search_ptr) {
        if (search_ptr->orderno == orderno ) {
            if (del_entry(search_ptr)) retval = true;
        }
        search_ptr=search_ptr->next;
    }
    if (logger->verboselevel >= VERBOSEORDER) {
        sprintf(debug,"Bestand nachher"); 
        logger->logmsg(VERBOSEORDER, debug);
        debug_print_buffer();
    }
    free(debug);
    return retval;
}

bool Order::del_node(uint16_t node) {
    bool retval = false;
    order_t *search_ptr;
    char *debug =  (char*) malloc (DEBUGSTRINGSIZE);
    if (logger->verboselevel >= VERBOSEORDER) {
        sprintf(debug,"Order: del_node 0%o", node); 
        logger->logmsg(VERBOSEORDER, debug);    
        sprintf(debug,"Bestand vorher"); 
        logger->logmsg(VERBOSEORDER, debug);
        debug_print_buffer();
    }
    search_ptr = Order::initial_ptr;
    while (search_ptr) {
        if (search_ptr->node == node ) {
            retval = Order::del_entry(search_ptr);
        }
        search_ptr=search_ptr->next;
    }
    if (logger->verboselevel >= VERBOSEORDER) {
        sprintf(debug,"Bestand nachher"); 
        logger->logmsg(VERBOSEORDER, debug);
        debug_print_buffer();
    }
    free(debug);
    return retval;
}
    
bool Order::is_orderno(uint16_t orderno) {
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
    
Order::order_t* Order::find_node(uint16_t node) {
    order_t* retval = NULL;
    order_t *search_ptr;
    search_ptr = initial_ptr;
    while (search_ptr) {
        if (search_ptr->node == node ) {
            retval = search_ptr;
        }
        search_ptr=search_ptr->next;
    }
    return retval;
}

uint16_t Order::del_old_entry(uint64_t deltime) {
    uint16_t retval = 0;
    order_t *search_ptr, *del_ptr;
    search_ptr = initial_ptr;
    while (search_ptr) {
        if (search_ptr->entrytime < deltime ) {
            retval = search_ptr->node;
            del_ptr = search_ptr;
            search_ptr=search_ptr->next;
            del_entry(del_ptr);
        } else {
            search_ptr=search_ptr->next;
        }
    }
    return retval;
}

void Order::add_order(uint16_t node, uint8_t type, bool HB_order, uint8_t channel1, float value1, uint64_t entrytime) {
    order_t *new_ptr = new order_t;
    orderno++;
    if ( orderno > 50000 ) orderno = 1;
    new_ptr->orderno = orderno;
    new_ptr->node = node;
    new_ptr->type = type;
    new_ptr->flags = 0x00;
    new_ptr->HB_order = HB_order;
    new_ptr->channel1 = channel1;
    new_ptr->value1 = value1;
    new_ptr->channel2 = 0;
    new_ptr->value2 = 0;
    new_ptr->channel3 = 0;
    new_ptr->value3 = 0;
    new_ptr->channel4 = 0;
    new_ptr->value4 = 0;
    new_ptr->entrytime = entrytime;
    new_ptr->last_send = 0;
    new_entry(new_ptr);
}

void Order::add_endorder(uint16_t node, uint64_t entrytime) {
    add_order(node, 52, true, 0, 0, entrytime);    
    modify_orderflags(node, 0x01);
}

void Order::modify_order(uint16_t node, uint8_t pos, uint8_t channel, float value) {
    order_t* my_ptr=NULL;
    my_ptr=find_node(node);
    if (my_ptr) {
        if ( 2 == pos ) {
            my_ptr->channel2 = channel;
            my_ptr->value2 = value;
        }
        if ( 3 == pos ) {
            my_ptr->channel3 = channel;
            my_ptr->value3 = value;
        }
        if ( 4 == pos ) {
            my_ptr->channel4 = channel;
            my_ptr->value4 = value;
        }
    }
    debug_print_buffer();
}

void Order::modify_orderflags(uint16_t node, uint16_t flags) {
    order_t* my_ptr=NULL;
    my_ptr=find_node(node);
    if (my_ptr) {
        my_ptr->flags = flags;
    }
    debug_print_buffer();
}

bool Order::get_order_for_transmission(uint16_t* orderno, uint16_t* node, unsigned char* type, uint16_t* flags,
                                        uint8_t* channel1, float* value1, uint8_t* channel2, float* value2, 
                                        uint8_t* channel3, float* value3, uint8_t* channel4, float* value4, 
                                        uint64_t mytime) {
    bool retval = false;
    order_t *delme_ptr = NULL;
    order_t *search_ptr;
    search_ptr = initial_ptr;
    char *debug =  (char*) malloc (DEBUGSTRINGSIZE);
    while (search_ptr) {
        sprintf(debug,"### Order::get_order_for_transmission Node: 0%o Onr: %u Last send: %llu Now: %llu Entry: %llu",search_ptr->node, search_ptr->orderno, search_ptr->last_send, mytime, search_ptr->entrytime);        
        logger->logmsg(VERBOSEOTHER, debug);
        if (search_ptr->last_send > mytime) search_ptr->last_send = mytime;
        if ( ((!search_ptr->HB_order) && search_ptr->last_send + (uint64_t)SENDINTERVAL < mytime) ||
             ((search_ptr->HB_order) && search_ptr->last_send + (uint64_t)HB_SENDINTERVAL < mytime) ) {
            *orderno = search_ptr->orderno;
            *node = search_ptr->node;
            *type = search_ptr->type;
            *flags = search_ptr->flags;
            *channel1 = search_ptr->channel1;
            *value1 = search_ptr->value1;
            *channel2 = search_ptr->channel2;
            *value2 = search_ptr->value2;
            *channel3 = search_ptr->channel3;
            *value3 = search_ptr->value3;
            *channel4 = search_ptr->channel4;
            *value4 = search_ptr->value4;
            search_ptr->last_send = mytime;
            if ( search_ptr->HB_order ) {
                sprintf(debug,"Order: <%p> OrderNo: %u (Node:0%o HB), TTL: %llu", search_ptr, search_ptr->orderno, search_ptr->node, search_ptr->entrytime + (uint64_t)HB_DELETEINTERVAL - mytime ); 
                logger->logmsg(VERBOSEORDER, debug);
                if ( search_ptr->entrytime + (uint64_t)HB_DELETEINTERVAL < mytime ) {
                    delme_ptr = search_ptr;
                    sprintf(debug,"Order: Timeout - lösche <%p> OrderNo: %u (Node:0%o HB), entry:%llu last send: %llu Delinterv: %llu Sendinterv: %llu", delme_ptr, delme_ptr->orderno, delme_ptr->node, delme_ptr->entrytime, delme_ptr->last_send, (uint64_t)HB_DELETEINTERVAL, (uint64_t)HB_SENDINTERVAL ); 
                    logger->logmsg(VERBOSEORDER, debug);
                }
            } else {
                sprintf(debug,"Order: <%p> OrderNo: %u (Node:0%o), TTL: %llu", search_ptr, search_ptr->orderno, search_ptr->node, search_ptr->entrytime + (uint64_t)DELETEINTERVAL - mytime ); 
                logger->logmsg(VERBOSEORDER, debug);
                if ( search_ptr->entrytime + (uint64_t)DELETEINTERVAL < mytime ) {
                    delme_ptr = search_ptr;
                    sprintf(debug,"Order: Timeout - lösche <%p> OrderNo: %u (Node:0%o ), entry:%llu last send: %llu Delinterv: %llu Sendinterv: %llu", delme_ptr, delme_ptr->orderno, delme_ptr->node, delme_ptr->entrytime, delme_ptr->last_send, (uint64_t)DELETEINTERVAL, (uint64_t)SENDINTERVAL ); 
                    logger->logmsg(VERBOSEORDER, debug);
                }
            }                
            retval = true;
        }
        search_ptr = search_ptr->next;
    }
    if (delme_ptr) {
        del_entry(delme_ptr);
    }
    free(debug);
    return retval;
}

void Order::debug_print_buffer(void) {
    order_t *search_ptr;
    search_ptr = initial_ptr;
    char *debug =  (char*) malloc (DEBUGSTRINGSIZE);
    sprintf(debug,"Order: ---- Buffercontent ----"); 
    logger->logmsg(VERBOSEORDER, debug);
    while (search_ptr) {
        sprintf(debug,"Order: <%p> O:%u N:0%o F:%02x (%u:%f) (%u:%f) (%u:%f) (%u:%f)", 
                search_ptr, search_ptr->orderno, search_ptr->node, search_ptr->flags, 
                search_ptr->channel1, search_ptr->value1, search_ptr->channel2, search_ptr->value2,
                search_ptr->channel3, search_ptr->value3, search_ptr->channel4, search_ptr->value4 );
        logger->logmsg(VERBOSEORDER, debug);
        search_ptr=search_ptr->next;
    }
    sprintf(debug,"Order: -- END Buffercontent --"); 
    logger->logmsg(VERBOSEORDER, debug);
}

void Order::print_buffer(int new_tn_in_socket) {
    order_t *search_ptr;
    char *client_message =  (char*) malloc (TELNETBUFFERSIZE);
    search_ptr = initial_ptr;
    sprintf(client_message,"------ Order: --------\n"); 
    write(new_tn_in_socket , client_message , strlen(client_message));
    while (search_ptr) {
        sprintf(client_message,"<%p> OrderNo:%u Node:0%o (%u:%f) (%u:%f) (%u:%f) (%u:%f)\n", search_ptr, search_ptr->orderno, search_ptr->node, 
        search_ptr->channel1, search_ptr->value1, search_ptr->channel2, search_ptr->value2,
        search_ptr->channel3, search_ptr->value3, search_ptr->channel4, search_ptr->value4 );
        write(new_tn_in_socket , client_message , strlen(client_message));
        search_ptr=search_ptr->next;
    }
    free(client_message);
    debug_print_buffer();
}

void Order::html_buffer(int new_tn_in_socket) {
    order_t *search_ptr;
    char *client_message =  (char*) malloc (TELNETBUFFERSIZE);
    search_ptr = initial_ptr;
	sprintf(client_message,"</table><br><big>Order</big><br><table><tr><th>OrderNo</th><th>Node</th><th>Type</th><th>Flags</th><th>Channel</th><th>Value</th></tr>\n"); 
    write(new_tn_in_socket , client_message , strlen(client_message));
    while (search_ptr) {
        sprintf(client_message,"<tr><td>%u</td><td>0%o</td><td>%u</td><td>%u</td><td>%u<br>%u<br>%u<br>%u</td><td>%f<br>%f<br>%f<br>%f</td></tr>\n", 
        search_ptr->orderno, search_ptr->node, 
        search_ptr->channel1, search_ptr->value1, search_ptr->channel2, search_ptr->value2,
        search_ptr->channel3, search_ptr->value3, search_ptr->channel4, search_ptr->value4 );
        write(new_tn_in_socket , client_message , strlen(client_message));
        search_ptr=search_ptr->next;
    }
	sprintf(client_message,"</table></center>\n"); 
	write(new_tn_in_socket , client_message , strlen(client_message));
    free(client_message);
}

void Order::begin(Logger* _logger) {
    logger = _logger;
    orderno = 1;
}
