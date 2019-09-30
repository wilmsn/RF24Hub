/*************************************************************************
 * A simple testdata generator
 *************************************************************************/

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <EEPROM.h>
#include <rf24hub.h>
#include <strings.h>

configDataWifi_t cfgWifi;
configDataRf24_t cfgRf24;
udp_data_t udp_data;
payload_t payload;

uint8_t i;
  
#define MAX_TELNET_CLIENTS 2

WiFiServer TelnetServer(0);
WiFiClient TelnetClient[MAX_TELNET_CLIENTS];
WiFiUDP Udp;
unsigned int remotePort;  // remote port to listen on
uint16_t loopno;
uint8_t sensor;
int ledState = LOW;

void setup()
{
  EEPROM.begin(4095);
  EEPROM.get( 0, cfgWifi );
  EEPROM.get( sizeof(cfgWifi), cfgRf24 );
  EEPROM.end();

  WiFi.mode(WIFI_STA);
  WiFi.hostname(cfgWifi.rf24GWHostname);
  WiFi.begin(cfgWifi.SSID, cfgWifi.password);

  Udp.begin(cfgWifi.rf24GWUdpPort);
  TelnetServer.begin(cfgWifi.rf24GWTelnetPort);

  // ... Give ESP 10 seconds to connect to station.
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000)
  {
    delay(200);
  }
  
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(3000);
    ESP.restart();
  }

  pinMode(BUILTIN_LED, OUTPUT);  // initialize onboard LED as output

  // OTA

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname(cfgWifi.rf24GWHostname);

  // No authentication by default
  //ArduinoOTA.setPassword((const char *)"1234");
  
  ArduinoOTA.begin();
  loopno=1;
}

void loop() {
  ArduinoOTA.handle(); // Wait for OTA connection
  udp_data.network_id=1234;
  udp_data.node_id=987;
  udp_data.msg_id=loopno;
  sensor=101;
  udp_data.sensor1=calcTransportValue(sensor,12.34);
  sensor=102;
  udp_data.sensor2=calcTransportValue(sensor,99.99);
  IPAddress remoteIP(cfgWifi.rf24hubHostIP[0],cfgWifi.rf24hubHostIP[1],cfgWifi.rf24hubHostIP[2],cfgWifi.rf24hubHostIP[3]);
  remotePort = cfgWifi.rf24HubUdpPort;
  Udp.beginPacket(remoteIP, remotePort);
  Udp.write((char*)&udp_data,sizeof(udp_data));
  Udp.endPacket();
  loopno++;
  digitalWrite(BUILTIN_LED, ledState);
  if (Udp.parsePacket() > 0 ) {
    Udp.read((char*)&udp_data,sizeof(udp_data));
    ledState = !ledState;   
  }
  delay(1000);
}
