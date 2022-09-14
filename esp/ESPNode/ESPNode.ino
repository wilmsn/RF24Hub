/*
A Node for the rf24hub based on esp8266.
Can be used as switch or a matrix display

Build in Parts (selectable):
Relais
Matrix Display
NeoPixel
LEDPWM
Dallas Temperature Sensor 18B20
Rf24GW

On Branch: V3.0  !!!!!


*/
//****************************************************
// My definitions for my nodes based on this sketch
// Select only one at one time !!!!
//#define TEICHPUMPE
#define TERASSENNODE
//#define FLURLICHT
//#define WOHNZIMMERNODE
//#define TESTNODE
//#define WITTYNODE
//#define RF24GWTEST
//#define ESPMINI
//#define TESTNODE
//****************************************************
// Default settings are in "default.h" !!!!!
#include "defaults.h"
// Default settings for the individual nodes are in "Node_settings.h"
#include "Node_settings.h"
//-----------------------------------------------------
//*****************************************************
/* Configuration of NTP */
#define MY_NTP_SERVER "de.pool.ntp.org"           
#define MY_TZ "CET-1CEST,M3.5.0/02,M10.5.0/03"   
/* Configuration of update server */
//#define ATOMIC_FS_UPDATE
// ------ End of configuration part ------------

#include <ESP8266WiFi.h>
//#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <WiFiUdp.h>
//#include <ArduinoOTA.h>
#include <EEPROM.h>
//#include <FS.h>
#include <LittleFS.h>
#include <PubSubClient.h>
#include <time.h>
#include <uptime.h>
#include <strings.h>
#include <logger.h>
#include "config.h"
#include "secrets.h"
#include "version.h"
#include "defaults.h"
#include "Node_settings.h"
#include "version.h"
#if defined(SENSOR_18B20)
#include <OneWire.h>
#include <DallasTemperature.h>
#endif
#include "rf24_config.h"
#if defined(RF24GW)
#include <RF24.h>
#include "dataformat.h"
#endif
#if defined(LEDMATRIX)
#include <LED_Matrix.h>
#endif
#if defined(NEOPIXEL)
#include <Adafruit_NeoPixel.h>
#endif

ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;
WiFiClient mqtt_wifi_client;
#if defined(MQTT)
PubSubClient mqttClient(mqtt_wifi_client);
#endif
Logger logger(LOGGER_NUMLINES,LOGGER_LINESIZE);
#if defined(SENSOR_18B20)
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
#endif
WiFiUDP udp;
#if defined(RF24GW)
RF24 radio(RF24_RADIO_CE_PIN,RF24_RADIO_CSN_PIN);
#endif
#if defined(LEDMATRIX)
LED_Matrix matrix(LEDMATRIX_DIN, LEDMATRIX_CLK, LEDMATRIX_CS, LEDMATRIX_DEVICES_X, LEDMATRIX_DEVICES_Y);
#endif
#if defined(NEOPIXEL)
Adafruit_NeoPixel pixels(NEOPIXELNUM, NEOPIXELPIN, NEO_GRB + NEO_KHZ800);
#endif

typedef enum {message=0, sensorweb, sensormqtt, sensorinfo} call_t;
typedef enum {ok_json=0, ok_html, ok_text, nochange, epromchange, error} status_t;
typedef enum {on=0, off, toggle, state, unknown} tristate_t;
time_t now;
tm tm;
char timeStr[9];
char mytopic[TOPIC_BUFFER_SIZE];
char info_str[INFOSIZE];
unsigned long lastDbg = 0;
unsigned long lastMsg = 0;
unsigned long lastInfo = 0;
const char c_on[] =  "Ein";
const char c_off[] = "Aus";
#if defined(NEOPIXEL)
uint32_t rgb = RGBINIT;
#endif
#if defined(LEDPWM)
uint8_t intensity = LEDPWMINIT;
#endif
//bool log_startup = false;
#if defined(SWITCH1)
bool state_switch1 = SWITCH1INITSTATE;
#endif
#if defined(SWITCH2)
bool state_switch2 = SWITCH2INITSTATE;
#endif
#if defined(SWITCH3)
bool state_switch3 = SWITCH3INITSTATE;
#endif
#if defined(SWITCH4)
bool state_switch4 = SWITCH4INITSTATE;
#endif
#if defined(RF24GW)
payload_t payload;
uint8_t  rf24_node2hub[] = RF24_NODE2HUB;
uint8_t  rf24_hub2node[] = RF24_HUB2NODE;
uint16_t rf24_verboselevel = RF24_GW_STARTUPVERBOSELEVEL;
#endif

struct eeprom_t {
   uint32_t  magicNo;
   bool      logfile;
   bool      logger;
   bool      log_startup;
   bool      log_rf24;
   bool      log_sensor;
   bool      log_mqtt;
   bool      log_webcmd;
   bool      log_sysinfo;
};
eeprom_t eepromdata;

udpdata_t udpdata;

