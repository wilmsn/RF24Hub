/**
 * @file Node_settimgs.h
 * @brief This file is used to define your Nodes
 * 
 * For selectable parts and documentation see "Parts.h"
 */

//*****************************************************
//    Individual settings
//-----------------------------------------------------
#if defined(AUSSENTHERMOMETER1)
#define RF24NODE        200
#define SENSOR_BOSCH
#define STATUSLED       3
#define STATUSLED_ON    HIGH
#define STATUSLED_OFF   LOW
#define VOLT_LV         2
#define EEPROM_VERSION  8
#define SLEEPTIME_SEC   900
#endif
//-----------------------------------------------------
#if defined(AUSSENTHERMOMETER2)
#define RF24NODE        201
#define SENSOR_BOSCH
#define STATUSLED       7
#define STATUSLED_ON    HIGH
#define STATUSLED_OFF   LOW
#define VOLT_LV         2
#define EEPROM_VERSION  7
#define SLEEPTIME_SEC   900
#endif
//-----------------------------------------------------
#if defined(SOLARNODE1)
#define RF24NODE               202
#define VOLT_LV                2.2
#define SLEEPTIME_LV           300
#define EEPROM_VERSION         7
#define STATUSLED              A2
#define SLEEPTIME              30
#define EMPTYLOOPS             9
// Mist die Spannung an der Referenzzelle die NUR mit einem Widerstand abgeschlossen ist
#define SOLARZELLE1            A0
// Mist die Spannung an der Versorgungssolarzelle vor dem Step-Up Wandler
#define SOLARZELLE2            A3
// Ist die Batteriespannung groesser als DISCHARGE1 wird der Tiefschlaf vom MC abgeschaltet
#define DISCHARGE1             2.7
// Ist die Batteriespannung groesser als DISCHARGE2 wird zusätzlich zu DISCHARGE1 die Status LED eingeschaltet
#define DISCHARGE2             2.75
// Ist die Batteriespannung groesser als DISCHARGE3 wird zusätzlich zu DISCHARGE2 Strom über einen Widerstand an DISCHARGE3_PIN gegen GND abgeleitet
#define DISCHARGE3             2.8
#define DISCHARGE3_PIN         5
// Load Ballancer
// Mittlerer Pol der Batterie ist über einen 680 Ohm Widerstand an den Pin angeschlossen
#define LOAD_BALLANCER_PIN     A1
// Maximale Differenzspannung zwischen den Zellen
#define LOAD_BALLANCER         0.1
#endif
//-----------------------------------------------------
#if defined(SOLARNODE2)
#define RF24NODE               203
#define VOLT_LV                2.2
#define SLEEPTIME_LV           300
#define EEPROM_VERSION         7
#define STATUSLED              A2
#define SLEEPTIME              30
#define EMPTYLOOPS             9
// Mist die Spannung an der Referenzzelle die NUR mit einem Widerstand abgeschlossen ist
#define SOLARZELLE1            A0
// Mist die Spannung an der Versorgungssolarzelle vor dem Step-Up Wandler
#define SOLARZELLE2            A3
// Ist die Batteriespannung groesser als DISCHARGE1 wird der Tiefschlaf vom MC abgeschaltet
#define DISCHARGE1             2.7
// Ist die Batteriespannung groesser als DISCHARGE2 wird zusätzlich zu DISCHARGE1 die Status LED eingeschaltet
#define DISCHARGE2             2.75
// Ist die Batteriespannung groesser als DISCHARGE3 wird zusätzlich zu DISCHARGE2 Strom über einen Widerstand an DISCHARGE3_PIN gegen GND abgeleitet
#define DISCHARGE3             2.8
#define DISCHARGE3_PIN         5
// Load Ballancer
// Mittlerer Pol der Batterie ist über einen 680 Ohm Widerstand an den Pin angeschlossen
#define LOAD_BALLANCER_PIN     A1
// Maximale Differenzspannung zwischen den Zellen
#define LOAD_BALLANCER         0.1
#endif
//-----------------------------------------------------
#if defined(BASTELZIMMERTHERMOMETER)
#define RF24NODE             100
#define SENSOR_18B20         8
#define DISPLAY_5110
#define EEPROM_VERSION       4
#define VOLT_OFF             0.55
#define EMPTYLOOPS           9
#define SLEEPTIME            120
#define SLEEPTIME_LV         1200
#define VOLT_LV              3.6
#define STATUSLED_ON         LOW
#define STATUSLED_OFF        HIGH
#endif

