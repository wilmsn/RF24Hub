/*
A thermometer.
Can be used with a display or only as a sensor without display

On Branch: no_network @ rpi1  !!!!!

*/
//****************************************************
// My definitions for my nodes based on this sketch
// Select only one at one time !!!!
//#define AUSSENTHERMOMETER
//#define AUSSENTHERMOMETER2
//#define SCHLAFZIMMERTHERMOMETER
#define TESTZIMMERTHERMOMETER
//#define BASTELZIMMERTHERMOMETER
//#define KUECHETHERMOMETER
//#define WOHNZIMMERTHERMOMETER
//#define ANKLEIDEZIMMERTHERMOMETER
//#define GAESTEZIMMERTHERMOMETER
//#define UNOTESTNODE
//#define UNOTESTNODE_AO
//****************************************************
//          Define node general settings
//  Can be overwritten in individual settings later
//****************************************************
// Dummy values, be sure to overwrite later!!
#define EEPROM_VERSION 0
#define RF24NODE 00
//****************************************************
// Delay between 2 transmission in ms
#define RF24SENDDELAY   50
// Delay between 2 transmission in ms
#define RF24RECEIVEDELAY 50
// Sleeptime in ms !! 
// (valid: 10.000 ... 3.600.000)
#define RF24SLEEPTIME   60000
// Max Number of Heartbeart Messages to send !!
#define RF24SENDLOOPCOUNT 10
// Max Number of Receiveloops !!
#define RF24RECEIVELOOPCOUNT 10
// number of empty loop after sending data
// valid: 0...9
#define RF24EMPTYLOOPCOUNT  0
// Voltage Faktor will be divided by 100 (Integer !!)!!!!
#define VOLTAGEFACTOR 100
// Add a constant number here (will be divided by 100)!!!
#define VOLTAGEADDED 0
// Kontrast of the display
#define DISPLAY_KONTRAST 65;
// Define low voltage level on processor
// below that level the thermometer will be switched off 
// until the battery will be reloaded
#define LOWVOLTAGELEVEL 2.8
// Change the versionnumber to store new values in EEPROM
// Set versionnumber to "0" to disable 
#define EEPROM_VERSION 1
// 5 voltages for the battery (empty ... full)
#define U0 3.6
#define U1 3.7
#define U2 3.8
#define U3 3.9
#define U4 4.0
// DISPLAY Nokia 5110 is 84 * 48 Pixel
//  012345678901234567890123456789012345678901234567890123456789012345678901234567890123
// 0                                                                          BBBBBBBBBB     B = Batterie
// 1                                                                          BBBBBBBBBB
// 2                                                                          BBBBBBBBBB
// 3                                                                          BBBBBBBBBB
// 4                                                                          BBBBBBBBBB
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
// 7                                                                          TTT   HHHH      T = Thermometer
// 8                                                                          TTT   HHHH 
// 9                                                                          TTT   HHHH      H = Heatbeat countdown
// 0                                                                          TTT   HHHH      
// 1                                                                          TTT   HHHH
// 2                                                                          TTT   HHHH
// 3                                                                                HHHH
// 4                                                                                HHHH
// 5------------------------------------------------------------------------------------
// 6|                                        |                                         |
// 7|                                        |                                         |
// 8|                                        |                                         |
// 9|                                        |                                         |
// 0|        Field 1                         |            Field 2                      |
// 1|                                        |                                         |
// 2|                                        |                                         |
// 3|                                        |                                         |
// 4|                                        |                                         |
// 5|                                        |                                         |
// 6------------------------------------------------------------------------------------
// 7|                                        |                                         |
// 8|                                        |                                         |
// 9|                                        |                                         |
// 0|                                        |                                         |
// 1|        Field 3                         |            Field 4                      |
// 2|                                        |                                         |
// 3|                                        |                                         |
// 4|                                        |                                         |
// 5|                                        |                                         |
// 6|                                        |                                         |
// 7------------------------------------------------------------------------------------
// Define the location of the display symbols
// set X0 and Y0 of battery symbol ( is 10 * 5 pixel )
#define BATT_X0 74
#define BATT_Y0 0
// set X0 and Y0 of antenna symbol ( is 10 * 10 pixel )
#define ANT_X0 74
#define ANT_Y0 6
// set X0 and Y0 of thermometer symbol ( is 3 * 6 pixel )
#define THERM_X0 74
#define THERM_Y0 17
// set X0 and Y0 of HB countdown symbol ( is 4 * 10 pixel )
#define HB_X0 80
#define HB_Y0 17
// The CE Pin of the Radio module
#define RADIO_CE_PIN 10
// The CS Pin of the Radio module
#define RADIO_CSN_PIN 9
// The pin of the statusled
#define STATUSLED 3
#define STATUSLED_ON HIGH
#define STATUSLED_OFF LOW
#define ONE_WIRE_BUS 8
#define RECEIVEDELAY 100

