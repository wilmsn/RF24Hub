/******************************************************
The following settings ca be used for the individual node:
mandatory:
// #define 

optional:
- This node has a buildin rf24-gateway
// #define RF24GW
- Select a sensor
-- A Dummy for test purposes
// #define SENSOR_DUMMY
-- a Bosch sensor like BMP185/BMP280/BME280
// #define SENSOR_BOSCH
-- A Dallas 18B20 temperature sensor
// #define SENSOR_18B20
-- A Switch / Relais
-- define a pin
// #define RELAIS_1 3
// #define RELAIS_2 3
-- Use Neopixel
// #define NEOPIXEL <Number of Pixel>
-- Use LEDMATRIX
// #define LEDMATRIX <Number of 8x8 Displays>
- the status LED
-- define a pin (default is 3)
// #define STATUSLED       <LED PIN>
-- a pin level for on (default is HIGH)
// #define STATUSLED_ON    <LEVEL>
-- a pin level for off (default is LOW)
// #define STATUSLED_OFF   <LEVEL>
- version of the EEPROM (only a different version stores new values!!)
// #define EEPROM_VERSION  5

******************************************************/
//*****************************************************
//    Individual settings
//-----------------------------------------------------
#if defined(TEICHPUMPE)
#define HOSTNAME               "teichpumpe"
#define MQTT_SERVER            "rpi1.fritz.box"
#define MQTT_NODENAME          "teichpumpe"
#define TITEL1                 "Teichpumpe"
#define RF24GW
#define RF24_HUB_SERVER        "rpi1.fritz.box"
#define RF24_GW_NO             101
#define SENSOR_18B20
#define SENSOR_NODE            1
#define SENSOR_CHANNEL         11
#define SWITCH1               
#define SWITCH1PIN1            0        
#define SWITCH1PIN2            2
#define SWITCH1TXT             "Relais"
#define SWITCH1INITSTATE       false
#define SWITCH1ACTIVESTATE     LOW
#define SWITCH1MQTT            "relais"
#define SWITCH1_NODE           1
#define SWITCH1_CHANNEL        12
#endif
//-----------------------------------------------------
#if defined(TERASSENNODE)
#define HOSTNAME               "terassennode"
#define MQTT_SERVER            "rpi1.fritz.box"
#define MQTT_NODENAME          "terassennode"
#define TITEL1                 "TerassenNode"
#define RF24GW
#define RF24_HUB_SERVER        "rpi1.fritz.box"
#define RF24_GW_NO             104
#define SENSOR_18B20
#define SENSOR_NODE            1
#define SENSOR_CHANNEL         1
#define SWITCH1               
#define SWITCH1PIN1            2
#define SWITCH1TXT             "Terasse"
#define SWITCH1INITSTATE       LOW
#define SWITCH1ACTIVESTATE     LOW
#define SWITCH1MQTT            "terasse"
#define SWITCH1_NODE           1
#define SWITCH1_CHANNEL        2
#define SWITCH2               
#define SWITCH2PIN1            0
#define SWITCH2TXT             "Balkon"
#define SWITCH2INITSTATE       LOW
#define SWITCH2ACTIVESTATE     LOW
#define SWITCH2MQTT            "balkon"
#define SWITCH2_NODE           1
#define SWITCH2_CHANNEL        3
#endif
//-----------------------------------------------------
#if defined(WOHNZIMMERNODE)
#define HOSTNAME               "wohnzimmernode"
#define MQTT_SERVER            "rpi1.fritz.box"
#define MQTT_NODENAME          "wohnzimmernode"
#define TITEL1                 "Wohnzimmernode"
#define LINE_SIZE              15
#define RF24_GW_NO             102
#define RF24_HUB_SERVER        "rpi1.fritz.box"
#define RF24GW
#define SENSOR_18B20
#define SENSOR_NODE            1
#define SENSOR_CHANNEL         6
#define LEDMATRIX
#define SWITCH1               
#define SWITCH1TXT             "Display"
#define SWITCH1MQTT            "display"
#define SWITCH1INITSTATE       true
#endif
//-----------------------------------------------------
#if defined(FLURLICHT)
#define HOSTNAME               "flurlicht"
#define MQTT_NODENAME          "flurlicht"
#define TITEL1                 "Flurlicht"
#define TITEL2                 "Licht im Flur"
#define LEDPWM
#define LEDPWMINIT             150
#define SWITCH1               
#define SWITCH1TXT             "Licht"
#define SWITCH1MQTT            "licht"
#define SWITCH1INITSTATE       false
#define SWITCH1_NODE           1
#define SWITCH1_CHANNEL        17
#define RF24_GW_NO             103
#define SENSOR_18B20
#define SENSOR_NODE            1
#define SENSOR_CHANNEL         16

