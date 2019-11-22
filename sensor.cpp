#include "sensor.h"

Sensor::Sensor(void) {
    initial_ptr = NULL;
}

void Sensor::new_entry(Sensor::sensor_t* new_ptr) {
    sensor_t *search_ptr;
    new_ptr->next = NULL;
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

void Sensor::add_sensor(uint32_t sensor, uint16_t node, uint8_t	channel, char s_type, char* fhem_dev, 
                        uint64_t last_ts, float last_val) {
    sensor_t* new_ptr = new Sensor::sensor_t;
    new_ptr->sensor = sensor;
    new_ptr->node = node;
    new_ptr->channel = channel;
    new_ptr->s_type = s_type;
    new_ptr->last_ts = last_ts;
    new_ptr->last_val = last_val;
    sprintf(new_ptr->fhem_dev,"%s", fhem_dev);
    new_entry(new_ptr);
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

void Sensor::find_fhem_dev(uint16_t* node_ptr, uint8_t* channel_ptr,char* fhem_dev) {
    Sensor::sensor_t *search_ptr;
    search_ptr=initial_ptr;
    while (search_ptr) {
        if ( search_ptr->node == *node_ptr && search_ptr->channel == *channel_ptr ) {
            sprintf(logger->debug,"sensor.find_fhem_dev: %s", search_ptr->fhem_dev); 
            logger->logmsg(VERBOSEORDER, logger->debug);
            sprintf(fhem_dev,"%s",search_ptr->fhem_dev);
        }
        search_ptr=search_ptr->next;
    }
}

void Sensor::print_buffer2tn(int new_tn_in_socket) {
    char *client_message =  (char*) malloc (TELNETBUFFERSIZE);
    sensor_t *search_ptr;
    search_ptr = initial_ptr;
    sprintf(client_message," ------ Sensor: ------\n"); 
    write(new_tn_in_socket , client_message , strlen(client_message));
    while (search_ptr) {
		sprintf(client_message,"Sensor: %u\tNode: 0%o,\tChannel:%u,\tFHEM: %s,\ttype: %c\tVal: %f\tTS:%llu\n", 
                 search_ptr->sensor, search_ptr->node, search_ptr->channel, search_ptr->fhem_dev, search_ptr->s_type, search_ptr->last_val, search_ptr->last_ts);   
		write(new_tn_in_socket , client_message , strlen(client_message));
        search_ptr=search_ptr->next;
	}
    free(client_message);
}

void Sensor::print_buffer2log(void) {
    sensor_t *search_ptr;
    search_ptr = initial_ptr;
    sprintf(logger->debug," ------ Sensor: ------"); 
    logger->logmsg(VERBOSECONFIG, logger->debug);
    while (search_ptr) {
		sprintf(logger->debug,"Sensor: %u\tNode: 0%o,\tChannel:%u,\tFHEM: %s,\ttype: %c\tVal: %f\tTS:%llu", 
                 search_ptr->sensor, search_ptr->node, search_ptr->channel, search_ptr->fhem_dev, search_ptr->s_type, search_ptr->last_val, search_ptr->last_ts);   
        logger->logmsg(VERBOSECONFIG, logger->debug);
        search_ptr=search_ptr->next;
	}
}

void Sensor::begin(Logger* _logger) {
    logger = _logger;
}
