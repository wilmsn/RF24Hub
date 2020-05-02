#include "order.h"

Order::Order(void) {
    hasOrder = false;
}

void* Order::findOrderNo(uint8_t orderno) {
    void* p_buffer = nextEntry(NULL);
    order_t* p_data;
    void* retval = NULL;
    while ( p_buffer ) {
        p_data = (order_t *)getDataPtr(p_buffer);
        if (p_data) {
            if ( p_data->orderno == orderno ) {
                retval = p_buffer;
                p_buffer = NULL;
            } else {
                p_buffer = nextEntry(p_buffer);
            }
        } else {
            printf("Order::findOrderNo => p_data <%p>\n", p_data);
        }
    }
    return retval;
}

void* Order::findNode(uint8_t node_id) {
    void* p_buffer = nextEntry(NULL);
    order_t* p_data;
    void* retval = NULL;
    while ( p_buffer ) {
        p_data = (order_t *)getDataPtr(p_buffer);
        if (p_data) {
            if ( p_data->node_id == node_id ) {
                retval = p_buffer;
                p_buffer = NULL;
            } else {
                p_buffer = nextEntry(p_buffer);
            }
        } else {
            printf("Order::findNode => p_data <%p>\n", p_data);
        }
    }
    return retval;
}

bool Order::delByOrderNo(uint8_t orderno) {
    bool retval = false;
    void* p_data;
    void* p_buffer = findOrderNo(orderno);
    if (p_buffer) {
        if (verboselevel & VERBOSEOTHER) {
            char buf[] = TSBUFFERSTRING;
            printf("%sOrder::delByOrderNo p_buffer:<%p> p_data:<%p>\n", log_ts(buf), p_buffer, getDataPtr(p_buffer));
        }
        p_data = getDataPtr(p_buffer);
        if (p_data) {
            free(p_data);
        } else {
            printf("Order::delByOrderNo => p_data <%p>\n", p_data);
        }
        delEntry(p_buffer);
        retval = true;
    }
    return retval;
}

bool Order::delByNode(uint8_t node_id) {
    bool retval = false;
    void* p_data;
    void* p_buffer = findNode(node_id);
    if (p_buffer) {
        if (verboselevel & VERBOSEOTHER) {
            char buf[] = TSBUFFERSTRING;
            printf("%sOrder::delByNode p_buffer:<%p> p_data:<%p>\n", log_ts(buf), p_buffer, getDataPtr(p_buffer));
        }
        p_data = getDataPtr(p_buffer);
        if (p_data) {
            free(p_data);
        } else {
            printf("Order::delByNode => p_data <%p>\n", p_data);
        }
        delEntry(p_buffer);
        retval = true;
    }
    return retval;
}
    
bool Order::isOrderNo(uint8_t orderno) {
    bool retval = false;
    void* p_buffer = findOrderNo(orderno);
    if (p_buffer) {
        retval = true;
    }
    return retval;
}

void Order::addOrder(uint8_t node_id, uint8_t msg_type, bool HB_order, uint32_t data, uint64_t entrytime) {
    order_t *p_data = new order_t;
    if (p_data) {
        orderno++;
        if (orderno == 0) orderno = 1;
        p_data->orderno = orderno;
        p_data->node_id = node_id;
        p_data->msg_id = 1;
        p_data->msg_type = msg_type;
        p_data->msg_flags = PAYLOAD_FLAG_EMPTY;
        p_data->HB_order = HB_order;
        p_data->data1 = data;
        p_data->data2 = 0;
        p_data->data3 = 0;
        p_data->data4 = 0;
        p_data->data5 = 0;
        p_data->data6 = 0;
        p_data->entrytime = entrytime;
        p_data->last_send = 0;
        newEntry(p_data);
        hasOrder = true;
    } else {
            printf("Order::addOrder => p_data <%p>\n", p_data);
        }
}

void Order::addEndOrder(uint8_t node_id, uint8_t msg_type, uint64_t entrytime) {
    addOrder(node_id, msg_type, true, 0, entrytime);    
    modifyOrderFlags(node_id, PAYLOAD_FLAG_LASTMESSAGE);
}

void Order::modifyOrder(uint8_t node_id, uint8_t pos, uint32_t data) {
    void* p_buffer = findNode(node_id);
    if (p_buffer) {
        order_t* p_data = (order_t*)getDataPtr(p_buffer);
        if (p_data) {
            switch (pos) {
                case 2:
                    p_data->data2 = data;
                break;
                case 3:
                    p_data->data3 = data;
                break;
                case 4:
                    p_data->data4 = data;
                break;
                case 5:
                    p_data->data5 = data;
                break;
                case 6:
                    p_data->data6 = data;
                break;
            }
        } else {
            printf("Order::modifyOrder => p_data <%p>\n", p_data);
        }
    }
    printBuffer(VERBOSEORDER);
}

