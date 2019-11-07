#ifndef _RF24HUB_DATA_TYPES_H_
#define _RF24HUB_DATA_TYPES_H_

typedef struct {
  uint16_t    network_id;
  uint16_t    node_id;
  uint16_t    msg_id;
  uint16_t    flags;
  uint32_t    sensor1;
  uint32_t    sensor2;
} udp_data_t;

typedef struct {
  uint16_t    network_id;
  uint16_t    node_id;
  uint16_t    msg_id;
  uint16_t    flags;
  uint32_t    sensor1;
  uint32_t    sensor2;
} payload_t;

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
  uint8_t rf24Speed;                // Valid values are: 0 = RF24_250KBPS; 1 = RF24_1MBPS, 2 = RF24_2MBPS
} configDataRf24_t;

typedef struct {
  uint16_t    network_id;			// The Network ID
  uint16_t    node_id;				// The Node ID
  uint16_t    sleeptime;            // Sleeptime to the next heartbeat    
  uint16_t    waketime;             // Time to stay awake after wakeup 
} configNode_t;

#endif
