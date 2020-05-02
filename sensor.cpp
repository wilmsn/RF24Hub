#include "sensor.h"

Sensor::Sensor(void) {
}

void* Sensor::findSensor(uint32_t sensor_id) {
    void* p_buffer = nextEntry(NULL);
    sensor_t* p_data;
    void* retval = NULL;
    while ( p_buffer ) {
        p_data = (sensor_t *)getDataPtr(p_buffer);
        if (p_data) {
            if ( p_data->sensor_id == sensor_id ) {
                retval = p_buffer;
                p_buffer = NULL;
            } else {
                p_buffer = nextEntry(p_buffer);
            }
        } else {
            printf("Sensor::findSensor => p_data <%p>\n", p_data);
        }
    }
    return retval;
}

void Sensor::cleanup(void) {
    void* p_buffer;
    void* p_data;
    p_buffer = nextEntry(NULL);
    while ( p_buffer ) {    
        p_data = getDataPtr(p_buffer);
        if (p_data) {
            free(p_data);
        } else {
            printf("Sensor::cleanup => p_data <%p>\n", p_data);
        }
        delEntry(p_buffer);
        p_buffer = nextEntry(NULL);
    }
}

void Sensor::addSensor(uint32_t sensor_id, uint8_t node_id, uint8_t channel, char* fhem_dev, uint64_t last_ts, float last_val) {
    sensor_t* p_data = new Sensor::sensor_t;
    if (p_data) {
        p_data->sensor_id = sensor_id;
        p_data->node_id = node_id;
        p_data->channel = channel;
        p_data->last_ts = last_ts;
        p_data->last_val = last_val;
        sprintf(p_data->fhem_dev,"%s", fhem_dev);
        newEntry((void*)p_data);
    } else {
        printf("Sensor::addSensor => p_data <%p>\n", p_data);
    }
}
   
bool Sensor::updateLastVal(uint32_t sensor_id, float value, uint64_t mymillis) {
    bool retval = false;
    sensor_t* p_data = NULL;
    void* p_buffer = findSensor(sensor_id);
    if (p_buffer) {
        p_data = (sensor_t*)getDataPtr(p_buffer);
        if (p_data) {
            p_data->last_val = value;
            p_data->last_ts = mymillis;
            retval = true;
        } else {
            printf("Sensor::updateLastVal => p_data <%p>\n", p_data);
        }
    }
    return retval;
}

uint32_t Sensor::getSensorByNodeChannel(uint8_t node_id, uint8_t channel) {
    void* p_buffer = nextEntry(NULL);
    sensor_t* p_data;
    uint32_t retval = 0;
    while ( p_buffer ) {
        p_data = (sensor_t *)getDataPtr(p_buffer);
        if (p_data) {
            if ( p_data->node_id == node_id && p_data->channel == channel ) {
                retval = p_data->sensor_id;
                p_buffer = NULL;
            } else {
                p_buffer = nextEntry(p_buffer);
            }
        } else {
            printf("Sensor::getSensorByNodeChannel => p_data <%p>\n", p_data);
        }
    }
    return retval;
}

bool Sensor::getNodeChannelBySensorID(uint8_t* p_node_id, uint8_t* p_channel, uint32_t sensor_id) {
    void* p_buffer = nextEntry(NULL);
    sensor_t* p_data;
    bool retval = false;
    while ( p_buffer ) {
        p_data = (sensor_t *)getDataPtr(p_buffer);
        if (p_data) {
            if ( p_data->sensor_id == sensor_id ) {
                *p_node_id = p_data->node_id;
                *p_channel = p_data->channel;
                retval = true;
                p_buffer = NULL;
            } else {
                p_buffer = nextEntry(p_buffer);
            }
        } else {
            printf("Sensor::getNodeChannelBySensorID => p_data <%p>\n", p_data);
        }
    }
    return retval;
}

bool Sensor::getNodeChannelByFhemDev(uint8_t* p_node_id, uint8_t* p_channel, char* fhem_dev) {
    void* p_buffer = nextEntry(NULL);
    sensor_t* p_data;
    bool retval = false;
    while ( p_buffer ) {
        p_data = (sensor_t *)getDataPtr(p_buffer);
        if (p_data) {
            if ( strcmp(fhem_dev,p_data->fhem_dev) == 0 ) {
                *p_node_id = p_data->node_id;
                *p_channel = p_data->channel;
                retval = true;
                p_buffer = NULL;
            } else {
                p_buffer = nextEntry(p_buffer);
            }
        } else {
            printf("Sensor::getNodeChannelByFhemDev => p_data <%p>\n", p_data);
        }
    }
    return retval;
}

char* Sensor::getFhemDevByNodeChannel(uint8_t node_id, uint8_t channel) {
    void* p_buffer = nextEntry(NULL);
    sensor_t* p_data;
    char* retval = NULL;
    while ( p_buffer ) {
        p_data = (sensor_t *)getDataPtr(p_buffer);
        if (p_data) {
            if ( p_data->node_id == node_id && p_data->channel == channel ) {
                retval = p_data->fhem_dev;
                p_buffer = NULL;
            } else {
                p_buffer = nextEntry(p_buffer);
            }
        } else {
            printf("Sensor::getFhemDevByNodeChannel => p_data <%p>\n", p_data);
        }
    }
    return retval;
}

void Sensor::printBuffer2tn(int new_tn_in_socket) {
    void* p_buffer;
    sensor_t* p_data;
    p_buffer = nextEntry(NULL);
    char *client_message =  (char*) malloc (TELNETBUFFERSIZE);
    sprintf(client_message," ------ Sensors: ------\n"); 
    write(new_tn_in_socket , client_message , strlen(client_message));
    while ( p_buffer ) {
        p_data = (sensor_t *)getDataPtr(p_buffer);
        if (p_data) {
            sprintf(client_message,"Sensor: %u\tNode: %u,\tChannel:%u,\tFHEM: %s,\tVal: %f,\tTS:%llu \n",p_data->sensor_id, p_data->node_id, p_data->channel, p_data->fhem_dev, p_data->last_val, p_data->last_ts); 
            write(new_tn_in_socket , client_message , strlen(client_message));
            p_buffer = nextEntry(p_buffer);
        } else {
            printf("Sensor::printBuffer2tn => p_data <%p>\n", p_data);
        }
    }
    free(client_message);
}

void Sensor::printBuffer(uint16_t debuglevel) {
    char buf[] = TSBUFFERSTRING;
    void* p_buffer = nextEntry(NULL);
    sensor_t* p_data;
    if (debuglevel & verboselevel) {
        while ( p_buffer ) {
            p_data = (sensor_t *)getDataPtr(p_buffer);
            if (p_data) {
                printf("%sSensor: %u\tNode: %u,\tChannel:%u,\tFHEM: %s,\tVal: %f,\tTS:%llu \n",log_ts(buf),p_data->sensor_id, p_data->node_id, p_data->channel, p_data->fhem_dev, p_data->last_val, p_data->last_ts); 
                p_buffer = nextEntry(p_buffer);
            } else {
                printf("Sensor::printBuffer => p_data <%p>\n", p_data);
            }
        }
    }
}