//*****************************************************
//    Individual settings
//-----------------------------------------------------
#if defined(AUSSENTHERMOMETER)
#define BME_280
#define RF24NODE        200
#define STATUSLED       3
#define STATUSLED_ON    HIGH
#define STATUSLED_OFF   LOW
#define LOWVOLTAGELEVEL 1
#define EEPROM_VERSION  2
#define HBNODE
#endif
//-----------------------------------------------------
#if defined(SCHLAFZIMMERTHERMOMETER)
#define DALLAS_18B20
#define DISPLAY_5110
#define RF24NODE             101
#define EEPROM_VERSION       2
#define VOLTAGEADDED         55
#define STATUSLED_ON         LOW
#define STATUSLED_OFF        HIGH
#define HBNODE
#endif
//-----------------------------------------------------
#if defined(TESTZIMMERTHERMOMETER)
#define DALLAS_18B20
#define DISPLAY_5110
#define RF24NODE             102
#define EEPROM_VERSION       2
#define VOLTAGEADDED         55
#define HBNODE
#define RF24EMPTYLOOPCOUNT   10
#define RF24RECEIVEDELAY     500
#endif
//-----------------------------------------------------
#if defined(BASTELZIMMERTHERMOMETER)
#define DALLAS_18B20
#define DISPLAY_5110
#define RF24NODE            100
#define EEPROM_VERSION      1
#define VOLTAGEADDED        55
#define RF24SLEEPTIME       300000
#define HBNODE
//#define RF24EMPTYLOOPCOUNT  3 
#endif
//-----------------------------------------------------
#if defined(KUECHETHERMOMETER)
#define DALLAS_18B20
#define DISPLAY_5110
#define RF24NODE            0
#define EEPROM_VERSION      3
#define VOLTAGEADDED        55
#define RF24SLEEPTIME       1200000
#define RF24EMPTYLOOPCOUNT  0 
#define HBNODE
#endif
//-----------------------------------------------------
#if defined(ANKLEIDEZIMMERTHERMOMETER)
#define DALLAS_18B20
#define DISPLAY_5110
#define DISPLAY_KONTRAST     65
#define RF24NODE             110
#define EEPROM_VERSION       1
#define VOLTAGEADDED         55
#define STATUSLED_ON         LOW
#define STATUSLED_OFF        HIGH
#define RF24EMPTYLOOPCOUNT   5
#define HBNODE
#endif
//-----------------------------------------------------
#if defined(GAESTEZIMMERTHERMOMETER)
#define DALLAS_18B20
//#define DISPLAY_5110
#define RF24NODE        0
#define EEPROM_VERSION  3
#define VOLTAGEADDED    55
#define HBNODE
#endif
//-----------------------------------------------------
#if defined(TESTNODE)
#define RF24NODE        1
#define EEPROM_VERSION  4
#define STATUSLED       13 
#define ACTOR           A5
#define RF24SLEEPTIME   60000
#define TEST_LED
#define SERIAL_DEBUG
#define HBNODE
#define sleep4ms        delay
#endif
//-----------------------------------------------------
#if defined(UNOTESTNODE)
#define RF24NODE         1
#define EEPROM_VERSION   3
#define STATUSLED        13 
#define ACTOR            A5
#define RF24SLEEPTIME    60000
#define RF24SENDDELAY    200
#define RF24RECEIVEDELAY 200
#define BMP_280
#define SERIAL_DEBUG
#define TEST_LED
#define HBNODE
#endif
//-----------------------------------------------------
#if defined(UNOTESTNODE_AO)
#define RF24NODE         2
#define EEPROM_VERSION   3
#define STATUSLED        13 
#define RF24SLEEPTIME    60000
#define RF24SENDDELAY    200
#define RF24RECEIVEDELAY 200
#define BMP_280
#define SERIAL_DEBUG
#define TEST_LED
#endif
//-----------------------------------------------------
//*****************************************************
// ------ End of configuration part ------------

#include <avr/pgmspace.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <SPI.h>
#include <sleeplib.h>
#include <Vcc.h>
#include <EEPROM.h>
#include "dataformat.h"
#include "printf.h"
#include "rf24_config.h"

#if defined(DISPLAY_5110)
#define HAS_DISPLAY
#include <LCD5110_Graph.h>
#endif

#if defined(DALLAS_18B20)
#include <OneWire.h>
#include <DallasTemperature.h>
#endif

#if defined(BME_280)
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#endif

#if defined(BMP_280)
#include <BMP280.h>
#endif

// ----- End of Includes ------------------------

Vcc vcc(1.0);

ISR(WDT_vect) { watchdogEvent(); }

#if defined(TEST_LED)
#define TEST_LED_PIN A0
#endif
#if defined(HAS_DISPLAY)
#if defined(DISPLAY_5110)
LCD5110 myGLCD(7,6,5,2,4);
extern uint8_t SmallFont[];
extern uint8_t BigNumbers[];
#endif
#endif

#if defined(DALLAS_18B20)
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensor(&oneWire);
float temp;
#endif

#if defined(BME_280)
Adafruit_BME280 sensor;
float temp, pres, humi;
#endif

#if defined(BMP_280)
BMP280 sensor(0x76);
float temp, pres;
#endif

payload_t payload;    

uint8_t  rf24_node2hub[] = RF24_NODE2HUB;
uint8_t  rf24_hub2node[] = RF24_HUB2NODE;

struct eeprom_t {
   uint8_t  versionnumber;
   uint32_t sleeptime;
   uint16_t sendloopcount;
   uint16_t receiveloopcount;
   uint16_t emptyloopcount;
   uint16_t voltagefactor;
   int      voltageadded;
   int      display_contrast;
};
eeprom_t eeprom;

boolean             display_down = false;
boolean             low_voltage_flag = false;
boolean             exec_pingTest = false;
//Some Var for restore after sleep of display
#if defined(HAS_DISPLAY)
boolean             monitormode = false;
float               field1_val, field2_val, field3_val, field4_val;
#endif
float               cur_voltage;
uint16_t            loopcount;
long int            sleep_kor_time;
uint32_t            last_send;
uint8_t             last_orderno = 0;
uint8_t             msg_id = 0;


