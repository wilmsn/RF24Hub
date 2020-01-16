#ifndef _PAYLOAD_H_   /* Include guard */
#define _PAYLOAD_H_

// Structure of our payload
typedef struct {   // Our payload can be 32 byte max.
  uint8_t    node_id;
  uint8_t     type;
  uint8_t     flags;
  uint8_t     orderno;
  uint8_t     reserved1;
  uint8_t     reserved2;
  uint8_t     reserved3;
  uint8_t     reserved4;
  uint32_t    data1;
  uint32_t    data2;
  uint32_t    data3;
  uint32_t    data4;
  uint32_t    data5;
  uint32_t    data6;
} payload_t;

#endif 
