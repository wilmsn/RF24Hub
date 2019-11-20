#include "sensor.h"

Sensor::Sensor(void) {
    initial_ptr = NULL;
}

void Sensor::new_entry(Sensor::sensor_t* new_ptr) {
    Sensor::sensor_t *search_ptr;
    if (initial_ptr) {
        search_ptr = initial_ptr;
        while (search_ptr->next) {
            search_ptr = search_ptr->next;
        }
        search_ptr->next = new_ptr;
    } else {
        initial_ptr = new_ptr;
    }
}

bool Sensor::update_last_val(uint16_t node, uint8_t channel, float value, uint64_t mymillis) {
    bool retval = false;
    Sensor::sensor_t *search_ptr;
    search_ptr=initial_ptr;
    while (search_ptr) {
        if ( search_ptr->node == node && search_ptr->channel == channel ) {
            search_ptr->last_val = value;
            if ( mymillis - search_ptr->last_ts > 1000 ) {
                sprintf(logger->debug,"sensor.update_last_val: N: 0%o C:%u V:%f ", node, channel, value); 
                logger->logmsg(VERBOSEORDER, logger->debug);
                search_ptr->last_ts = mymillis;
                retval = true;
            } else {
                sprintf(logger->debug,"sensor.update_last_val: Old value - dropped!"); 
                logger->logmsg(VERBOSEORDER, logger->debug);
                search_ptr->last_ts = mymillis;
                retval = false;
            }
        }
        search_ptr=search_ptr->next;
    }
    return retval;
}

void Sensor::find_node_chanel(uint16_t* node_ptr, uint8_t* channel_ptr,char* fhem_dev, uint32_t mysensor) {
    Sensor::sensor_t *search_ptr;
    search_ptr=initial_ptr;
    while (search_ptr) {
        if ( strcmp(search_ptr->fhem_dev,fhem_dev) == 0 || search_ptr->sensor == mysensor ) {
            sprintf(logger->debug,"sensor.find_node_chanel: N: 0%o C:%u", search_ptr->node, search_ptr->channel); 
            logger->logmsg(VERBOSEORDER, logger->debug);
            *node_ptr = search_ptr->node;
            *channel_ptr = search_ptr->channel;
        }
        search_ptr=search_ptr->next;
    }
}

void Sensor::print_buffer(int new_tn_in_socket) {
    char *client_message =  (char*) malloc (TELNETBUFFERSIZE);
    sensor_t *search_ptr;
    search_ptr = initial_ptr;
    sprintf(client_message," ------ Sensor: ------\n"); 
    write(new_tn_in_socket , client_message , strlen(client_message));
    while (search_ptr) {
		sprintf(client_message,"Sensor: %u\tNode: 0%o,\tChannel:%u,\ttype: %c\tVal: %f\tTS:%llu\n", 
                 search_ptr->sensor, search_ptr->node, search_ptr->channel, search_ptr->s_type, search_ptr->last_val, search_ptr->last_ts);   
		write(new_tn_in_socket , client_message , strlen(client_message));
        search_ptr=search_ptr->next;
	}
    free(client_message);
}

void Sensor::begin(Logger* _logger) {
    logger = _logger;
}
