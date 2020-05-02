#include "node.h"

Node::Node(void) {
}

Node::node_t* Node::findNode(uint8_t node_id) {
    void* p_buffer;
    node_t* p_data;
    node_t* retval = NULL;
    p_buffer = nextEntry(NULL);
    while ( p_buffer ) {
        p_data = (node_t *)getDataPtr(p_buffer);
        if (p_data) {
            if ( p_data->node_id == node_id ) {
                retval = p_data;
                p_buffer = NULL;
            } else {
                p_buffer = nextEntry(p_buffer);
            }
        } else {
            printf("Node::findNode => p_data <%p>\n", p_data);
        }
    }
    return retval;
}

void Node::cleanup(void) {
    void* p_buffer;
    void* p_data;
    p_buffer = nextEntry(NULL);
    while ( p_buffer ) {    
        p_data = getDataPtr(p_buffer);
        if (p_data) {
            free(p_data);
        } else {
            printf("Node::cleanup => p_data <%p>\n", p_data);
        }
        delEntry(p_buffer);
        p_buffer = nextEntry(NULL);
    }
}

void Node::addNode(uint8_t node_id, float u_batt, bool is_HB_node, uint8_t pa_level ) {
    node_t *p_data = new node_t;
    if (p_data) {
        p_data->node_id = node_id;
        p_data->u_batt = u_batt;
        p_data->is_HB_node = is_HB_node;
        p_data->HB_ts = 0;
        p_data->pa_level = pa_level;
        newEntry((void*)p_data);
    } else {
        printf("Node::addNode => p_data <%p>\n", p_data);
    }
}

bool Node::isNewHB(uint8_t node_id, uint64_t mymillis) {
    node_t* p_data;
    bool retval = false;
    p_data = findNode(node_id);
    if (p_data) {
        if (p_data->HB_ts < mymillis - 5000) retval = true;
        p_data->HB_ts = mymillis;
    } else {
        printf("Node::isNewHB => p_data <%p>\n", p_data);
    }
    return retval;
}

bool Node::isHBNode(uint8_t node_id) {
    node_t* p_data; 
    p_data = findNode(node_id);
    if (p_data) {
        return p_data->is_HB_node;
    } else {
        printf("Node::isHBNode => p_data <%p>\n", p_data);
        return false;
    }
}

void Node::setPaLevel(uint8_t node_id, uint8_t pa_level) {
    node_t* p_data;
    bool retval = false;
    char buf[] = TSBUFFERSTRING;
    p_data = findNode(node_id);
    if (p_data) {
        p_data->pa_level = pa_level;  
        sprintf(p_data->pa_level_datestr,"%s",str_ts(buf,2));
    } else {
        printf("Node::setPaLevel => p_data <%p>\n", p_data);
    }
}    

void Node::setVoltage(uint8_t node_id, float u_batt) {
    node_t* p_data;
    bool retval = false;
    p_data = findNode(node_id);
    if (p_data) {
        p_data->u_batt = u_batt;    
    } else {
        printf("Node::setVoltage => p_data <%p>\n", p_data);
    }
}

void Node::printBuffer2tn(int new_tn_in_socket) {
    void* p_buffer = nextEntry(NULL);
    node_t* p_data;
    char *client_message =  (char*) malloc (TELNETBUFFERSIZE);
    char paLevel[30];
    sprintf(client_message," ------ Nodes: ------\n"); 
    write(new_tn_in_socket , client_message , strlen(client_message));
    while ( p_buffer ) {
        p_data = (node_t *)getDataPtr(p_buffer);
        if (p_data) {
            switch (p_data->pa_level) {
                case 0:
                    sprintf(paLevel,"%s(%s)","Min",p_data->pa_level_datestr);
                break;
                case 1:
                    sprintf(paLevel,"%s(%s)","Low",p_data->pa_level_datestr);
                break;
                case 2:
                    sprintf(paLevel,"%s(%s)","High",p_data->pa_level_datestr);
                break;
                case 3:
                    sprintf(paLevel,"%s(%s)","Max",p_data->pa_level_datestr);
                break;
                default:
                    sprintf(paLevel,"%s(-------------------)","???");
            }
            sprintf(client_message,"Node %s%s%u,\tU-Batt:\t%f V,\tPA-Lev: %s,\t%s \n", 
                p_data->node_id<100? " ":"", p_data->node_id<10? " ":"", p_data->node_id, p_data->u_batt, 
                paLevel, p_data->is_HB_node? "HeartBeat":"Normal");    
            write(new_tn_in_socket , client_message , strlen(client_message));
        } else {
            printf("Node::printBuffer2tn => p_data <%p>\n", p_data);
        }
        p_buffer = nextEntry(p_buffer);
    }
    free(client_message);
}

void Node::printBuffer(uint16_t debuglevel) {
    char buf[] = TSBUFFERSTRING;
    void* p_buffer = nextEntry(NULL);
    node_t* p_data;
    char paLevel[6];
    if (debuglevel & verboselevel) {
        while ( p_buffer ) {
            p_data = (node_t *)getDataPtr(p_buffer);
            if (p_data) {
                switch (p_data->pa_level) {
                    case 0:
                        sprintf(paLevel,"%s","Min");
                    break;
                    case 1:
                        sprintf(paLevel,"%s","Low");
                    break;
                    case 2:
                        sprintf(paLevel,"%s","High");
                    break;
                    case 3:
                        sprintf(paLevel,"%s","Max");
                    break;
                    default:
                        sprintf(paLevel,"%s","???");
                }
                printf("Node %s%s%u,\tU-Batt:\t%f V,\tPA-Lev: %s,\t%s \n", 
                    p_data->node_id<100? " ":"", p_data->node_id<10? " ":"", p_data->node_id, p_data->u_batt, 
                    paLevel, p_data->is_HB_node? "HeartBeat":"Normal");    
            } else {
                printf("Node::printBuffer => p_data <%p>\n", p_data);
            }
            p_buffer = nextEntry(p_buffer);
        }
    }
}

