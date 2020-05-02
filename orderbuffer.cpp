#include "orderbuffer.h"

OrderBuffer::OrderBuffer(void) {
}

void* OrderBuffer::findNode(uint8_t node_id) {
    void* p_buffer = nextEntry(NULL);
    orderbuffer_t* p_data;
    void* retval = NULL;
    while ( p_buffer ) {
        p_data = (orderbuffer_t *)getDataPtr(p_buffer);
        if (p_data) {
            if ( p_data->node_id == node_id ) {
                retval = p_buffer;
                p_buffer = NULL;
            } else {
                p_buffer = nextEntry(p_buffer);
            }
        } else {
            printf("OrderBuffer::findNode <%p>\n", p_data);
        }
    }
    return retval;
}

bool OrderBuffer::delByNodeChannel(uint8_t node_id, uint8_t channel) {
    void* p_buffer = nextEntry(NULL);
    orderbuffer_t* p_data;
    bool retval = false;
    while ( p_buffer ) {
        p_data = (orderbuffer_t *)getDataPtr(p_buffer);
        if (p_data) {
            if ( p_data->node_id == node_id && p_data->channel == channel ) {
                free(p_data);
                delEntry(p_buffer);
                p_buffer = NULL;
                retval = true;
            } else {
                p_buffer = nextEntry(p_buffer);
            }
        } else {
            printf("OrderBuffer::delByNodeChannel <%p>\n", p_data);
        }
    }
    return retval;
}

bool OrderBuffer::delByNode(uint8_t node_id) {
    bool retval = false;
    void* p_data;
    void* p_buffer = findNode(node_id);
    while ( p_buffer ) {
        p_data = getDataPtr(p_buffer);
        if (p_data) {
            free(p_data);
        } else {
            printf("OrderBuffer::delByNode <%p>\n", p_data);
        }
        delEntry(p_buffer);
        p_buffer = findNode(node_id);
        retval = true;
    }
    return retval;
}

bool OrderBuffer::nodeHasEntry(uint8_t node_id) {
    bool retval = false;
    if ( findNode(node_id) ) retval = true;
    return retval;
}

void OrderBuffer::addOrderBuffer(uint64_t millis, uint8_t node_id, uint8_t channel, float value) {
    orderbuffer_t *p_data = new orderbuffer_t;
    delByNodeChannel(node_id, channel);
    if (p_data) {
        p_data->entrytime = millis;
        p_data->node_id = node_id;
        p_data->channel = channel;
        p_data->value = value;
        newEntry(p_data);
    } else {
        printf("OrderBuffer::addOrderBuffer <%p>\n", p_data);
    }
}

void *OrderBuffer::findOrder4Node(uint8_t node_id, void* p_last, uint8_t* channel, float* value) {
    orderbuffer_t* p_data;
    void* p_buffer = nextEntry(p_last);
    void* retval = NULL;
    while ( p_buffer && ! retval ) {
        p_data = (orderbuffer_t*)getDataPtr(p_buffer);
        if ( p_data ) {
            if ( p_data->node_id == node_id ) {
                *channel = p_data->channel;
                *value = p_data->value;
                retval = p_buffer;
            }
        } else {
            printf("OrderBuffer::findOrder4Node <%p>\n", p_data);
        }
        p_buffer = nextEntry(p_buffer);
    } 
    return retval; 
}

void OrderBuffer::printBuffer(uint16_t debuglevel) {
    char buf[] = TSBUFFERSTRING;
    void* p_buffer = nextEntry(NULL);
    orderbuffer_t* p_data;
    if (debuglevel & verboselevel) {
        printf("%s OrderBuffer: ---- Buffercontent ----\n", log_ts(buf)); 
        while ( p_buffer ) {
            p_data = (orderbuffer_t *)getDataPtr(p_buffer);
            if ( p_data ) {
                printf("%s OrderBuffer: E:%llu N:%u C:%u V:%g \n", log_ts(buf), p_data->entrytime, p_data->node_id, p_data->channel, p_data->value );
            } else {
                printf("OrderBuffer::printBuffer <%p>\n", p_data);
            }
            p_buffer = nextEntry(p_buffer);
        }
        printf("%s OrderBuffer: -- END Buffercontent --\n", log_ts(buf)); 
    }
}

void OrderBuffer::printBuffer2tn(int new_tn_in_socket) {
    char *client_message =  (char*) malloc (TELNETBUFFERSIZE);
    void* p_buffer = nextEntry(NULL);
    orderbuffer_t* p_data;
    sprintf(client_message, "OrderBuffer: ---- Buffercontent ----\n"); 
	write(new_tn_in_socket , client_message , strlen(client_message));
    while ( p_buffer ) {
        p_data = (orderbuffer_t *)getDataPtr(p_buffer);
        if ( p_data ) {
            sprintf(client_message, "OrderBuffer: E:%llu N:%u C:%u V:%g \n", p_data->entrytime, p_data->node_id, p_data->channel, p_data->value );
            write(new_tn_in_socket , client_message , strlen(client_message));
        } else {
            printf("OrderBuffer::printBuffer2tn <%p>\n", p_data);
        }
        p_buffer = nextEntry(p_buffer);
    }
    sprintf(client_message, "OrderBuffer: -- END Buffercontent --\n"); 
    write(new_tn_in_socket , client_message , strlen(client_message));
    free(client_message);
}

void OrderBuffer::htmlBuffer2tn(int new_tn_in_socket) {
    char *client_message =  (char*) malloc (TELNETBUFFERSIZE);
    void* p_buffer = nextEntry(NULL);
    orderbuffer_t* p_data;
    sprintf(client_message,"\n<center><big>Orderbuffer</big><table><tr><th>EntryTime</th><th>Node</th><th>Channel</th><th>Value</th></tr>\n"); 
    write(new_tn_in_socket , client_message , strlen(client_message));
    while ( p_buffer ) {
        p_data = (orderbuffer_t *)getDataPtr(p_buffer);
        if ( p_data ) {
            sprintf(client_message,"<tr><td>%llu</td><td>%u</td><td>%u</td><td>%g</td></tr>\n", 
                    p_data->entrytime, p_data->node_id, p_data->channel, p_data->value );
            write(new_tn_in_socket , client_message , strlen(client_message));
        } else {
            printf("OrderBuffer::htmlBuffer2tn <%p>\n", p_data);
        }
        p_buffer = nextEntry(p_buffer);
    }
	sprintf(client_message,"</table><br>\n"); 
	write(new_tn_in_socket , client_message , strlen(client_message));
    free(client_message);
}
