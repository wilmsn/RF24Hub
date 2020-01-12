#include "order.h"

Order::Order(void) {
    initial_ptr = NULL;
    has_order = false;
}

void Order::new_entry(order_t* new_ptr) {
    order_t *search_ptr;
    new_ptr->next = NULL;
    char *debug =  (char*) malloc (DEBUGSTRINGSIZE);
    if (logger->verboselevel & VERBOSEORDER) {
        sprintf(debug,"Order: new_entry %p", new_ptr); 
        logger->logmsg(VERBOSEORDER, debug);    
        sprintf(debug,"Bestand vorher"); 
        logger->logmsg(VERBOSEORDER, debug);
        debug_print_buffer(VERBOSEORDER);
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
    if (logger->verboselevel & VERBOSEORDER) {
        sprintf(debug,"Bestand nachher"); 
        logger->logmsg(VERBOSEORDER, debug);
        debug_print_buffer(VERBOSEORDER);
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

bool Order::del_orderno(uint8_t orderno) {
    bool retval = false;
    order_t *search_ptr;
    char *debug =  (char*) malloc (DEBUGSTRINGSIZE);
    if (logger->verboselevel & VERBOSEORDER) {
        sprintf(debug,"Order: del_orderno %u", orderno); 
        logger->logmsg(VERBOSEORDER, debug);    
        sprintf(debug,"Bestand vorher"); 
        logger->logmsg(VERBOSEORDER, debug);
        debug_print_buffer(VERBOSEORDER);
    }
    search_ptr = initial_ptr;
    while (search_ptr) {
        if (search_ptr->orderno == orderno ) {
            if (del_entry(search_ptr)) retval = true;
        }
        search_ptr=search_ptr->next;
    }
    if (logger->verboselevel & VERBOSEORDER) {
        sprintf(debug,"Bestand nachher"); 
        logger->logmsg(VERBOSEORDER, debug);
        debug_print_buffer(VERBOSEORDER);
    }
    free(debug);
    return retval;
}

bool Order::del_node(uint16_t node) {
    bool retval = false;
    order_t *search_ptr;
    char *debug =  (char*) malloc (DEBUGSTRINGSIZE);
    if (logger->verboselevel & VERBOSEORDER) {
        sprintf(debug,"Order: del_node %u", node); 
        logger->logmsg(VERBOSEORDER, debug);    
        sprintf(debug,"Bestand vorher"); 
        logger->logmsg(VERBOSEORDER, debug);
        debug_print_buffer(VERBOSEORDER);
    }
    search_ptr = Order::initial_ptr;
    while (search_ptr) {
        if (search_ptr->node == node ) {
            retval = Order::del_entry(search_ptr);
        }
        search_ptr=search_ptr->next;
    }
    if (logger->verboselevel & VERBOSEORDER) {
        sprintf(debug,"Bestand nachher"); 
        logger->logmsg(VERBOSEORDER, debug);
        debug_print_buffer(VERBOSEORDER);
    }
    free(debug);
    return retval;
}
    
bool Order::is_orderno(uint8_t orderno) {
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

void Order::add_order(uint16_t node, uint8_t type, bool HB_order, uint32_t data, uint64_t entrytime) {
    order_t *new_ptr = new order_t;
    orderno++;
//    if ( orderno > 50000 ) orderno = 1;
    new_ptr->orderno = orderno;
    new_ptr->node = node;
    new_ptr->type = type;
    new_ptr->flags = 0x00;
    new_ptr->HB_order = HB_order;
    new_ptr->data1 = data;
    new_ptr->data2 = 0;
    new_ptr->data3 = 0;
    new_ptr->data4 = 0;
    new_ptr->data5 = 0;
    new_ptr->data6 = 0;
    new_ptr->entrytime = entrytime;
    new_ptr->last_send = 0;
    new_entry(new_ptr);
}

void Order::add_endorder(uint16_t node, uint8_t msg_type, uint64_t entrytime) {
    add_order(node, msg_type, true, 0, entrytime);    
    modify_orderflags(node, 0x01);
}

void Order::modify_order(uint16_t node, uint8_t pos, uint32_t data) {
    order_t* my_ptr=NULL;
    my_ptr=find_node(node);
    if (my_ptr) {
        switch (pos) {
            case 2:
                my_ptr->data2 = data;
            break;
            case 3:
                my_ptr->data3 = data;
            break;
            case 4:
                my_ptr->data4 = data;
            break;
            case 5:
                my_ptr->data5 = data;
            break;
            case 6:
                my_ptr->data6 = data;
            break;
        }
    }
    debug_print_buffer(VERBOSEORDER);
}

void Order::modify_orderflags(uint16_t node, uint8_t flags) {
    order_t* my_ptr=NULL;
    my_ptr=find_node(node);
    if (my_ptr) {
        my_ptr->flags = flags;
    }
    if (logger->verboselevel & VERBOSEORDER) {
        debug_print_buffer(VERBOSEORDER);
    }
}

bool Order::get_order_for_transmission(uint8_t* orderno, uint8_t* node, uint8_t* type, uint8_t* flags,
                uint32_t* data1, uint32_t* data2, uint32_t* data3, uint32_t* data4, uint32_t* data5, uint32_t* data6, uint64_t mytime){
    bool retval = false;
    order_t *delme_ptr = NULL;
    order_t *search_ptr;
    search_ptr = initial_ptr;
    char *debug =  (char*) malloc (DEBUGSTRINGSIZE);
    while (search_ptr) {
        if (logger->verboselevel & VERBOSEOTHER) {
            sprintf(debug,"### Order::get_order_for_transmission Node: %u Onr: %u Last send: %llu Now: %llu Entry: %llu",search_ptr->node, search_ptr->orderno, search_ptr->last_send, mytime, search_ptr->entrytime);        
            logger->logmsg(VERBOSEOTHER, debug);
        }
        if (search_ptr->last_send > mytime) search_ptr->last_send = mytime;
        if ( ((!search_ptr->HB_order) && search_ptr->last_send + (uint64_t)SENDINTERVAL < mytime) ||
             ((search_ptr->HB_order) && search_ptr->last_send + (uint64_t)HB_SENDINTERVAL < mytime) ) {
            *orderno = search_ptr->orderno;
            *node = search_ptr->node;
            *type = search_ptr->type;
            *flags = search_ptr->flags;
            *data1 = search_ptr->data1;
            *data2 = search_ptr->data2;
            *data3 = search_ptr->data3;
            *data4 = search_ptr->data4;
            *data5 = search_ptr->data5;
            *data6 = search_ptr->data6;
            search_ptr->last_send = mytime;
            if ( search_ptr->HB_order ) {
                if (logger->verboselevel & VERBOSEORDER) {
                    sprintf(debug,"Order: <%p> OrderNo: %u (Node:%u HB), TTL: %llu", search_ptr, search_ptr->orderno, search_ptr->node, search_ptr->entrytime + (uint64_t)HB_DELETEINTERVAL - mytime ); 
                    logger->logmsg(VERBOSEORDER, debug);
                }
                if ( search_ptr->entrytime + (uint64_t)HB_DELETEINTERVAL < mytime ) {
                    delme_ptr = search_ptr;
                    if (logger->verboselevel & VERBOSEORDER) {
                        sprintf(debug,"Order: Timeout - lösche <%p> OrderNo: %u (Node:%u HB), entry:%llu last send: %llu Delinterv: %llu Sendinterv: %llu", delme_ptr, delme_ptr->orderno, delme_ptr->node, delme_ptr->entrytime, delme_ptr->last_send, (uint64_t)HB_DELETEINTERVAL, (uint64_t)HB_SENDINTERVAL ); 
                        logger->logmsg(VERBOSEORDER, debug);
                    }
                }
            } else {
                if (logger->verboselevel & VERBOSEORDER) {
                    sprintf(debug,"Order: <%p> OrderNo: %u (Node:%u), TTL: %llu", search_ptr, search_ptr->orderno, search_ptr->node, search_ptr->entrytime + (uint64_t)DELETEINTERVAL - mytime ); 
                    logger->logmsg(VERBOSEORDER, debug);
                }
                if ( search_ptr->entrytime + (uint64_t)DELETEINTERVAL < mytime ) {
                    delme_ptr = search_ptr;
                    if (logger->verboselevel & VERBOSEORDER) {
                        sprintf(debug,"Order: Timeout - lösche <%p> OrderNo: %u (Node:%u ), entry:%llu last send: %llu Delinterv: %llu Sendinterv: %llu", delme_ptr, delme_ptr->orderno, delme_ptr->node, delme_ptr->entrytime, delme_ptr->last_send, (uint64_t)DELETEINTERVAL, (uint64_t)SENDINTERVAL ); 
                        logger->logmsg(VERBOSEORDER, debug);
                    }
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

void Order::debug_print_buffer(uint16_t debuglevel) {
    order_t *search_ptr;
    search_ptr = initial_ptr;
    char *debug =  (char*) malloc (DEBUGSTRINGSIZE);
    sprintf(debug,"Order: ---- Buffercontent ----"); 
    logger->logmsg(debuglevel, debug);
    while (search_ptr) {
        sprintf(debug,"Order: <%p> O:%u N:%u T:%u F:%02x (%u/%g) (%u/%g) (%u/%g) (%u/%g) (%u/%g) (%u/%g)", 
                search_ptr, search_ptr->orderno, search_ptr->node, search_ptr->type, search_ptr->flags 
                ,getChannel(search_ptr->data1), getValue_f(search_ptr->data1)
                ,getChannel(search_ptr->data2), getValue_f(search_ptr->data2)
                ,getChannel(search_ptr->data3), getValue_f(search_ptr->data3)
                ,getChannel(search_ptr->data4), getValue_f(search_ptr->data4)
                ,getChannel(search_ptr->data5), getValue_f(search_ptr->data5)
                ,getChannel(search_ptr->data6), getValue_f(search_ptr->data6)
               );
        logger->logmsg(debuglevel, debug);
        search_ptr=search_ptr->next;
    }
    sprintf(debug,"Order: -- END Buffercontent --"); 
    logger->logmsg(debuglevel, debug);
}

void Order::print_buffer(int new_tn_in_socket) {
    order_t *search_ptr;
    char *client_message =  (char*) malloc (TELNETBUFFERSIZE);
    search_ptr = initial_ptr;
    sprintf(client_message,"------ Order: --------\n"); 
    write(new_tn_in_socket , client_message , strlen(client_message));
    while (search_ptr) {
        sprintf(client_message,"Order: <%p> O:%u N:%u T:%u F:%02x (%u/%g) (%u/%g) (%u/%g) (%u/%g) (%u/%g) (%u/%g)", 
                search_ptr, search_ptr->orderno, search_ptr->node, search_ptr->type, search_ptr->flags 
                ,getChannel(search_ptr->data1), getValue_f(search_ptr->data1)
                ,getChannel(search_ptr->data2), getValue_f(search_ptr->data2)
                ,getChannel(search_ptr->data3), getValue_f(search_ptr->data3)
                ,getChannel(search_ptr->data4), getValue_f(search_ptr->data4)
                ,getChannel(search_ptr->data5), getValue_f(search_ptr->data5)
                ,getChannel(search_ptr->data6), getValue_f(search_ptr->data6)
               );
        search_ptr=search_ptr->next;
    }
    free(client_message);
    debug_print_buffer(VERBOSEORDER);
}

void Order::html_buffer(int new_tn_in_socket) {
    order_t *search_ptr;
    char *client_message =  (char*) malloc (TELNETBUFFERSIZE);
    search_ptr = initial_ptr;
	sprintf(client_message,"</table><br><big>Order</big><br><table><tr><th>OrderNo</th><th>Node</th><th>Type</th><th>Flags</th><th>Channel</th><th>Value</th></tr>\n"); 
    write(new_tn_in_socket , client_message , strlen(client_message));
    while (search_ptr) {
        sprintf(client_message,"<tr><td>%u</td><td>%u</td><td>%u</td><td>%u</td><td>%u<br>%u<br>%u<br>%u<br>%u<br>%u</td><td>%g<br>%g<br>%g<br>%g<br>%g<br>%g</td></tr>\n", 
        search_ptr->orderno, search_ptr->node, 
        getChannel(search_ptr->data1), 
        getChannel(search_ptr->data2), 
        getChannel(search_ptr->data3), 
        getChannel(search_ptr->data4), 
        getChannel(search_ptr->data5), 
        getChannel(search_ptr->data6),  
        getValue_f(search_ptr->data1), 
        getValue_f(search_ptr->data2),
        getValue_f(search_ptr->data3),
        getValue_f(search_ptr->data4),
        getValue_f(search_ptr->data5),
        getValue_f(search_ptr->data6)  );
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
