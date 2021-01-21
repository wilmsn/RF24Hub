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

udpdata_t udpdata;
payload_t payload;

WiFiUDP udp;
RF24 radio(RADIO_CE_PIN,RADIO_CSN_PIN);

WiFiServer tcpServer(GW_TCP_PORTNO);
WiFiClient clients[MAX_TCP_CONNECTIONS];

unsigned int remotePort;  // remote port to listen on
uint16_t loopno;
uint8_t sensor;
int ledState = LOW;
char* buf;
uint8_t  rf24_node2hub[] = RF24_NODE2HUB;
uint8_t  rf24_hub2node[] = RF24_HUB2NODE;
// Buffer for incoming text
char tcp_buffer[MAX_TCP_CONNECTIONS][30];

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

bool append_until(Stream& source, char* buffer, int bufSize, char terminator) {
    int data=source.read();
    if (data>=0)  {
        int len=static_cast<int>(strlen(buffer));
        do {
            if (len<bufSize-1) {
                buffer[len++]=static_cast<char>(data);
            }
            if (data==terminator) {
                buffer[len]='\0';
                return true;
            }
            data=source.read();
        } while (data>=0);
        buffer[len]='\0';  
    }
    return false;
}

void setup() {
  buf = (char*)malloc(10);
  EEPROM.begin(4095);
  EEPROM.get( 0, eepromdata );
  EEPROM.end();
  if ( eepromdata.versionnumber != EEPROM_VERSION && EEPROM_VERSION > 0) { 
     eepromdata.versionnumber = EEPROM_VERSION;
     sprintf(eepromdata.ssid,"%s", ssid);
     sprintf(eepromdata.key,"%s", password);
     sprintf(eepromdata.hostname,"%s", HOSTNAME);
     sprintf(eepromdata.hub_ip,"%s",HUB_IP);
     eepromdata.hub_udp_portno = HUB_UDP_PORTNO;
     eepromdata.gw_udp_portno = GW_UDP_PORTNO;
     eepromdata.gw_no = GW_NO;
     EEPROM.put( 0, eepromdata );
  }

  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.hostname(eepromdata.hostname);
  WiFi.begin(eepromdata.ssid, eepromdata.key);

  // ... Give ESP 10 seconds to connect to station.
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startTime < 10000) {
    delay(200);
  }
  
  while (WiFi.status() != WL_CONNECTED) {
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
  tcpServer.begin();

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
/*    Serial.print(unpackData(payload->data1, buf));
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
    Serial.print(unpackData(payload->data6, buf)); */
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
  WiFiClient client = tcpServer.available();
  if (client) {
    Serial.print(F("New connection from "));
    Serial.println(client.remoteIP().toString());        
        // Find a freee space in the array   
        for (int i = 0; i < MAX_TCP_CONNECTIONS; i++) {
            if (!clients[i].connected()) {
                // Found free space
                clients[i] = client;
                tcp_buffer[i][0]='\0';
                Serial.print(F("Channel="));
                Serial.println(i);
                // Send a welcome message
                client.print(F("RF24GW> "));
                return;
            }
        }
        Serial.println(F("To many connections"));
        client.stop();
  }
    static int i=0;
    // Only one connection is checked in each call
    if (clients[i].available()) {
        // Collect characters until line break
        if (append_until(clients[i],tcp_buffer[i],sizeof(tcp_buffer[i]),'\n')) {        
            // Display the received line
            Serial.print(F("Received from "));
            Serial.print(i);
            Serial.print(": ");
            Serial.print(tcp_buffer[i]);

            // Send an echo back
            clients[i].print(F("Echo: "));
            clients[i].print(tcp_buffer[i]);
            
            // Execute some commands
            if (strstr(tcp_buffer[i], "on")) {
                digitalWrite(BUILTIN_LED, LOW);
                clients[i].println(F("LED is on"));
                clients[i].stop();
            }
            else if (strstr(tcp_buffer[i], "off")) {
                digitalWrite(BUILTIN_LED, HIGH);
                clients[i].println(F("LED is off"));
                clients[i].stop();
            }    
            
            // Clear the buffer to receive the next line
            tcp_buffer[i][0]='\0';
        }
    }
    
    // Switch to the next connection for the next call
    if (++i >= MAX_TCP_CONNECTIONS) {
        i=0;
    }
}