void wifi_con(void) {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.persistent(false);
    WiFi.mode(WIFI_STA);
    WiFi.hostname(HOSTNAME);
    WiFi.begin(ssid, password);

    // ... Give ESP 10 seconds to connect to station.
    unsigned int i=0;
    while (WiFi.status() != WL_CONNECTED && i < 100) {
      delay(200);
      i++;
    }
    configTime(MY_TZ, MY_NTP_SERVER); 
    while (WiFi.status() != WL_CONNECTED) {
      delay(3000);
      ESP.restart();
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, LOW);
  Serial.begin(115200);

  EEPROM.begin(512);
  EEPROM.get( 0, eepromdata );
  //EEPROM.end();

  if ( eepromdata.magicNo != MAGICNO ) {
    eepromdata.magicNo = MAGICNO;
    eepromdata.logfile = false;
    eepromdata.logger = false;
    eepromdata.log_startup = false;
    eepromdata.log_rf24 = false;
    eepromdata.log_sensor = false;
    eepromdata.log_mqtt = false;
    eepromdata.log_webcmd = false;
    eepromdata.log_sysinfo = false;
    EEPROM.put( 0, eepromdata );
    EEPROM.commit();
  }
  logger.begin();
  httpServer.begin();

  if (!LittleFS.begin()) {
    ESP.restart();
    return;
  } else {
    if (eepromdata.log_startup) {
      snprintf(info_str,INFOSIZE,"%s",F("+++++ Begin Startup ++++++ LittleFS mounted +++++"));
      write2log(info_str);
    }
  }

  wifi_con();
  
  if (eepromdata.log_startup) {
    snprintf(info_str,INFOSIZE,"%s %s %s %s", F("WLAN: Connected to "), ssid, F(" IP address: "), WiFi.localIP().toString().c_str() );
    write2log(info_str);
  } 
  
#if defined(RF24GW)
  udp.begin(RF24_GW_UDP_PORTNO);
  if (eepromdata.log_startup) {
    snprintf(info_str,INFOSIZE,"%s %u", F("RF24: Opened UDP Port: "), RF24_GW_UDP_PORTNO );
    write2log(info_str);
  } 
#endif  

  //MQTT
#if defined(MQTT)
  mqttClient.setServer(MQTT_SERVER, 1883);
  mqttClient.setCallback(callback);
  mqttClient.setBufferSize(512);
  if (eepromdata.log_startup) {
    snprintf(info_str,INFOSIZE,"%s %s %s", F("MQTT: Connected to Server "), MQTT_SERVER, F(" Port: 1883") );
    write2log(info_str);
  }
#endif
  // OTA
//  ArduinoOTA.setHostname(HOSTNAME);
//  ArduinoOTA.begin();
//  MDNS.begin(HOSTNAME);
  httpUpdater.setup(&httpServer);
//  httpServer.begin();
//  MDNS.addService("http", "tcp", 80);
//  setupFS();
  httpServer.on("/", handleWebRoot);
  httpServer.on("/cmd", handleCmd);
  httpServer.on("/restart", []() { httpServer.send(304, "message/http"); ESP.restart(); });
  httpServer.on("/console", []() { httpServer.send(200, "text/plain", logger.printBuffer()); });
  httpServer.onNotFound([]() {
    if (!handleFile(httpServer.urlDecode(httpServer.uri())))
      httpServer.send(404, "text/plain", "FileNotFound");
  });

// Init additional modules
#if defined(RF24GW)
  // init rf24
  radio.begin();
  delay(100);
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
  radio.startListening();
  radio.printDetails();
#endif
#if defined(LEDMATRIX)
  matrix.begin();
  for (int address=0; address < (LEDMATRIX_DEVICES_X * LEDMATRIX_DEVICES_Y); address++) {
    matrix.displayTest(address, true);
    delay(200);
    matrix.displayTest(address, false);
  }
  matrix.setFont(2);
  matrix.setIntensity(1);
  matrix.setCursor(3,8);
  matrix.print("Init");
  matrix.display();
  delay(500);
  matrix.off();
  delay(500);
  matrix.on();
  delay(500);
  matrix.clear();    
  matrix.display();
#endif
#if defined(NEOPIXEL)
  pixels.begin();
#endif
#if defined(LEDPWM)
  pinMode(LEDPWM_PIN, OUTPUT);
  //analogWriteFreq(10000);
  //analogWrite(LEDPWM_PIN, 0);
#endif
 
// Init sensors
#if defined(SENSOR_18B20)
  sensors.begin();
  sensors.setResolution(SENSOR_18B20_RESOLUTION);
#endif

#if defined(SWITCH1)
#if defined(SWITCH1PIN1)
  pinMode(SWITCH1PIN1, OUTPUT);
#endif
#if defined(SWITCH1PIN2)
  pinMode(SWITCH1PIN2, OUTPUT);
#endif
  if (SWITCH1INITSTATE) { handleSwitch1(on); } else { handleSwitch1(off); }
#endif

#if defined(SWITCH2)
#if defined(SWITCH2PIN1)
  pinMode(SWITCH2PIN1, OUTPUT);
#endif
#if defined(SWITCH2PIN2)
  pinMode(SWITCH2PIN2, OUTPUT);
#endif
  if (SWITCH2INITSTATE) { handleSwitch2(on); } else { handleSwitch2(off); }
#endif

#if defined(SWITCH3)
#if defined(SWITCH3PIN1)
  pinMode(SWITCH3PIN1, OUTPUT);
#endif
#if defined(SWITCH3PIN2)
  pinMode(SWITCH3PIN2, OUTPUT);
#endif
  if (SWITCH3INITSTATE) { handleSwitch3(on); } else { handleSwitch3(off); }
#endif

#if defined(SWITCH4)
#if defined(SWITCH4PIN1)
  pinMode(SWITCH4PIN1, OUTPUT);
#endif
#if defined(SWITCH4PIN2)
  pinMode(SWITCH4PIN2, OUTPUT);
#endif
  if (SWITCH4INITSTATE) { handleSwitch4(on); } else { handleSwitch4(off); }
#endif

#if defined(VALUE1_PIN)
  pinMode(VALUE1_PIN, INPUT);
#endif
  delay(200);
  if (eepromdata.log_startup) {
    snprintf(info_str,INFOSIZE,"%s", F("-------------------Ende Startup-------------------------"));
    write2log(info_str);
  }
  digitalWrite(BUILTIN_LED, HIGH);  
#if defined(LEDPWM)
  analogWrite(LEDPWM_PIN, 0);
#endif  
}

tristate_t get_tristate(const char* mystate) {
  tristate_t retval = unknown;
  if (strncmp(mystate, "Ein", 3)==0 || strncmp(mystate, "ein", 3)==0 || strncmp(mystate, "on", 2)==0 || strncmp(mystate, "On", 2)==0 || strncmp(mystate, "1", 1)==0 )
     retval = on;
  if (strncmp(mystate, "Aus", 3)==0 || strncmp(mystate, "aus", 3)==0 || strncmp(mystate, "off", 3)==0 || strncmp(mystate, "Off", 3)==0 || strncmp(mystate, "0", 1)==0 )
     retval = off;
  // togg* oder Togg*     
  if (strncmp(mystate, "toggle", 4)==0 || strncmp(mystate, "Toggle", 4)==0 )
     retval = toggle;
  // state, State, status oder Status   
  if (strncmp(mystate, "state", 4)==0 || strncmp(mystate, "Stat", 3)==0 )
     retval = state;     
  return retval;
}

