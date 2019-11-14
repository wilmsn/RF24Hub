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

