#include "sensorclass.h"

SensorClass::SensorClass(void) {
    p_initial = NULL;
    verboseLevel = 0;
    buf = (char*)malloc(TSBUFFERSIZE);
    buf1 = (char*)malloc(TSBUFFERSIZE);
    tsbuf = (char*)malloc(TSBUFFERSIZE);
}

void SensorClass::cleanup(void) {
    sensor_t *p_search;
    p_search = p_initial;
    while ( p_search ) {
        p_initial = p_search->p_next;
        delete p_search;
        p_search = p_initial;
    }
}

void SensorClass::newEntry(SensorClass::sensor_t* p_new) {
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

void SensorClass::addSensor(uint32_t sensor_id, NODE_DATTYPE node_id, uint8_t channel, uint8_t datatype, char* fhem_dev, uint32_t last_utime, uint32_t last_data, char* sensor_name) {
    sensor_t* p_new = new SensorClass::sensor_t;
    p_new->sensor_id = sensor_id;
    p_new->node_id = node_id;
    p_new->channel = channel;
    p_new->datatype = datatype;
    p_new->last_utime = last_utime;
    p_new->last_data = last_data;
    sprintf(p_new->fhem_dev,"%s", fhem_dev);
    sprintf(p_new->sensor_name,"%s", sensor_name);
    newEntry(p_new);
}
   
bool SensorClass::updateLastVal(uint32_t sensor_id, uint32_t last_data) {
    bool retval = false;
    uint8_t channel = getChannel(last_data);
    sensor_t *p_search = p_initial;
    //p_search=p_initial;
    while (p_search) {
        if ( p_search->sensor_id == sensor_id) {
            p_search->last_data = last_data;
            p_search->last_utime = time(0);
            if ( verboseLevel & VERBOSESENSOR) 
                printf("%ssensor.updateLastVal: S:%u N:%u C:%u V:%s\n", ts(tsbuf), p_search->sensor_id, p_search->node_id
                        , p_search->channel, unpackTransportValue(last_data, buf) ); 
            retval = true;
        }
        p_search=p_search->p_next;
    }
    return retval;
}

uint32_t SensorClass::getSensorByNodeChannel(NODE_DATTYPE node_id, uint8_t channel) {
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

char* SensorClass::getFhemDevByNodeChannel(NODE_DATTYPE node_id, uint8_t channel) {
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

bool SensorClass::getNodeChannelBySensorID(NODE_DATTYPE* p_node_id, uint8_t* p_channel, uint32_t sensor_id) {
    sensor_t *p_search;
    bool retval = false;
    p_search=p_initial;
    while (p_search) {
        if ( p_search->sensor_id == sensor_id ) {
            if (verboseLevel & VERBOSESENSOR) printf("%ssensor.getNodeChannelBySensorID: S:%u N:%u C:%u\n", ts(tsbuf), p_search->sensor_id, p_search->node_id, p_search->channel); 
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

bool SensorClass::getNodeChannelByFhemDev(NODE_DATTYPE *p_node_id, uint8_t* p_channel, char* fhem_dev) {
    sensor_t *p_search;
    bool retval = false;
    p_search=p_initial;
    while (p_search) {
        if ( strcmp(p_search->fhem_dev,fhem_dev) == 0 ) {
            if (verboseLevel & VERBOSESENSOR) printf("%ssensor.getNodeChannelByFhemDev: FHEM:%s N:%u C:%u\n", ts(tsbuf), p_search->fhem_dev, p_search->node_id, p_search->channel); 
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

bool SensorClass::getNodeChannelBySensorName(NODE_DATTYPE *p_node_id, uint8_t* p_channel, char* sensor_name) {
    sensor_t *p_search;
    bool retval = false;
    p_search=p_initial;
    while (p_search) {
        if ( strcmp(p_search->sensor_name,sensor_name) == 0 ) {
            if (verboseLevel & VERBOSESENSOR) printf("%ssensor.getNodeChannelBySensorName: SensorName:%s N:%u C:%u\n", ts(tsbuf), p_search->sensor_name, p_search->node_id, p_search->channel); 
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

int8_t SensorClass::getDataTypeByNodeChannel(NODE_DATTYPE node_id, uint8_t channel) {
    sensor_t *p_search;
    int8_t retval = -1;
    p_search=p_initial;
    while (p_search) {
        if ( p_search->node_id == node_id && p_search->channel == channel ) {
            retval = p_search->datatype;
            p_search = NULL;    
        } else {
            p_search=p_search->p_next;
        }
    }
    return retval;
}

void SensorClass::printBuffer(int tn_socket, bool html) {
    char *client_message =  (char*) malloc (TELNETBUFFERSIZE);
    sensor_t *p_search;
    p_search = p_initial;
    sprintf(client_message," ------ Sensor: ------\n"); 
    write(tn_socket , client_message , strlen(client_message));
    while (p_search) {
        int fhemLen = strlen(p_search->fhem_dev);
	sprintf(client_message,"Sensor: %u\tNode: %u%s\tChannel: %u,\tFHEM: %s%s\tVal:%s\t%s(%s)\n", 
            p_search->sensor_id, 
            p_search->node_id, 
            p_search->node_id<10? "  ":p_search->node_id<100? " ":"",
            p_search->channel, 
            p_search->fhem_dev, 
            fhemLen<2? "\t\t\t\t":fhemLen<10? "\t\t\t":fhemLen<18? "\t\t":fhemLen<26? "\t":"",
            unpackTransportValue(p_search->last_data, buf), 
            strlen(unpackTransportValue(p_search->last_data, buf))<5? "\t":"", 
            utime2str(p_search->last_utime, buf1, 1) 
        );   
	write(tn_socket , client_message , strlen(client_message));
        p_search=p_search->p_next;
    }
    free(client_message);
}

void SensorClass::setVerbose(uint16_t _verboseLevel) {
    verboseLevel = _verboseLevel;
}