// nRF24L01(+) radio attached using Getting Started board 
// Usage: radio(CE_pin, CS_pin)
RF24 radio(RADIO_CE_PIN,RADIO_CSN_PIN);

void get_sensordata(void) {
#if defined(DALLAS_18B20)
  sensor.requestTemperatures(); // Send the command to get temperatures
  sleep4ms(800);
  delay(10);
  temp=sensor.getTempCByIndex(0);
#endif
#if defined(BME_280)
  sensor.takeForcedMeasurement();
  sleep4ms(800);
  delay(10);
  temp=sensor.readTemperature();
  pres=pow(((95*0.0065)/(sensor.readTemperature()+273.15)+1),5.257)*sensor.readPressure()/100.0;
  humi=sensor.readHumidity();
#endif
#if defined(BMP_280)
  sensor.startSingleMeasure();
  sleep4ms(800);
  delay(10);
  temp = sensor.readTemperature();
  pres = sensor.readPressureAtSealevel(91);
#endif
}

uint32_t action_loop(uint32_t data) {
  uint32_t retval = 0;
  uint8_t channel = getChannel(data);
  float value = getValue_f(data);
//print_field(value,4);
    switch (channel) {
#if defined(TEST_LED)
      case 21:
        // Set field 1
        if ( value > 0.5 ) {
          digitalWrite(TEST_LED_PIN,STATUSLED_ON); 
#if defined(SERIAL_DEBUG)
  Serial.println("LED ein");
#endif
        } else {
          digitalWrite(TEST_LED_PIN,STATUSLED_OFF); 
#if defined(SERIAL_DEBUG)
  Serial.println("LED aus");
#endif
        }
      break;
#endif
#if defined(HAS_DISPLAY)
      case 21:
        // Set field 1
        field1_val = value;
        print_field(field1_val,1);
      break;
      case 22:
        // Set field 2
        field2_val= value;
        print_field(field2_val,2);
      break;
      case 23:
        // Set field 3
        field3_val= value;
        print_field(field3_val,3);
      break;
      case 24:
        // Set field 4
        field4_val= value;
        print_field(field4_val,4);
      break;
      case 31:
        // Displaylight ON <-> OFF
        if (value > 0.5 && value < 1.5) {
          digitalWrite(STATUSLED,STATUSLED_ON); 
        } else  {
          digitalWrite(STATUSLED,STATUSLED_OFF);
        }
      break;
      case 41:
        // Display Sleepmode ON <-> OFF
        display_sleep(value < 0.5);
      break;
#endif
      case 101:  
      // battery voltage
        value = cur_voltage;
      break;      
#if defined(HAS_DISPLAY)
      case 109:
          if ( value > 0.5 && value < 1.5 ) {
            monitor(eeprom.sleeptime/2); 
            monitormode = true;
          } else {
            monitormode = false;
          }
      break;
      case 110:
#if defined(DISPLAY_5110)
        if (value > 0.5 || value < 101) {
          eeprom.display_contrast=(uint8_t)value;
          myGLCD.setContrast(eeprom.display_contrast);
          EEPROM.put(0, eeprom);
        }
        value = (float)eeprom.display_contrast;
#endif
#endif
      break;
      case 111:
      // sleeptime in ms!
        if (value > 9999 && value < 3600000) {
          eeprom.sleeptime=(uint32_t)value;
          EEPROM.put(0, eeprom);
        }
        value = (float)eeprom.sleeptime;
      break;
      case 112:
      // sendloopcount - number sendloop befor giving up 
        if (value > 0.5 || value < 21) {
          eeprom.sendloopcount=(uint16_t)value;
          EEPROM.put(0, eeprom);
        }
        value = (float)eeprom.sendloopcount;
      break;
      case 113:
      // receiveloopcount - number of receivloops befor giving up.
        if (value > 0.5 || value < 21) {
          eeprom.receiveloopcount=(uint16_t)value;
          EEPROM.put(0, eeprom);
        }
        value = (float)eeprom.receiveloopcount;
      break;
      case 114:
      // emptyloopcount - only loop 0 will transmit all other loops will only read and display
        if (value >= 0 && value < 21) {
          eeprom.emptyloopcount=(uint16_t)value;
          EEPROM.put(0, eeprom);
        } 
        value = (float)eeprom.emptyloopcount;
      break;
      case 115:
      // sleep korrektion faktor in ms - will only be used once!
        if ((value > 0.5 && value < 600001) || (value < -0.5 && value > -600001)) {
          sleep_kor_time = (long int)value;
        }
        value = sleep_kor_time;
      break;
      case 116:
      // Voltagefactor - will be divided by 100
        if (value > 10 && value < 1000) {
          eeprom.voltagefactor=(uint16_t)value;
          EEPROM.put(0, eeprom);
        }
        value = (float)eeprom.voltagefactor;
      break;
      case 117:
      // Voltageadded - will be divided by 100
        if (value > -300 && value < 300) {
          eeprom.voltageadded=(int)value;
          EEPROM.put(0, eeprom);
        }
        value = (float)eeprom.voltageadded;
      break;
      case 118:
        exec_pingTest = true;
      break;
    }  
    return calcTransportValue_f(channel, value);
}  