void handleCmd() {
  status_t status = error;
  bool do_eepromchange = false;
  for (int argNo=0; argNo <  httpServer.args(); argNo++ ) { 
    if ( httpServer.argName(argNo) == "dellogfile" ) {
      if (LittleFS.remove("logfile.txt")) {
        snprintf(info_str,INFOSIZE,"%s","Logfile deleted");
        status = ok_text;  
      }
    }
    if ( httpServer.argName(argNo) == F("saveeprom") ) {
      if (httpServer.arg(argNo) == "1" ) {
        do_eepromchange = true;
        status = epromchange;
      }
    }
    if ( httpServer.argName(argNo) == F("logfile") ) {
      if (status == error) status = nochange;
      if (httpServer.arg(argNo) == "1" ) {
        if ( ! eepromdata.logfile) {
           eepromdata.logfile = true;
           status = epromchange;  
        }
      } else {
        if ( eepromdata.logfile) {
           eepromdata.logfile = false;
           status = epromchange;  
        }
      }
    }
    if ( httpServer.argName(argNo) == F("logger") ) {
      if (status == error) status = nochange;
      if (httpServer.arg(argNo) == "1" ) {
        if ( ! eepromdata.logger) {
           eepromdata.logger = true;
           status = epromchange;  
        }
      } else {
        if ( eepromdata.logger) {
           eepromdata.logger = false;
           status = epromchange;  
        }
      }
    }
    if ( httpServer.argName(argNo) == F("log_startup") ) {
      if (status == error) status = nochange;
      if (httpServer.arg(argNo) == "1" ) {
        if ( ! eepromdata.log_startup) {
           eepromdata.log_startup = true;
           status = epromchange;  
        }
      } else {
        if ( eepromdata.log_startup) {
           eepromdata.log_startup = false;
           status = epromchange;  
        }
      }
    }
    if ( httpServer.argName(argNo) == F("log_rf24") ) {
      if (status == error) status = nochange;
      if (httpServer.arg(argNo) == "1" ) {
        if ( ! eepromdata.log_rf24) {
           eepromdata.log_rf24 = true;
           status = epromchange;  
        }
      } else {
        if ( eepromdata.log_rf24) {
           eepromdata.log_rf24 = false;
           status = epromchange;  
        }
      }
    }
    if ( httpServer.argName(argNo) == F("log_sensor") ) {
      if (status == error) status = nochange;
      if (httpServer.arg(argNo) == "1" ) {
        if ( ! eepromdata.log_sensor) {
           eepromdata.log_sensor = true;
           status = epromchange;  
        }
      } else {
        if ( eepromdata.log_sensor) {
           eepromdata.log_sensor = false;
           status = epromchange;  
        }
      }
    }
    if ( httpServer.argName(argNo) == F("log_mqtt") ) {
      if (status == error) status = nochange;
      if (httpServer.arg(argNo) == "1" ) {
        if ( ! eepromdata.log_mqtt) {
           eepromdata.log_mqtt = true;
           status = epromchange;  
        }
      } else {
        if ( eepromdata.log_mqtt) {
           eepromdata.log_mqtt = false;
           status = epromchange;  
        }
      }
    }
    if ( httpServer.argName(argNo) == F("log_webcmd") ) {
      if (status == error) status = nochange;
      if (httpServer.arg(argNo) == "1" ) {
        if ( ! eepromdata.log_webcmd) {
           eepromdata.log_webcmd = true;
           status = epromchange;  
        }
      } else {
        if ( eepromdata.log_webcmd) {
           eepromdata.log_webcmd = false;
           status = epromchange;  
        }
      }
    }
    if ( httpServer.argName(argNo) == F("log_sysinfo") ) {
      if (status == error) status = nochange;
      if (httpServer.arg(argNo) == "1" ) {
        if ( ! eepromdata.log_sysinfo) {
           eepromdata.log_sysinfo = true;
           status = epromchange;  
        }
      } else {
        if ( eepromdata.log_sysinfo) {
           eepromdata.log_sysinfo = false;
           status = epromchange;  
        }
      }
    }
    if ( httpServer.argName(argNo) == F("sysinfo1") ) {
      fill_sysinfo1(info_str);
      status = ok_json;
    }
    if ( httpServer.argName(argNo) == F("sysinfo2") ) {
      fill_sysinfo2(info_str);
      status = ok_json;
    }
    if ( httpServer.argName(argNo) == F("sysinfo3") ) {
      fill_sysinfo3(info_str);
      status = ok_json;
    }
    if ( httpServer.argName(argNo) == F("sysinfo4") ) {
      fill_sysinfo4(info_str);
      status = ok_json;
    }
    if ( httpServer.argName(argNo) == F("sysinfo5") ) {
      fill_sysinfo5(info_str);
      status = ok_json;
    }
    if ( httpServer.argName(argNo) == F("sysinfo6") ) {
      fill_sysinfo6(info_str);
      status = ok_json;
    }
    if ( httpServer.argName(argNo) == F("webcfg1") ) {
      fill_webcfg1(info_str);
      status = ok_json;
    }
    if ( httpServer.argName(argNo) == F("webcfg2") ) {
      fill_webcfg2(info_str);
      status = ok_json;
    }
    if ( httpServer.argName(argNo) == F("status") ) {
      handlestatus(info_str);
      status = ok_json;
    }
    if ( httpServer.argName(argNo) == F("sensor1") ) {
      handlesensor(info_str, sensorweb);
      status = ok_json;
    }
    if ( httpServer.argName(argNo) == F("message1") ) {
      handlesensor(info_str, message);
      status = ok_json;
    }
    
#if defined(SWITCH1)
    if ( httpServer.argName(argNo) == "sw1" ) {
      handleSwitch1(get_tristate(httpServer.arg(argNo).c_str()));
      status = ok_json;
    }
#endif  
#if defined(SWITCH2)
    if ( httpServer.argName(argNo) == "sw2" ) {
      handleSwitch2(get_tristate(httpServer.arg(argNo).c_str()));
      status = ok_json;
    }
#endif  
#if defined(SWITCH3)
    if ( httpServer.argName(argNo) == "sw3" ) {
      handleSwitch3(get_tristate(httpServer.arg(argNo).c_str()));
      status = ok_json;
    }
#endif  
#if defined(SWITCH4)
    if ( httpServer.argName(argNo) == "sw4" ) {
      handleSwitch4(get_tristate(httpServer.arg(argNo).c_str()));
      status = ok_json;
    }
#endif  
#if defined(RF24GW)
    if ( httpServer.argName(argNo) == "rf24gw" ) {
      handlerf24gw(info_str);
      status = ok_json;
    }
#endif  
#if defined(LEDMATRIX)
    if ( httpServer.argName(argNo) == "matrixFB" ) {
      getMatrixFB(info_str);
      status = ok_text;  
    }
    if ( httpServer.argName(argNo) == "intensity" ) {
      uint8_t intensity = atoi(httpServer.arg(argNo).c_str());
      if ( intensity < 16) {
        matrix.setIntensity(intensity);
        snprintf(mytopic,TOPIC_BUFFER_SIZE,"%s/%s/%s","stat",MQTT_NODENAME,"intensity");
        mqttClient.publish(mytopic, httpServer.arg(argNo).c_str());
        if (eepromdata.log_mqtt) {
          snprintf(info_str,INFOSIZE,"MQTT: %s : %s",mytopic, httpServer.arg(argNo).c_str());
          write2log(info_str);
        }
      }
      snprintf(info_str,INFOSIZE,"%s",httpServer.arg(argNo).c_str());
      status = ok_text;  
    }
#endif  
#if defined(NEOPIXEL)
    if ( httpServer.argName(argNo) == "getrgb" ) {
      snprintf(info_str,INFOSIZE,"%u",rgb);
      status = ok_text;  
    }
    if ( httpServer.argName(argNo) == "setrgb" ) {
      rgb = httpServer.arg(argNo).toInt();
      snprintf(mytopic,TOPIC_BUFFER_SIZE,"%s/%s/%s","stat",MQTT_NODENAME,"RGB");
      mqttClient.publish(mytopic, httpServer.arg(argNo).c_str());
      if (state_switch1) set_neopixel(on);
      status = ok_text;  
    }
#endif
#if defined(LEDPWM)
    if ( httpServer.argName(argNo) == "getledpwm" ) {
      snprintf(info_str,INFOSIZE,"%u",intensity);
      status = ok_text;  
    }
    if ( httpServer.argName(argNo) == "setledpwm" ) {
      intensity = httpServer.arg(argNo).toInt();
      snprintf(mytopic,TOPIC_BUFFER_SIZE,"%s/%s/%s","stat",MQTT_NODENAME,"intensity");
      mqttClient.publish(mytopic, httpServer.arg(argNo).c_str());
      if (eepromdata.log_mqtt) {
        snprintf(info_str,INFOSIZE,"MQTT: %s : %s",mytopic, httpServer.arg(argNo).c_str());
        write2log(info_str);
      }
      if (state_switch1) set_ledpwm(on);
      status = ok_text;  
    }
/*    if ( httpServer.argName(argNo) == "intensity" ) {
      intensity = httpServer.arg(argNo).toInt();
      snprintf(mytopic,TOPIC_BUFFER_SIZE,"%s/%s/%s","stat",MQTT_NODENAME,"intensity");
      mqttClient.publish(mytopic, httpServer.arg(argNo).c_str());
      if (eepromdata.log_mqtt) {
        snprintf(info_str,INFOSIZE,"MQTT: %s : %s",mytopic, httpServer.arg(argNo).c_str());
        write2log(info_str);
      }
      snprintf(info_str,INFOSIZE,"%s",httpServer.arg(argNo).c_str());
      status = ok_text;  
    }*/
#endif
  }
  switch (status) {
    case ok_json:
      httpServer.send(200, "application/json", info_str );
    break;
    case ok_text:
      httpServer.send(200, "text/plain", info_str );
    break;
    case ok_html:
      httpServer.send(200, "text/html", info_str );
    break;
    case epromchange:
      if ( do_eepromchange ) {
        EEPROM.put( 0, eepromdata );
        EEPROM.commit(); 
        httpServer.send(200, "text/plain", "EEPROM changed" );
      } else {
        httpServer.send(200, "text/plain", "Settings temporary changed" );
      }
    break;
    case nochange:
        httpServer.send(200, "text/plain", "No settings changed" );
    break;
    case error:
      httpServer.send(200, "text/plain", "ERROR" );
    break;
  }
}

