#include "sensor.h"

Sensor::Sensor(void) {
    initial_ptr = NULL;
}

void Sensor::cleanup(void) {
    sensor_t *search_ptr;
    search_ptr = initial_ptr;
    while ( search_ptr ) {
        initial_ptr = search_ptr->next;
        free(search_ptr);
        search_ptr = initial_ptr;
    }
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

void Sensor::add_sensor(uint32_t sensor_id, uint16_t node_id, uint8_t	channel, char* fhem_dev, 
                        uint64_t last_ts, float last_val) {
    sensor_t* new_ptr = new Sensor::sensor_t;
    new_ptr->sensor_id = sensor_id;
    new_ptr->node_id = node_id;
    new_ptr->channel = channel;
    new_ptr->last_ts = last_ts;
    new_ptr->last_val = last_val;
    sprintf(new_ptr->fhem_dev,"%s", fhem_dev);
    new_entry(new_ptr);
}
   
bool Sensor::update_last_val(uint32_t sensor_id, float value, uint64_t mymillis) {
    bool retval = false;
    Sensor::sensor_t *search_ptr;
    search_ptr=initial_ptr;
    char *debug =  (char*) malloc (DEBUGSTRINGSIZE);
    while (search_ptr) {
        if ( search_ptr->sensor_id == sensor_id ) {
            search_ptr->last_val = value;
            if ( mymillis - search_ptr->last_ts > 1000 ) {
                sprintf(debug,"sensor.update_last_val: S: %u V:%f ", sensor_id, value); 
                logger->logmsg(VERBOSEORDER, debug);
                search_ptr->last_ts = mymillis;
                retval = true;
            } else {
                sprintf(debug,"sensor.update_last_val: Old value - dropped!"); 
                logger->logmsg(VERBOSEORDER, debug);
                search_ptr->last_ts = mymillis;
                retval = false;
            }
        }
        search_ptr=search_ptr->next;
    }
    return retval;
    free(debug);
}

uint32_t Sensor::getSensor(uint16_t node_id, uint8_t channel) {
    uint32_t retval = 0;
    Sensor::sensor_t *search_ptr;
    search_ptr=initial_ptr;
    while (search_ptr) {
        if ( search_ptr->node_id == node_id && search_ptr->channel == channel ) {
            retval = search_ptr->sensor_id;
        }
        search_ptr=search_ptr->next;
    }
    return retval;
}

void Sensor::find_node_chanel(uint16_t* node_ptr, uint8_t* channel_ptr, char* fhem_dev, uint32_t mysensor) {
    Sensor::sensor_t *search_ptr;
    search_ptr=initial_ptr;
    char *debug =  (char*) malloc (DEBUGSTRINGSIZE);
    while (search_ptr) {
        if ( strcmp(search_ptr->fhem_dev,fhem_dev) == 0 || search_ptr->sensor_id == mysensor ) {
            sprintf(debug,"sensor.find_node_chanel: N: %u C:%u", search_ptr->node_id, search_ptr->channel); 
            logger->logmsg(VERBOSEORDER, debug);
            *node_ptr = search_ptr->node_id;
            *channel_ptr = search_ptr->channel;
        }
        search_ptr=search_ptr->next;
    }
    free(debug);
}

void Sensor::find_fhem_dev(uint16_t* node_ptr, uint8_t* channel_ptr,char* fhem_dev) {
    Sensor::sensor_t *search_ptr;
    search_ptr=initial_ptr;
    char *debug =  (char*) malloc (DEBUGSTRINGSIZE);
    while (search_ptr) {
        if ( search_ptr->node_id == *node_ptr && search_ptr->channel == *channel_ptr ) {
            sprintf(debug,"sensor.find_fhem_dev: %s", search_ptr->fhem_dev); 
            logger->logmsg(VERBOSEORDER, debug);
            sprintf(fhem_dev,"%s",search_ptr->fhem_dev);
        }
        search_ptr=search_ptr->next;
    }
    free(debug);
}

void Sensor::print_buffer2tn(int new_tn_in_socket) {
    char *client_message =  (char*) malloc (TELNETBUFFERSIZE);
    sensor_t *search_ptr;
    search_ptr = initial_ptr;
    sprintf(client_message," ------ Sensor: ------\n"); 
    write(new_tn_in_socket , client_message , strlen(client_message));
    while (search_ptr) {
		sprintf(client_message,"Sensor: %u\tNode: %u,\tChannel:%u,\tFHEM: %s,\tVal: %f,\tTS:%llu\n", 
                 search_ptr->sensor_id, search_ptr->node_id, search_ptr->channel, search_ptr->fhem_dev, search_ptr->last_val, search_ptr->last_ts);   
		write(new_tn_in_socket , client_message , strlen(client_message));
        search_ptr=search_ptr->next;
	}
    free(client_message);
    debug_print_buffer(VERBOSETELNET);
}

void Sensor::debug_print_buffer(uint16_t debuglevel) {
    sensor_t *search_ptr;
    search_ptr = initial_ptr;
    char *debug =  (char*) malloc (DEBUGSTRINGSIZE);
    sprintf(debug," ------ Sensor: ------"); 
    logger->logmsg(debuglevel, debug);
    while (search_ptr) {
		sprintf(debug,"Sensor: %u\tNode: %u,\tChannel:%u,\tFHEM: %s,\tVal: %f,\tTS:%llu", 
                 search_ptr->sensor_id, search_ptr->node_id, search_ptr->channel, search_ptr->fhem_dev, search_ptr->last_val, search_ptr->last_ts);   
        logger->logmsg(debuglevel, debug);
        search_ptr=search_ptr->next;
	}
	free(debug);
}

void Sensor::begin(Logger* _logger) {
    logger = _logger;
}