void setup(void) {
  delay(500);
  pinMode(STATUSLED, OUTPUT);     
#if defined(TEST_LED)
  pinMode(TEST_LED_PIN, OUTPUT);
  digitalWrite(TEST_LED_PIN,STATUSLED_ON); 
#endif
  digitalWrite(STATUSLED,STATUSLED_ON); 
  EEPROM.get(0, eeprom);
  if (eeprom.versionnumber != EEPROM_VERSION && EEPROM_VERSION > 0) {
    eeprom.versionnumber = EEPROM_VERSION;
    eeprom.sleeptime = RF24SLEEPTIME;
    eeprom.sendloopcount = RF24SENDLOOPCOUNT;
    eeprom.receiveloopcount = RF24RECEIVELOOPCOUNT;
    eeprom.emptyloopcount = RF24EMPTYLOOPCOUNT;
    eeprom.voltagefactor = VOLTAGEFACTOR;
    eeprom.voltageadded = VOLTAGEADDED;
    eeprom.display_contrast = DISPLAY_KONTRAST;
    EEPROM.put(0, eeprom);
  }
#if defined(SERIAL_DEBUG)
  Serial.begin(115200);
  Serial.println("Debug started xxx ");
  printf_begin();
#endif
  SPI.begin();
#if defined(DALLAS_18B20)
  sensor.begin(); 
#endif
#if defined(BME_280)
  sensor.begin(0x76, &Wire); 
#endif
#if defined(BMP_280)
  sensor.begin(); 
#endif
  radio.begin();
  radio.setChannel(RF24_CHANNEL);
  radio.setDataRate( RF24_SPEED );
  radio.setPALevel( RF24_PA_MAX ) ;
  radio.setRetries(15, 5);
//  radio.setAutoAck(0,0);
  radio.enableDynamicPayloads();
  radio.openWritingPipe(rf24_node2hub);
  radio.openReadingPipe(1,rf24_hub2node);
#if defined(SERIAL_DEBUG)
  radio.printDetails();
#endif    
  delay(1000);
  digitalWrite(STATUSLED,STATUSLED_OFF); 
#if defined(HAS_DISPLAY)
#if defined(DISPLAY_5110)
  myGLCD.InitLCD();
  myGLCD.setContrast(eeprom.display_contrast);
  myGLCD.clrScr();
  myGLCD.update();
#endif
#endif
#if defined(HAS_DISPLAY)
  monitor(1000);
  draw_antenna(ANT_X0, ANT_Y0);
  draw_therm(THERM_X0, THERM_Y0);
  draw_hb_countdown(8);
#endif
  loopcount = 0;
  last_send = 0;
// on init send config to hub
  payload.node_id  = RF24NODE;
  payload.msg_type = PAYLOAD_TYPE_INIT;
  payload.msg_id   = 1;
  payload.orderno  = 0;
  payload.data1 = calcTransportValue_f(111, (float)eeprom.sleeptime);
  payload.data2 = calcTransportValue_f(112, (float)eeprom.sendloopcount);
  payload.data3 = calcTransportValue_f(113, (float)eeprom.receiveloopcount);
  payload.data4 = calcTransportValue_f(114, (float)eeprom.emptyloopcount);
  payload.data5 = calcTransportValue_f(116, (float)eeprom.voltagefactor);
  payload.data6 = calcTransportValue_f(117, (float)eeprom.voltageadded);  
  radio.stopListening();
  radio.openWritingPipe(rf24_node2hub);
  while ( payload.msg_id < 11 ) { 
    if (radio.write(&payload, sizeof(payload))) {
#if defined(SERIAL_DEBUG)
      Serial.println("write OK");
#endif    
      payload.msg_id=11;
    } else {
#if defined(SERIAL_DEBUG)
      Serial.println("write failed");
#endif    
    }
    payload.msg_id++;
    delay(100);
  }
  radio.startListening();
  radio.openReadingPipe(1,rf24_hub2node);
  delay(500);
  while ( radio.isValid() && radio.available() ) {
    radio.read(&payload,sizeof(payload));
#if defined(SERIAL_DEBUG)
      Serial.println("read data");
#endif    
    delay(50);
    if (payload.data1 > 0) payload.data1 = action_loop(payload.data1); 
    if (payload.data2 > 0) payload.data2 = action_loop(payload.data2); 
    if (payload.data3 > 0) payload.data3 = action_loop(payload.data3); 
    if (payload.data4 > 0) payload.data4 = action_loop(payload.data4); 
    if (payload.data5 > 0) payload.data5 = action_loop(payload.data5); 
    if (payload.data6 > 0) payload.data6 = action_loop(payload.data6); 
  } 
#if defined(TEST_LED)
  digitalWrite(TEST_LED_PIN,STATUSLED_OFF); 
#endif
  pingTest();
}

