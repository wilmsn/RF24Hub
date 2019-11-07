// Global confif file
// schould also be included into the nodes
//
#ifndef _RF24HUBD_COMMON_H_   /* Include guard */
#define _RF24HUBD_COMMON_H_

#include <stdint.h>
#include "rf24hub_config.h"


// Achtung: Float arbeitet nur wenn die vorherigen Felder ein vielfaches vor 32 bit regeben!!!!
struct udp_data_t {
  uint16_t    network_id;
  uint16_t    node_id;
  uint16_t    msg_id;
  uint16_t    flags;
  uint32_t    sensor1;
  uint32_t    sensor2;
};

struct payload_t {
  uint16_t    network_id;
  uint16_t    node_id;
  uint16_t    msg_id;
  uint16_t    flags;
  uint32_t    sensor1;
  uint32_t    sensor2;
};


enum sockType_t { TCP, UDP};
	
//extern char config_file[PARAM_MAXLEN_CONFIGFILE];
extern void init_system(void);

#endif // _RF24HUBD_COMMON_H_