#if defined(SWITCH1)
void mqtt_send_swtch1() {
  snprintf(mytopic,TOPIC_BUFFER_SIZE,"%s/%s/%s","stat",MQTT_NODENAME,SWITCH1MQTT);
#if defined(MQTT)
  mqttClient.publish(mytopic, state_switch1? c_on: c_off);
#endif
  if (eepromdata.log_mqtt) {
    snprintf(info_str,INFOSIZE,"MQTT: %s : %s",mytopic, state_switch1? c_on: c_off);
    write2log(info_str);
  }
}

void switchSwitch1(bool stat) {
#if defined(SWITCH1_NODE)
//  char tmp[10];
//  snprintf(tmp,9,"%u",stat?1:0);
  send_udp_msg(SWITCH1_NODE, calcTransportValue(mykey,SWITCH1_CHANNEL, stat?1:0 ));   
#endif  
  if ( stat ) { 
#if defined(SWITCH1PIN1)
    digitalWrite(SWITCH1PIN1, SWITCH1ACTIVESTATE);
#endif    
#if defined(SWITCH1PIN2)
    digitalWrite(SWITCH1PIN2, SWITCH1ACTIVESTATE);
#endif
#if defined(LEDMATRIX)
    matrix.on();
#endif
#if defined(NEOPIXEL)
     set_neopixel(on);
#endif
#if defined(LEDPWM)
     set_ledpwm(on);
#endif
  } else {
#if defined(SWITCH1PIN1)
    digitalWrite(SWITCH1PIN1, ! SWITCH1ACTIVESTATE);
#endif    
#if defined(SWITCH1PIN2)
    digitalWrite(SWITCH1PIN2, ! SWITCH1ACTIVESTATE);
#endif
#if defined(LEDMATRIX)
     matrix.off();
#endif
#if defined(NEOPIXEL)
     set_neopixel(off);
#endif
#if defined(LEDPWM)
     set_ledpwm(off);
#endif
  }
}

void handleSwitch1(tristate_t stat) {
  if ( stat == on || stat == off || stat == toggle ) {
    if ( stat == on ) { 
      state_switch1 = true;
    }  
    if ( stat == off ) {
      state_switch1 = false;
    } 
    if ( stat == toggle ) {
      state_switch1 = !state_switch1;
    } 
    switchSwitch1(state_switch1);
#if defined(SWITCH1PIN1)
    state_switch1 = (digitalRead(SWITCH1PIN1) == SWITCH1ACTIVESTATE);
#endif
    mqtt_send_swtch1();
    if (eepromdata.log_sensor) {
      snprintf(info_str,INFOSIZE, "Schalter1 %s", state_switch1 ? c_on : c_off);
      write2log(info_str);
    }
  }
  snprintf(info_str,INFOSIZE,"{\"state\":\"%s\",\"label\":\"%s\"}", state_switch1 ? c_on : c_off, SWITCH1TXT);
}
#endif

#if defined(SWITCH2)
void mqtt_send_swtch2() {
  snprintf(mytopic,TOPIC_BUFFER_SIZE,"%s/%s/%s","stat",MQTT_NODENAME,SWITCH2MQTT);
#if defined(MQTT)
  mqttClient.publish(mytopic, state_switch2? c_on: c_off);
#endif
  if (eepromdata.log_mqtt) {
    snprintf(info_str,INFOSIZE,"MQTT: %s : %s",mytopic, state_switch2? c_on: c_off);
    write2log(info_str);
  }
}

void switchSwitch2(bool stat) {
#if defined(SWITCH2_NODE)
  char tmp[10];
  snprintf(tmp,9,"%u",stat?1:0);
  send_udp_msg(SWITCH2_NODE, calcTransportValue(SWITCH2_CHANNEL, tmp ));   
#endif  
  if ( stat ) { 
#if defined(SWITCH2PIN1)
    digitalWrite(SWITCH2PIN1, SWITCH2ACTIVESTATE);
#endif    
#if defined(SWITCH2PIN2)
    digitalWrite(SWITCH2PIN2, SWITCH2ACTIVESTATE);
#endif
  } else {
#if defined(SWITCH2PIN1)
    digitalWrite(SWITCH2PIN1, ! SWITCH2ACTIVESTATE);
#endif    
#if defined(SWITCH2PIN2)
    digitalWrite(SWITCH2PIN2, ! SWITCH2ACTIVESTATE);
#endif
  }
}

void handleSwitch2(tristate_t stat) {
  if ( stat == on || stat == off || stat == toggle ) {
    if ( stat == on ) { 
      state_switch2 = true;
    }  
    if ( stat == off ) {
      state_switch2 = false;
    } 
    if ( stat == toggle ) {
      state_switch2 = !state_switch2;
    } 
    switchSwitch2(state_switch2);
#if defined(SWITCH2PIN1)
    state_switch2 = (digitalRead(SWITCH2PIN1) == SWITCH2ACTIVESTATE);
#endif
    mqtt_send_swtch2();
    if (eepromdata.log_sensor) {
      snprintf(info_str,INFOSIZE, "Schalter2 %s", state_switch2 ? c_on : c_off);
      write2log(info_str);
    }
  }
  snprintf(info_str,INFOSIZE,"{\"state\":\"%s\",\"label\":\"%s\"}", state_switch2 ? c_on : c_off, SWITCH2TXT);
}
#endif