#if defined(HAS_DISPLAY)
void monitor(uint32_t delaytime) {
  const char string_1[] PROGMEM = "Temp: ";
  const char string_2[] PROGMEM = "Ubatt: ";
  const char string_3[] PROGMEM = "Sleep: ";
  const char string_4[] PROGMEM = "send/rec/empty: ";
  const char string_5[] PROGMEM = "RF24 Network: ";
  const char string_6[] PROGMEM = "Node: ";
  const char string_7[] PROGMEM = "Channel: ";
  const char string_8[] PROGMEM = "Kontrast: ";
#if defined(DISPLAY_5110)
  myGLCD.setFont(SmallFont);
  get_sensordata();
  myGLCD.print(string_1, 0, 0);
  myGLCD.printNumF(temp,1, 30, 0);
  cur_voltage = (vcc.Read_Volts()+((float)eeprom.voltageadded/100.0))*((float)eeprom.voltagefactor)/100.0;
  float mes_voltage = vcc.Read_Volts();
  myGLCD.print(string_2, 0, 10);
  myGLCD.printNumF(cur_voltage,1, 40, 10);
  myGLCD.printNumF(mes_voltage,1, 65, 10);
  myGLCD.print(string_3, 0, 20);
  myGLCD.printNumI(eeprom.sleeptime, 45, 20);
  myGLCD.print(string_4, 0, 30);
  myGLCD.printNumI(eeprom.sendloopcount, 0, 40);
  myGLCD.print("/", 20, 40);
  myGLCD.printNumI(eeprom.receiveloopcount, 30, 40);
  myGLCD.print("/", 60, 40);
  myGLCD.printNumI(eeprom.emptyloopcount, 70, 40);
  myGLCD.update();
  sleep4ms(delaytime);
  delay(10);
  myGLCD.clrScr();
  myGLCD.print(string_5, 0, 0);
  myGLCD.print(string_6, 0, 10);
  myGLCD.printNumI(RF24NODE, 55, 10);
  myGLCD.print(string_7, 0, 20);
  myGLCD.printNumI(RF24_CHANNEL, 60, 20);
  myGLCD.update();
  sleep4ms(delaytime);
  myGLCD.clrScr();
  myGLCD.print(string_8, 0, 0);
  myGLCD.printNumI(eeprom.display_contrast, 55, 0);
  myGLCD.setFont(BigNumbers);
  for (int i=0; i<100; i+=5) {
    myGLCD.printNumI(i, 20, 20);        
    myGLCD.setContrast(i);
    myGLCD.update();
    delay(500);
  }
  sleep4ms(1000);
  myGLCD.setContrast(eeprom.display_contrast);
  delay(10);
  myGLCD.clrScr();
#endif  
}

void display_sleep(boolean dmode) {
  display_down = dmode;
  if ( dmode ) { // Display go to sleep
#if defined(DISPLAY_5110)
    myGLCD.enableSleep(); 
#endif
  } else {
    if ( ! low_voltage_flag ) {  
#if defined(DISPLAY_5110)
      myGLCD.disableSleep(); 
#endif
        get_sensordata();
        draw_temp(temp);
        print_field(field1_val,1);
        print_field(field2_val,2);
        print_field(field3_val,3);
        print_field(field4_val,4);
    }
  }  
}

void draw_therm(byte x, byte y) {
  if ( ! display_down ) {
#if defined(DISPLAY_5110)
    myGLCD.drawRect(x+1,y,x+1,y+3);
    myGLCD.drawRect(x,y+4,x+2,y+5);
    myGLCD.update();
#endif
  }
}

void draw_hb_countdown(uint8_t watermark) {
  uint8_t i = HB_X0;
  uint8_t j = HB_Y0 + 7;
  if ( ! display_down ) {
    if ( watermark > 8 ) watermark = 8;
#if defined(DISPLAY_5110)
    for (uint8_t y=j; y>=HB_Y0; y--) {
      for (byte x=i; x<i+4; x++) { 
        myGLCD.clrPixel(x,y);
      }
    }
    for (uint8_t y=j; y>j-watermark; y--) {
      for (byte x=i; x<i+4; x++) { 
        myGLCD.setPixel(x,y);
      }
    }
    myGLCD.update();
#endif
  }
}

void wipe_therm(byte x, byte y) {
  if ( ! display_down ) {
    for (byte i=x; i<x+3; i++) {
      for (byte j=y; j<y+6; j++) {
#if defined(DISPLAY_5110)
        myGLCD.clrPixel(i,j);
        myGLCD.update();
#endif
      }
    }
  }
}

void draw_temp(float t) {
  int temp_abs, temp_i, temp_dez_i; 
  if ( ! display_down ) {
#if defined(DISPLAY_5110)
    if (t < 0.0) {
      temp_abs=t*-1;
      myGLCD.drawRect(0,10,9,12);
    } else {
      temp_abs=t;
    }      
    temp_i=(int)temp_abs;
    temp_dez_i=t*10-temp_i*10;
    for(byte i=0; i<74; i++) {
      for (byte j=0; j<25; j++) {
        myGLCD.clrPixel(i,j);
      }
    }
    if (temp_i < 100) {
      myGLCD.setFont(BigNumbers);
      if ( temp_i < 10 ) {
        myGLCD.printNumI(temp_i, 20, 0);        
      } else {
        myGLCD.printNumI(temp_i, 10, 0);
      }
      myGLCD.drawRect(40,20,43,23);
      myGLCD.printNumI(temp_dez_i, 45, 0);
      myGLCD.drawRect(61,2,64,5);
    } else {
      myGLCD.drawRect(15,10,24,12);
      myGLCD.drawRect(30,10,39,12);
      myGLCD.drawRect(45,10,54,12);
    }
    myGLCD.update();
#endif
  }
}

void print_field(float val, int field) {
  int x0, y0;
  if ( ! display_down ) {
#if defined(DISPLAY_5110)
    switch (field) {
      case 1: x0=0; y0=25; break;
      case 2: x0=42; y0=25; break;
      case 3: x0=0; y0=36; break;
      case 4: x0=42; y0=36; break;
    }
    for (int i=x0; i < x0+42; i++) {
      for (int j=y0; j< y0+12; j++) {
        myGLCD.clrPixel(i,j);
      }
    }
    myGLCD.drawRect(x0,y0,x0+41,y0+11);
    myGLCD.setFont(SmallFont);
    if ( val > 100 ) {
      if (val+0.5 > 1000) { 
       myGLCD.printNumI(val, x0+9, y0+3);
      } else {
       myGLCD.printNumI(val, x0+12, y0+3);
      }    
    } else {
      if (val >= 10) {
        myGLCD.printNumF(val,1, x0+9, y0+3);
      } else {
        myGLCD.printNumF(val,2, x0+9, y0+3);
      }      
    }
    myGLCD.update();
#endif
  }
}

