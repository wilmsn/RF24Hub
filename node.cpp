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

void Node::add_node(uint16_t node, float u_batt, bool is_HB_node ) {
    node_t *new_ptr = new node_t;
    new_ptr->node = node;
    new_ptr->u_batt = u_batt;
    new_ptr->is_HB_node = is_HB_node;
    new_entry(new_ptr);
}

bool Node::is_new_HB(uint16_t node, uint64_t mymillis) {
    node_t *search_ptr;
    bool retval = false;
    search_ptr = initial_ptr;
    while (search_ptr) {
        if (search_ptr->node == node) {
            if (search_ptr->HB_ts < mymillis - 1000) retval = true;
            search_ptr->HB_ts = mymillis;
        }
        search_ptr = search_ptr->next;
    }    
    if (retval) {
        sprintf(logger->debug,"Node: New HeartBeat from Node 0%o", node); 
        logger->logmsg(VERBOSEORDER, logger->debug);
    } else {
        sprintf(logger->debug,"Node: Old HeartBeat from Node 0%o", node); 
        logger->logmsg(VERBOSEORDER, logger->debug);
    }        
    return retval;
}

bool Node::is_HB_node(uint16_t mynode) {
    bool retval=false;
    node_t *search_ptr;
    search_ptr=initial_ptr;
    while (search_ptr) {
		if (search_ptr->node == mynode && search_ptr->is_HB_node) {
			retval=true;
		}
		search_ptr=search_ptr->next;
	}
	return retval;
}

void Node::print_buffer(int new_tn_in_socket) {
    char *client_message =  (char*) malloc (TELNETBUFFERSIZE);
    node_t *search_ptr;
    search_ptr = initial_ptr;
    sprintf(client_message," ------ Nodes: ------\n"); 
    write(new_tn_in_socket , client_message , strlen(client_message));
    while (search_ptr) {
        sprintf(client_message,"Node 0%o,\tU-Batt:\t%f V,\t%s\n",
                  search_ptr->node, search_ptr->u_batt, search_ptr->is_HB_node? "HeartBeat":"Normal");    
		write(new_tn_in_socket , client_message , strlen(client_message));
        search_ptr=search_ptr->next;
	}
    free(client_message);
}

void Node::begin(Logger* _logger) {
    logger = _logger;
}