#endif
//-----------------------------------------------------
#if defined(WITTYNODE)
#define HOSTNAME               "wittynode"
#define MQTT_NODENAME          "wittynode"
#define TITEL1                 "Wittynode"
#define TITEL2                 "A Witty Node"
#define SWITCH1
#define SWITCH1PIN1           WITTY_RGB_BL
#define SWITCH1TXT            "Blau"
#define SWITCH1INITSTATE      LOW
#define SWITCH1ACTIVESTATE    HIGH
#define SWITCH1MQTT           "RGB_BL"
#define SWITCH2
#define SWITCH2PIN1           WITTY_RGB_GN
#define SWITCH2TXT            "Grün"
#define SWITCH2INITSTATE      LOW
#define SWITCH2ACTIVESTATE    HIGH
#define SWITCH2MQTT           "RGB_GN"
#define SWITCH3
#define SWITCH3PIN1           WITTY_RGB_RT
#define SWITCH3TXT            "Rot"
#define SWITCH3INITSTATE      LOW
#define SWITCH3ACTIVESTATE    HIGH
#define SWITCH3MQTT           "RGB_RT"
#define SENSOR_ANALOG
#define SENSOR_TEXT           "LDR: "      
#endif
//-----------------------------------------------------
#if defined(RF24GWTEST)
#define HOSTNAME               "rf24gwtest"
#define MQTT_NODENAME          "rf24gwtest"
#define TITEL1                 "rf24gwtest"
#define RF24GW
#define RF24_GW_NO             102
#define SENSOR_18B20
#define SWITCH1
#define SWITCH1TXT             "int. LED"
#define SWITCH1PIN1            BUILTIN_LED
#define SWITCH1INITSTATE       LOW
#define SWITCH1ACTIVESTATE     LOW
#define SWITCH1MQTT            "intLED"
#endif
//-----------------------------------------------------
#if defined(ESPMINI)
#define HOSTNAME               "espmini"
#define MQTT_NODENAME          "espmini"
#define TITEL1                 "ESPmini"
#define SWITCH1
#define SWITCH1TXT             "int. LED"
#define SWITCH1PIN1            BUILTIN_LED
#define SWITCH1INITSTATE       LOW
#define SWITCH1ACTIVESTATE     LOW
#define SWITCH1MQTT            "intLED"
#endif
//-----------------------------------------------------
#if defined(TESTNODE)
#define HOSTNAME               "testnode"
#define MQTT_NODENAME          "testnode"
#define TITEL1                 "TestNode"
#undef  MQTT
//#define RF24GW
#define RF24_GW_NO             105
//#define SENSOR_18B20
#define SWITCH1               
#define SWITCH1PIN1            2
#define SWITCH1TXT             "Terasse"
#define SWITCH1INITSTATE       LOW
#define SWITCH1ACTIVESTATE     LOW
#define SWITCH1MQTT            "terasse"
#define SWITCH2               
#define SWITCH2PIN1            0
#define SWITCH2TXT             "Balkon"
#define SWITCH2INITSTATE       LOW
#define SWITCH2ACTIVESTATE     LOW
#define SWITCH2MQTT            "balkon"
#endif
//-----------------------------------------------------

//define constrains for precompiler
#if defined(SENSOR_18B20)
#define MESSAGE1
#undef NOSENSOR
#endif
#if defined(SENSOR_ANALOG)
#define MESSAGE1
#undef NOSENSOR
#endif
#if defined(RF24_GW_NO)
#define RF24GW
#endif
