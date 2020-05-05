#include "order.h"

Order::Order(void) {
    p_initial = NULL;
    has_order = false;
}

void Order::newEntry(order_t* p_new) {
    order_t *p_search;
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
    has_order = true;
}

bool Order::delEntry(order_t* p_del) {
    bool retval = false;
    order_t* p_search;
    order_t* p_tmp;
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
    if ( ! p_initial) has_order = false;
    return retval;
}

bool Order::delByOrderNo(uint8_t orderno) {
    bool retval = false;
    order_t* p_search;
    p_search = p_initial;
    while (p_search) {
        if (p_search->orderno == orderno ) {
            if (delEntry(p_search)) retval = true;
        }
        p_search=p_search->p_next;
    }
    return retval;
}

bool Order::delByNode(uint8_t node_id) {
    bool retval = false;
    order_t *p_del;
    p_del = findNode(node_id);
    if ( p_del) {
        delEntry(p_del);
        retval = true;
    }
    return retval;
}
    
bool Order::isOrderNo(uint8_t orderno) {
    int retval = false;
    order_t *p_search;
    p_search = p_initial;
    while (p_search) {
        if (p_search->orderno == orderno ) {
            retval = true;
            p_search = NULL;
        } else {
            p_search=p_search->p_next;
        }
    }
    return retval;
}
    
Order::order_t* Order::findNode(uint8_t node_id) {
    order_t* retval = NULL;
    order_t *p_search;
    p_search = p_initial;
    while (p_search) {
        if (p_search->node_id == node_id ) {
            retval = p_search;
            p_search = NULL;
        } else {
            p_search=p_search->p_next;
        }
    }
    return retval;
}

void Order::addOrder(uint8_t node_id, uint8_t msg_type, bool HB_order, uint32_t data, uint64_t entrytime) {
    order_t *p_new = new order_t;
    orderno++;
//    if ( orderno > 50000 ) orderno = 1;
    p_new->orderno = orderno;
    p_new->node_id = node_id;
    p_new->msg_type = msg_type;
    p_new->msg_flags = PAYLOAD_FLAG_EMPTY;
    p_new->HB_order = HB_order;
    p_new->data1 = data;
    p_new->data2 = 0;
    p_new->data3 = 0;
    p_new->data4 = 0;
    p_new->data5 = 0;
    p_new->data6 = 0;
    p_new->entrytime = entrytime;
    p_new->last_send = 0;
    newEntry(p_new);
}

void Order::addEndOrder(uint8_t node_id, uint8_t msg_type, uint64_t entrytime) {
    addOrder(node_id, msg_type, true, 0, entrytime);    
    modifyOrderFlags(node_id, PAYLOAD_FLAG_LASTMESSAGE);
}

void Order::modifyOrder(uint8_t node_id, uint8_t pos, uint32_t data) {
    order_t* p_buffer = NULL;
    p_buffer=findNode(node_id);
    if (p_buffer) {
        switch (pos) {
            case 2:
                p_buffer->data2 = data;
            break;
            case 3:
                p_buffer->data3 = data;
            break;
            case 4:
                p_buffer->data4 = data;
            break;
            case 5:
                p_buffer->data5 = data;
            break;
            case 6:
                p_buffer->data6 = data;
            break;
        }
    }
}

void Order::modifyOrderFlags(uint8_t node_id, uint8_t msg_flags) {
    order_t* p_buffer = NULL;
    p_buffer=findNode(node_id);
    if (p_buffer) {
        p_buffer->msg_flags = msg_flags;
    }
//    printBuffer(VERBOSEORDER);
}

bool Order::getOrderForTransmission(payload_t* payload, uint64_t mytime){
    bool retval = false;
    order_t *p_del = NULL;
    order_t *p_search;
    p_search = p_initial;
    uint64_t sendInterval;
    uint64_t deleteInterval;
    while (p_search) {
        if (p_search->last_send > mytime) p_search->last_send = mytime;
        p_search->HB_order? sendInterval=SENDINTERVAL_HB : sendInterval=SENDINTERVAL;
        p_search->HB_order? deleteInterval=DELETEINTERVAL_HB : deleteInterval=DELETEINTERVAL;
        if ( (p_search->last_send + sendInterval) < mytime) {
            payload->node_id = p_search->node_id;
            payload->msg_id = p_search->msg_id;
            payload->msg_type = p_search->msg_type;
            payload->msg_flags = p_search->msg_flags;
            payload->orderno = p_search->orderno;
            payload->data1 = p_search->data1;
            payload->data2 = p_search->data2;
            payload->data3 = p_search->data3;
            payload->data4 = p_search->data4;
            payload->data5 = p_search->data5;
            payload->data6 = p_search->data6;
            p_search->last_send = mytime;
            retval = true;
            switch (p_search->msg_type) {
                    case PAYLOAD_TYPE_HB_RESP:
                    {
                        if ( (p_search->entrytime + deleteInterval) < mytime ) {
                            delByOrderNo(p_search->orderno);
                            //p_search = NULL;
                        }
                    }
                    break;
                    case PAYLOAD_TYPE_DAT:
                    {
                        if ( (p_search->entrytime + deleteInterval) < mytime ) {
                            delByOrderNo(p_search->orderno);
                            //p_search = NULL;
                        }
                        
                    }
                    break;
                    case PAYLOAD_TYPE_DATSTOP:
                    {
                        if ( p_search->msg_id > SENDSTOPCOUNT ) {
                            delByOrderNo(p_search->orderno);
                            //p_search = NULL;
                        }
                        
                    }
                    break;
/*                    case PAYLOAD_TYPE_PING_POW_MIN:
                    case PAYLOAD_TYPE_PING_POW_LOW:
                    case PAYLOAD_TYPE_PING_POW_HIGH:
                    case PAYLOAD_TYPE_PING_POW_MAX:
                    {
                    // ToDo Ping response
                    }
                    break; */
            }
            p_search->msg_id++;
            p_search = NULL;
            retval = true;
        } else {
            p_search = p_search->p_next;
        }
    }
    if (p_del) {
        delEntry(p_del);
    }
    return retval;
}

