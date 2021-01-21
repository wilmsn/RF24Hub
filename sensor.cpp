#include "sensor.h"

Sensor::Sensor(void) {
    p_initial = NULL;
    verboselevel = 0;
    buf = (char*)malloc(TSBUFFERSIZE);
    buf1 = (char*)malloc(TSBUFFERSIZE);
    tsbuf = (char*)malloc(TSBUFFERSIZE);
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

void Sensor::addSensor(uint32_t sensor_id, NODE_DATTYPE node_id, uint8_t channel, char* fhem_dev, uint32_t last_utime, uint32_t last_data) {
    sensor_t* p_new = new Sensor::sensor_t;
    p_new->sensor_id = sensor_id;
    p_new->node_id = node_id;
    p_new->channel = channel;
    p_new->last_utime = last_utime;
    p_new->last_data = last_data;
    sprintf(p_new->fhem_dev,"%s", fhem_dev);
    newEntry(p_new);
}
   
bool Sensor::updateLastVal(uint32_t sensor_id, uint32_t last_data) {
    bool retval = false;
    uint8_t channel = getChannel(last_data);
    sensor_t *p_search = p_initial;
    //p_search=p_initial;
    while (p_search) {
        if ( p_search->sensor_id == sensor_id) {
            p_search->last_data = last_data;
            p_search->last_utime = time(0);
            if ( verboselevel & VERBOSESENSOR) 
                printf("%ssensor.updateLastVal: S:%u N:%u C:%u V:%s\n", ts(tsbuf), p_search->sensor_id, p_search->node_id, p_search->channel, unpackTransportValue(last_data, buf) ); 
            retval = true;
        }
        p_search=p_search->p_next;
    }
    return retval;
}

bool::Sensor::isSystemRegister(bool isHBNode, uint8_t channel) {
  bool retval=false;
  if (isHBNode) {
    switch (channel) {
      case 102 ... 104:
      case 106 ... 107:
      case 111 ... 119:
      case 124:
          retval = true;
          break;
      default:
          retval = false;
    }
  } else {
    switch (channel) {
      case 102 ... 103:
      case 111 ... 112:
      case 116 ... 118:
      case 124:
          retval = true;
          break;
      default:
          retval = false;
    }
  }
  return retval;
}

uint32_t Sensor::getSensorByNodeChannel(NODE_DATTYPE node_id, uint8_t channel) {
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

char* Sensor::getFhemDevByNodeChannel(NODE_DATTYPE node_id, uint8_t channel) {
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

bool Sensor::getNodeChannelBySensorID(NODE_DATTYPE* p_node_id, uint8_t* p_channel, uint32_t sensor_id) {
    sensor_t *p_search;
    bool retval = false;
    p_search=p_initial;
    while (p_search) {
        if ( p_search->sensor_id == sensor_id ) {
            if (verboselevel & VERBOSESENSOR) printf("%ssensor.getNodeChannelBySensorID: S:%u N:%u C:%u\n", ts(tsbuf), p_search->sensor_id, p_search->node_id, p_search->channel); 
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

bool Sensor::getNodeChannelByFhemDev(NODE_DATTYPE *p_node_id, uint8_t* p_channel, char* fhem_dev) {
    sensor_t *p_search;
    bool retval = false;
    p_search=p_initial;
    while (p_search) {
        if ( strcmp(p_search->fhem_dev,fhem_dev) == 0 ) {
            if (verboselevel & VERBOSESENSOR) printf("%ssensor.getNodeChannelByFhemDev: FHEM:%s N:%u C:%u\n", ts(tsbuf), p_search->fhem_dev, p_search->node_id, p_search->channel); 
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

void Sensor::printBuffer(int tn_socket, bool html) {
    char *client_message =  (char*) malloc (TELNETBUFFERSIZE);
    sensor_t *p_search;
    p_search = p_initial;
    sprintf(client_message," ------ Sensor: ------\n"); 
    write(tn_socket , client_message , strlen(client_message));
    while (p_search) {
		sprintf(client_message,"Sensor: %u\tNode: %u,\tChannel:%u,\tFHEM: %s,\tVal: %s (%s)\n", 
                 p_search->sensor_id, p_search->node_id, p_search->channel, p_search->fhem_dev, unpackTransportValue(p_search->last_data, buf), utime2str(p_search->last_utime, buf1, 1) );   
		write(tn_socket , client_message , strlen(client_message));
        p_search=p_search->p_next;
	}
    free(client_message);
}

void Sensor::setVerbose(uint16_t _verboselevel) {
    verboselevel = _verboselevel;
}