void Order::modifyOrderFlags(uint8_t node_id, uint8_t msg_flags) {
    void* p_buffer = findNode(node_id);
    if (p_buffer) {
        order_t* p_data = (order_t*)getDataPtr(p_buffer);
        if (p_data) {
            p_data->msg_flags = msg_flags;
        } else {
            printf("Order::modifyOrderFlags => p_data <%p>\n", p_data);
        }
    }
    printBuffer(VERBOSEORDER);
}

bool Order::getOrderForTransmission(payload_t* payload, uint64_t mytime){
    char buf[] = TSBUFFERSTRING;
    bool retval = false;
//    bool isHbNode = false;
    uint64_t sendInterval;
    uint64_t deleteInterval;
    void* p_buffer = nextEntry(NULL);
    order_t* p_data;
    while (p_buffer) {
        p_data = (order_t *)getDataPtr(p_buffer);
        if ( p_data ) {
            p_data->HB_order? sendInterval=SENDINTERVAL_HB : sendInterval=SENDINTERVAL;
            p_data->HB_order? deleteInterval=DELETEINTERVAL_HB : deleteInterval=DELETEINTERVAL;
            if (verboselevel & VERBOSEOTHER) {
                printf("%sOrder::getOrderForTransmission Node: %u Onr: %u Last send: %llu Now: %llu Entry: %llu \n",log_ts(buf),p_data->node_id, p_data->orderno, p_data->last_send, mytime, p_data->entrytime);        
            } 
            if (p_data->last_send > mytime) p_data->last_send = mytime;
            if ( (p_data->last_send + sendInterval) < mytime) {
                payload->node_id = p_data->node_id;
                payload->msg_id = p_data->msg_id;
                payload->msg_type = p_data->msg_type;
                payload->msg_flags = p_data->msg_flags;
                payload->orderno = p_data->orderno;
                payload->data1 = p_data->data1;
                payload->data2 = p_data->data2;
                payload->data3 = p_data->data3;
                payload->data4 = p_data->data4;
                payload->data5 = p_data->data5;
                payload->data6 = p_data->data6;
                p_data->last_send = mytime;
                retval = true;
                if (verboselevel & VERBOSEORDER) {
                    printf("%sOrder: OrderNo: %u (Node:%u HB), TTL: %llu \n",log_ts(buf), p_data->orderno, p_data->node_id, p_data->entrytime + deleteInterval - mytime );
                } 
                switch (p_data->msg_type) {
                    case PAYLOAD_TYPE_HB_RESP:
                    {
                        if ( (p_data->entrytime + deleteInterval) < mytime ) {
                            if (verboselevel & VERBOSEORDER) {
                                printf("%sOrder: Timeout - lösche OrderNo: %u (Node:%u HB), entry:%llu last send: %llu Delinterv: %llu Sendinterv: %llu \n", log_ts(buf), p_data->orderno, p_data->node_id, p_data->entrytime, p_data->last_send, deleteInterval, sendInterval ); 
                            } 
                            delByOrderNo(p_data->orderno);
                            p_buffer = NULL;
                        }
                    }
                    break;
                    case PAYLOAD_TYPE_DAT:
                    {
                        if ( (p_data->entrytime + deleteInterval) < mytime ) {
                            if (verboselevel & VERBOSEORDER) {
                                printf("%sp_buffer: %p p_data: %p Order: Timeout - lösche OrderNo: %u (Node:%u ), entry:%llu last send: %llu Delinterv: %llu Sendinterv: %llu \n", log_ts(buf), p_buffer, p_data, p_data->orderno, p_data->node_id, p_data->entrytime, p_data->last_send, deleteInterval, sendInterval ); 
                            } 
                            delByOrderNo(p_data->orderno);
                            p_buffer = NULL;
                        }
                        
                    }
                    break;
                    case PAYLOAD_TYPE_DATSTOP:
                    {
                        if ( p_data->msg_id > SENDSTOPCOUNT ) {
                            if (verboselevel & VERBOSEORDER) {
                                printf("%sp_buffer: %p p_data: %p Order: Timeout - lösche OrderNo: %u (Node:%u ), entry:%llu last send: %llu Delinterv: %llu Sendinterv: %llu \n", log_ts(buf), p_buffer, p_data, p_data->orderno, p_data->node_id, p_data->entrytime, p_data->last_send, deleteInterval, sendInterval ); 
                            } 
                            delByOrderNo(p_data->orderno);
                            p_buffer = NULL;
                        }
                        
                    }
                    break;
                    case PAYLOAD_TYPE_PING_POW_MIN:
                    case PAYLOAD_TYPE_PING_POW_LOW:
                    case PAYLOAD_TYPE_PING_POW_HIGH:
                    case PAYLOAD_TYPE_PING_POW_MAX:
                    {
                    // ToDo Ping response
                    }
                    break;
                }
                p_data->msg_id++;
                p_buffer = NULL;
            }
        } else {
            printf("Order::getOrderForTransmission => p_data <%p>\n", p_data);
        }
        if (p_buffer) p_buffer = nextEntry(p_buffer);
    }
    return retval; 
}

