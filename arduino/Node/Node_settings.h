/******************************************************
The following settings ca be used for the individual node:
mandatory:
// #define RF24NODE        <NODE NUMBER>

optional:
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
#if defined(SOLARNODE)
#define RF24NODE               202
#define LOW_VOLT_LEVEL         2.2
#define EEPROM_VERSION         5
#define STATUSLED              A2
#define SLEEPTIME_SEC          30
#define EMPTYLOOPS             9
#define SOLARZELLE             A0
#define R_SOLAR                340.0
#define LOAD_BALANCER_BATT     A1
// Ist die Batteriespannung groesser als USE_BATTERIE wird der Tiefschlaf von MC und Radio abgeschaltet
//#define USE_BATTERIE           2.7
//#define DISCHARGE_U            2,8
// Am DISCHARGE_PIN liegt ein Widerstand von 340 Ohm gegen Vcc 
//#define DISCHARGE_PIN          5
// geaenderte Sendeparameter zum Test
#define MAX_SENDCOUNT          3
#define SENDDELAY              500
#define LOW_VOLT_LOOPS         90

#endif
//-----------------------------------------------------
#if defined(AUSSENTHERMOMETER)
#define RF24NODE        200
#define SENSOR_BOSCH
#define STATUSLED       3
#define STATUSLED_ON    HIGH
#define STATUSLED_OFF   LOW
#define LOW_VOLT_LEVEL  1
#define EEPROM_VERSION  5
#define SLEEPTIME_SEC   900
#endif
//-----------------------------------------------------
#if defined(AUSSENTHERMOMETER2)
#define RF24NODE        201
#define SENSOR_BOSCH
#define STATUSLED       7
#define STATUSLED_ON    HIGH
#define STATUSLED_OFF   LOW
#define LOW_VOLT_LEVEL  1
#define EEPROM_VERSION  1
#define SLEEPTIME_SEC   900
#endif
//-----------------------------------------------------
#if defined(SCHLAFZIMMERTHERMOMETER)
#define RF24NODE             101
#define SENSOR_18B20
#define DISPLAY_5110
// #define MONITOR
#define EEPROM_VERSION       1
#define EMPTYLOOPS           9
#define VOLT_OFF             0.55
#define LOW_VOLT_LEVEL       3.6
//#define SLEEPTIME_SEC        110
//#define STATUSLED_ON         LOW
//#define STATUSLED_OFF        HIGH
#endif
//-----------------------------------------------------
#if defined(WOHNZIMMERTHERMOMETER)
#define RF24NODE             102
#define SENSOR_BOSCH
#define DISPLAY_5110
#define EEPROM_VERSION       3
#define EMPTYLOOPS           9
#define VOLT_OFF             0.65
#define LOW_VOLT_LEVEL       3.6
#define STATUSLED_ON         LOW
#define STATUSLED_OFF        HIGH
#endif
//-----------------------------------------------------
#if defined(KUECHETHERMOMETER)
#define RF24NODE             103
#define SENSOR_18B20
#define DISPLAY_5110
//#define MONITOR
#define EEPROM_VERSION       1
#define EMPTYLOOPS           9
#define VOLT_OFF             0.55
#define LOW_VOLT_LEVEL       3.6
#define STATUSLED_ON         LOW
#define STATUSLED_OFF        HIGH
#endif
//-----------------------------------------------------
#if defined(GAESTEZIMMERTHERMOMETER)
#define RF24NODE             104
#define SLEEPTIME_SEC        900
#define STATUSLED            7
#define SENSOR_18B20
#define EEPROM_VERSION       1
#define EMPTYLOOPS           0
#define LOW_VOLT_LEVEL       3.6
#endif
//-----------------------------------------------------
#if defined(KUGELNODE1)
#define RF24NODE             106
#define SLEEPTIME_SEC        300
#define SENSOR_BOSCH
#define DISPLAY_5110
#define EEPROM_VERSION       2
#define EMPTYLOOPS           0
#define LOW_VOLT_LEVEL       3
#define VOLT_OFF             0.55
#define CONTRAST             55
#endif
//-----------------------------------------------------
#if defined(KUGELNODE2)
#define RF24NODE             107
#define SLEEPTIME_SEC        300
#define SENSOR_BOSCH
#define DISPLAY_5110
#define EEPROM_VERSION       1
#define EMPTYLOOPS           0
#define LOW_VOLT_LEVEL       3.5
#define VOLT_OFF             0.55
//#define CONTRAST             45
//#define RADIO_CE_PIN    9
//#define RADIO_CSN_PIN   10
#endif
//-----------------------------------------------------
#if defined(BASTELZIMMERTHERMOMETER)
#define RF24NODE             100
#define SENSOR_18B20
#define DISPLAY_5110
//#define MONITOR
#define EEPROM_VERSION       3
#define VOLT_OFF             0.55
#define EMPTYLOOPS           9
#define LOW_VOLT_LEVEL       3.6
#define STATUSLED_ON         LOW
#define STATUSLED_OFF        HIGH
#endif
//-----------------------------------------------------
#if defined(BASTELZIMMERTHERMOMETER_SW)
#define RF24NODE             105
#define SENSOR_18B20
#define DISPLAY_5110
//#define MONITOR
#define EEPROM_VERSION       3
#define VOLT_OFF             0.55
#define EMPTYLOOPS           9
#define LOW_VOLT_LEVEL       3.6
#define STATUSLED_ON         LOW
#define STATUSLED_OFF        HIGH
#endif
//-----------------------------------------------------
#if defined(ANKLEIDEZIMMERTHERMOMETER)
#define RF24NODE             110
#define SENSOR_18B20
#define DISPLAY_5110
//#define MONITOR
#define EEPROM_VERSION       1
#define EMPTYLOOPS           9
#define VOLT_OFF             0.68
#define LOW_VOLT_LEVEL       3.0
#define SLEEP4MS_FAC         950
#define STATUSLED_ON         LOW
#define STATUSLED_OFF        HIGH
#endif
//-----------------------------------------------------
//    Testnodes
//-----------------------------------------------------
#if defined(FEUCHTESENSOR_170)
#define RF24NODE             170
#define EEPROM_VERSION       1
#define LOW_VOLT_LEVEL       1.8
#define EEPROM_VERSION       5
#define SLEEPTIME_SEC        900
#define EMPTYLOOPS           0
#endif
//-----------------------------------------------------
#if defined(TESTNODE_UNO)
#define RF24NODE             165
#define SLEEPTIME_SEC        60
#define EMPTYLOOPS           0
#define SENSOR_BOSCH
#define DISPLAY_5110
#define EEPROM_VERSION       2
#define LOW_VOLT_LEVEL       3.5
//#define VOLT_OFF             0.55
#define CONTRAST             45
#define DEBUG_SERIAL_SENSOR
#define DEBUG_SERIAL_RADIO
#define DEBUG_SERIAL_PROC
#endif
//-----------------------------------------------------
#if defined(TESTNODE)
#define RF24NODE             101
#define SENSOR_18B20         8
#define EEPROM_VERSION       1
#define VOLT_OFF             0.55
#define LOWVOLTAGELEVEL      2.0
#define EEPROM_VERSION       17
#endif


// ------ End of configuration part ------------

//define constrains
#if defined(LOAD_BALANCER_BATT)
#define LOAD_BALANCER          
#endif

//define constrains for debugging
#if defined(DEBUG_SERIAL_SENSOR)
#define DEBUG_SERIAL
#endif
#if defined(DEBUG_SERIAL_RADIO)
#define DEBUG_SERIAL
#endif
#if defined(DEBUG_SERIAL_PROC)
#define DEBUG_SERIAL
#endif
