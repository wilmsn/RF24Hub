// Global confif file
// schould also be included into the nodes
//
#ifndef _RF24HUBD_COMMON_H_   /* Include guard */
#define _RF24HUBD_COMMON_H_

#include <stdint.h>
#include "rf24hub_config.h"

#ifndef TESTBUILD
#include <RF24/RF24.h>
#include <RF24/utility/RPi/bcm2835.h>
#include <RF24Network/RF24Network.h>
#endif //TESTBUILD
#ifdef TESTBUILD
typedef enum { RF24_1MBPS = 0, RF24_2MBPS, RF24_250KBPS } rf24_datarate_e;



#endif //TESTBUILD

// Structure of our payload
struct payload_t {
  uint16_t      orderno;      // the orderno as primary key for our message for the nodes
  uint16_t		flags;        // a field for varies flags
  uint8_t		channel1;     // the channel of sensor1 on the node
  uint8_t		channel2;     // the channel of sensor2 on the node
  uint8_t		channel3;     // the channel of sensor3 on the node
  uint8_t		channel4;     // the channel of sensor4 on the node
  float 		value1;       // the value from or for sensor1
  float 		value2;       // the value from or for sensor2
  float 		value3;       // the value from or for sensor3
  float 		value4;       // the value from or for sensor4
};
extern payload_t payload;


//extern char config_file[PARAM_MAXLEN_CONFIGFILE];
extern void init_system(void);

#endif // _RF24HUBD_COMMON_H_

