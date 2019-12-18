#include "node.h"

Node::Node(void) {
    initial_ptr = NULL;
}

void Node::new_entry(Node::node_t* new_ptr) {
    node_t *search_ptr;
    new_ptr->next = NULL;
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

void Node::add_node(uint16_t node_id, float u_batt, bool is_HB_node ) {
    node_t *new_ptr = new node_t;
    new_ptr->node_id = node_id;
    new_ptr->u_batt = u_batt;
    new_ptr->is_HB_node = is_HB_node;
    new_ptr->HB_ts = 0;
    new_entry(new_ptr);
}

bool Node::is_new_HB(uint16_t node_id, uint64_t mymillis) {
    node_t *search_ptr;
    bool retval = false;
    search_ptr = initial_ptr;
    char *debug =  (char*) malloc (DEBUGSTRINGSIZE);
    while (search_ptr) {
printf("##>>(node): (%p)  %u %u \n",  search_ptr, search_ptr->node_id, node_id);      
        if (search_ptr->node_id == node_id) {
            sprintf(debug,"Node.is_new_HB: Node %u last HB: %llu this HB: %llu", node_id, search_ptr->HB_ts, mymillis); 
            logger->logmsg(VERBOSEORDER, debug);
            if (search_ptr->HB_ts < mymillis - 100) retval = true;
printf("##>>(node): %llu\n",  search_ptr->HB_ts);          
            search_ptr->HB_ts = mymillis;
        }
        search_ptr = search_ptr->next;
    }    
    if (retval) {
        sprintf(debug,"Node: New HeartBeat from Node %u", node_id); 
        logger->logmsg(VERBOSEORDER, debug);
    } else {
        sprintf(debug,"Node: Old HeartBeat from Node %u", node_id); 
        logger->logmsg(VERBOSEORDER, debug);
    }        
    free(debug);
    return retval;
}

bool Node::is_HB_node(uint16_t node_id) {
    bool retval=false;
    node_t *search_ptr;
    search_ptr=initial_ptr;
    while (search_ptr) {
		if (search_ptr->node_id == node_id && search_ptr->is_HB_node) {
			retval=true;
		}
		search_ptr=search_ptr->next;
	}
	return retval;
}

void Node::print_buffer2tn(int new_tn_in_socket) {
    char *client_message =  (char*) malloc (TELNETBUFFERSIZE);
    node_t *search_ptr;
    search_ptr = initial_ptr;
    sprintf(client_message," ------ Nodes: ------\n"); 
    write(new_tn_in_socket , client_message , strlen(client_message));
    while (search_ptr) {
        sprintf(client_message,"Node %u,\tU-Batt:\t%f V,\t%s\n",
                  search_ptr->node_id, search_ptr->u_batt, search_ptr->is_HB_node? "HeartBeat":"Normal");    
		write(new_tn_in_socket , client_message , strlen(client_message));
        search_ptr=search_ptr->next;
	}
    free(client_message);
    print_buffer2log();
}

void Node::print_buffer2log(void) {
    node_t *search_ptr;
    search_ptr = initial_ptr;
    char *debug =  (char*) malloc (DEBUGSTRINGSIZE);
    sprintf(debug," ------ Nodes: ------"); 
    logger->logmsg(VERBOSECONFIG, debug);
    while (search_ptr) {
        sprintf(debug,"Node %u,\tU-Batt:\t%f V,\t%s",
                  search_ptr->node_id, search_ptr->u_batt, search_ptr->is_HB_node? "HeartBeat":"Normal");    
        logger->logmsg(VERBOSECONFIG, debug);
        search_ptr=search_ptr->next;
	}
	free(debug);
}

void Node::begin(Logger* _logger) {
    logger = _logger;
}
