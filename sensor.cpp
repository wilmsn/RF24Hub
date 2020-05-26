#include "sensor.h"

Sensor::Sensor(void) {
    p_initial = NULL;
    verboselevel = 0;
    buf = (char*)alloc_str(VERBOSEPOINTER,"Order::Order buf",TSBUFFERSIZE);
}

void Sensor::cleanup(void) {
    sensor_t *p_search;
    p_search = p_initial;
    while ( p_search ) {
        p_initial = p_search->p_next;
        delete p_search;
        p_search = p_initial;
    }
}

void Sensor::newEntry(Sensor::sensor_t* p_new) {
    sensor_t *p_search;
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

void Sensor::addSensor(uint32_t sensor_id, NODE_DATTYPE node_id, uint8_t channel, char* fhem_dev, 
                        uint32_t last_val_utime, float last_val, int16_t last_int, uint16_t last_ui) {
    sensor_t* p_new = new Sensor::sensor_t;
    p_new->sensor_id = sensor_id;
    p_new->node_id = node_id;
    p_new->channel = channel;
    p_new->last_ts = 0;
    p_new->last_val_utime = last_val_utime;
    p_new->last_val = last_val;
    p_new->last_int = last_int;
    p_new->last_ui = last_ui;
    sprintf(p_new->fhem_dev,"%s", fhem_dev);
    newEntry(p_new);
}
   
bool Sensor::updateLastVal_f(uint32_t sensor_id, float value, uint64_t mymillis) {
    bool retval = false;
    sensor_t *p_search;
    p_search=p_initial;
    while (p_search) {
        if ( p_search->sensor_id == sensor_id) {
            if ( mymillis - p_search->last_ts > 1000 ) {
                p_search->last_val = value;
                p_search->last_val_utime = time(0);
                p_search->last_ts = mymillis;
                if ( verboselevel & VERBOSESENSOR) 
                    printf("%ssensor.updateLastVal: S:%u N:%u C:%u V:%f\n", ts(buf), p_search->sensor_id, p_search->node_id, p_search->channel, value); 
                retval = true;
            } else {
                if ( verboselevel & VERBOSESENSOR) printf("%ssensor.updateLastVal: Old value - dropped!\n", ts(buf)); 
                p_search->last_ts = mymillis;
            }
        }
        p_search=p_search->p_next;
    }
    return retval;
}

bool Sensor::updateLastVal_i(uint32_t sensor_id, int16_t value, uint64_t mymillis) {
    bool retval = false;
    sensor_t *p_search;
    p_search=p_initial;
    while (p_search) {
        if ( p_search->sensor_id == sensor_id) {
            if ( mymillis - p_search->last_ts > 1000 ) {
                p_search->last_int = value;
                p_search->last_val_utime = time(0);
                p_search->last_ts = mymillis;
                if ( verboselevel & VERBOSESENSOR) 
                    printf("%ssensor.updateLastVal: S:%u N:%u C:%u V:%d\n", ts(buf), p_search->sensor_id, p_search->node_id, p_search->channel, value); 
                retval = true;
            } else {
                if ( verboselevel & VERBOSESENSOR) printf("%ssensor.updateLastVal: Old value - dropped!\n", ts(buf)); 
                p_search->last_ts = mymillis;
            }
        }
        p_search=p_search->p_next;
    }
    return retval;
}

bool Sensor::updateLastVal_ui(uint32_t sensor_id, uint16_t value, uint64_t mymillis) {
    bool retval = false;
    sensor_t *p_search;
    p_search=p_initial;
    while (p_search) {
        if ( p_search->sensor_id == sensor_id) {
            if ( mymillis - p_search->last_ts > 1000 ) {
                p_search->last_ui = value;
                p_search->last_val_utime = time(0);
                p_search->last_ts = mymillis;
                if ( verboselevel & VERBOSESENSOR) 
                    printf("%ssensor.updateLastVal: S:%u N:%u C:%u V:%u\n", ts(buf), p_search->sensor_id, p_search->node_id, p_search->channel, value); 
                retval = true;
            } else {
                if ( verboselevel & VERBOSESENSOR) printf("%ssensor.updateLastVal: Old value - dropped!\n", ts(buf)); 
                p_search->last_ts = mymillis;
            }
        }
        p_search=p_search->p_next;
    }
    return retval;
}

uint32_t Sensor::getSensorByNodeChannel(NODE_DATTYPE node_id, uint8_t channel) {
    uint32_t retval = 0;
    sensor_t *p_search;
    p_search=p_initial;
    while (p_search) {
        if ( p_search->node_id == node_id && p_search->channel == channel ) {
            retval = p_search->sensor_id;
        }
        p_search=p_search->p_next;
    }
    return retval;
}

char* Sensor::getFhemDevByNodeChannel(NODE_DATTYPE node_id, uint8_t channel) {
    char* retval = NULL;
    sensor_t *p_search;
    p_search=p_initial;
    while (p_search) {
        if ( p_search->node_id == node_id && p_search->channel == channel ) {
            retval = p_search->fhem_dev;
            p_search = NULL;    
        } else {
            p_search=p_search->p_next;
        }
    }
    return retval;
}

bool Sensor::getNodeChannelBySensorID(NODE_DATTYPE* p_node_id, uint8_t* p_channel, uint32_t sensor_id) {
    sensor_t *p_search;
    bool retval = false;
    p_search=p_initial;
    while (p_search) {
        if ( p_search->sensor_id == sensor_id ) {
            if (verboselevel & VERBOSESENSOR) printf("%ssensor.getNodeChannelBySensorID: S:%u N:%u C:%u\n", ts(buf), p_search->sensor_id, p_search->node_id, p_search->channel); 
            *p_node_id = p_search->node_id;
            *p_channel = p_search->channel;
            p_search = NULL;
            retval = true;
        } else {
            p_search=p_search->p_next;
        }
    }
    return retval;
}

bool Sensor::getNodeChannelByFhemDev(NODE_DATTYPE *p_node_id, uint8_t* p_channel, char* fhem_dev) {
    sensor_t *p_search;
    bool retval = false;
    p_search=p_initial;
    while (p_search) {
        if ( strcmp(p_search->fhem_dev,fhem_dev) == 0 ) {
            if (verboselevel & VERBOSESENSOR) printf("%ssensor.getNodeChannelByFhemDev: FHEM:%s N:%u C:%u\n", ts(buf), p_search->fhem_dev, p_search->node_id, p_search->channel); 
            *p_node_id = p_search->node_id;
            *p_channel = p_search->channel;
            p_search = NULL;
            retval = true;
        } else {
            p_search=p_search->p_next;
        }
    }
    return retval;
}

void Sensor::printBuffer2tn(int tn_socket) {
    char *client_message =  (char*) malloc (TELNETBUFFERSIZE);
    sensor_t *p_search;
    p_search = p_initial;
    sprintf(client_message," ------ Sensor: ------\n"); 
    write(tn_socket , client_message , strlen(client_message));
    while (p_search) {
		sprintf(client_message,"Sensor: %u\tNode: %u,\tChannel:%u,\tFHEM: %s,\tVal: %f (%s)\n", 
                 p_search->sensor_id, p_search->node_id, p_search->channel, p_search->fhem_dev, p_search->last_val, utime2str(p_search->last_val_utime,buf,1) );   
		write(tn_socket , client_message , strlen(client_message));
        p_search=p_search->p_next;
	}
    free(client_message);
}

void Sensor::printBuffer(void) {
    sensor_t *p_search;
    p_search = p_initial;
    printf(" ------ Sensor: ------\n"); 
    while (p_search) {
		printf("Sensor: %u\tNode: %u,\tChannel:%u,\tFHEM: %s,\tVal: %f (%s)\n", 
                 p_search->sensor_id, p_search->node_id, p_search->channel, p_search->fhem_dev, p_search->last_val, utime2str(p_search->last_val_utime,buf,1));   
        p_search=p_search->p_next;
	}
}

void Sensor::setVerbose(uint16_t _verboselevel) {
    verboselevel = _verboselevel;
}
