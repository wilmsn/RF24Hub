#include <EEPROM.h>
#include <rf24hub.h>
#include <strings.h>
#include <rf24hub.h>
#include "myConfig.h"

configDataWifi_t cfgWifi;
configDataRf24_t cfgRf24;

void setup() {
  strcpy(cfgWifi.SSID,MYSSID);
  strcpy(cfgWifi.password,MYPWD);
  strcpy(cfgWifi.rf24HubHostName,MYRF24HUBHOST);
  cfgWifi.rf24HubUdpPort = MYRF24HUBUDPPORT;
  strcpy(cfgWifi.rf24GWHostname,MYGWHOSTNAME);
  cfgWifi.rf24GWTelnetPort = MYGWTELNETPORT;
  cfgWifi.rf24GWUdpPort = MYGWUDPPORT;
  cfgWifi.rf24hubHostIP[0] = MYRF24HUBIP_1;
  cfgWifi.rf24hubHostIP[1] = MYRF24HUBIP_2;
  cfgWifi.rf24hubHostIP[2] = MYRF24HUBIP_3;
  cfgWifi.rf24hubHostIP[3] = MYRF24HUBIP_4;

  cfgRf24.rf24Channel=MYRF24CHANNEL;
  strcpy(cfgRf24.rf24Speed,MYRF24SPEED);

  EEPROM.begin(4095);
  EEPROM.put( 0, cfgWifi );
  EEPROM.put( sizeof(cfgWifi), cfgRf24 );
  
  delay(200);
  EEPROM.commit();                      // Only needed for ESP8266 to get data written
  EEPROM.end();                         // Free RAM copy of structure

}

void loop() {
  // put your main code here, to run repeatedly:

}