//-----------------------------------------------------
#if defined(NODE_101)
#define RF24NODE             101
#define SENSOR_18B20         8
#define DISPLAY_5110
#define EEPROM_VERSION       4
#define SLEEPTIME            60
#define EMPTYLOOPS           4
#define VOLT_OFF             0.55
#define VOLT_LV              3.6
#endif
//-----------------------------------------------------
#if defined(BASTELZIMMERTHERMOMETER_SW)
#define RF24NODE             105
#define EEPROM_VERSION       5
#define SENSOR_18B20         8
#define DISPLAY_5110
//#define MONITOR
#define VOLT_OFF             0.55
#define EMPTYLOOPS           9
#define VOLT_LV              3.6
#define SLEEPTIME_LV         1200
#define STATUSLED_ON         LOW
#define STATUSLED_OFF        HIGH
#endif
//-----------------------------------------------------
#if defined(ANKLEIDEZIMMERTHERMOMETER)
#define RF24NODE             110
#define EEPROM_VERSION       4
#define SENSOR_18B20         8
#define DISPLAY_5110
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
#if defined(TESTNODE_240)
#define RF24NODE                      240
#define EEPROM_VERSION                7
#define BATTERY_READ_EXTERNAL         A2
#define BATTERY_VOLTAGEDIVIDER_R1     220
#define BATTERY_VOLTAGEDIVIDER_R2     220
#define VOLT_LV                       3.3
#define SLEEPTIME                     60
#define EMPTYLOOPS                    4
#define SENSOR_18B20                  8
#define STATUSLED                     7
#endif
//-----------------------------------------------------
#if defined(TESTNODE_UNO)
#define RF24NODE             165
#define SLEEPTIME            60
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
#endif


// ------ End of Node configuration part ------------
// The following parameters have to be configured once



// Altitude above sealevel
#ifndef ALTITUDEABOVESEALEVEL
#define ALTITUDEABOVESEALEVEL   95.0
#endif

//         DO NOT EDIT BELOW 
//   - unless you know what you do -
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//define constrains
#if defined(LOAD_BALANCER_PIN)
#ifndef LOAD_BALANCER
#define LOAD_BALANCER      0.2
#endif 
#endif

#if defined(DISPLAY_5110)
#define DISPLAY_ALL
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
#if defined(LOAD_BALANCER) || defined(DISCHARGE1) || defined(DISCHARGE1)
#define BATT_MONITOR
#endif

//basic definitions - can be overwritten in node settings
//EEPROM Version disabled by default !!!!!
#ifndef EEPROM_VERSION
#define EEPROM_VERSION  0
#endif
// The CE Pin of the Radio module
#ifndef RADIO_CE_PIN
#define RADIO_CE_PIN    10
#endif
// The CS Pin of the Radio module
#ifndef RADIO_CSN_PIN
#define RADIO_CSN_PIN   9
#endif

// The status LED
#ifndef STATUSLED
#define STATUSLED 3
#endif
#ifndef STATUSLED_ON
#define STATUSLED_ON  HIGH
#endif
#ifndef STATUSLED_OFF
#define STATUSLED_OFF LOW
#endif

// Relais
#ifndef RELAIS_ON
#define RELAIS_ON     HIGH
#endif
#ifndef RELAIS_OFF
#define RELAIS_OFF    LOW
#endif

// Sensors

// A dummy temperature
#ifdef SENSOR_DUMMY
#ifndef DUMMY_TEMP
#define DUMMY_TEMP              33.3
#endif
// Channel for temperature
#ifndef TEMPERATURE_CHANNEL
#define TEMPERATURE_CHANNEL     1
#endif
#endif

// Dallas 18B20 Sensor
#ifdef SENSOR_18B20
// Resolution for DS18B20 sensor
#ifndef SENSOR_18B20_RESOLUTION
#define SENSOR_18B20_RESOLUTION 9
#endif
// Delaytime for 18B20 measurement
#ifndef SENSOR_18B20_DELAYTIME
#define SENSOR_18B20_DELAYTIME  100
#endif
// Channel for temperature
#ifndef TEMPERATURE_CHANNEL
#define TEMPERATURE_CHANNEL     1
#endif
#endif

