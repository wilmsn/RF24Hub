#define RF24CHANNEL 10
#define RF24ADDRESS2NODE1 0x61
#define RF24ADDRESS2NODE2 0x61
#define RF24ADDRESS2NODE3 0x61
#define RF24ADDRESS2NODE4 0x61
#define RF24ADDRESS2NODE5 0x61
#define RF24ADDRESS2GW1 0x62
#define RF24ADDRESS2GW2 0x62
#define RF24ADDRESS2GW3 0x62
#define RF24ADDRESS2GW4 0x62
#define RF24ADDRESS2GW5 0x62
#define RF24SPEED 0
#define NETWORKID 5813
#define NODEID 100
#define SLEEPTIME 2
#define WAKETIME 2
#include <EEPROM.h>
#include <rf24hub.h>

struct eeprom_data_t {
  configDataRf24_t configDataRf24;
  configNode_t configNode;
};

eeprom_data_t eeprom_data;

void setup() {
  eeprom_data.configDataRf24.address2Node[0] = RF24ADDRESS2NODE1;
  eeprom_data.configDataRf24.address2Node[1] = RF24ADDRESS2NODE2;
  eeprom_data.configDataRf24.address2Node[2] = RF24ADDRESS2NODE3;
  eeprom_data.configDataRf24.address2Node[3] = RF24ADDRESS2NODE4;
  eeprom_data.configDataRf24.address2Node[4] = RF24ADDRESS2NODE5;
  eeprom_data.configDataRf24.address2GW[0] = RF24ADDRESS2GW1;
  eeprom_data.configDataRf24.address2GW[1] = RF24ADDRESS2GW2;
  eeprom_data.configDataRf24.address2GW[2] = RF24ADDRESS2GW3;
  eeprom_data.configDataRf24.address2GW[3] = RF24ADDRESS2GW4;
  eeprom_data.configDataRf24.address2GW[4] = RF24ADDRESS2GW5;
  eeprom_data.configDataRf24.rf24Channel=RF24CHANNEL;
  eeprom_data.configDataRf24.rf24Speed = RF24SPEED;
  eeprom_data.configNode.network_id=NETWORKID;
  eeprom_data.configNode.node_id=NODEID;
  eeprom_data.configNode.waketime=WAKETIME;
  eeprom_data.configNode.sleeptime=SLEEPTIME;
  int eeAddress = 0;  
  EEPROM.put(eeAddress, eeprom_data);
}

void loop() {}
