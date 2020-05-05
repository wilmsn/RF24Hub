#include "orderbuffer.h"

OrderBuffer::OrderBuffer(void) {
    p_initial = NULL;
}

void OrderBuffer::newEntry(OrderBuffer::orderbuffer_t* p_new) {
    orderbuffer_t *p_search;
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

bool OrderBuffer::delEntry(OrderBuffer::orderbuffer_t* p_del) {
    bool retval = false;
    orderbuffer_t *p_search, *p_tmp;
    p_search = p_initial;
    p_tmp = p_initial;
    while (p_search) {
        if (p_search == p_del ) {
            if (p_search == p_initial) {
                if (p_initial->p_next) { 
                    p_tmp=p_initial->p_next;
                    delete p_initial;
                    p_initial=p_tmp;
                } else {
                    delete p_initial;
                    p_initial = NULL;
                }
            } else            {
                p_tmp->p_next=p_search->p_next;
                delete p_search;
            }
            p_search = NULL;
            retval = true;
        } else {
            p_tmp = p_search;
            p_search=p_search->p_next;
        }
    }
    return retval;
}

bool OrderBuffer::delByNodeChannel(uint8_t node_id, uint8_t channel) {
    int retval = false;
    orderbuffer_t *p_search;
    p_search = p_initial;
    while (p_search) {
        if (p_search->node_id == node_id && p_search->channel == channel) {
            if (delEntry(p_search)) retval = true;
        }
        p_search=p_search->p_next;
    }
    return retval;
}

bool OrderBuffer::delByNode(uint8_t node_id) {
    int retval = false;
    orderbuffer_t *p_search;
    p_search = p_initial;
    while (p_search) {
        if (p_search->node_id == node_id) {
            if (delEntry(p_search)) retval = true;
        }
        p_search=p_search->p_next;
    }
    return retval;
}

bool OrderBuffer::nodeHasEntry(uint8_t node_id) {
    int retval = false;
    orderbuffer_t *p_search;
    p_search = p_initial;
    while (p_search) {
        if (p_search->node_id == node_id) {
            retval = true;
        }
        p_search=p_search->p_next;
    }
    return retval;
}

void OrderBuffer::addOrderBuffer(uint64_t millis, uint8_t node_id, uint8_t channel, float value) {
    orderbuffer_t *p_new = new orderbuffer_t;
    delByNodeChannel(node_id, channel);
    p_new->entrytime = millis;
    p_new->node_id = node_id;
    p_new->channel = channel;
    p_new->value = value;
    newEntry(p_new);
}

void* OrderBuffer::findOrder4Node(uint8_t node_id, void* p_last, uint8_t* channel, float* value) {
    orderbuffer_t *p_search;
    void* retval = NULL;
    if (p_last) {
        p_search = (orderbuffer_t*)p_last;
        p_search = p_search->p_next;
    } else {
        p_search = p_initial;
    }
    while ( p_search ) {
        if ( p_search->node_id == node_id ) {
            *channel = p_search->channel;
            *value = p_search->value;
            retval = (void*)p_search;
            p_search = NULL;;
        } else {
            p_search=p_search->p_next;
        }
    }
    return retval;
}

void OrderBuffer::printBuffer(uint16_t debuglevel) {
    orderbuffer_t *p_search;
    p_search = p_initial;
    if (verboselevel & debuglevel) {
        printf("OrderBuffer: ---- Buffercontent ----"); 
        while (p_search) {
            printf("OrderBuffer: %p N:%u C:%u V:%g", p_search, p_search->node_id, p_search->channel, p_search->value );
            p_search=p_search->p_next;
        }
        printf("OrderBuffer: -- END Buffercontent --"); 
    }
}

void OrderBuffer::printBuffer2tn(int new_tn_in_socket) {
    char *client_message =  (char*) malloc (TELNETBUFFERSIZE);
    orderbuffer_t *p_search;
    p_search = p_initial;
    sprintf(client_message," ---- OrderBuffer ----\n"); 
    write(new_tn_in_socket , client_message , strlen(client_message));
    while (p_search) {
        sprintf(client_message,"<%p> Node:%u Channel:%u Value:%g\n", p_search, p_search->node_id, p_search->channel, p_search->value );
        write(new_tn_in_socket , client_message , strlen(client_message));
        p_search=p_search->p_next;
    }
    free(client_message);
}

void OrderBuffer::htmlBuffer2tn(int new_tn_in_socket) {
    char *client_message =  (char*) malloc (TELNETBUFFERSIZE);
    orderbuffer_t *p_search;
    p_search = p_initial;
	sprintf(client_message,"\n<center><big>Orderbuffer</big><table><tr><th>EntryTime</th><th>Node</th><th>Channel</th><th>Value</th></tr>\n"); 
    write(new_tn_in_socket , client_message , strlen(client_message));
    while (p_search) {
		sprintf(client_message,"<tr><td>%llu</td><td>%u</td><td>%u</td><td>%g</td></tr>\n", 
                p_search->entrytime, p_search->node_id, 
                p_search->channel, p_search->value );
        write(new_tn_in_socket , client_message , strlen(client_message));
        p_search=p_search->p_next;
    }
	sprintf(client_message,"</table><br>\n"); 
	write(new_tn_in_socket , client_message , strlen(client_message));
    free(client_message);
}