// Bosch Sensor (BMP180; BMP280; BME280)
#ifdef SENSOR_BOSCH
// Channel for temperature
#ifndef TEMPERATURE_CHANNEL
#define TEMPERATURE_CHANNEL     1
#endif
// Channel for temperature
#ifndef PRESSURE_CHANNEL
#define PRESSURE_CHANNEL        2
#endif
// Channel for temperature
#ifndef HUMIDITY_CHANNEL
#define HUMIDITY_CHANNEL        3
#endif
#endif

#ifdef SOLARZELLE1
#ifndef SOLARZELLE1_CHANNEL
#define SOLARZELLE1_CHANNEL     5
#endif

#endif

#ifdef SOLARZELLE2
#ifndef SOLARZELLE2_CHANNEL
#define SOLARZELLE2_CHANNEL     6
#endif

#endif

#if defined(LOAD_BALLANCER) || defined(DISCHARGE1) || defined(DISCHARGE2)
#ifndef BATT1_CHANNEL
#define BATT1_CHANNEL           7
#endif
#ifndef BATT_MOD_CHANNEL
#define BATT_MOD_CHANNEL        8
#endif

#endif



// Neopixel
// Arduino Pin where the Data-Pin from the Neopixels is connected to
#ifdef NEOPIXEL
#ifndef NEOPIXEL_PIN
#define NEOPIXEL_PIN 7
#endif
// Default Color Values for the Neopixel
#ifndef NEOPIXEL_R_DEFAULT
#define NEOPIXEL_R_DEFAULT 100
#endif
#ifndef NEOPIXEL_G_DEFAULT
#define NEOPIXEL_G_DEFAULT 100
#endif
#ifndef NEOPIXEL_B_DEFAULT
#define NEOPIXEL_B_DEFAULT 100
#endif
#endif
// Sleeptime in Seconds !! 
// (valid: 10 ... 32.400)
#ifndef SLEEPTIME
#define SLEEPTIME       120
#endif
// adjust the length of a second (aprox. in ms) - will be used as a factor to sleep4ms()
// (valid: 500 ... 2000)
#ifndef SLEEP4MS_FAC
#define SLEEP4MS_FAC    1000
#endif
// number of empty loop (non sending loops) after sending data
// (valid: 0...20)
#ifndef EMPTYLOOPS
#define EMPTYLOOPS      4
#endif
// waiting time between 2 transmissions in ms
// (valid 100 ... 1000)
#ifndef SENDDELAY
#define SENDDELAY       300
#endif
// Max number of attempts to send for a nomal message!!
// (valid 1 ... 20)
#ifndef MAX_SENDCOUNT
#define MAX_SENDCOUNT   3
#endif
// Max number of attempts to send for a stop message!!
// (valid 1 ... 20)
#ifndef MAX_STOPCOUNT
#define MAX_STOPCOUNT   3
#endif
// Mesurment of Vcc; Default is internal
#ifdef BATTERY_READ_EXTERNAL
#ifndef BATTERY_VOLTAGEDIVIDER_R1
#define BATTERY_VOLTAGEDIVIDER_R1     0
#endif
#ifndef BATTERY_VOLTAGEDIVIDER_R2
#define BATTERY_VOLTAGEDIVIDER_R2     470
#endif
#endif
// Voltage faktor will be multiplied to the messured value !!!!
// (valid 0.1 ... 10)
#ifndef VOLT_FAC
#define VOLT_FAC        1.0
#endif
// Voltage offset will be added to messured value * volt_fac !!!
// (valid -10 ... 10)
#ifndef VOLT_OFF
#define VOLT_OFF        0
#endif
// Define low voltage level on processor
// below that level the thermometer will be switched off 
// until the battery will be reloaded
#ifndef VOLT_LV
#define VOLT_LV 2
#endif
// Sleeptime in case of low Voltage
// 900 => every 2 hours (with emptyloops = 4)
#ifndef SLEEPTIME_LV
#define SLEEPTIME_LV  1440
#endif

