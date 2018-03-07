// Global confif file
// schould also be included into the nodes
//
#ifndef _RF24HUBD_COMMON_H_   /* Include guard */
#define _RF24HUBD_COMMON_H_

#include <stdint.h>


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
payload_t payload;

#endif // _RF24HUBD_COMMON_H_

