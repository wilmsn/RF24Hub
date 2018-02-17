// Global confif file
// schould also be included into the nodes
//
#ifndef _RF24HUBD_COMMON_H_   /* Include guard */
#define _RF24HUBD_COMMON_H_

#include <stdint.h>


// Structure of our payload
struct payload_t {
  uint16_t      orderno;    // the orderno as primary key for our message for the nodes
//  uint16_t		flags;      // a field for varies flags
  char          value[10];  // the information that is send to the node
};
payload_t payload;

#endif // _RF24HUBD_COMMON_H_

