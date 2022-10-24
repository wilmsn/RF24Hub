#include "nodeclass.h"

NodeClass::NodeClass(void) {
    p_initial = NULL;
    _verboseLevel = 0;
    buf = (char*)malloc(TSBUFFERSIZE);
    tsbuf = (char*)malloc(TSBUFFERSIZE);
}

bool NodeClass::isValidNode(NODE_DATTYPE node_id) {
    nodeClass_t *p_search;
    bool retval = false;
    p_search = p_initial;
    while (p_search) {
        if (p_search->node_id == node_id && p_search->is_mastered) {
            retval = true;
            p_search = NULL;
        } else {
            p_search = p_search->p_next;
        }
    }
    return retval;
}

void NodeClass::cleanup(void) {
    nodeClass_t *p_search;
    p_search = p_initial;
    while ( p_search ) {
        p_initial = p_search->p_next;
        delete p_search;
        p_search = p_initial;
    }
}

void NodeClass::newEntry(nodeClass_t* p_new) {
    nodeClass_t *p_search;
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

void NodeClass::addNode(NODE_DATTYPE node_id, char* node_name, bool isMastered, uint8_t pa_level, uint8_t rec_level, float lv_volt, bool lv_flag ) {
    nodeClass_t *p_new = new nodeClass_t;
    p_new->node_id = node_id;
    sprintf(p_new->node_name, "%s", node_name);
    p_new->lv_volt = lv_volt;
    p_new->lv_flag = lv_flag;
    p_new->is_mastered = isMastered;
    p_new->pa_level = pa_level;
    p_new->rec_level = rec_level;
    p_new->hb_no = 0;
    p_new->hb_utime = 0;
    p_new->u_batt = 0;
    if (_verboseLevel & VERBOSENODE) printf("%sNodeClass.addNodeClass: N:%u Ma:%s PA:%s Rec:%s LV:%fV Flag:%s \n",ts(tsbuf),node_id,          
                isMastered? "    Mastered":"Not Mastered",
                pa_level==0? "Low ":pa_level==1? "Min ":pa_level==2? "High":pa_level==3? "Max ":"??? ",
                rec_level==0? "Low ":rec_level==1? "Min ":rec_level==2? "High":rec_level==3? "Max ":"??? ",
                lv_volt, lv_flag? "ja  ":"nein" );
    newEntry(p_new);
}

char* NodeClass::getNodeName(NODE_DATTYPE node_id) {
    nodeClass_t *p_search;
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

bool NodeClass::isNewHB(NODE_DATTYPE node_id, uint8_t hb_no, uint32_t hb_utime) {
    nodeClass_t *p_search;
    bool retval = false;
    p_search = p_initial;
    if ( hb_no == 0 ) {
        retval = true;
    } else {
        while (p_search) {
            if (p_search->node_id == node_id) {
                if (_verboseLevel & VERBOSEORDER)
                    printf("%sNodeClass.is_new_HB: Node:%u last HB %u (%u) this HB %u => ", ts(tsbuf), node_id, p_search->hb_no, p_search->hb_utime, hb_no );
                if ( hb_utime > p_search->hb_utime + 50 || 
                     hb_no > p_search->hb_no || 
                     (p_search->hb_no - hb_no > 50) || 
                     (p_search->hb_no > 200 && p_search->hb_no != hb_no) ) {
                    retval = true;
                    p_search->hb_no = hb_no;
                    p_search->hb_utime = hb_utime;
                }
                p_search = NULL;
            } else {
                p_search = p_search->p_next;
            }
        }
    }
    if (_verboseLevel & VERBOSEORDER) {
        if (retval) {
            printf("New HeartBeat or Heartbeatno == 0\n");;
        } else {
            printf("Old HeartBeat\n");
        }
    }        
    return retval;
}

bool NodeClass::isCurHB(NODE_DATTYPE node_id, uint8_t hb_no) {
    nodeClass_t *p_search;
    bool retval = false;
    p_search = p_initial;
    while (p_search) {
        if (p_search->node_id == node_id && p_search->hb_no == hb_no) {
            if (_verboseLevel & VERBOSEORDER)
                printf("%sNodeClass.isCurHB: Node:%u last HB %u this HB %u => ", ts(tsbuf), node_id, p_search->hb_no, hb_no );
            retval = true;
            p_search = NULL;
        } else {
            p_search = p_search->p_next;
        }
    }
    if (_verboseLevel & VERBOSEORDER) {
        if (retval) {
            printf("Current Heartbeat\n");
        } else {
            printf("Old HeartBeat\n");
        }
    }
    return retval;
}

void NodeClass::setRecLevel(NODE_DATTYPE node_id, uint8_t rec_level) {
    nodeClass_t *p_search;
    p_search=p_initial;
    if (_verboseLevel & VERBOSENODE) printf("%sNodeClass.setRecLevel: N:%u PA:%s\n",ts(tsbuf),node_id, rec_level==0? "Min ":rec_level==1? "Low ":rec_level==2? "High":rec_level==3? "Max ":"??? ");
    while (p_search) {
        if (p_search->node_id == node_id) {
            p_search->rec_level = rec_level;
            p_search = NULL;
        } else {
            p_search=p_search->p_next;
        }
    }
}

uint8_t NodeClass::getRecLevel(NODE_DATTYPE node_id) {
    nodeClass_t *p_search;
    uint8_t retval=9;
    p_search=p_initial;
    while (p_search) {
        if (p_search->node_id == node_id) {
            retval = p_search->rec_level;
            p_search = NULL;
        } else {
            p_search=p_search->p_next;
        }
    }
    return retval;
}

void NodeClass::setPaLevel(NODE_DATTYPE node_id, uint8_t pa_level) {
    nodeClass_t *p_search;
    p_search=p_initial;
    if (_verboseLevel & VERBOSENODE) printf("%sNodeClass.setPaLevel: N:%u PA:%s\n",ts(tsbuf),node_id, pa_level==0? "Min ":pa_level==1? "Low ":pa_level==2? "High":pa_level==3? "Max ":"??? ");
    while (p_search) {
        if (p_search->node_id == node_id) {
            p_search->pa_level = pa_level;
            p_search = NULL;
        } else {
            p_search=p_search->p_next;
        }
    }
}

uint8_t NodeClass::getPaLevel(NODE_DATTYPE node_id) {
    nodeClass_t *p_search;
    uint8_t retval = 9;
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

void NodeClass::setVoltage(NODE_DATTYPE node_id, float u_batt) {
    nodeClass_t *p_search;
    p_search=p_initial;
    if (_verboseLevel & VERBOSENODE) printf("%sNodeClass.setVoltage: N:%u U:%f\n",ts(tsbuf),node_id, u_batt);
    while (p_search) {
        if (p_search->node_id == node_id) {
            p_search->u_batt = u_batt;
            p_search = NULL;
        } else {
            p_search=p_search->p_next;
        }
    }
}

bool NodeClass::setMasteredNode(NODE_DATTYPE node_id, bool isMastered){
    bool retval=false;
    nodeClass_t *p_search;
    p_search=p_initial;
    while (p_search) {
        if (p_search->node_id == node_id) {
            p_search->is_mastered = isMastered;
            retval = true;
            p_search = NULL;
        } else {
            p_search=p_search->p_next;
        }
    }
    if (_verboseLevel & VERBOSENODE) printf("%sNodeClass.setMasteredNodeClass: N:%u is %s\n", ts(tsbuf), node_id, isMastered? "Mastered":"Not Mastered");
    return retval;
}

bool NodeClass::setLVFlag(NODE_DATTYPE node_id, bool lv_flag) {
    nodeClass_t *p_search;
    bool retval=false;
    p_search=p_initial;
    while (p_search) {
        if (p_search->node_id == node_id) {
            if (p_search->lv_flag != lv_flag) {
                p_search->lv_flag = lv_flag;
                retval=true;
                if (_verboseLevel & VERBOSENODE) printf("%sNodeClass.setLVFlag: N:%u LV Flag %s\n", ts(tsbuf), node_id, lv_flag? "set":"Not set");    
            }
            p_search = NULL;
        } else {
            p_search=p_search->p_next;
        }
    }
    return retval;
}

void NodeClass::setLVVolt(NODE_DATTYPE node_id, float lv_volt) {
    nodeClass_t *p_search;
    p_search=p_initial;
    while (p_search) {
        if (p_search->node_id == node_id) {
            p_search->lv_volt = lv_volt;
            if (_verboseLevel & VERBOSENODE) printf("%sNodeClass.setLVFlag: N:%u LV: %fV\n", ts(tsbuf), node_id, lv_volt);    
            p_search = NULL;
        } else {
            p_search=p_search->p_next;
        }
    }
}

void NodeClass::printBuffer(int out_socket, bool htmlformat) {
    char *client_message =  (char*) malloc (TELNETBUFFERSIZE);
    nodeClass_t *p_search;
    p_search = p_initial;
    sprintf(client_message," ------ Node: ------\n"); 
    write(out_socket , client_message , strlen(client_message));
    while (p_search) {
        sprintf(client_message,"Node %s%s%u,\t%s%s\tU-Batt:\t%.2fV (LV: %.2fV),\t%s\tPA: gem:%s ist:%s\n", 
                p_search->node_id<100? " ":"", p_search->node_id<10? " ":"", 
                p_search->node_id, 
                p_search->node_name, 
                strlen(p_search->node_name)<8? "\t\t\t":strlen(p_search->node_name)<16? "\t\t":strlen(p_search->node_name)<24? "\t":"",
                p_search->u_batt, 
                p_search->lv_volt,
                p_search->is_mastered? "    Mastered":"Not Mastered",
                p_search->rec_level==0? "Min ":p_search->rec_level==1? "Low ":p_search->rec_level==2? "High":p_search->rec_level==3? "Max ":"??? ",
                p_search->pa_level==0? "Min ":p_search->pa_level==1? "Low ":p_search->pa_level==2? "High":p_search->pa_level==3? "Max ":"??? ");
        write(out_socket , client_message , strlen(client_message));
        p_search=p_search->p_next;
    }
    free(client_message);
}

void NodeClass::setVerbose(uint16_t verboseLevel) {
    _verboseLevel = verboseLevel;
}
