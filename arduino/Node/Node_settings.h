/******************************************************
The following settings ca be used for the individual node:
mandatory:
// #define RF24NODE        <NODE NUMBER>

optional:
- To mark a node as heartbeatnode use:
// #define HBNODE
- Select a sensor
-- a Bosch sensor like BMP185/BMP280/BME280
// #define BOSCH_SENSOR
-- A Dallas 18B20 temperature sensor
// #define DALLAS_18B20
-- A Switch / Relais
-- define a pin
// #define RELAIS_1 3
// #define RELAIS_2 3
// #define RELAIS_3 3
// #define RELAIS_4 3
-- Use Neopixel
// #define NEOPIXEL <Number of Pixel>
- the status LED
-- define a pin (default is 3)
// #define STATUSLED       <LED PIN>
-- a pin level for on (default is HIGH)
// #define STATUSLED_ON    <LEVEL>
-- a pin level for off (default is LOW)
// #define STATUSLED_OFF   <LEVEL>
- define a low Voltage Level - where the node needs help (default is 2V)
// #define LOW_VOLT_LEVEL  <VOLT>
- version of the EEPROM (only a different version stores new values!!)
// #define EEPROM_VERSION  5
- the slleptime between 2 readings
// #define SLEEPTIME_SEC   900

debugging options:
// To get more Details about the sensor on serial monitor use:
// #define DEBUG_SERIAL_SENSOR
// To get more Details about the radio on serial monitor use:
// #define DEBUG_SERIAL_RADIO
// To get more Details about processing of data on serial monitor use:
// #define DEBUG_SERIAL_PROC

******************************************************/
//*****************************************************
//    Individual settings
//-----------------------------------------------------
#if defined(AUSSENTHERMOMETER)
#define HBNODE
#define RF24NODE        200
#define BOSCH_SENSOR
#define STATUSLED       3
#define STATUSLED_ON    HIGH
#define STATUSLED_OFF   LOW
#define LOW_VOLT_LEVEL  1
#define EEPROM_VERSION  5
#define SLEEPTIME_SEC   900
#endif
//-----------------------------------------------------
#if defined(SCHLAFZIMMERTHERMOMETER)
#define HBNODE
#define RF24NODE             101
#define DALLAS_18B20
#define DISPLAY_5110
#define MONITOR
#define EEPROM_VERSION       9
#define EMPTYLOOPS           9
#define VOLT_FAC             1
#define VOLT_OFF             0.55
#define LOW_VOLT_LEVEL       3.6
#define STATUSLED_ON         LOW
#define STATUSLED_OFF        HIGH
#endif
//-----------------------------------------------------
#if defined(TESTZIMMERTHERMOMETER)
#define HBNODE
#define RF24NODE             102
//#define DALLAS_18B20
#define BOSCH_SENSOR
#define DISPLAY_5110
//#define MONITOR
#define EEPROM_VERSION       8
#define EMPTYLOOPS           0
//#define VOLT_FAC             1
//#define VOLT_OFF             0.55
#define LOW_VOLT_LEVEL       0
//#define DEBUG_SERIAL_SENSOR
//#define DEBUG_SERIAL_RADIO
//#define DEBUG_SERIAL_PROC
#endif
//-----------------------------------------------------
#if defined(TESTZIMMER1THERMOMETER)
#define HBNODE
#define RF24NODE             103
#define DALLAS_18B20
#define DISPLAY_5110
//#define DEBUG_LED
//#define MONITOR
#define SLEEPTIME_SEC        20
#define EEPROM_VERSION       7
#define VOLT_FAC             1
#define VOLT_OFF             0.55
#define EMPTYLOOPS           5
#define LOWVOLTAGELEVEL      3.5
#define STATUSLED_ON         LOW
#define STATUSLED_OFF        HIGH
#endif
//-----------------------------------------------------
#if defined(BASTELZIMMERTHERMOMETER)
#define HBNODE
#define RF24NODE             100
#define DALLAS_18B20
#define DISPLAY_5110
#define MONITOR
#define EEPROM_VERSION       3
#define VOLT_FAC             1
#define VOLT_OFF             0.55
#define EMPTYLOOPS           9
#define LOW_VOLT_LEVEL       3.6
#endif
//-----------------------------------------------------
#if defined(ANKLEIDEZIMMERTHERMOMETER)
#define HBNODE
#define RF24NODE             110
#define DALLAS_18B20
#define DISPLAY_5110
#define MONITOR
#define EEPROM_VERSION       22
#define EMPTYLOOPS           9
#define VOLT_FAC             1
#define VOLT_OFF             0.55
#define LOW_VOLT_LEVEL       3.6
#define STATUSLED_ON         LOW
#define STATUSLED_OFF        HIGH
#endif
//-----------------------------------------------------
#if defined(TERASSE)
#define RF24NODE             160
#define EEPROM_VERSION       3
#define RELAIS_1             4
#define RELAIS_2             5
#define RELAIS_ON            LOW
#define RELAIS_OFF           HIGH
#define STATUSLED            A2
#endif
//-----------------------------------------------------
#if defined(TESTSWITCH)
#define RF24NODE             150
#define EEPROM_VERSION       1
#define NEOPIXEL
// The Number of Pixels that are connected to
#define NEOPIXEL             46
#define DEBUG_SERIAL_SENSOR
#endif
