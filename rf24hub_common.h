// Global confif file
// schould also be included into the nodes
//
#ifndef _RF24HUBD_COMMON_H_   /* Include guard */
#define _RF24HUBD_COMMON_H_

#include <stdint.h>

//Payload V3
typedef struct {   
 uint8_t     node_id;         
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

typedef struct {
  uint8_t     node_id;         
  uint8_t     msg_id;          
  uint8_t     msg_type;        
  uint8_t     msg_flags;       
  uint8_t     orderno;         
  uint8_t     network_id;      
  uint8_t     reserved1;      
  uint8_t     reserved2;      
  uint32_t    data1;         
  uint32_t    data2;         
  uint32_t    data3;         
  uint32_t    data4;         
  uint32_t    data5;         
  uint32_t    data6;         
} udp_data_t;


typedef struct {
  char SSID[31];                    // SSID of WiFi
  char password[31];                // Password of WiFi
  char rf24HubHostName[31];         // Hostname or IP of RF24HUb
  uint8_t rf24hubHostIP[4];         // IP Address of Host (IP V4 only!!!)
  uint16_t rf24HubUdpPort;          // UDP Port of Hub
  char rf24GWHostname[31];          // Hostname of this Gateway
  uint16_t rf24GWTelnetPort;        // Telnet Port for this Gateway
  uint16_t rf24GWUdpPort;           // Udp Port for this Gateway
} configDataWifi_t;

typedef struct {
  uint8_t address2Node[5];          // This address will be used for messages from GW to Node 
  uint8_t address2GW[5];            // This address will be used for messages from Node to GW
  uint8_t rf24Channel;              // Channel for rf24
  char rf24Speed[12];               // Valid values are: RF24_250KBPS; RF24_1MBPS, RF24_2MBPS
} configDataRf24_t;

typedef struct {
  uint16_t    network_id;			// The Network ID
  uint16_t    node_id;				// The Node ID
} configNode_t;

enum sockType_t { TCP, UDP};

#endif // _RF24HUBD_COMMON_H_