void Order::printBuffer(uint16_t debuglevel) {
    char buf[] = TSBUFFERSTRING;
    order_t* p_data;
    void* p_buffer = nextEntry(NULL);
    if (verboselevel & debuglevel) {
        printf("%sOrder: ---- Buffercontent ----\n", log_ts(buf)); 
        printf("%sOrder: -- hasOrder = %s --\n", log_ts(buf), hasOrder? "true":"false"); 
        while ( p_buffer ) {
            p_data = (order_t *)getDataPtr(p_buffer);
            if (p_data) {
                printf("%sOrder: O:%u N:%u T:%u F:%02x (%u/%g) (%u/%g) (%u/%g) (%u/%g) (%u/%g) (%u/%g) E:%llu LS:%llu\n", log_ts(buf),
                    p_data->orderno, p_data->node_id, p_data->msg_type, p_data->msg_flags 
                    ,getChannel(p_data->data1), getValue_f(p_data->data1)
                    ,getChannel(p_data->data2), getValue_f(p_data->data2)
                    ,getChannel(p_data->data3), getValue_f(p_data->data3)
                    ,getChannel(p_data->data4), getValue_f(p_data->data4)
                    ,getChannel(p_data->data5), getValue_f(p_data->data5)
                    ,getChannel(p_data->data6), getValue_f(p_data->data6)
                    ,p_data->entrytime, p_data->last_send
                    );
            } else {
                printf("Order::printBuffer => p_data <%p>\n", p_data);
            }
            p_buffer = nextEntry(p_buffer);
        }
        printf("%sOrder: -- END Buffercontent --\n", log_ts(buf)); 
    }
}

void Order::printBuffer2tn(int new_tn_in_socket) {
    void* p_buffer = nextEntry(NULL);
    order_t* p_data;
    char *client_message =  (char*) malloc (TELNETBUFFERSIZE);
    sprintf(client_message,"Order: ---- Buffercontent ----\n"); 
    write(new_tn_in_socket , client_message , strlen(client_message));
    while ( p_buffer ) {
        p_data = (order_t *)getDataPtr(p_buffer);
        if (p_data) {
            sprintf(client_message,"Order: O:%u N:%u T:%u F:%02x (%u/%g) (%u/%g) (%u/%g) (%u/%g) (%u/%g) (%u/%g)\n", 
               p_data->orderno, p_data->node_id, p_data->msg_type, p_data->msg_flags 
                ,getChannel(p_data->data1), getValue_f(p_data->data1)
                ,getChannel(p_data->data2), getValue_f(p_data->data2)
                ,getChannel(p_data->data3), getValue_f(p_data->data3)
                ,getChannel(p_data->data4), getValue_f(p_data->data4)
                ,getChannel(p_data->data5), getValue_f(p_data->data5)
                ,getChannel(p_data->data6), getValue_f(p_data->data6)
               );
            write(new_tn_in_socket , client_message , strlen(client_message));
        } else {
            printf("Order::printBuffer2tn => p_data <%p>\n", p_data);
        }
        p_buffer = nextEntry(p_buffer);
    }
    sprintf(client_message,"Order: -- END Buffercontent --\n"); 
    write(new_tn_in_socket , client_message , strlen(client_message));
    free(client_message);
}

void Order::htmlBuffer2tn(int new_tn_in_socket) {
    void* p_buffer = nextEntry(NULL);
    order_t* p_data;
    char *client_message =  (char*) malloc (TELNETBUFFERSIZE);
	sprintf(client_message,"</table><br><big>Order</big><br><table><tr><th>OrderNo</th><th>Node</th><th>Type</th><th>Flags</th><th>Channel</th><th>Value</th></tr>\n"); 
    write(new_tn_in_socket , client_message , strlen(client_message));
    while ( p_buffer ) {
        p_data = (order_t *)getDataPtr(p_buffer);
        if (p_data) {
            sprintf(client_message,"<tr><td>%u</td><td>%u</td><td>%u</td><td>%u</td><td>%u<br>%u<br>%u<br>%u<br>%u<br>%u</td><td>%g<br>%g<br>%g<br>%g<br>%g<br>%g</td></tr>\n", 
               p_data->orderno, p_data->node_id, p_data->msg_type, p_data->msg_flags 
                ,getChannel(p_data->data1), getValue_f(p_data->data1)
                ,getChannel(p_data->data2), getValue_f(p_data->data2)
                ,getChannel(p_data->data3), getValue_f(p_data->data3)
                ,getChannel(p_data->data4), getValue_f(p_data->data4)
                ,getChannel(p_data->data5), getValue_f(p_data->data5)
                ,getChannel(p_data->data6), getValue_f(p_data->data6)
               );
            write(new_tn_in_socket , client_message , strlen(client_message));
            } else {
                printf("Order::htmlBuffer2tn => p_data <%p>\n", p_data);
            }
        p_buffer = nextEntry(p_buffer);
    }
    sprintf(client_message,"</table></center>\n"); 
    write(new_tn_in_socket , client_message , strlen(client_message));
    free(client_message);
}

