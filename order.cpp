#include "order.h"
#include <stdio.h> 
#include <iostream>

Order::Order(void) {
    initial_ptr = NULL;
    has_order = false;
}

void Order::new_entry(order_t* new_ptr) {
    order_t *search_ptr;
    new_ptr->next = NULL;
    if (logger->verboselevel >= VERBOSEORDER) {
        sprintf(logger->debug,"Order: new_entry %p", new_ptr); 
        logger->logmsg(VERBOSEORDER, logger->debug);    
        sprintf(logger->debug,"Bestand vorher"); 
        logger->logmsg(VERBOSEORDER, logger->debug);
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
        sprintf(logger->debug,"Bestand nachher"); 
        logger->logmsg(VERBOSEORDER, logger->debug);
        debug_print_buffer();
    }
    has_order = true;
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
    has_order = (initial_ptr);
}

bool Order::del_orderno(uint16_t orderno) {
    bool retval = false;
    order_t *search_ptr;
    if (logger->verboselevel >= VERBOSEORDER) {
        sprintf(logger->debug,"Order: del_orderno %u", orderno); 
        logger->logmsg(VERBOSEORDER, logger->debug);    
        sprintf(logger->debug,"Bestand vorher"); 
        logger->logmsg(VERBOSEORDER, logger->debug);
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
        sprintf(logger->debug,"Bestand nachher"); 
        logger->logmsg(VERBOSEORDER, logger->debug);
        debug_print_buffer();
    }
    return retval;
}

bool Order::del_node(uint16_t node) {
    bool retval = false;
    order_t *search_ptr;
    if (logger->verboselevel >= VERBOSEORDER) {
        sprintf(logger->debug,"Order: del_node 0%o", node); 
        logger->logmsg(VERBOSEORDER, logger->debug);    
        sprintf(logger->debug,"Bestand vorher"); 
        logger->logmsg(VERBOSEORDER, logger->debug);
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
        sprintf(logger->debug,"Bestand nachher"); 
        logger->logmsg(VERBOSEORDER, logger->debug);
        debug_print_buffer();
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

void Order::add_order(uint16_t orderno, uint16_t node, uint8_t type, unsigned int flags,
                      uint8_t channel1, float value1, uint8_t channel2, float value2, 
                      uint8_t channel3, float value3, uint8_t channel4, float value4, 
                      uint64_t entrytime) {
    order_t *new_ptr = new order_t;
    new_ptr->orderno = orderno;
    new_ptr->node = node;
    new_ptr->type = type;
    new_ptr->flags = flags;
    new_ptr->channel1 = channel1;
    new_ptr->value1 = value1;
    new_ptr->channel2 = channel2;
    new_ptr->value2 = value2;
    new_ptr->channel3 = channel3;
    new_ptr->value3 = value3;
    new_ptr->channel4 = channel4;
    new_ptr->value4 = value4;
    new_ptr->entrytime = entrytime;
    new_entry(new_ptr);
}

bool Order::get_order_for_transmission(uint16_t* orderno, uint16_t* node, unsigned char* type, uint16_t* flags,
                                        uint8_t* channel1, float* value1, uint8_t* channel2, float* value2, 
                                        uint8_t* channel3, float* value3, uint8_t* channel4, float* value4, 
                                        uint64_t systime) {
    bool retval = false;
    order_t *search_ptr;
    search_ptr = initial_ptr;
    while (search_ptr) {
        if (search_ptr->entrytime < systime - SENDINTERVAL ) {
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
            search_ptr->entrytime = systime;
            retval = true;
        }
        search_ptr = search_ptr->next;
    }
    return retval;
}

void Order::debug_print_buffer(void) {
    order_t *search_ptr;
    search_ptr = initial_ptr;
    sprintf(logger->debug,"Order: ---- Buffercontent ----"); 
    logger->logmsg(VERBOSEORDER, logger->debug);
    while (search_ptr) {
        sprintf(logger->debug,"Order: %p O:%u N:0%o (%u:%f) (%u:%f) (%u:%f) (%u:%f)", search_ptr, search_ptr->orderno, search_ptr->node, 
        search_ptr->channel1, search_ptr->value1, search_ptr->channel2, search_ptr->value2,
        search_ptr->channel3, search_ptr->value3, search_ptr->channel4, search_ptr->value4 );
        logger->logmsg(VERBOSEORDER, logger->debug);
        search_ptr=search_ptr->next;
    }
    sprintf(logger->debug,"Order: -- END Buffercontent --"); 
    logger->logmsg(VERBOSEORDER, logger->debug);
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
}

void Order::html_buffer(int new_tn_in_socket) {
    order_t *search_ptr;
    char *client_message =  (char*) malloc (TELNETBUFFERSIZE);
    search_ptr = initial_ptr;
	sprintf(client_message,"</table><br><big>Order</big><br><table><tr><th>OrderNo</th><th>EntryTime</th><th>Node</th><th>Type</th><th>Flags</th><th>Channel</th><th>Value</th></tr>\n"); 
    write(new_tn_in_socket , client_message , strlen(client_message));
    while (search_ptr) {
        sprintf(client_message,"<tr><td>%u</td><td>%llu (%d sec.)</td><td>0%o</td><td>%u</td><td>%u</td><td>%u<br>%u<br>%u<br>%u</td><td>%f<br>%f<br>%f<br>%f</td></tr>\n", 
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
}