#if defined(SWITCH3)
void mqtt_send_swtch3() {
  snprintf(mytopic,TOPIC_BUFFER_SIZE,"%s/%s/%s","stat",MQTT_NODENAME,SWITCH3MQTT);
#if defined(MQTT)
  mqttClient.publish(mytopic, state_switch3? c_on: c_off);
#endif
  if (eepromdata.log_mqtt) {
    snprintf(info_str,INFOSIZE,"MQTT: %s : %s",mytopic, state_switch3? c_on: c_off);
    write2log(info_str);
  }
}

void switchSwitch3(bool stat) {
  if ( stat ) { 
#if defined(SWITCH3PIN1)
    digitalWrite(SWITCH3PIN1, SWITCH3ACTIVESTATE);
#endif    
#if defined(SWITCH3PIN2)
    digitalWrite(SWITCH3PIN2, SWITCH3ACTIVESTATE);
#endif
  } else {
#if defined(SWITCH3PIN1)
    digitalWrite(SWITCH3PIN1, ! SWITCH3ACTIVESTATE);
#endif    
#if defined(SWITCH3PIN2)
    digitalWrite(SWITCH3PIN2, ! SWITCH3ACTIVESTATE);
#endif
  }
}

void handleSwitch3(tristate_t stat) {
  if ( stat == on || stat == off || stat == toggle ) {
    if ( stat == on ) { 
      state_switch3 = true;
    }  
    if ( stat == off ) {
      state_switch3 = false;
    } 
    if ( stat == toggle ) {
      state_switch3 = !state_switch3;
    } 
    switchSwitch3(state_switch3);
#if defined(SWITCH3PIN1)
    state_switch3 = (digitalRead(SWITCH3PIN1) == SWITCH3ACTIVESTATE);
#endif
    mqtt_send_swtch3();
    if (eepromdata.log_sensor) {
      snprintf(info_str,INFOSIZE, "Schalter3 %s", state_switch3 ? c_on : c_off);
      write2log(info_str);
    }
  }
  snprintf(info_str,INFOSIZE,"{\"state\":\"%s\",\"label\":\"%s\"}", state_switch3 ? c_on : c_off, SWITCH3TXT);
}
#endif

#if defined(SWITCH4)
void mqtt_send_swtch4() {
  snprintf(mytopic,TOPIC_BUFFER_SIZE,"%s/%s/%s","stat",MQTT_NODENAME,SWITCH4MQTT);
#if defined(MQTT)
  mqttClient.publish(mytopic, state_switch4? c_on: c_off);
#endif
  if (eepromdata.log_mqtt) {
    snprintf(info_str,INFOSIZE,"MQTT: %s : %s",mytopic, state_switch4? c_on: c_off);
    write2log(info_str);
  }
}

void switchSwitch4(bool stat) {
  if ( stat ) { 
#if defined(SWITCH4PIN1)
    digitalWrite(SWITCH4PIN1, SWITCH4ACTIVESTATE);
#endif    
#if defined(SWITCH4PIN2)
    digitalWrite(SWITCH4PIN2, SWITCH4ACTIVESTATE);
#endif
  } else {
#if defined(SWITCH4PIN1)
    digitalWrite(SWITCH4PIN1, ! SWITCH4ACTIVESTATE);
#endif    
#if defined(SWITCH4PIN2)
    digitalWrite(SWITCH4PIN2, ! SWITCH4ACTIVESTATE);
#endif
  }
}

void handleSwitch4(tristate_t stat) {
  if ( stat == on || stat == off || stat == toggle ) {
    if ( stat == on ) { 
      state_switch4 = true;
    }  
    if ( stat == off ) {
      state_switch4 = false;
    } 
    if ( stat == toggle ) {
      state_switch4 = !state_switch4;
    } 
    switchSwitch4(state_switch4);
#if defined(SWITCH4PIN1)
    state_switch4 = (digitalRead(SWITCH4PIN1) == SWITCH4ACTIVESTATE);
#endif
    mqtt_send_swtch4();
    if (eepromdata.log_sensor) {
      snprintf(info_str,INFOSIZE, "Schalter4 %s", state_switch4 ? c_on : c_off);
      write2log(info_str);
    }
  }
  snprintf(info_str,INFOSIZE,"{\"state\":\"%s\",\"label\":\"%s\"}", state_switch4 ? c_on : c_off, SWITCH4TXT);
}
#endif

#if defined(NEOPIXEL)
void  set_neopixel(tristate_t mystate) {
  uint32_t red;
  uint32_t green;
  uint32_t blue;
  if ( mystate == on ) {
    red =  rgb & 0x0000FF;  
    green = rgb & 0x00FF00;
    green >>= 8;
    blue = rgb & 0xFF0000;
    blue >>= 16;
  } 
  if ( mystate == off ) {
    red = 0;
    green = 0;
    blue = 0;
  }
  for(int i=0; i<NEOPIXELNUM; i++) {
    pixels.setPixelColor(i, pixels.Color(red, green, blue));
  }
  pixels.show();
}
#endif

#if defined(LEDPWM)
void  set_ledpwm(tristate_t mystate) {
  uint8_t myintensity;
  if ( mystate == on ) {
    myintensity =  intensity;  
  } 
  if ( mystate == off ) {
    myintensity = 0;
  }
  analogWrite(LEDPWM_PIN, myintensity);
}
#endif

#if defined(RF24GW)
void handlerf24gw(char* response) {
  snprintf(response,INFOSIZE,"{\"gwno\":%d}", RF24_GW_NO);
}
#endif

void handlestatus(char* myjson) {
  char tmp[10];
  snprintf(myjson,10,"%s","{ ");  
#if defined(LEDMATRIX)
  if (strlen(myjson) > 5) strcat(myjson,",");
  strcat(myjson," \"intensity\":");
  snprintf(tmp,10,"%u",matrix.getIntensity());
  strcat(myjson,tmp);
  strcat(myjson,", \"display\":");
  snprintf(tmp,10,"\"%s\"",matrix.displayIsOn()? c_on:c_off);
  strcat(myjson,tmp);
#endif
  strcat(myjson,"}");    
}

void send_udp_msg(NODE_DATTYPE node_id, uint32_t data) {
#if defined(RF24GW)
  udpdata.gw_no = RF24_GW_NO;
  udpdata.payload.node_id = node_id;
  udpdata.payload.msg_id = 0;
  udpdata.payload.msg_type = PAYLOAD_TYPE_ESP;
  udpdata.payload.msg_flags = PAYLOAD_FLAG_LASTMESSAGE;
  udpdata.payload.orderno = 0;
  udpdata.payload.data1 = data;
  udpdata.payload.data2 = 0;
  udpdata.payload.data3 = 0;
  udpdata.payload.data4 = 0;
  udpdata.payload.data5 = 0;
  udpdata.payload.data6 = 0;
  udp.beginPacket(RF24_HUB_SERVER, RF24_HUB_UDP_PORTNO);
  udp.write((char*)&udpdata, sizeof(udpdata));
  udp.endPacket();
  if (eepromdata.log_rf24) write2log(printPayload("S>H", &udpdata.payload, info_str));
#endif
}

