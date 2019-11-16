#include "sensor.h"
#include <stdio.h> 
#include <iostream>

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
                search_ptr->last_ts = mymillis;
                retval = true;
            } else {
                search_ptr->last_ts = mymillis;
                retval = false;
            }
        }
        search_ptr=search_ptr->next;
    }
    return retval;
}