void draw_battery_filled(int x, int y) {
#if defined(DISPLAY_5110)
  myGLCD.setPixel(x,y); 
  myGLCD.setPixel(x,y+1); 
  myGLCD.setPixel(x,y+2); 
  myGLCD.setPixel(x+1,y); 
  myGLCD.setPixel(x+1,y+1); 
  myGLCD.setPixel(x+1,y+2); 
#endif
}

void draw_battery(int x, int y, float u) {
  if ( ! display_down ) {
#if defined(DISPLAY_5110)
    // Clear the drawing field
    for (byte i=x; i<=x+9; i++) {
      for (byte j=y; j<=y+5; j++) {
        myGLCD.clrPixel(i,j);
      }
    }
    // Drawing a symbol of an battery
    // Size: 10x5 pixel
    // at position x and y
    myGLCD.drawRect(x+2,y,x+9,y+4);
    myGLCD.drawRect(x,y+1,x+1,y+3);
    if ( u > U1 ) draw_battery_filled(x+8,y+1); else myGLCD.drawLine(x+3,y,x+7,y+4);
    if ( u > U2 ) draw_battery_filled(x+6,y+1);
    if ( u > U3 ) draw_battery_filled(x+4,y+1);
    if ( u > U4 ) draw_battery_filled(x+2,y+1);
    myGLCD.update();
#endif
  }
}

void draw_antenna(int x, int y) {
  if ( ! display_down ) {
#if defined(DISPLAY_5110)
    // Drawing a symbol of an antenna
    // Size: 10x10 pixel
    // at position x and y
    myGLCD.setPixel(x+7,y+0);
    myGLCD.setPixel(x+1,y+1);
    myGLCD.setPixel(x+8,y+1);
    myGLCD.setPixel(x+0,y+2);
    myGLCD.setPixel(x+3,y+2);
    myGLCD.setPixel(x+6,y+2);
    myGLCD.setPixel(x+9,y+2);
    myGLCD.setPixel(x+0,y+3);
    myGLCD.setPixel(x+2,y+3);
    myGLCD.setPixel(x+7,y+3);
    myGLCD.setPixel(x+9,y+3);
    myGLCD.setPixel(x+0,y+4);
    myGLCD.setPixel(x+2,y+4);
    myGLCD.setPixel(x+4,y+4);
    myGLCD.setPixel(x+5,y+4);
    myGLCD.setPixel(x+7,y+4);
    myGLCD.setPixel(x+9,y+4);
    myGLCD.setPixel(x+0,y+5);
    myGLCD.setPixel(x+2,y+5);
    myGLCD.setPixel(x+4,y+5);
    myGLCD.setPixel(x+5,y+5);
    myGLCD.setPixel(x+7,y+5);
    myGLCD.setPixel(x+9,y+5);
    myGLCD.setPixel(x+0,y+6);
    myGLCD.setPixel(x+3,y+6);
    myGLCD.setPixel(x+4,y+6);
    myGLCD.setPixel(x+5,y+6);
    myGLCD.setPixel(x+6,y+6);
    myGLCD.setPixel(x+9,y+6);
    myGLCD.setPixel(x+1,y+7);
    myGLCD.setPixel(x+4,y+7);
    myGLCD.setPixel(x+5,y+7);
    myGLCD.setPixel(x+8,y+7);
    myGLCD.setPixel(x+4,y+8);
    myGLCD.setPixel(x+5,y+8);
    myGLCD.setPixel(x+4,y+9);
    myGLCD.setPixel(x+5,y+9);
    myGLCD.update();
#endif
  }
}   
 
void wipe_antenna(int x, int y) {
  if ( ! display_down ) {
#if defined(DISPLAY_5110)
    for (int i=x; i<x+10; i++) {
      for (int j=y; j<y+10; j++) {
        myGLCD.clrPixel(i,j);
      }
    }
    myGLCD.update();
#endif
  }
}  
#endif

void payloadInitHeader(uint8_t msg_type, uint8_t msg_flags, uint8_t orderno) {
  payload.node_id = RF24NODE;
  payload.msg_type = msg_type;
  payload.msg_flags = msg_flags;
  payload.msg_id = 1;
  payload.orderno = orderno;
}

void payloadInitData(void) {
  payload.data1 = 0;
  payload.data2 = 0;
  payload.data3 = 0;
  payload.data4 = 0;
  payload.data5 = 0;
  payload.data6 = 0;
}

#if defined(SERIAL_DEBUG)
void printPayload(payload_t* pl) {
    String buf;
    buf = F(" Typ: ");
    buf += String(payload.msg_type);
    buf += F(" OrderNo: ");
    buf += String(payload.orderno);
    buf += F(" (");
    buf += String(getChannel(payload.data1));
    buf += F("/");
    buf += String(getValue_f(payload.data1));
    buf += F(")");
    buf += F("(");
    buf += String(getChannel(payload.data2));
    buf += F("/");
    buf += String(getValue_f(payload.data2));
    buf += F(")");
    buf += F("(");
    buf += String(getChannel(payload.data3));
    buf += F("/");
    buf += String(getValue_f(payload.data3));
    buf += F(")");
    buf += F("(");
    buf += String(getChannel(payload.data4));
    buf += F("/");
    buf += String(getValue_f(payload.data4));
    buf += F(")");
    buf += F("(");
    buf += String(getChannel(payload.data5));
    buf += F("/");
    buf += String(getValue_f(payload.data5));
    buf += F(")");
    buf += F("(");
    buf += String(getChannel(payload.data6));
    buf += F("/");
    buf += String(getValue_f(payload.data6));
    buf += F(")");
    Serial.println(buf);
  
}
#endif

