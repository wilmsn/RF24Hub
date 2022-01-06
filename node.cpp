#include "node.h"

Node::Node(void) {
    p_initial = NULL;
    verboselevel = 0;
    buf = (char*)malloc(TSBUFFERSIZE);
    tsbuf = (char*)malloc(TSBUFFERSIZE);
}

bool Node::isValidNode(NODE_DATTYPE node_id) {
    node_t *p_search;
    bool retval = false;
    p_search = p_initial;
    while (p_search) {
        if (p_search->node_id == node_id) {
            retval = true;
            p_search = NULL;
        } else {
            p_search = p_search->p_next;
        }
    }
    return retval;
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

void Node::newEntry(node_t* p_new) {
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

void Node::addNode(NODE_DATTYPE node_id, char* node_name, float u_batt, bool isMastered, uint8_t PALevel, uint32_t PAUtime ) {
    node_t *p_new = new node_t;
    p_new->node_id = node_id;
    sprintf(p_new->node_name, node_name);
    p_new->u_batt = u_batt;
    p_new->is_mastered = isMastered;
    p_new->pa_level = PALevel;
    p_new->pa_utime = PAUtime;
    if (verboselevel & VERBOSESENSOR) printf("%s Node.addNode: N:%u U:%f HB:%s PA:%s(%s)\n",ts(tsbuf),node_id, u_batt, isMastered? "    Mastered":"Not Mastered",
                   PALevel==0? "??? ":PALevel==1? "Low ":PALevel==2? "Min ":PALevel==3? "High":"Max ", utime2str(PAUtime, buf, 1));
    newEntry(p_new);
}

char* Node::getNodeName(NODE_DATTYPE node_id) {
    node_t *p_search;
    char *retval = NULL;
    p_search = p_initial;
    while (p_search) {
        if (p_search->node_id == node_id) {
            retval = p_search->node_name;
            p_search = NULL;
        } else {
            p_search = p_search->p_next;
        }
    }    
    return retval;
}

bool Node::isNewHB(NODE_DATTYPE node_id, uint8_t heartbeatno) {
    node_t *p_search;
    bool retval = false;
    p_search = p_initial;
    if ( heartbeatno == 0 ) {
        retval = true;
    } else {
        while (p_search) {
            if (p_search->node_id == node_id) {
                if (verboselevel & VERBOSEORDER)
                    printf("%s Node.is_new_HB: Node:%u last HB %u this HB %u => ", ts(tsbuf), node_id, p_search->heartbeatno, heartbeatno );
                if (p_search->heartbeatno != heartbeatno ) retval = true;
                p_search->heartbeatno = heartbeatno;
                p_search = NULL;
            } else {
                p_search = p_search->p_next;
            }
        }
    }
    if (verboselevel & VERBOSEORDER) {
        if (retval) {
            printf("New HeartBeat or Heartbeatno == 0\n");;
        } else {
            printf("Old HeartBeat\n");
        }
    }        
    return retval;
}

void Node::setPaLevel(NODE_DATTYPE node_id, uint8_t PALevel) {
    node_t *p_search;
    p_search=p_initial;
    if (verboselevel & VERBOSESENSOR) printf("%s Node.setPaLevel: N:%u PA:%s\n",ts(tsbuf),node_id, PALevel==0? "??? ":PALevel==1? "Min ":PALevel==2? "Low ":PALevel==3? "High":"Max ");
    while (p_search) {
		if (p_search->node_id == node_id) {
			p_search->pa_level = PALevel;
            p_search->pa_utime = time(0);
            p_search = NULL;
		} else {
            p_search=p_search->p_next;
        }
	}
}

uint8_t Node::getPaLevel(NODE_DATTYPE node_id) {
    node_t *p_search;
    uint8_t retval=0;
    p_search=p_initial;
    while (p_search) {
		if (p_search->node_id == node_id) {
			retval = p_search->pa_level;
            p_search = NULL;
		} else {
            p_search=p_search->p_next;
        }
	}
	return retval;
}

void Node::setVoltage(NODE_DATTYPE node_id, float u_batt) {
    node_t *p_search;
    p_search=p_initial;
    if (verboselevel & VERBOSESENSOR) printf("%s Node.setVoltage: N:%u U:%f\n",ts(tsbuf),node_id, u_batt);
    while (p_search) {
		if (p_search->node_id == node_id) {
			p_search->u_batt = u_batt;
            p_search = NULL;
		} else {
            p_search=p_search->p_next;
        }
	}
}

bool Node::isMasteredNode(NODE_DATTYPE node_id) {
    bool retval=false;
    node_t *p_search;
    p_search=p_initial;
    while (p_search) {
		if (p_search->node_id == node_id) {
			retval =  p_search->is_mastered;
            p_search = NULL;
		} else {
            p_search=p_search->p_next;
        }
	}
    if (verboselevel & VERBOSESENSOR) printf("%s Node.isMasteredNode: N:%u is %s\n", ts(tsbuf), node_id, retval? "Mastered":"Not Mastered");
	return retval;
}

void Node::printBuffer(int out_socket, bool htmlformat) {
    char *client_message =  (char*) malloc (TELNETBUFFERSIZE);
    node_t *p_search;
    p_search = p_initial;
    sprintf(client_message," ------ Nodes: ------\n"); 
    write(out_socket , client_message , strlen(client_message));
    while (p_search) {
        sprintf(client_message,"Node %s%s%u,\t%s\tU-Batt:\t%f V,\t%s\tPA: %s (%s)\n", 
                p_search->node_id<100? " ":"", p_search->node_id<10? " ":"", 
                p_search->node_id, p_search->node_name, p_search->u_batt, p_search->is_mastered? 
                "    Mastered":"Not Mastered",
                p_search->pa_level==0? "Min ":p_search->pa_level==1? "Low ":p_search->pa_level==2? "High":p_search->pa_level==3? "Max ":"??? ",
                utime2str(p_search->pa_utime, buf, 1) );    
		write(out_socket , client_message , strlen(client_message));
        p_search=p_search->p_next;
	}
    free(client_message);
}

void Node::setVerbose(uint16_t _verboselevel) {
    verboselevel = _verboselevel;
}