// PA_Level for the radio, default is maximum
#ifndef PA_LEVEL
#define PA_LEVEL RF24_PA_MAX
#endif

// Some definitions for the display
#if defined(DISPLAY)
// 5 voltages for the battery (empty ... full)
#ifndef U0
#define U0 3.6
#endif
#ifndef U1
#define U1 3.7
#endif
#ifndef U2
#define U2 3.8
#endif
#ifndef U3
#define U3 3.9
#endif
#ifndef U4
#define U4 4.0
#endif
// Kontrast of the display
#ifndef CONTRAST
#define CONTRAST 65
#endif
// Brightnes of the display - unused until now
#ifndef BRIGHTNES
#define BRIGHTNES 0
#endif
//
// Definitions for Nokia 5110 Display
//
#if defined(DISPLAY_5110)
// DISPLAY Nokia 5110 is 84 * 48 Pixel
#ifndef N5110_CLK
#define N5110_CLK         7
#endif
#ifndef N5110_DIN
#define N5110_DIN         6
#endif
#ifndef N5110_DC
#define N5110_DC          5
#endif
#ifndef N5110_RST
#define N5110_RST         2
#endif
#ifndef N5110_CE
#define N5110_CE          4
#endif
//  012345678901234567890123456789012345678901234567890123456789012345678901234567890123
// 0                                                                           BBBBBBBBB     B = Batterie
// 1                                                                          BBBBBBBBBB
// 2                                                                          BBBBBBBBBB
// 3                                                                          BBBBBBBBBB
// 4                                                                           BBBBBBBBB
// 5
// 6                                                                          AAAAAAAAAA     A = Antenna
// 7                                                                          AAAAAAAAAA
// 8                                                                          AAAAAAAAAA
// 9                                                                          AAAAAAAAAA
// 0                                                                          AAAAAAAAAA
// 1                                                                          AAAAAAAAAA
// 2                                                                          AAAAAAAAAA
// 3                                                                          AAAAAAAAAA
// 4                                                                          AAAAAAAAAA
// 5                                                                          AAAAAAAAAA
// 6
// 7                                                                           TT   HHHH      T = Thermometer
// 8                                                                           TT   HHHH 
// 9                                                                           TT   HHHH      H = Heatbeat countdown
// 0                                                                           TT   HHHH      
// 1                                                                           TT   HHHH
// 2                                                                          TTTT  HHHH
// 3                                                                          T  T  HHHH
// 4                                                                          TTTT  HHHH
// 5
// 6
// 7------------------------------------------------------------------------------------
// 8|                                        ||                                        |
// 9|                                        ||                                        |
// 0|                                        ||                                        |
// 1|                                        ||                                        |
// 2|        Field 1                         ||           Field 2                      |
// 3|                                        ||                                        |
// 4|                                        ||                                        |
// 5|                                        ||                                        |
// 6|                                        ||                                        |
// 7------------------------------------------------------------------------------------
// 8|                                        ||                                        |
// 9|                                        ||                                        |
// 0|                                        ||                                        |
// 1|        Field 3                         ||           Field 4                      |
// 2|                                        ||                                        |
// 3|                                        ||                                        |
// 4|                                        ||                                        |
// 5|                                        ||                                        |
// 6|                                        ||                                        |
// 7------------------------------------------------------------------------------------
// Define the location of the display symbols
// set X0 and Y0 of battery symbol ( is 10 * 5 pixel )
#ifndef BATT_X0
#define BATT_X0 74
#endif
#ifndef BATT_Y0
#define BATT_Y0 0
#endif
// set X0 and Y0 of antenna symbol ( is 10 * 10 pixel )
#ifndef ANT_X0
#define ANT_X0 74
#endif
#ifndef ANT_Y0
#define ANT_Y0 6
#endif
// set X0 and Y0 of thermometer symbol ( is 3 * 6 pixel )
#ifndef THERM_X0
#define THERM_X0 74
#endif
#ifndef THERM_Y0
#define THERM_Y0 17
#endif
// set X0 and Y0 of HB countdown symbol ( is 4 * 10 pixel )
#ifndef HB_X0
#define HB_X0 80
#endif
#ifndef HB_Y0
#define HB_Y0 17
#endif
//
// END Definition Nokia 5110 Display
#endif
// END Definition Display
#endif