void Order::printBuffer(uint16_t debuglevel) {
    order_t *p_search;
    p_search = p_initial;
    char* buf = alloc_str(VERBOSEPOINTER,"Order::printBuffer buf",TSBUFFERSIZE);
    if (verboselevel & debuglevel) {
        printf("%sOrder: ---- Buffercontent ----"), log_ts(buf); 
        while (p_search) {
            printf("%sOrder: <%p> O:%u N:%u T:%u F:%02x (%u/%g) (%u/%g) (%u/%g) (%u/%g) (%u/%g) (%u/%g)", log_ts(buf), 
                    p_search, p_search->orderno, p_search->node_id, p_search->msg_type, p_search->msg_flags 
                    ,getChannel(p_search->data1), getValue_f(p_search->data1)
                    ,getChannel(p_search->data2), getValue_f(p_search->data2)
                    ,getChannel(p_search->data3), getValue_f(p_search->data3)
                    ,getChannel(p_search->data4), getValue_f(p_search->data4)
                    ,getChannel(p_search->data5), getValue_f(p_search->data5)
                    ,getChannel(p_search->data6), getValue_f(p_search->data6)    );
            p_search=p_search->p_next;
        }
        printf("%sOrder: -- END Buffercontent --", log_ts(buf)); 
    }
    free_str(VERBOSEPOINTER,"Order::printBuffer buf",buf);
}

void Order::printBuffer2tn(int new_tn_in_socket) {
    order_t *p_search;
    char* client_message = alloc_str(VERBOSEPOINTER,"Order::printBuffer2tn client_message",TELNETBUFFERSIZE);
    p_search = p_initial;
    sprintf(client_message,"------ Order: --------\n"); 
    write(new_tn_in_socket , client_message , strlen(client_message));
    while (p_search) {
        sprintf(client_message,"Order: <%p> O:%u N:%u T:%u F:%02x (%u/%g) (%u/%g) (%u/%g) (%u/%g) (%u/%g) (%u/%g)", 
                p_search, p_search->orderno, p_search->node_id, p_search->msg_type, p_search->msg_flags 
                ,getChannel(p_search->data1), getValue_f(p_search->data1)
                ,getChannel(p_search->data2), getValue_f(p_search->data2)
                ,getChannel(p_search->data3), getValue_f(p_search->data3)
                ,getChannel(p_search->data4), getValue_f(p_search->data4)
                ,getChannel(p_search->data5), getValue_f(p_search->data5)
                ,getChannel(p_search->data6), getValue_f(p_search->data6)
               );
        p_search=p_search->p_next;
    }
    free_str(VERBOSEPOINTER,"Order::printBuffer2tn client_message",client_message);
}

void Order::htmlBuffer2tn(int new_tn_in_socket) {
    order_t *p_search;
    char* client_message = alloc_str(VERBOSEPOINTER,"Order::printBuffer2tn client_message",TELNETBUFFERSIZE);
    p_search = p_initial;
	sprintf(client_message,"</table><br><big>Order</big><br><table><tr><th>OrderNo</th><th>Node</th><th>Type</th><th>Flags</th><th>Channel</th><th>Value</th></tr>\n"); 
    write(new_tn_in_socket , client_message , strlen(client_message));
    while (p_search) {
        sprintf(client_message,"<tr><td>%u</td><td>%u</td><td>%u</td><td>%u</td><td>%u<br>%u<br>%u<br>%u<br>%u<br>%u</td><td>%g<br>%g<br>%g<br>%g<br>%g<br>%g</td></tr>\n", 
        p_search->orderno, p_search->node_id, 
        getChannel(p_search->data1), 
        getChannel(p_search->data2), 
        getChannel(p_search->data3), 
        getChannel(p_search->data4), 
        getChannel(p_search->data5), 
        getChannel(p_search->data6),  
        getValue_f(p_search->data1), 
        getValue_f(p_search->data2),
        getValue_f(p_search->data3),
        getValue_f(p_search->data4),
        getValue_f(p_search->data5),
        getValue_f(p_search->data6)  );
        write(new_tn_in_socket , client_message , strlen(client_message));
        p_search=p_search->p_next;
    }
	sprintf(client_message,"</table></center>\n"); 
	write(new_tn_in_socket , client_message , strlen(client_message));
    free_str(VERBOSEPOINTER,"Order::printBuffer2tn client_message",client_message);
}

