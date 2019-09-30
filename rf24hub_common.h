// Global confif file
// schould also be included into the nodes
//
#ifndef _RF24HUBD_COMMON_H_   /* Include guard */
#define _RF24HUBD_COMMON_H_

#include <stdint.h>
#include "rf24hub_config.h"
#include "rf24hub_data_types.h"
#include "rf24hub_payload_data.h"


enum sockType_t { TCP, UDP};
	
//extern char config_file[PARAM_MAXLEN_CONFIGFILE];
extern void init_system(void);

#endif // _RF24HUBD_COMMON_H_