void handlesensor(char* myjson, call_t call) {
  char tmp[10];
#if defined(SENSOR_18B20)
  uint8_t resolution = 0;
  float tempC = -99;
  uint8_t numDev = sensors.getDeviceCount();
  if ( numDev > 0 ) {
    resolution = sensors.getResolution();
    sensors.requestTemperatures();
    tempC = sensors.getTempCByIndex(0);
  }
  switch ( call ) {
    case sensormqtt:
#if defined(SENSOR_CHANNEL)
      snprintf(tmp,9,"%4.1f",tempC);
      send_udp_msg(SENSOR_NODE, calcTransportValue(SENSOR_CHANNEL,tempC));
#endif
    case sensorweb:
      sprintf(myjson,"{\"Sensor\":\"18B20\", \"Temperatur\":%4.1f, \"Resolution\":%u }", tempC, resolution);
    break;
    default:
      sprintf(myjson,"{\"msg1txt\":\"Temperatur %4.1f &deg;C\"}", tempC);
  }
#endif
#if defined(SENSOR_ANALOG)
  int myval = analogRead(A0);
  switch ( call ) {
    case sensorinfo:
      sprintf(myjson,"{\"Sensor\":\"analog an A0\", \"Messwert\":%d }", myval);
    break;
    default:
      sprintf(myjson,"{\"msg1txt\":\"%s %d\"}", SENSOR_TEXT, myval);
  }  
#endif
#if defined(NOSENSOR)
  switch ( call ) {
    case sensorinfo:
      sprintf(myjson,"{\"Sensor\":\"Kein Sensor angeschlossen\" }");
    break;
    default:
      sprintf(myjson,"{\"msg1\":0}");
  }  
#endif
}

void handleWebRoot() {
  File file = LittleFS.open("/index.html", "r");
  if (file.available()) {
    httpServer.send(200, "text/html", file.readString() );
  } else {
    httpServer.send(200, "text/plain", "Datei nicht vorhanden" );
  }
  file.close();
}

void setupFS() {                                                                       // Funktionsaufruf "setupFS();" muss im Setup eingebunden werden
  LittleFS.begin();
  httpServer.onNotFound([]() {
    if (!handleFile(httpServer.urlDecode(httpServer.uri())))
      httpServer.send(404, "text/plain", "FileNotFound");
  });
}

bool handleFile(String &&path) {
  if (path.endsWith("/")) path += "index.html";
  return LittleFS.exists(path) ? ({File f = LittleFS.open(path, "r"); httpServer.streamFile(f, mime::getContentType(path)); f.close(); true;}) : false;
}

void fill_sysinfo1(char* mystr) {
  int rssi = WiFi.RSSI();
  int rssi_quality = 0;
  if (rssi <= -100) { rssi_quality = 0; } else if (rssi >= -50) { rssi_quality = 100; } else { rssi_quality = 2 * (rssi + 100); }
  snprintf (mystr,INFOSIZE, "{\"Hostname\":\"%s\", \"SSID\":\"%s (%ddBm / %d%%)\", \"IP\":\"%s\", \"Channel\":\"%d\", \"GW-IP\":\"%s\"}",
             WiFi.hostname().c_str(), WiFi.SSID().c_str(), rssi, rssi_quality, WiFi.localIP().toString().c_str(), WiFi.channel(), WiFi.gatewayIP().toString().c_str());           
}

void fill_sysinfo2(char* mystr) {
   snprintf (mystr,INFOSIZE, "{\"Freespace\":\"%0.0fKB\", \"Sketchsize\":\"%0.0fKB\", \"FlashSize\":\"%dMB\", \"FlashFreq\":\"%dMHz\", \"CpuFreq\":\"%dMHz\"}",
           ESP.getFreeSketchSpace() / 1024.0, ESP.getSketchSize() / 1024.0, (int)(ESP.getFlashChipSize() / 1024 / 1024), (int)(ESP.getFlashChipSpeed() / 1000000), (int)(F_CPU / 1000000)    );   
}

void fill_sysinfo3(char* mystr) {
  uint32_t free;
  uint16_t max;
  uint8_t frag;
  ESP.getHeapStats(&free, &max, &frag);
  snprintf (mystr,INFOSIZE, "{\"MAC\":\"%s\",\"SubNetMask\":\"%s\",\"ResetReason\":\"%s\",\"Heap_max\":\"%0.2fKB\",\"Heap_free\":\"%0.2fKB\",\"Heap_frag\":\"%u\"}",
           WiFi.macAddress().c_str(), WiFi.subnetMask().toString().c_str(), ESP.getResetReason().c_str(), (float)max/1024.0, (float)free/1024.0, frag);
}

void fill_sysinfo4(char* mystr) {
  snprintf (mystr,INFOSIZE, "{\"DnsIP\":\"%s\", \"BSSID\":\"%s\", \"CoreVer\":\"%s\", \"IdeVer\":\"%u\", \"SdkVer\":\"%s\"}",
          WiFi.dnsIP().toString().c_str(), WiFi.BSSIDstr().c_str(), ESP.getCoreVersion().c_str(), ARDUINO, ESP.getSdkVersion());  
}

void fill_sysinfo5(char* mystr) {
  uptime::calculateUptime();
  snprintf (mystr,INFOSIZE, "{\"MQTT-Server\":\"%s\", \"MQTT-Hostname\":\"%s\", \"UpTime\":\"%uT%02u:%02u:%02u\", \"SW\":\"%s / %s\"}",
          MQTT_SERVER, MQTT_NODENAME, uptime::getDays(), uptime::getHours(), uptime::getMinutes(), uptime::getSeconds(), SWVERSION_STR, __DATE__ );
}

void fill_sysinfo6(char* mystr) {
  snprintf (mystr,INFOSIZE, "{\"RF24HUB-Server\":\"%s\", \"RF24HUB-Port\":%d, \"RF24GW-Port\":%d}",
          RF24_HUB_SERVER, RF24_HUB_UDP_PORTNO, RF24_GW_UDP_PORTNO );
}

void fill_webcfg1(char* mystr) {
  sprintf(mystr, "{\"titel1\":\"");
  strcat(mystr, TITEL1);
  strcat(mystr, "\"");
#if defined(TITEL2)  
  strcat(mystr, ",\"titel2\":\"");
  strcat(mystr, TITEL2);
  strcat(mystr, "\"");
#endif               
#if defined(MESSAGE1)
  strcat(mystr, ",\"msg1\":1 "); 
#endif
#if defined(MESSAGE2)
  strcat(mystr, ",\"msg2\":1 ");
#endif
#if defined(SWITCH1)
  strcat(mystr, ",\"sw1\":1 ");
#endif
#if defined(SWITCH2)
  strcat(mystr, ",\"sw2\":1 ");
#endif
#if defined(SWITCH3)
  strcat(mystr, ",\"sw3\":1 ");
#endif
#if defined(SWITCH4)
  strcat(mystr, ",\"sw4\":1 ");
#endif
#if defined(RF24GW)
  strcat(mystr, ",\"rf24\":1 ");
#endif
#if defined(LEDMATRIX)
  strcat(mystr,", \"ledmatrix\":1 ");
#endif
#if defined(NEOPIXEL)
  strcat(mystr,", \"neopixel\":1 ");
#endif
#if defined(LEDPWM)
  strcat(mystr,", \"ledpwm\":1 ");
#endif
  strcat(mystr,"}");
}