void payload_data(uint8_t pos, uint8_t channel, float value) {
  switch (pos) {
    case 1:
      payload.data1 = calcTransportValue_f(channel, value);
    break;
    case 2:
      payload.data2 = calcTransportValue_f(channel, value);
    break;
    case 3:
      payload.data3 = calcTransportValue_f(channel, value);
    break;
    case 4:
      payload.data4 = calcTransportValue_f(channel, value);
    break;
    case 5:
      payload.data5 = calcTransportValue_f(channel, value);
    break;
    case 6:
      payload.data6 = calcTransportValue_f(channel, value);
    break;    
  }
}

/*
 * Ping Test durchführen
 * Ergebnis ist die geringste Sendeleistung die vom Hub erkannt wurde
 */
void pingTest(void) {
  uint8_t PA_Level = radio.getPALevel();
  radio.setPALevel( RF24_PA_MAX) ;
  payloadInitHeader(PAYLOAD_TYPE_PING_POW_MAX, PAYLOAD_FLAG_LASTMESSAGE, 0);
  payloadInitData();
  sendData(3);
  radio.setPALevel( RF24_PA_HIGH ) ;
  payloadInitHeader(PAYLOAD_TYPE_PING_POW_HIGH, PAYLOAD_FLAG_LASTMESSAGE, 0);
  payloadInitData();
  sendData(3);
  radio.setPALevel( RF24_PA_LOW ) ;
  payloadInitHeader(PAYLOAD_TYPE_PING_POW_LOW, PAYLOAD_FLAG_LASTMESSAGE, 0);
  payloadInitData();
  sendData(3);
  radio.setPALevel( RF24_PA_MIN ) ;
  payloadInitHeader(PAYLOAD_TYPE_PING_POW_MIN, PAYLOAD_FLAG_LASTMESSAGE, 0);
  payloadInitData();
  sendData(3);
  radio.setPALevel( PA_Level ) ;
  exec_pingTest = false;
}
  
/****************************************************
 * Returns true if data are send to the hub
 * and data are acknoledged, otherwise false
 ***************************************************/
bool sendData(uint8_t maxSendLoop) {
  bool retval = false;
  uint8_t sendloopcount = 0;
  while ( sendloopcount < maxSendLoop ) {
// Solange keine Nachricht für uns vorliegt UND die Sendezeit nicht abgelaufen ist ==> senden
    radio.stopListening();
    radio.openWritingPipe(rf24_node2hub);
    delay(10);
#if defined(SERIAL_DEBUG)
    if (radio.write(&payload, sizeof(payload))) {
      Serial.println("write Msg => OK");
    } else {
      Serial.println("write Msg => failed");
    } 
#else
    radio.write(&payload, sizeof(payload));
#endif         
    radio.startListening();
    for(uint8_t i=0; i<RF24SENDDELAY; i++) {
      delay(2);
      if ( receiveData() ) {
#if defined(SERIAL_DEBUG)
        Serial.println("Got Msg; Stopp transmitting ");
#endif    
// Wenn wir die erste Nachricht empfangen, wird das Senden eingestellt
        sendloopcount = maxSendLoop;
        i = RF24SENDDELAY;
        retval = true;
      }
      i++;
    }
    payload.msg_id++;
    sendloopcount++;
  }
  return retval;
}

/****************************************************
 * Returns true if there are valid data
 * for this node, otherwise returns false
 ***************************************************/
bool receiveData(void) {        
  bool retval = false;
  if ( radio.available() ) {
    radio.read(&payload,sizeof(payload));
    if (payload.node_id == RF24NODE ) {
#if defined(SERIAL_DEBUG)
      Serial.println("Msg for this Node received");
      printPayload(&payload);
#endif    
      radio.flush_tx();
      retval = true;
    } else {
      retval = false;
    }
  } else {
    retval = false;
  }
  return retval;
}

