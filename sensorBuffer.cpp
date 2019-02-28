#include "sensorBuffer.h"
#include <string.h>
#include <stdio.h> 
#include <iostream>

/*
 * Constructor
 * Hier wird ein Pointer für den Start der verketteten Liste angelegt.
 */
sensorBuffer::sensorBuffer(void)
{
    initial_sensorBuffer_ptr = NULL;
}

int sensorBuffer::newSensor(uint32_t sensor, uint16_t node, uint16_t channel, char s_type, char* fhem_dev, float last_val) 
{
    sensorBuffer::sensorBuffer_t *akt_sensorBuffer_ptr, *new_sensorBuffer_ptr;
    new_sensorBuffer_ptr = new sensorBuffer::sensorBuffer_t;
    if (! sensorBuffer::initial_sensorBuffer_ptr) {
        sensorBuffer::initial_sensorBuffer_ptr = new_sensorBuffer_ptr;
        akt_sensorBuffer_ptr = new_sensorBuffer_ptr;
    } else {
        akt_sensorBuffer_ptr = sensorBuffer::initial_sensorBuffer_ptr;
        while ( akt_sensorBuffer_ptr->next ) akt_sensorBuffer_ptr = akt_sensorBuffer_ptr->next;
        akt_sensorBuffer_ptr->next = new_sensorBuffer_ptr;
        akt_sensorBuffer_ptr = new_sensorBuffer_ptr;
    }
    akt_sensorBuffer_ptr->sensor = sensor;
    akt_sensorBuffer_ptr->node = node;
    akt_sensorBuffer_ptr->channel = channel;
    akt_sensorBuffer_ptr->s_type = s_type;
    akt_sensorBuffer_ptr->last_val = last_val;
    strcpy(akt_sensorBuffer_ptr->fhem_dev, fhem_dev);
    akt_sensorBuffer_ptr->next = NULL;
    return 1;
}

void sensorBuffer::listSensor(void)
{
    sensorBuffer::sensorBuffer_t *akt_sensorBuffer_ptr;
    akt_sensorBuffer_ptr = initial_sensorBuffer_ptr;
    while (akt_sensorBuffer_ptr) {
        printf("Sensor: %u Node: %u Channel: %u Value: %f S_Type: %u FHEM_Dev: %s \n",
               akt_sensorBuffer_ptr->sensor, akt_sensorBuffer_ptr->node, akt_sensorBuffer_ptr->channel, akt_sensorBuffer_ptr->last_val, akt_sensorBuffer_ptr->s_type, akt_sensorBuffer_ptr->fhem_dev);
        akt_sensorBuffer_ptr = akt_sensorBuffer_ptr->next;
    }    

}
