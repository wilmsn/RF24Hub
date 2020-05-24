#include "orderbuffer.h"

OrderBuffer::OrderBuffer(void) {
    p_initial = NULL;
    verboselevel = 0;
    buf = (char*)alloc_str(VERBOSEPOINTER,"OrderBuffer::OrderBuffer buf",TSBUFFERSIZE);
}

void OrderBuffer::newEntry(OrderBuffer::orderbuffer_t* p_new) {
    orderbuffer_t *p_search;
    p_new->p_next = NULL;
    if (verboselevel & VERBOSEOBUFFER) 
        printf("%sOrderBuffer: newEntry <%p> N:%u C:%u V:%f\n", ts(buf), p_new, p_new->node_id, p_new->channel, p_new->data); 
    if (verboselevel & VERBOSEOBUFFEREXT) {
        printf("%sBestand vorher:\n", ts(buf)); 
        printBuffer();
    }
    if (p_initial) {
        p_search = p_initial;
        while (p_search->p_next) {
            p_search = p_search->p_next;
        }
        p_search->p_next = p_new;
    } else {
        p_initial = p_new;
    }
    if (verboselevel & VERBOSEOBUFFEREXT) {
        printf("%sBestand nachher\n", ts(buf)); 
        printBuffer();
    }
}

bool OrderBuffer::delEntry(orderbuffer_t* p_del) {
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

bool OrderBuffer::delByNodeChannel(NODE_DATTYPE node_id, uint8_t channel) {
    int retval = false;
    orderbuffer_t *p_search;
    if (verboselevel & VERBOSEOBUFFER) printf("%sOrderBuffer:delByNodeChannel N:%u C:%u\n", ts(buf), node_id, channel); 
    if (verboselevel & VERBOSEOBUFFEREXT) {
        printf("%sBestand vorher\n", ts(buf)); 
        printBuffer();
    }
    p_search = p_initial;
    while (p_search) {
        if (p_search->node_id == node_id && p_search->channel == channel) {
            if (delEntry(p_search)) retval = true;
            p_search = NULL;
        } else {
            p_search=p_search->p_next;
        }
    }
    if (verboselevel & VERBOSEOBUFFEREXT) {
        printf("%sBestand nachher\n", ts(buf)); 
        printBuffer();
    }
    return retval;
}

bool OrderBuffer::delByNode(NODE_DATTYPE node_id) {
    int retval = false;
    orderbuffer_t *p_search;
    if (verboselevel & VERBOSEOBUFFER) printf("%sOrderBuffer: delByNode N:%u\n", ts(buf), node_id); 
    if (verboselevel & VERBOSEOBUFFEREXT) {
        printf("%sBestand vorher\n", ts(buf)); 
        printBuffer();
    }
    p_search = p_initial;
    while (p_search) {
        if (p_search->node_id == node_id) {
            if (delEntry(p_search)) retval = true;
        }
        p_search=p_search->p_next;
    }
    if (verboselevel & VERBOSEOBUFFEREXT) {
        printf("%sBestand nachher\n", ts(buf)); 
        printBuffer();
    }
    return retval;
}

bool OrderBuffer::nodeHasEntry(NODE_DATTYPE node_id) {
    int retval = false;
    orderbuffer_t *p_search;
    p_search = p_initial;
    while (p_search) {
        if (p_search->node_id == node_id) {
            retval = true;
            p_search = NULL;
        } else {
            p_search=p_search->p_next;
        }
    }
    return retval;
}

void OrderBuffer::addOrderBuffer(uint64_t millis, NODE_DATTYPE node_id, uint8_t channel, uint32_t data) {
    orderbuffer_t *p_new = new orderbuffer_t;
    if (p_new) {
        delByNodeChannel(node_id, channel);
        p_new->entrytime = millis;
        p_new->node_id = node_id;
        p_new->channel = channel;
        p_new->data = data;
        p_new->utime = time(0);
        newEntry(p_new);
    }
}

void* OrderBuffer::findOrder4Node(NODE_DATTYPE node_id, void* p_last, uint8_t* channel, uint32_t* data) {
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
            *data = p_search->data;
            retval = (void*)p_search;
            p_search = NULL;;
        } else {
            p_search=p_search->p_next;
        }
    }
    return retval;
}

void OrderBuffer::printBuffer(void) {
    orderbuffer_t *p_search;
    p_search = p_initial;
    printf("OrderBuffer: ---- Buffercontent ----\n"); 
    while (p_search) {
        printf("OrderBuffer: <%p> N:%u C:%u V:%g E:%s <%p>\n", 
               p_search, p_search->node_id, p_search->channel, p_search->data, utime2str(p_search->utime, buf, 1), p_search->p_next );
        p_search=p_search->p_next;
    }
    printf("OrderBuffer: -- END Buffercontent --\n"); 
}

void OrderBuffer::printBuffer2tn(int tn_socket) {
    char *client_message =  (char*) malloc (TELNETBUFFERSIZE);
    orderbuffer_t *p_search;
    p_search = p_initial;
    sprintf(client_message," ---- OrderBuffer ----\n"); 
    write(tn_socket , client_message , strlen(client_message));
    while (p_search) {
        switch (p_search->channel) {
            case 1 ... 40:
            case 101 ... 105:  
            {
                sprintf(client_message,"Node:%u Channel:%u Value:%g Entry:%s\n", 
                p_search->node_id, p_search->channel, getValue_f(p_search->data), utime2str(p_search->utime, buf, 1) );
            }
            break;
            case 41 ... 50:
            case 106 ... 110:
            {
                sprintf(client_message,"Node:%u Channel:%u Value:%d Entry:%s\n", 
                p_search->node_id, p_search->channel, getValue_i(p_search->data), utime2str(p_search->utime, buf, 1) );
            }
            break;
            case 51 ... 60:
            case 111 ... 125:
            {
                sprintf(client_message,"Node:%u Channel:%u Value:%u Entry:%s\n", 
                p_search->node_id, p_search->channel, getValue_ui(p_search->data), utime2str(p_search->utime, buf, 1) );
            }
            break;
            case 61 ... 80:
            {
                //ToDo
            }
        }            
        write(tn_socket , client_message , strlen(client_message));
        p_search=p_search->p_next;
    }
    free(client_message);
}

void OrderBuffer::htmlBuffer2tn(int tn_socket) {
    char *client_message =  (char*) malloc (TELNETBUFFERSIZE);
    orderbuffer_t *p_search;
    p_search = p_initial;
	sprintf(client_message,"\n<center><big>Orderbuffer</big><table><tr><th>Node</th><th>Channel</th><th>Value</th><th>Entry</th></tr>\n"); 
    write(tn_socket, client_message , strlen(client_message));
    while (p_search) {
		sprintf(client_message,"<tr><td>%u</td><td>%u</td><td>%g</td><td>%s</td></tr>\n", 
                p_search->node_id, p_search->channel, p_search->data, utime2str(p_search->utime, buf, 1) );
        write(tn_socket, client_message , strlen(client_message));
        p_search=p_search->p_next;
    }
	sprintf(client_message,"</table><br>\n"); 
	write(tn_socket, client_message , strlen(client_message));
    free(client_message);
}

void OrderBuffer::setVerbose(uint16_t _verboselevel) {
    verboselevel = _verboselevel;
}