void processData(uint8_t maxReceiveLoop) {
  uint8_t receiveloopcount = 0;
  while ( receiveloopcount < maxReceiveLoop) {
// Wenn Flag PAYLOAD_FLAG_LASTMESSAGE gesetzt ist dann wird keine nachfolgende Nachricht erwartet
    if ((payload.msg_flags & PAYLOAD_FLAG_LASTMESSAGE) == PAYLOAD_FLAG_LASTMESSAGE ) {
      receiveloopcount = maxReceiveLoop;
    } else {
      receiveloopcount = 0;
    }
#if defined(SERIAL_DEBUG)
        Serial.print("Payload Type = ");
        Serial.println(payload.msg_type);
#endif    
    switch(payload.msg_type) {
      case PAYLOAD_TYPE_HB_RESP: {
        // Eine Nachricht vom Typ PAYLOAD_TYPE_HBRESP ist eine leere ENDE Nachricht            
        // Keine Verarbeitung nötig
        payloadInitHeader(PAYLOAD_TYPE_HB_STOP, PAYLOAD_FLAG_LASTMESSAGE, payload.orderno);
#if defined(SERIAL_DEBUG)
        Serial.println("Sende Quittung HB_RESP");
        printPayload(&payload);
#endif    
        sendData(3);
        receiveloopcount = maxReceiveLoop;
      }
      case PAYLOAD_TYPE_DATSTOP: {
        // Eine Nachricht vom Typ PAYLOAD_TYPE_DATSTOP ist eine leere ENDE Nachricht            
        // Keine Verarbeitung nötig
#if defined(SERIAL_DEBUG)
        Serial.println("Nachricht DATSTOP");
        printPayload(&payload);
#endif    
        receiveloopcount = maxReceiveLoop;
      }
      break;
      case PAYLOAD_TYPE_DAT:
      default: {
        // Andere Nachrichten werden durch den "action_loop" geschickt              
        if (payload.data1 > 0) { payload.data1 = action_loop(payload.data1); }
        if (payload.data2 > 0) { payload.data2 = action_loop(payload.data2); }
        if (payload.data3 > 0) { payload.data3 = action_loop(payload.data3); }
        if (payload.data4 > 0) { payload.data4 = action_loop(payload.data4); }
        if (payload.data5 > 0) { payload.data5 = action_loop(payload.data5); }
        if (payload.data6 > 0) { payload.data6 = action_loop(payload.data6); }
// Quittung senden           
        payloadInitHeader(PAYLOAD_TYPE_DATRESP, PAYLOAD_FLAG_LASTMESSAGE, payload.orderno);
#if defined(SERIAL_DEBUG)
        Serial.println("Send Quittung Daten");
        printPayload(&payload);
#endif    
        sendData(eeprom.sendloopcount);
      }
    }
    if (receiveloopcount < maxReceiveLoop) delay(RF24RECEIVEDELAY);
    receiveloopcount++;
  }
// Buffer leeren
  while ( radio.available() ) radio.read(&payload,sizeof(payload));  
}

void exec_jobs(void) {
  // Test if there are some jobs to do
  if (exec_pingTest) pingTest();
}

void loop(void) {
  delay(10);  
#if defined(HBNODE)  
  cur_voltage = (vcc.Read_Volts()+((float)eeprom.voltageadded/100.0))*((float)eeprom.voltagefactor)/100.0;
  low_voltage_flag = (vcc.Read_Volts() <= LOWVOLTAGELEVEL);
  if ((! low_voltage_flag) || (last_send > 3600000)) {
#if defined(HAS_DISPLAY)
    if ( ! monitormode ) {
      draw_battery(BATT_X0,BATT_Y0,cur_voltage);
      draw_therm(THERM_X0, THERM_Y0);
      draw_hb_countdown((uint8_t) 8 * (1- ((float)loopcount / eeprom.emptyloopcount)) );
    }
#endif
    get_sensordata();
#if defined(HAS_DISPLAY)
    draw_temp(temp);
    wipe_therm(THERM_X0, THERM_Y0);
#endif
    if ( loopcount == 0) {
#if defined(HAS_DISPLAY)
      draw_antenna(ANT_X0, ANT_Y0);
#endif
      radio.powerUp();
      radio.startListening();
      radio.openReadingPipe(1,rf24_hub2node);
      delay(10);
      payloadInitHeader(PAYLOAD_TYPE_HB, PAYLOAD_FLAG_LASTMESSAGE, 0);
      payloadInitData();
#if defined(SERIAL_DEBUG)
          delay(100);
          Serial.println("WakeUp");
          Serial.print("Voltage: ");
          Serial.println(cur_voltage);
          Serial.print("Temperature: ");
          Serial.println(temp);
          Serial.print("Pressure: ");
          Serial.println(pres);
#endif  
      payload_data(1, 101, cur_voltage);
#if defined(DALLAS_18B20)
      payload_data(2, 1, temp);
#endif
#if defined(BME_280)
      payload_data(2, 1, temp);
      payload_data(3, 2, pres);
      payload_data(4, 3, humi);
#endif
#if defined(BMP_280)
      payload_data(2, 1, temp);
      payload_data(3, 2, pres);
#endif
      if ( sendData(eeprom.sendloopcount) ) processData(eeprom.receiveloopcount);
      exec_jobs();
      radio.stopListening();
#if defined(SERIAL_DEBUG)
      Serial.println("GoSleep");
      delay(100);
#endif
#if defined(HAS_DISPLAY)
      wipe_antenna(ANT_X0, ANT_Y0);
#endif
      radio.powerDown();
    }
  } else {
#if defined(HAS_DISPLAY)
    display_sleep(true);
#endif
  }  if ( sleep_kor_time != 0 ) {
    long int tempsleeptime = eeprom.sleeptime + sleep_kor_time;
#if defined(HAS_DISPLAY)
    if ( monitormode ) {
      monitor(tempsleeptime/2);
    } else {
#endif
      sleep4ms(tempsleeptime);
#if defined(HAS_DISPLAY)
    }
#endif
    sleep_kor_time=0;
  } else {
#if defined(HAS_DISPLAY)
    if ( monitormode ) {
      monitor(eeprom.sleeptime/2);
    } else {
#endif
      sleep4ms(eeprom.sleeptime);
#if defined(HAS_DISPLAY)
    }
#endif
  } 
  last_send += eeprom.sleeptime;
  loopcount++;
  if (loopcount > eeprom.emptyloopcount) loopcount=0;
#else
// Always On Node from here  
  if ( receiveData() ) { 
    processData(1);
    exec_jobs();
  } else {
    delay(300);
  }
#endif
}
