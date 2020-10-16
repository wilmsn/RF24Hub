/***********************************************************************
 * 
 * 
 * Note:
 * The file "secrets.h" is not included!
 * Just add a file with the following content:
 * 
 * #define SSID "mySSID"
 * #define KEY  "mySecretPassword"
 *
 **********************************************************************/
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <EEPROM.h>
#include <RF24.h>
#include <strings.h>
#include "secrets.h"
#include "settings.h"
#include "dataformat.h"
#include "rf24_config.h"
#include "version.h"
//#include "printf.h"

udpdata_t udpdata;
payload_t payload;

WiFiUDP udp;
RF24 radio(RADIO_CE_PIN,RADIO_CSN_PIN);

char* buf;
uint8_t  rf24_node2hub[] = RF24_NODE2HUB;
uint8_t  rf24_hub2node[] = RF24_HUB2NODE;
  
#define MAX_TELNET_CLIENTS 2

WiFiServer TelnetServer(0);
WiFiClient TelnetClient[MAX_TELNET_CLIENTS];
WiFiUDP Udp;
unsigned int remotePort;  // remote port to listen on
uint16_t loopno;
uint8_t sensor;
int ledState = LOW;

struct eeprom_t {
   uint8_t      versionnumber;
   char         ssid[30];
   char         key[30];
   char         hostname[10];
   char         hub_ip[16];
   uint16_t     hub_udp_portno;
   uint16_t     gw_udp_portno;
   uint16_t     gw_no; 
};
eeprom_t eepromdata;

void setup()
{
  buf = (char*)malloc(10);
  EEPROM.begin(4095);
  EEPROM.get( 0, eepromdata );
  EEPROM.end();
  if ( eepromdata.versionnumber != EEPROM_VERSION && EEPROM_VERSION > 0) { 
     eepromdata.versionnumber = EEPROM_VERSION;
     sprintf(eepromdata.ssid,"%s", SSID);
     sprintf(eepromdata.key,"%s", KEY);
     sprintf(eepromdata.hostname,"%s", HOSTNAME);
     sprintf(eepromdata.hub_ip,"%s",HUB_IP);
     eepromdata.hub_udp_portno = HUB_UDP_PORTNO;
     eepromdata.gw_udp_portno = GW_UDP_PORTNO;
     eepromdata.gw_no = GW_NO;
     EEPROM.put( 0, eepromdata );
  }

  Serial.begin(9600);
  //printf_begin();

  WiFi.mode(WIFI_STA);
  WiFi.hostname(eepromdata.hostname);
  WiFi.begin(eepromdata.ssid, eepromdata.key);

//  Udp.begin(cfgWifi.rf24GWUdpPort);
//  TelnetServer.begin(cfgWifi.rf24GWTelnetPort);

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
  ArduinoOTA.setHostname(eepromdata.hostname);
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);
  // No authentication by default
  //ArduinoOTA.setPassword((const char *)"1234");  
  ArduinoOTA.begin();

  // init rf24
  radio.begin();
  radio.setChannel(RF24_CHANNEL);
  radio.setDataRate(RF24_SPEED);
  radio.setPALevel(RF24_PA_MAX);
  radio.setRetries(0, 0);
  radio.setAutoAck(false);
  radio.disableDynamicPayloads();
  radio.setPayloadSize(32);
  radio.setCRCLength(RF24_CRC_16);
  radio.openWritingPipe(rf24_hub2node);
  radio.openReadingPipe(1,rf24_node2hub);
  radio.powerUp();
  radio.startListening();
  radio.printDetails();
  
  udp.begin(eepromdata.gw_udp_portno);

}

void printPayload(char * h1, char * h2, payload_t *payload) {
    Serial.print(h1);
    Serial.print(h2);
    Serial.print(" N:");
    Serial.print(payload->node_id);
    Serial.print(" T:");
    Serial.print(payload->msg_type);
    Serial.print(" m:");
    Serial.print(payload->msg_id);
    Serial.print(" F:");
    Serial.print(payload->msg_flags, HEX);
    Serial.print(" O:");
    Serial.print(payload->orderno);
    Serial.print(" H:");
    Serial.print(payload->heartbeatno);
    Serial.print(" (");
    Serial.print(getChannel(payload->data1));
    Serial.print("/");
    Serial.print(unpackData(payload->data1, buf));
    Serial.print(")(");
    Serial.print(getChannel(payload->data2));
    Serial.print("/");
    Serial.print(unpackData(payload->data2, buf));
    Serial.print(")(");
    Serial.print(getChannel(payload->data3));
    Serial.print("/");
    Serial.print(unpackData(payload->data3, buf));
    Serial.print(")(");
    Serial.print(getChannel(payload->data4));
    Serial.print("/");
    Serial.print(unpackData(payload->data4, buf));
    Serial.print(")(");
    Serial.print(getChannel(payload->data5));
    Serial.print("/");
    Serial.print(unpackData(payload->data5, buf));
    Serial.print(")(");
    Serial.print(getChannel(payload->data6));
    Serial.print("/");
    Serial.print(unpackData(payload->data6, buf));
    Serial.println(")");  
}

void loop() {
  ArduinoOTA.handle(); // Wait for OTA connection
  while ( radio.available() ) {
    radio.read(&payload, sizeof(payload));
    printPayload("N>G", " ", &payload);
    udpdata.gw_no = eepromdata.gw_no;
    memcpy(&udpdata.payload, &payload, sizeof(payload));
    udp.beginPacket(eepromdata.hub_ip,eepromdata.hub_udp_portno);
    udp.write((char*)&udpdata, sizeof(udpdata));
    udp.endPacket();

    //delay(10);
    Serial.println("-------------------");
  }
  if (udp.parsePacket() > 0 ) {
    udp.read((char*)&udpdata, sizeof(udpdata));
    memcpy(&payload, &udpdata.payload, sizeof(payload));
    printPayload("G>N", " ", &payload);
    radio.stopListening();
    radio.write(&payload, sizeof(payload));
    radio.startListening(); 
    ledState = !ledState; 
    digitalWrite(LED_BUILTIN, ledState);  
  }

  delay(100);
}
