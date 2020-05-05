#include "node.h"

Node::Node(void) {
    p_initial = NULL;
}

void Node::printError(const char* txt, node_t* pointer) {
    if ( verboselevel & VERBOSECRITICAL) {    
        char* buf = alloc_str(VERBOSEPOINTER,"Node::printError buf",TSBUFFERSIZE);
        printf("%sError: %s<%p>\n", log_ts(buf), txt, pointer);
        free_str(VERBOSEPOINTER,"Node::printError buf",buf);
    }
}

void Node::cleanup(void) {
    node_t *p_search;
    p_search = p_initial;
    while ( p_search ) {
        p_initial = p_search->p_next;
        delete p_search;
        p_search = p_initial;
    }
}

void Node::newEntry(Node::node_t* p_new) {
    node_t *p_search;
    p_new->p_next = NULL;
    if (p_initial) {
        p_search = p_initial;
        while (p_search->p_next) {
            p_search = p_search->p_next;
        }
        p_search->p_next = p_new;
    } else {
        p_initial = p_new;
    }
}

void Node::addNode(uint8_t node_id, float u_batt, bool is_HB_node, uint8_t pa_level ) {
    node_t* p_buffer = new node_t;
    if (p_buffer) {
        p_buffer->node_id = node_id;
        p_buffer->u_batt = u_batt;
        p_buffer->is_HB_node = is_HB_node;
        p_buffer->HB_ts = 0;
        p_buffer->pa_level = pa_level;
        newEntry(p_buffer);
    } else {
        printError("Node::addNode => p_buffer", p_buffer);
    }
}

bool Node::isNewHB(uint8_t node_id, uint64_t mymillis) {
    node_t *p_search;
    bool retval = false;
    p_search = p_initial;
    char* buf = alloc_str(VERBOSEPOINTER,"Node::isNewHB buf",TSBUFFERSIZE);
    while (p_search) {
        if (p_search->node_id == node_id) {
            if (verboselevel & VERBOSEORDER) {
                printf("%sNode.is_new_HB: Node %u last HB: %llu this HB: %llu\n", log_ts(buf), node_id, p_search->HB_ts, mymillis); 
            }
            if (p_search->HB_ts < mymillis - 5000) retval = true;
            p_search->HB_ts = mymillis;
        }
        p_search = p_search->p_next;
    }    
    if (retval) {
        if (verboselevel & VERBOSEORDER) {
            printf("%sNode: New HeartBeat from Node %u\n", log_ts(buf), node_id); 
        }
    } else {
        if (verboselevel & VERBOSEORDER) {
            printf("%sNode: Old HeartBeat from Node %u\n", log_ts(buf), node_id); 
        }
    }   
    free_str(VERBOSEPOINTER,"Node::isNewHB buf",buf);
    return retval;
}

void Node::setPaLevel(uint8_t node_id, uint8_t pa_level) {
    node_t *p_search;
    p_search=p_initial;
    while (p_search) {
		if (p_search->node_id == node_id) {
			p_search->pa_level = pa_level;
            char* buf = alloc_str(VERBOSEPOINTER,"Node::setPaLevel buf",TSBUFFERSIZE);
            sprintf(p_search->pa_level_datestr,"%s",str_ts(buf,2));
            free_str(VERBOSEPOINTER,"Node::setPaLevel buf",buf);
            p_search = NULL;
		} else {
            p_search=p_search->p_next;
        }
	}
}

void Node::setVoltage(uint8_t node_id, float u_batt) {
    node_t *p_search;
    p_search=p_initial;
    while (p_search) {
		if (p_search->node_id == node_id) {
			p_search->u_batt = u_batt;
            p_search = NULL;
		} else {
            p_search=p_search->p_next;
        }
	}
}

bool Node::isHBNode(uint8_t node_id) {
    bool retval=false;
    node_t *p_search;
    p_search=p_initial;
    while (p_search) {
		if (p_search->node_id == node_id && p_search->is_HB_node) {
			retval=true;
            p_search = NULL;
		} else {
            p_search=p_search->p_next;
        }
	}
	return retval;
}

bool Node::isValidNode(uint8_t node_id) {
    bool retval=false;
    node_t *p_search;
    p_search=p_initial;
    while (p_search) {
		if (p_search->node_id == node_id) {
			retval=true;
            p_search = NULL;
		} else {
            p_search=p_search->p_next;
        }
	}
	return retval;
}

void Node::printBuffer2tn(int new_tn_in_socket) {
    char *client_message =  (char*) malloc (TELNETBUFFERSIZE);
    node_t *p_search;
    p_search = p_initial;
    sprintf(client_message," ------ Nodes: ------\n"); 
    write(new_tn_in_socket , client_message , strlen(client_message));
    while (p_search) {
        sprintf(client_message,"Node %s%s%u,\tU-Batt:\t%f V,\t%s\n", 
                   p_search->node_id<100? " ":"", p_search->node_id<10? " ":"",
                   p_search->node_id, p_search->u_batt, p_search->is_HB_node? "HeartBeat":"Normal   ",
                   p_search->pa_level==0? "??? ":p_search->pa_level==1? "Min ":p_search->pa_level==2? "Low ":p_search->pa_level==3? "High":"Max ");    
		write(new_tn_in_socket , client_message , strlen(client_message));
        p_search=p_search->p_next;
	}
    free(client_message);
}

void Node::printBuffer(uint16_t debuglevel) {
    node_t *p_search;
    p_search = p_initial;
    if (verboselevel & debuglevel) {
        printf(" ------ Nodes: ------\n"); 
        while (p_search) {
            printf("Node %s%s%u,\tU-Batt:\t%f V,\t%s\tPA: %s\n", p_search->node_id<100? " ":"", p_search->node_id<10? " ":"", 
                   p_search->node_id, p_search->u_batt, p_search->is_HB_node? "HeartBeat":"Normal   ",
                   p_search->pa_level==0? "??? ":p_search->pa_level==1? "Min ":p_search->pa_level==2? "Low ":p_search->pa_level==3? "High":"Max ");    
        p_search=p_search->p_next;
        }
    }
}

