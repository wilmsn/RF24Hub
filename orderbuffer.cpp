#include "orderbuffer.h"

OrderBuffer::OrderBuffer(void) {
    p_initial = NULL;
    verboselevel = 0;
    buf = (char*)malloc(TSBUFFERSIZE);
    buf1 = (char*)malloc(TSBUFFERSIZE);
    tsbuf = (char*)malloc(TSBUFFERSIZE);
}

void OrderBuffer::newEntry(OrderBuffer::orderbuffer_t* p_new) {
    orderbuffer_t *p_search;
    p_new->p_next = NULL;
    if (verboselevel & VERBOSEOBUFFER) 
        printf("%sOrderBuffer: newEntry <%p> N:%u C:%u V:%f\n", ts(tsbuf), p_new, p_new->node_id, p_new->channel, p_new->data); 
    if (verboselevel & VERBOSEOBUFFEREXT) {
        printf("%sBestand vorher:\n", ts(tsbuf)); 
        printBuffer(fileno(stdout), false);
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
        printf("%sBestand nachher\n", ts(tsbuf)); 
        printBuffer(fileno(stdout), false);
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
    if (verboselevel & VERBOSEOBUFFER) printf("%sOrderBuffer:delByNodeChannel N:%u C:%u\n", ts(tsbuf), node_id, channel); 
    if (verboselevel & VERBOSEOBUFFEREXT) {
        printf("%sBestand vorher\n", ts(tsbuf)); 
        printBuffer(fileno(stdout), false);
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
        printf("%sBestand nachher\n", ts(tsbuf)); 
        printBuffer(fileno(stdout), false);
    }
    return retval;
}

bool OrderBuffer::delByNode(NODE_DATTYPE node_id) {
    int retval = false;
    orderbuffer_t *p_search;
    if (verboselevel & VERBOSEOBUFFER) printf("%sOrderBuffer: delByNode N:%u\n", ts(tsbuf), node_id); 
    if (verboselevel & VERBOSEOBUFFEREXT) {
        printf("%sBestand vorher\n", ts(tsbuf)); 
        printBuffer(fileno(stdout), false);
    }
    p_search = p_initial;
    while (p_search) {
        if (p_search->node_id == node_id) {
            if (delEntry(p_search)) retval = true;
        }
        p_search=p_search->p_next;
    }
    if (verboselevel & VERBOSEOBUFFEREXT) {
        printf("%sBestand nachher\n", ts(tsbuf)); 
        printBuffer(fileno(stdout), false);
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

void* OrderBuffer::findOrder4Node(NODE_DATTYPE node_id, void* p_last, uint32_t* p_data) {
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
            *p_data = p_search->data;
            retval = (void*)p_search;
            p_search = NULL;
        } else {
            p_search=p_search->p_next;
        }
    }
    return retval;
}

void OrderBuffer::printBuffer(int out_socket, bool htmlFormat) {
    char* client_message =  (char*) malloc (TELNETBUFFERSIZE);
    orderbuffer_t *p_search;
    bool writeTS = ( out_socket == fileno(stdout) );
    p_search = p_initial;
    if ( htmlFormat ) {
        sprintf(client_message,"\n<center><big>Orderbuffer</big><table><tr><th>Node</th><th>Channel</th><th>Value</th><th>Entry</th></tr>\n"); 
    } else {
        if (writeTS) sprintf(client_message,"%s",ts(tsbuf));
        sprintf(client_message," ---- OrderBuffer ----\n"); 
    }        
    write(out_socket , client_message , strlen(client_message));
    while (p_search) {
        if ( htmlFormat ) {
            sprintf(client_message,"<tr><td>%u</td><td>%u</td><td>%s</td><td>%s</td></tr>\n", 
            p_search->node_id, p_search->channel, unpackTransportValue(p_search->data, buf1), utime2str(p_search->utime, buf, 1) );
        } else {
            if (writeTS) sprintf(client_message,"%s",ts(tsbuf));
            sprintf(client_message,"Node:%u Channel:%u Value:%s Entry:%s\n", 
            p_search->node_id, p_search->channel, unpackTransportValue(p_search->data, buf1), utime2str(p_search->utime, buf, 1) );
        }
        write(out_socket , client_message , strlen(client_message));
        p_search=p_search->p_next;
    }
    if ( htmlFormat ) {
        sprintf(client_message,"</table><br>\n"); 
        write(out_socket, client_message , strlen(client_message));
    }
    free(client_message);
}

void OrderBuffer::setVerbose(uint16_t _verboselevel) {
    verboselevel = _verboselevel;
}

void OrderBuffer::setKey(uint32_t _key) {
    mykey = _key;
}
