#include "order.h"

Order::Order(void) {
    p_initial = NULL;
    has_order = false;
    orderno = 1;
    verboselevel = 0;
    buf = (char*)malloc(TSBUFFERSIZE);
    tsbuf = (char*)malloc(TSBUFFERSIZE);
    buf1 = (char*)malloc(20);
    buf2 = (char*)malloc(20);
    buf3 = (char*)malloc(20);
    buf4 = (char*)malloc(20);
    buf5 = (char*)malloc(20);
    buf6 = (char*)malloc(20);
}

bool Order::hasEntry(void) {
    return has_order;    
}

void Order::newEntry(order_t* p_new) {
    order_t *p_search;
    p_new->p_next = NULL;
    if (verboselevel & VERBOSEORDER) printf("%sOrder: newEntry <%p>\n", ts(tsbuf), p_new); 
    if (verboselevel & VERBOSEORDEREXT) {
        printf("%sBestand vorher\n",ts(tsbuf)); 
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
    if (verboselevel & VERBOSEORDEREXT) {
        printf("%sBestand nachher\n",ts(tsbuf)); 
        printBuffer(fileno(stdout), false);
    }
    has_order = true;
}

bool Order::delEntry(order_t* p_del) {
    bool retval = false;
    order_t *p_search, *p_tmp;
    p_search = p_initial;
    
    p_tmp = p_initial;
    if (verboselevel & VERBOSEORDEREXT) {
        printf("%sBestand vorher\n",ts(tsbuf)); 
        printBuffer(fileno(stdout), false);
    }
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
    if (verboselevel & VERBOSEORDER) printf("%sOrder: delEntry <%p> %s\n", ts(tsbuf), p_del, retval? "true":"false"); 
    if (verboselevel & VERBOSEORDEREXT) {
        printf("%sBestand nachher\n",ts(tsbuf)); 
        printBuffer(fileno(stdout), false);
    }
    return retval;
}

bool Order::delByOrderNo(ONR_DATTYPE orderno) {
    bool retval = false;
    order_t *p_search;
    if (verboselevel & VERBOSEORDER) printf("%sOrder: del_orderno %u\n", ts(tsbuf), orderno); 
    if (verboselevel & VERBOSEORDEREXT) {
        printf("%sBestand vorher\n", ts(tsbuf)); 
        printBuffer(fileno(stdout), false);
    }
    p_search = p_initial;
    while (p_search) {
        if (p_search->orderno == orderno ) {
            if (delEntry(p_search)) retval = true;
        }
        p_search=p_search->p_next;
    }
    if (verboselevel & VERBOSEORDEREXT) {
        printf("%sBestand nachher\n", ts(tsbuf)); 
        printBuffer(fileno(stdout), false);
    }
    return retval;
}

bool Order::delByNode(NODE_DATTYPE node_id) {
    bool retval = false;
    order_t *p_search;
    if (verboselevel & VERBOSEORDER) printf("%sOrder: del_node N:%u\n", ts(tsbuf), node_id); 
    if (verboselevel & VERBOSEORDEREXT) {
        printf("%sBestand vorher\n", ts(tsbuf)); 
        printBuffer(fileno(stdout), false);
    }
    
    p_search = Order::p_initial;
    while (p_search) {
        if (p_search->node_id == node_id ) {
            retval = Order::delEntry(p_search);
        }
        p_search=p_search->p_next;
    }
    if (verboselevel & VERBOSEORDEREXT) {
        printf("%sBestand nachher\n", ts(tsbuf)); 
        printBuffer(fileno(stdout), false);
    }
    return retval;
}
    
bool Order::isOrderNo(ONR_DATTYPE orderno) {
    int retval = false;
    order_t *p_search;
    p_search = p_initial;
    while (p_search) {
        if (p_search->orderno == orderno ) {
            retval = true;
        }
        p_search=p_search->p_next;
    }
    return retval;
}
    
Order::order_t* Order::findNode(NODE_DATTYPE node_id) {
    order_t* retval = NULL;
    order_t *p_search;
    p_search = p_initial;
    while (p_search) {
        if (p_search->node_id == node_id ) {
            retval = p_search;
        }
        p_search=p_search->p_next;
    }
    return retval;
}

void Order::addOrder(NODE_DATTYPE node_id, uint8_t msg_type, bool HB_order, uint32_t data, uint64_t entrytime) {
    order_t *p_new = new order_t;
    orderno++;
    if ( orderno == 0 ) orderno = 1;
    if ( p_new) {
        if (verboselevel & VERBOSEORDER) printf("%sOrder: addOrder <%p> N:%u T:%u HB:%s D1:(%u/%s)\n", ts(tsbuf), p_new, node_id, msg_type, HB_order? "true":"false", getChannel(data), unpackTransportValue(data,buf) ); 
        p_new->orderno = orderno;
        p_new->node_id = node_id;
        p_new->msg_id = 1;
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
        p_new->p_next = NULL;
        newEntry(p_new);
    }
}

void Order::modifyOrder(NODE_DATTYPE node, uint8_t pos, uint32_t data) {
    order_t* p_mod=NULL;
    p_mod=findNode(node);
    if (p_mod) {
        switch (pos) {
            case 2:
                p_mod->data2 = data;
            break;
            case 3:
                p_mod->data3 = data;
            break;
            case 4:
                p_mod->data4 = data;
            break;
            case 5:
                p_mod->data5 = data;
            break;
            case 6:
                p_mod->data6 = data;
            break;
        }
    }
    if (verboselevel & VERBOSEORDEREXT) printBuffer(fileno(stdout), false);
}

void Order::modifyOrderFlags(NODE_DATTYPE node_id, uint8_t msg_flags) {
    order_t* p_mod=NULL;
    p_mod=findNode(node_id);
    if (p_mod) {
        p_mod->msg_flags = msg_flags;
    }
    if (verboselevel & VERBOSEORDEREXT) printBuffer(fileno(stdout), false);
}

void Order::adjustEntryTime(NODE_DATTYPE node_id, uint64_t newEntrytime) {
    order_t *p_search;
    p_search = p_initial;
    while (p_search) {
        if (p_search->node_id == node_id ) {
            p_search->entrytime = newEntrytime;
        }
        p_search=p_search->p_next;
    }    
}

bool Order::getOrderForTransmission(payload_t* payload, uint64_t mytime){
    bool retval = false;
    order_t *p_delme = NULL;
    order_t *p_search;
    uint64_t sendInterval;
    uint64_t deleteInterval;
    uint64_t stopmsg_deleteInterval;
    p_search = p_initial;
    while (p_search) {
        if (verboselevel & VERBOSEORDEREXT) {
            printf("%sOrder::getOrderForTransmission N:%u O:%u Last send: %llu Now: %llu Entry: %llu\n",ts(tsbuf),p_search->node_id, p_search->orderno, p_search->last_send, mytime, p_search->entrytime);        
        }
        if (p_search->last_send > mytime) p_search->last_send = mytime;
        p_search->HB_order? sendInterval=SENDINTERVAL_HB : sendInterval=SENDINTERVAL;
        p_search->HB_order? deleteInterval=DELETEINTERVAL_HB : deleteInterval=DELETEINTERVAL;
        if ( (p_search->last_send + sendInterval) < mytime) {
            payload->orderno = p_search->orderno;
            payload->node_id = p_search->node_id;
            payload->msg_id = p_search->msg_id++;
            payload->msg_type = p_search->msg_type;
            payload->msg_flags = p_search->msg_flags;
            payload->data1 = p_search->data1;
            payload->data2 = p_search->data2;
            payload->data3 = p_search->data3;
            payload->data4 = p_search->data4;
            payload->data5 = p_search->data5;
            payload->data6 = p_search->data6;
            p_search->last_send = mytime;
            if (verboselevel & VERBOSEORDER) {
                printf("%sOrder::getOrderForTransmission <%p> O: %u (N:%u %s), TTL: %llu\n", ts(tsbuf), p_search, p_search->orderno, p_search->node_id, p_search->HB_order? "HB":"  ", p_search->entrytime + (uint64_t)deleteInterval - mytime ); 
            }
            if ( (p_search->entrytime + (uint64_t)deleteInterval < mytime) || 
                (p_search->msg_type == PAYLOAD_TYPE_DATSTOP && (p_search->entrytime + (SENDSTOPCOUNT * sendInterval)) < mytime) ) {
                p_delme = p_search;
                if (verboselevel & VERBOSEORDER) {
                    printf("%sOrder::getOrderForTransmission Timeout - lösche <%p> O:%u (N:%u %s), entry:%llu last send: %llu Delinterv: %llu Sendinterv: %llu\n", ts(tsbuf), p_delme, p_delme->orderno, p_delme->node_id, p_search->HB_order? "HB":"  ", p_delme->entrytime, p_delme->last_send, (uint64_t)deleteInterval, (uint64_t)sendInterval ); 
                }
                delEntry(p_delme);
            }
            retval = true;
        }
        p_search = p_search->p_next;
    }
    return retval;
}

void Order::printBuffer(int out_socket, bool htmlFormat) {
    order_t *p_search;
    bool writeTS = ( out_socket == fileno(stdout) );
    char *client_message =  (char*) malloc (TELNETBUFFERSIZE);
    p_search = p_initial;
    if (htmlFormat) {
        sprintf(client_message,"</table><br><big>Order</big><br><table><tr><th>OrderNo</th><th>Node</th><th>Type</th><th>Flags</th><th>Channel</th><th>Value</th></tr>\n"); 
    } else {
        if (writeTS) sprintf(client_message,"%s",ts(tsbuf));
        sprintf(client_message,"------ Order: --------\n"); 
    }
    write(out_socket , client_message , strlen(client_message));
    while (p_search) {
        if (htmlFormat) {
            sprintf(client_message,"<tr><td>%u</td><td>%u</td><td>%u</td><td>%u</td><td>%u<br>%u<br>%u<br>%u<br>%u<br>%u</td><td>%s<br>%s<br>%s<br>%s<br>%s<br>%s</td></tr>\n", 
            p_search->orderno, p_search->node_id, 
            getChannel(p_search->data1), 
            getChannel(p_search->data2), 
            getChannel(p_search->data3), 
            getChannel(p_search->data4), 
            getChannel(p_search->data5), 
            getChannel(p_search->data6),  
            unpackTransportValue(p_search->data1, buf1), 
            unpackTransportValue(p_search->data2, buf2),
            unpackTransportValue(p_search->data3, buf3),
            unpackTransportValue(p_search->data4, buf4),
            unpackTransportValue(p_search->data5, buf5),
            unpackTransportValue(p_search->data6, buf6)  );
        } else {
            if (writeTS) sprintf(client_message,"%s",ts(tsbuf));
            sprintf(client_message,"Order: <%p> O:%u N:%u T:%u F:%02x (%u/%s) (%u/%s) (%u/%s) (%u/%s) (%u/%s) (%u/%s) <%p>\n", 
                p_search, p_search->orderno, p_search->node_id, p_search->msg_type, p_search->msg_flags 
                ,getChannel(p_search->data1), unpackTransportValue(p_search->data1, buf1)
                ,getChannel(p_search->data2), unpackTransportValue(p_search->data2, buf2)
                ,getChannel(p_search->data3), unpackTransportValue(p_search->data3, buf3)
                ,getChannel(p_search->data4), unpackTransportValue(p_search->data4, buf4)
                ,getChannel(p_search->data5), unpackTransportValue(p_search->data5, buf5)
                ,getChannel(p_search->data6), unpackTransportValue(p_search->data6, buf6)
                ,p_search->p_next
               );
        }
        write(out_socket , client_message , strlen(client_message));
        p_search=p_search->p_next;
    }
    if (htmlFormat) {
        sprintf(client_message,"</table></center>\n"); 
        write(out_socket , client_message , strlen(client_message));
    }
    free(client_message);
}

void Order::setVerbose(uint16_t _verboselevel) {
    verboselevel = _verboselevel;
}
