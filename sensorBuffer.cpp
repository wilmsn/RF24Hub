#include "sensorBuffer.h"
#include <string.h>
#include <stdio.h> 
#include <iostream>

/*
 * Constructor
 * Hier wird ein Pointer für den Start der verketteten Liste angelegt.
 */
SENSORBUFFER::SENSORBUFFER(void) {
    initial_sensorBuffer_ptr = NULL;
}

int SENSORBUFFER::newSensor(uint32_t sensor, uint16_t node, uint16_t channel, char* fhem_dev) {
    sensorBuffer_t *akt_sensorBuffer_ptr, *new_sensorBuffer_ptr;
    new_sensorBuffer_ptr = new sensorBuffer_t;
    if (! initial_sensorBuffer_ptr) {
        initial_sensorBuffer_ptr = new_sensorBuffer_ptr;
        akt_sensorBuffer_ptr = new_sensorBuffer_ptr;
    } else {
        akt_sensorBuffer_ptr = initial_sensorBuffer_ptr;
        while ( akt_sensorBuffer_ptr->next ) akt_sensorBuffer_ptr = akt_sensorBuffer_ptr->next;
        akt_sensorBuffer_ptr->next = new_sensorBuffer_ptr;
        akt_sensorBuffer_ptr = new_sensorBuffer_ptr;
    }
    akt_sensorBuffer_ptr->sensor = sensor;
    akt_sensorBuffer_ptr->node = node;
    akt_sensorBuffer_ptr->channel = channel;
//    akt_sensorBuffer_ptr->s_type = s_type;
//    akt_sensorBuffer_ptr->last_val = last_val;
printf(">a< Fhem_Dev: %s \n",fhem_dev);    
    akt_sensorBuffer_ptr->fhem_dev = static_cast<char*>(malloc(strlen(fhem_dev)+1));
printf("FHEM_DEV size: %lu\n",strlen(fhem_dev));
    strcpy(akt_sensorBuffer_ptr->fhem_dev, fhem_dev);
    akt_sensorBuffer_ptr->next = NULL;
printf("<<<<<\n");    
    return 1;
}

void SENSORBUFFER::clearSensor(void){
    sensorBuffer_t *akt_sensorBuffer_ptr, *next_sensorBuffer_ptr;
    akt_sensorBuffer_ptr = initial_sensorBuffer_ptr;
    while (akt_sensorBuffer_ptr) {
        initial_sensorBuffer_ptr = akt_sensorBuffer_ptr->next;
        free(akt_sensorBuffer_ptr->fhem_dev);
        delete akt_sensorBuffer_ptr;
        akt_sensorBuffer_ptr = initial_sensorBuffer_ptr;
    }
}

int SENSORBUFFER::findSensor(uint16_t *node, uint16_t *channel, char* fhem_dev){
    int retval = 0;
    sensorBuffer_t *akt_sensorBuffer_ptr;
    akt_sensorBuffer_ptr = initial_sensorBuffer_ptr;
    while (akt_sensorBuffer_ptr) {
        if (strcmp(fhem_dev, akt_sensorBuffer_ptr->fhem_dev) == 0) {
            *node=akt_sensorBuffer_ptr->node;
            *channel=akt_sensorBuffer_ptr->channel;
            retval = 1;
        }
        akt_sensorBuffer_ptr = akt_sensorBuffer_ptr->next;
    }
    return retval;
}

void SENSORBUFFER::listSensor(void) {
    sensorBuffer_t *akt_sensorBuffer_ptr;
    akt_sensorBuffer_ptr = initial_sensorBuffer_ptr;
    while (akt_sensorBuffer_ptr) {
        printf("Sensor: %u Node: %u Channel: %u Value: %f S_Type: %u FHEM_Dev: %s \n",
               akt_sensorBuffer_ptr->sensor, akt_sensorBuffer_ptr->node, akt_sensorBuffer_ptr->channel, akt_sensorBuffer_ptr->last_val, akt_sensorBuffer_ptr->s_type, akt_sensorBuffer_ptr->fhem_dev);
        akt_sensorBuffer_ptr = akt_sensorBuffer_ptr->next;
    }    

}