void fill_webcfg2(char* mystr) {
  snprintf(mystr,INFOSIZE, "{\"logfile\":\"%d\", \"logger\":\"%d\", \"log_sensor\":\"%d\", \"log_rf24\":\"%d\", \"log_startup\":\"%d\", \"log_mqtt\":\"%d\", \"log_webcmd\":\"%d\", \"log_sysinfo\":\"%d\"}",
          eepromdata.logfile, eepromdata.logger, eepromdata.log_sensor, eepromdata.log_rf24, eepromdata.log_startup, eepromdata.log_mqtt, eepromdata.log_webcmd, eepromdata.log_sysinfo);
}

void fill_timeStr() {
  time(&now);                       // read the current time
  localtime_r(&now, &tm);           // update the structure tm with the current time
  snprintf(timeStr,9,"%02d:%02d:%02d",tm.tm_hour,tm.tm_min,tm.tm_sec);
}

void write2log(char* text) {
  fill_timeStr();
  if (eepromdata.logfile) {
    File f = LittleFS.open("/logfile.txt", "a");
    if (f) {
      f.print(timeStr);
      f.print(": ");
      f.print(text);
      f.print("\n");
      f.close();
    }
  }  
  if (eepromdata.logger) {
    logger.print(timeStr);
    logger.println(text);
  }
}

#if defined(MQTT)
void mqtt_send_stat() {
  char tmp[LOGGER_LINESIZE];
  if (eepromdata.log_mqtt) {
    snprintf(info_str,INFOSIZE,"%s",F("Sende MQTT stat Interval"));
    write2log(info_str);
  }
#if defined(SWITCH1)
  mqtt_send_swtch1();
#endif
#if defined(SWITCH2)
  mqtt_send_swtch2();
#endif
#if defined(SWITCH3)
  mqtt_send_swtch3();
#endif
#if defined(SWITCH4)
  mqtt_send_swtch4();
#endif
#if defined(SENSOR_18B20) || defined(SENSOR_ANALOG)
  handlesensor(info_str, sensormqtt);
  snprintf(mytopic,TOPIC_BUFFER_SIZE,"%s/%s/%s","stat",MQTT_NODENAME,"sensordata");
  mqttClient.publish(mytopic, info_str);
  if (eepromdata.log_mqtt) {
    write2log(mytopic);
    write2log(info_str);
  }
#endif
  handlestatus(info_str);
  if ( strlen(info_str) > 5 ) {
    snprintf(mytopic,TOPIC_BUFFER_SIZE,"%s/%s/%s","stat",MQTT_NODENAME,"devicestatus");
    mqttClient.publish(mytopic, info_str);
    if (eepromdata.log_mqtt) {
      write2log(mytopic);
      write2log(info_str);
    }
  }
}

void mqtt_send_tele() {
    if (eepromdata.log_mqtt) {
      snprintf(info_str,INFOSIZE,"%s",F("Sende MQTT tele Interval"));
      write2log(info_str);
    }
    fill_sysinfo1(info_str);
    snprintf(mytopic,TOPIC_BUFFER_SIZE,"%s/%s/%s","tele",MQTT_NODENAME,"info1");
    mqttClient.publish(mytopic, info_str);
    if (eepromdata.log_mqtt) { write2log(mytopic); write2log(info_str); }
    fill_sysinfo2(info_str);
    snprintf(mytopic,TOPIC_BUFFER_SIZE,"%s/%s/%s","tele",MQTT_NODENAME,"info2");
    mqttClient.publish(mytopic, info_str);
    if (eepromdata.log_mqtt) { write2log(mytopic); write2log(info_str); }
    fill_sysinfo3(info_str);
    snprintf(mytopic,TOPIC_BUFFER_SIZE,"%s/%s/%s","tele",MQTT_NODENAME,"info3");
    mqttClient.publish(mytopic, info_str);
    if (eepromdata.log_mqtt) { write2log(mytopic); write2log(info_str); }
    fill_sysinfo4(info_str);
    snprintf(mytopic,TOPIC_BUFFER_SIZE,"%s/%s/%s","tele",MQTT_NODENAME,"info4");
    mqttClient.publish(mytopic, info_str);
    if (eepromdata.log_mqtt) { write2log(mytopic); write2log(info_str); }
    fill_sysinfo5(info_str);
    snprintf(mytopic,TOPIC_BUFFER_SIZE,"%s/%s/%s","tele",MQTT_NODENAME,"info5");
    mqttClient.publish(mytopic, info_str);
    if (eepromdata.log_mqtt) { write2log(mytopic); write2log(info_str); }
}

