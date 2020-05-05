#include "sensor.h"

Sensor::Sensor(void) {
    p_initial = NULL;
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

void Sensor::addSensor(uint32_t sensor_id, uint8_t node_id, uint8_t channel, char* fhem_dev, uint64_t last_ts, float last_val) {
    sensor_t* p_new = new Sensor::sensor_t;
    p_new->sensor_id = sensor_id;
    p_new->node_id = node_id;
    p_new->channel = channel;
    p_new->last_ts = last_ts;
    p_new->last_val = last_val;
    sprintf(p_new->fhem_dev,"%s", fhem_dev);
    newEntry(p_new);
}
   
bool Sensor::updateLastVal(uint32_t sensor_id, float value, uint64_t mymillis) {
    bool retval = false;
    sensor_t *p_search;
    p_search=p_initial;
    char* buf = alloc_str(VERBOSEPOINTER,"Sensor::updateLastVal buf",TSBUFFERSIZE);
    while (p_search) {
        if ( p_search->sensor_id == sensor_id ) {
            p_search->last_val = value;
            if ( mymillis - p_search->last_ts > 1000 ) {
                if ( verboselevel & VERBOSEORDER) printf("%ssensor.update_last_val: S: %u V:%f \n", log_ts(buf), sensor_id, value); 
                p_search->last_ts = mymillis;
                retval = true;
            } else {
                if ( verboselevel & VERBOSEORDER) printf("%ssensor.update_last_val: Old value - dropped!\n", log_ts(buf)); 
                p_search->last_ts = mymillis;
                retval = false;
            }
            p_search = NULL;
        } else {
            p_search=p_search->p_next;
        }
    }
    free_str(VERBOSEPOINTER,"Sensor::updateLastVal buf",buf);
    return retval;
}

uint32_t Sensor::getSensorByNodeChannel(uint8_t node_id, uint8_t channel) {
    uint32_t retval = 0;
    sensor_t *p_search;
    p_search=p_initial;
    while (p_search) {
        if ( p_search->node_id == node_id && p_search->channel == channel ) {
            retval = p_search->sensor_id;
            p_search = NULL;
        } else {
            p_search=p_search->p_next;
        }
    }
    return retval;
}

bool Sensor::getNodeChannelByFhemDev(uint8_t* node_id, uint8_t* channel, char* fhem_dev) {
    sensor_t *p_search;
    p_search=p_initial;
    bool retval = false;
    while (p_search) {
        if ( strcmp(p_search->fhem_dev,fhem_dev) == 0 ) {
            *node_id = p_search->node_id;
            *channel = p_search->channel;
            retval = true;
            p_search = NULL;
        } else {
            p_search=p_search->p_next;
        }
    }
    return retval;
}

bool Sensor::getNodeChannelBySensorID(uint8_t* node_id, uint8_t* channel, uint32_t sensor_id) {
    sensor_t *p_search;
    p_search = p_initial;
    bool retval = false;
    while (p_search) {
        if ( p_search->sensor_id  == sensor_id ) {
            *node_id = p_search->node_id;
            *channel = p_search->channel;
            retval = true;
            p_search = NULL;
        } else {
            p_search=p_search->p_next;
        }
    }
    return retval;
}
    
char* Sensor::getFhemDevByNodeChannel(uint8_t node_id, uint8_t channel) {
    sensor_t *p_search;
    p_search=p_initial;
    char* retval;
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

void Sensor::printBuffer2tn(int new_tn_in_socket) {
    char *client_message =  (char*) malloc (TELNETBUFFERSIZE);
    sensor_t *p_search;
    p_search = p_initial;
    sprintf(client_message," ------ Sensor: ------\n"); 
    write(new_tn_in_socket , client_message , strlen(client_message));
    while (p_search) {
		sprintf(client_message,"Sensor: %u\tNode: %u,\tChannel:%u,\tFHEM: %s,\tVal: %f,\tTS:%llu\n", 
                 p_search->sensor_id, p_search->node_id, p_search->channel, p_search->fhem_dev, p_search->last_val, p_search->last_ts);   
		write(new_tn_in_socket , client_message , strlen(client_message));
        p_search=p_search->p_next;
	}
    free(client_message);
}

void Sensor::printBuffer(uint16_t debuglevel) {
    sensor_t *p_search;
    p_search = p_initial;    
    if ( verboselevel & VERBOSECONFIG) {    
        printf(" ------ Sensor: ------\n"); 
        while (p_search) {
            printf("Sensor: %u\tNode: %u,\tChannel:%u,\tFHEM: %s,\tVal: %f,\tTS:%llu\n", 
                 p_search->sensor_id, p_search->node_id, p_search->channel, p_search->fhem_dev, p_search->last_val, p_search->last_ts);   
            p_search=p_search->p_next;
        }
	}
}

