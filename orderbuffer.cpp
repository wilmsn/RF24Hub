#include "orderbuffer.h"

OrderBuffer::OrderBuffer(void) {
    initial_ptr = NULL;
}

void OrderBuffer::new_entry(OrderBuffer::orderbuffer_t* new_ptr) {
    orderbuffer_t *search_ptr;
    new_ptr->next = NULL;
    if (logger->verboselevel >= VERBOSEORDER) {
        sprintf(logger->debug,"OrderBuffer: new_entry %p", new_ptr); 
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

bool OrderBuffer::del_node_channel(uint16_t node, uint8_t channel) {
    int retval = false;
    orderbuffer_t *search_ptr;
    if (logger->verboselevel >= VERBOSEORDER) {
        sprintf(logger->debug,"OrderBuffer: del_node_channel N:0%o C:%u", node, channel); 
        logger->logmsg(VERBOSEORDER, logger->debug);    
        sprintf(logger->debug,"Bestand vorher"); 
        logger->logmsg(VERBOSEORDER, logger->debug);
        debug_print_buffer();
    }
    search_ptr = initial_ptr;
    while (search_ptr) {
        if (search_ptr->node == node && search_ptr->channel == channel) {
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

bool OrderBuffer::del_node(uint16_t node) {
    int retval = false;
    orderbuffer_t *search_ptr;
    if (logger->verboselevel >= VERBOSEORDER) {
        sprintf(logger->debug,"OrderBuffer: del_node N:0%o", node); 
        logger->logmsg(VERBOSEORDER, logger->debug);    
        sprintf(logger->debug,"Bestand vorher"); 
        logger->logmsg(VERBOSEORDER, logger->debug);
        debug_print_buffer();
    }
    search_ptr = initial_ptr;
    while (search_ptr) {
        if (search_ptr->node == node) {
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

void OrderBuffer::add_orderbuffer(uint64_t millis, uint16_t node, uint8_t channel, float value) {
    orderbuffer_t *new_ptr = new orderbuffer_t;
    del_node_channel(node, channel);
    new_ptr->entrytime = millis;
    new_ptr->node = node;
    new_ptr->channel = channel;
    new_ptr->value = value;
    new_entry(new_ptr);
}

void *OrderBuffer::find_order4node(uint16_t node, void* last_ptr, uint8_t* channel, float* value) {
    orderbuffer_t *search_ptr;
    void* retval = NULL;
    bool rec_found = false;
    if (last_ptr) {
        search_ptr = (orderbuffer_t*)last_ptr;
        search_ptr = search_ptr->next;
    } else {
        search_ptr = initial_ptr;
    }
    while ( ! rec_found && search_ptr ) {
        if ( search_ptr->node == node ) {
            *channel = search_ptr->channel;
            *value = search_ptr->value;
            retval = (void*)search_ptr;
            rec_found = true;
        }
        search_ptr=search_ptr->next;
    }
    return retval;
}

void OrderBuffer::debug_print_buffer(void) {
    orderbuffer_t *search_ptr;
    search_ptr = initial_ptr;
    sprintf(logger->debug,"OrderBuffer: ---- Buffercontent ----"); 
    logger->logmsg(VERBOSEORDER, logger->debug);
    while (search_ptr) {
        sprintf(logger->debug,"OrderBuffer: %p N:0%o C:%u V:%f", 
                search_ptr, search_ptr->node, 
                search_ptr->channel, search_ptr->value );
        logger->logmsg(VERBOSEORDER, logger->debug);
        search_ptr=search_ptr->next;
    }
    sprintf(logger->debug,"OrderBuffer: -- END Buffercontent --"); 
    logger->logmsg(VERBOSEORDER, logger->debug);
}

void OrderBuffer::print_buffer(int new_tn_in_socket) {
    char *client_message =  (char*) malloc (TELNETBUFFERSIZE);
    orderbuffer_t *search_ptr;
    search_ptr = initial_ptr;
    sprintf(client_message," ---- OrderBuffer ----\n"); 
    write(new_tn_in_socket , client_message , strlen(client_message));
    while (search_ptr) {
        sprintf(client_message,"<%p> Node:0%o Channel:%u Value:%f\n", 
                search_ptr, search_ptr->node, 
                search_ptr->channel, search_ptr->value );
        write(new_tn_in_socket , client_message , strlen(client_message));
        search_ptr=search_ptr->next;
    }
    free(client_message);
    debug_print_buffer();
}

void OrderBuffer::html_buffer(int new_tn_in_socket) {
    char *client_message =  (char*) malloc (TELNETBUFFERSIZE);
    orderbuffer_t *search_ptr;
    search_ptr = initial_ptr;
	sprintf(client_message,"\n<center><big>Orderbuffer</big><table><tr><th>EntryTime</th><th>Node</th><th>Channel</th><th>Value</th></tr>\n"); 
    write(new_tn_in_socket , client_message , strlen(client_message));
    while (search_ptr) {
		sprintf(client_message,"<tr><td>%llu</td><td>0%o</td><td>%u</td><td>%f</td></tr>\n", 
                search_ptr->entrytime, search_ptr->node, 
                search_ptr->channel, search_ptr->value );
        write(new_tn_in_socket , client_message , strlen(client_message));
        search_ptr=search_ptr->next;
    }
	sprintf(client_message,"</table><br>\n"); 
	write(new_tn_in_socket , client_message , strlen(client_message));
    free(client_message);
}

void OrderBuffer::begin(Logger* _logger) {
    logger = _logger;
}