void callback(char* topic, byte* payload, unsigned int length) {
  char delimiter[] = "/";
  char *ptr;
  tristate_t mystate;
  char part1[TOPIC_PART1_SIZE];
  char part2[TOPIC_PART2_SIZE];
  char part3[TOPIC_PART3_SIZE];
  char msg[10]; 
  char* cmd = (char*)malloc(length+2);
  snprintf(cmd,length+1,"%s",(char*)payload);
  snprintf (info_str,INFOSIZE, "Callback Msg: T:%s l:%u c:%s", topic, length, cmd);
  if (eepromdata.log_mqtt) {
    write2log(info_str);
  }
  ptr = strtok(topic, delimiter);
  if(ptr != NULL) snprintf(part1, TOPIC_PART1_SIZE, "%s", ptr);
  ptr = strtok(NULL, delimiter);
  if(ptr != NULL) snprintf(part2, TOPIC_PART2_SIZE, "%s", ptr);
  ptr = strtok(NULL, delimiter);
  if(ptr != NULL) snprintf(part3, TOPIC_PART3_SIZE, "%s", ptr);
  if ( strncmp(part1,MQTT_CMD, sizeof MQTT_CMD) == 0 ) {
    if ( strncmp(part2, MQTT_NODENAME, sizeof MQTT_NODENAME) == 0 ) {
#if defined(SWITCH1)      
      if ( strncmp(part3, SWITCH1MQTT, sizeof SWITCH1MQTT) == 0 ) {
        mystate = get_tristate(cmd);
        handleSwitch1(mystate);
        snprintf(msg,10,"%s",mystate==on? c_on: c_off);
        snprintf(mytopic,TOPIC_BUFFER_SIZE,"%s/%s/%s","stat",MQTT_NODENAME,SWITCH1MQTT);
        mqttClient.publish(mytopic, msg, strlen(msg) );
      }
#endif      
#if defined(SWITCH2)      
      if ( strncmp(part3, SWITCH2MQTT, sizeof SWITCH2MQTT) == 0 ) {
        mystate = get_tristate(cmd);
        handleSwitch2(mystate);
        snprintf(msg,10,"%s",mystate==on? c_on: c_off);
        snprintf(mytopic,TOPIC_BUFFER_SIZE,"%s/%s/%s","stat",MQTT_NODENAME,SWITCH2MQTT);
        mqttClient.publish(mytopic, msg, strlen(msg) );
      }
#endif      
#if defined(SWITCH3)      
      if ( strncmp(part3, SWITCH3MQTT, sizeof SWITCH3MQTT) == 0 ) {
        mystate = get_tristate(cmd);
        handleSwitch3(mystate);
        snprintf(msg,10,"%s",mystate==on? c_on: c_off);
        snprintf(mytopic,TOPIC_BUFFER_SIZE,"%s/%s/%s","stat",MQTT_NODENAME,SWITCH3MQTT);
        mqttClient.publish(mytopic, msg, strlen(msg) );
      }
#endif      
#if defined(SWITCH4)      
      if ( strncmp(part3, SWITCH4MQTT, sizeof SWITCH4MQTT) == 0 ) {
        mystate = get_tristate(cmd);
        handleSwitch4(mystate);
        snprintf(msg,10,"%s",mystate==on? c_on: c_off);
        snprintf(mytopic,TOPIC_BUFFER_SIZE,"%s/%s/%s","stat",MQTT_NODENAME,SWITCH4MQTT);
        mqttClient.publish(mytopic, msg, strlen(msg) );
      }
#endif      
#if defined(LEDMATRIX)
      if ( strncmp(part3, "graph", sizeof "graph") == 0 ) {
        for(unsigned int pos=0; pos+5<=length; pos+=5){
          unsigned int cur_x = (cmd[pos]-'0')*10 + (cmd[pos+1]-'0');
          unsigned int cur_y = (cmd[pos+2]-'0')*10 + (cmd[pos+3]-'0');
          matrix.setPixel(cur_x, cur_y, cmd[pos+4]-'0'); 
        }
        matrix.display();
      }
      if ( strncmp(part3, "intensity", sizeof "intensity") == 0 ) {
        uint8_t intensity = atoi(cmd);
        if ( intensity <16 ) {
          matrix.setIntensity(intensity);
          snprintf(info_str,INFOSIZE,"{ \"intensity\":%u }",matrix.getIntensity());
          snprintf(mytopic,TOPIC_BUFFER_SIZE,"%s/%s/%s","stat",MQTT_NODENAME,"matrixdata");
          mqttClient.publish(mytopic, info_str);
        }
      }
      if ( strncmp(part3, "line", sizeof "line") == 0 ) {
        print_line(cmd);
      }
#endif
#if defined(NEOPIXEL)
      if ( strncmp(part3, "RGB", sizeof "RGB") == 0 ) {
        char *eptr;
        rgb = strtoul(cmd, &eptr, 10);
        if (state_switch1) set_neopixel(on);
        snprintf(mytopic,TOPIC_BUFFER_SIZE,"%s/%s/%s","stat",MQTT_NODENAME,"devicestatus");
        mqttClient.publish(mytopic, cmd);
      }
#endif
#if defined(LEDPWM)
      if ( strncmp(part3, "intensity", sizeof "intensity") == 0 ) {
        char *eptr;
        intensity = strtoul(cmd, &eptr, 10);
        if (state_switch1) set_ledpwm(on);
        snprintf(mytopic,TOPIC_BUFFER_SIZE,"%s/%s/%s","stat",MQTT_NODENAME,"intensity");
        mqttClient.publish(mytopic, cmd);
      }
#endif
    }
  }
  // Free the memory
  free(cmd);  
}

void mqtt_reconnect() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    // Attempt to connect
    if (mqttClient.connect(MQTT_NODENAME)) {
      // Once connected, publish an announcement...
      // mqtt_send_stat();
      // ... and resubscribe
      snprintf(mytopic,TOPIC_BUFFER_SIZE,"%s/%s/%s","cmnd",MQTT_NODENAME,"#");
      mqttClient.subscribe(mytopic);  
    } else {
      // Wait 1 seconds before retrying
      delay(1000);
    }
  }
}
#endif

#if defined(RF24GW)
char* printPayload(char* prefix, payload_t *payload, char* placeholder) {
  char buf1[10];
  char buf2[10];
  char buf3[10];
  char buf4[10];
  char buf5[10];
  char buf6[10];
  snprintf(placeholder,INFOSIZE,"%s: N:%u T:%u m:%u F:0x%02x O:%u H:%u (%u/%s)(%u/%s)(%u/%s)(%u/%s)(%u/%s)(%u/%s)", prefix
                     ,payload->node_id, payload->msg_type, payload->msg_id, payload->msg_flags, payload->orderno, payload->heartbeatno
                     ,getChannel(payload->data1), unpackTransportValue(payload->data1, buf1)
                     ,getChannel(payload->data2), unpackTransportValue(payload->data2, buf2)
                     ,getChannel(payload->data3), unpackTransportValue(payload->data3, buf3)
                     ,getChannel(payload->data4), unpackTransportValue(payload->data4, buf4)
                     ,getChannel(payload->data5), unpackTransportValue(payload->data5, buf5)
                     ,getChannel(payload->data6), unpackTransportValue(payload->data6, buf6)
                     );
  return placeholder;
}
#endif

void loop() {
  delay(0);
#if defined(RF24GW)
  while ( radio.available() ) {
    radio.read(&payload, sizeof(payload));
    if (eepromdata.log_rf24) write2log(printPayload("N>G", &payload, info_str));
    udpdata.gw_no = RF24_GW_NO;
    memcpy(&udpdata.payload, &payload, sizeof(payload));
    udp.beginPacket(RF24_HUB_SERVER, RF24_HUB_UDP_PORTNO);
    udp.write((char*)&udpdata, sizeof(udpdata));
    udp.endPacket();
  }
  if (udp.parsePacket() > 0 ) {
    udp.read((char*)&udpdata, sizeof(udpdata));
    memcpy(&payload, &udpdata.payload, sizeof(payload));
    if (eepromdata.log_rf24) write2log(printPayload("G>N", &payload, info_str));
    radio.stopListening();
    radio.write(&payload, sizeof(payload));
    radio.startListening(); 
  }
#endif
#if defined(MQTT)
  if ( ! mqttClient.connected()) {
    if (eepromdata.log_mqtt) {
      sprintf(info_str,"%s",F("MQTT reconnect"));
      write2log(info_str);
    }
    mqtt_reconnect();
  }
  mqttClient.loop();
  delay(0);
  if ((millis() - lastMsg) > STATINTERVAL) {
    lastMsg = millis();
    mqtt_send_stat();
  }
  if ((millis() - lastInfo) > TELEINTERVAL) {
    lastInfo = millis();
    mqtt_send_tele();
  }
#endif  
  delay(0);
  httpServer.handleClient();
//  MDNS.update();
//  ArduinoOTA.handle(); // Wait for OTA connection
  wifi_con();
  delay(0);
}
