/*
A thermometer.
Can be used with a display or only as a sensor without display

On Branch: rpi1_entw @ rpi1  !!!!!


*/
//****************************************************
// My definitions for my nodes based on this sketch
// Select only one at one time !!!!
//#define AUSSENTHERMOMETER
//#define SCHLAFZIMMERTHERMOMETER
#define TESTZIMMERTHERMOMETER
//#define TESTZIMMER1THERMOMETER
//#define BASTELZIMMERTHERMOMETER
//#define KUECHETHERMOMETER
//#define WOHNZIMMERTHERMOMETER
//#define ANKLEIDEZIMMERTHERMOMETER
//#define GAESTEZIMMERTHERMOMETER
//#define UNOTESTNODE_AO
//****************************************************
// Default values => can be oberwritten by node config
// Kontrast of the display
#define CONTRAST 65;
// Brightnes of the display - unused until now
#define BRIGHTNES 0;
// Sleeptime in Seconds !! 
// (valid: 10 ... 32.400)
#define SLEEPTIME_SEC   120
// Sleeptime adjust - will be added to sleeptime_sec
// (valid: -1000 ... 1000)
#define SLEEPTIME_ADJ   0
// number of empty loop after sending data
// (valid: 0...20)
#define EMPTYLOOPS      0
// waiting time between 2 transmissions in ms
// (valid 100 ... 1000)
#define SENDDELAY       1000
// Max number of attempts to send for a nomal message!!
// (valid 1 ... 20)
#define MAX_SENDCOUNT   10
// Max number of attempts to send for a stop message!!
// (valid 1 ... 20)
#define MAX_STOPCOUNT   3
// Voltage faktor will be multiplied to the messured value !!!!
// (valid 0.1 ... 10)
#define VOLT_FAC        1.0
// Voltage offset will be added to messured value * volt_fac !!!
// (valid -10 ... 10)
#define VOLT_OFF        0
// Define low voltage level on processor
// below that level the thermometer will be switched off 
// until the battery will be reloaded
#define LOW_VOLT_LEVEL 2
// In case of low Voltage send every X Seconds
// 43200 => every 12 hours
#define LOW_VOLT_SENDINT  43200
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
#define BOSCH_SENSOR
#define DISPLAY_5110
//#define MONITOR
#define EEPROM_VERSION       2
#define EMPTYLOOPS           1
#define VOLT_FAC             1
#define VOLT_OFF             0.55
#define LOW_VOLT_LEVEL       3.6
#define SERIAL_DEBUG_SENSOR
#define SERIAL_DEBUG_TXRX
#endif
//-----------------------------------------------------
#if defined(TESTZIMMER1THERMOMETER)
#define HBNODE
#define RF24NODE             103
#define DALLAS_18B20
#define DISPLAY_5110
#define MONITOR
#define EEPROM_VERSION       4
#define VOLT_FAC             1
#define VOLT_OFF             0.55
#define EMPTYLOOPS           1
#define LOWVOLTAGELEVEL      3.6
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
//*****************************************************
// ------ End of configuration part ------------

//define constrains for debugging
#if defined(SERIAL_DEBUG_SENSOR)
#define SERIAL_DEBUG
#endif
#if defined(SERIAL_DEBUG_TXRX)
#define SERIAL_DEBUG
#endif
#if defined(SERIAL_DEBUG_CONFIG)
#define SERIAL_DEBUG
#endif
#if defined(SERIAL_DEBUG_PAYLOAD)
#define SERIAL_DEBUG
#endif

#include <avr/pgmspace.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <SPI.h>
#include <sleeplib.h>
#include <Vcc.h>
#include <EEPROM.h>
#include "dataformat.h"
#include "printf.h"
#include "version.h"
#include "rf24_config.h"

#if defined(DISPLAY_5110)
#define HAS_DISPLAY
#include <LCD5110_Graph.h>
#endif

#if defined(DALLAS_18B20)
#include <OneWire.h>
#include <DallasTemperature.h>
#endif

#if defined(BOSCH_SENSOR)
#include <BMX_sensor.h>
#endif

// ----- End of Includes ------------------------

Vcc vcc(1.0);

ISR(WDT_vect) { watchdogEvent(); }

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

#if defined(BOSCH_SENSOR)
BMX_SENSOR sensor;
float temp, pres, humi;
#endif

payload_t r_payload, s_payload;    

uint8_t  rf24_node2hub[] = RF24_NODE2HUB;
uint8_t  rf24_hub2node[] = RF24_HUB2NODE;

struct eeprom_t {
   uint8_t  versionnumber;
   uint8_t  contrast;
   uint8_t  brightnes;
   uint16_t sleeptime_sec;
   int      sleeptime_adj;
   uint8_t  emptyloops;
   uint16_t senddelay;
   uint8_t  max_sendcount;
   uint8_t  max_stopcount;
   uint8_t  pa_level;
   uint64_t low_volt_sendint;
   float    volt_fac;
   float    volt_off;
   float    low_volt_level;
};
eeprom_t eeprom;

boolean             display_down = false;
boolean             low_voltage_flag = false;
boolean             exec_pingTest = false;
boolean             exec_RegTrans = false;
float               cur_voltage;
uint8_t             loopcount;
int                 sleeptime_kor;
uint32_t            last_send;
uint8_t             last_orderno = 0;
uint8_t             msg_id = 0;
boolean             monitormode = false;

//Some Var for restore after sleep of display
#if defined(HAS_DISPLAY)
float               field1_val, field2_val, field3_val, field4_val;
#endif


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
#if defined(BOSCH_SENSOR)
#if defined(SERIAL_DEBUG_SENSOR)
  if (sensor.isBMP180()) Serial.println("BMP180");
  if (sensor.isBMP280()) Serial.println("BMP280");
  if (sensor.isBME280()) Serial.println("BME280"); 
#endif
  sensor.startSingleMeasure();
  if (sensor.hasTemperature() ) temp=sensor.getTemperature();
  if (sensor.hasPressure() ) pres=sensor.getPressureAtSealevel(95.0);
  if (sensor.hasHumidity() ) humi=sensor.getHumidity();
#endif
}

uint32_t action_loop(uint32_t data) {
  uint32_t retval = 0;
  int      intval;
  uint8_t  channel = getChannel(data);
  
    switch (channel) {
#if defined(HAS_DISPLAY)
      case 21:
        // Set field 1
        field1_val = getValue_f(data);;
        print_field(field1_val,1);
      break;
      case 22:
        // Set field 2
        field2_val = getValue_f(data);;
        print_field(field2_val,2);
      break;
      case 23:
        // Set field 3
        field3_val = getValue_f(data);;
        print_field(field3_val,3);
      break;
      case 24:
        // Set field 4
        field4_val = getValue_f(data);;
        print_field(field4_val,4);
      break;
      case 51:
        // Displaylight ON <-> OFF
        if ( getValue_ui(data) & 0x01 ) {
          digitalWrite(STATUSLED,STATUSLED_ON); 
        } else  {
          digitalWrite(STATUSLED,STATUSLED_OFF);
        }
      break;
      case 52:
        // Display Sleepmode ON <-> OFF
        display_sleep( getValue_ui(data) & 0x01 );
      break;
#endif
      case REG_BATT:  
      // battery voltage
        data = calcTransportValue_f(REG_BATT, cur_voltage);
      break;      
      case REG_TRANSREG:  
        exec_RegTrans = true;
      break;      
      case REG_MONITOR:
        monitormode = (getValue_ui(data) & 1);
      break;
      case REG_DISPLAY: {
        uint16_t val = getValue_ui(data);
        uint8_t contrast = (val & 0x00FF);
        uint8_t brightnes = (val>>8);
        if (contrast > 0 && contrast < 101) {
#if defined(DISPLAY_5110)
          myGLCD.setContrast(eeprom.contrast);
#endif
          eeprom.contrast = contrast;
          EEPROM.put(0, eeprom);
        }
      }
      break;
      case REG_SLEEPTIME: {
        // sleeptime in sec!
        uint16_t val = getValue_ui(data);
        if ( val > 9 && val < 32401) {
          eeprom.sleeptime_sec = val;
          EEPROM.put(0, eeprom);
        }
      }
      break;
      case REG_SLEEPTIMEADJ: {
        // sleeptime adjust in sec!
        int16_t val = getValue_i(data);
        if (val >= -1000 && val <= 1000) {
          eeprom.sleeptime_adj = val;
          EEPROM.put(0, eeprom);
        }
      }
      break;
      case REG_EMPTYLOOPS: {
        // emptyloops - number of loops without sending to hub / messure and display only!
        uint8_t val = getValue_ui(data);
        if (val < 21) {
          eeprom.emptyloops=val;
          EEPROM.put(0, eeprom);
        }
      }
      break;
      case REG_SLEEPTIMEKOR: {
        // sleeptime_kor: onetime adjust of sleeptime, will be reset to 0 after use 
        int16_t val = getValue_i(data);
        if (val > -1001 && val <= 1001) {
          sleeptime_kor = val;
        }
      }
      break;
      case REG_SENDDELAY: {
        // senddelay in millisec.
        uint16_t val = getValue_ui(data);
        if (val > 49 && val < 1001) {
          eeprom.senddelay = val;
          EEPROM.put(0, eeprom);
        }
      }
      break;
      case REG_SNDCNTN: {
        // max_sendcount: numbers of attempts to send for normal messages
        uint16_t val = getValue_ui(data);
        if (val > 0 && val < 21) {
          eeprom.max_sendcount = val;
          EEPROM.put(0, eeprom);
        }
      }
      break;
      case REG_SNDCNTS: {
      // max_stopcount: numbers of attempts to send for stop messages
        uint16_t val = getValue_ui(data);
        if (val > 0 && val < 21) {
          eeprom.max_stopcount = val;
          EEPROM.put(0, eeprom);
        }
      }
      break;
      case REG_VOLTFAC: {
        // Volt_fac - V = Vmess * Volt_fac
        float val = getValue_f(data);
        if (val >= 0.1 && val <= 10) {
          eeprom.volt_fac = val;
          EEPROM.put(0, eeprom);
        }
      }
      break;
      case REG_VOLTOFF: {
        // Volt_off - V = (Vmess * Volt_fac) + Volt_off
        float val = getValue_f(data);
        if (val >= -10 && val <= 10) {
          eeprom.volt_off = val;
          EEPROM.put(0, eeprom);
        }
      }
      break;
      case REG_LOWVOLTLEV: {
        // Low Voltage Level
        float val = getValue_f(data);
        if (val >= 1 && val <= 5) {
          eeprom.low_volt_level = val;
          EEPROM.put(0, eeprom);
        }
      }
      break;
      case REG_LOWVOLTINT: {
        // Low Voltage send interval
        uint16_t val = getValue_ui(data);
        if (val > 59 && val < 1441) {
          eeprom.volt_off = val;
          EEPROM.put(0, eeprom);
        }
      }
      break;
      case REG_PALEVEL: {
        // PA Level
        uint16_t val = getValue_ui(data);
        if (val == 9) exec_pingTest = true;
        if (val > 0 && val < 5) {
          eeprom.pa_level = val;
          // Speichern im EEPROM (noch) nicht nötig
        }
      }
      break;
      case REG_SW:
        data = calcTransportValue_f(REG_SW, SWVERSION);
      break;
    }  
    return data; 
}  

void setup(void) {
  delay(500);
  pinMode(STATUSLED, OUTPUT);     
  digitalWrite(STATUSLED,STATUSLED_ON); 
  EEPROM.get(0, eeprom);
  if (eeprom.versionnumber != EEPROM_VERSION && EEPROM_VERSION > 0) {
    eeprom.versionnumber    = EEPROM_VERSION;
    eeprom.brightnes        = BRIGHTNES
    eeprom.contrast         = CONTRAST;
    eeprom.sleeptime_sec    = SLEEPTIME_SEC;
    eeprom.sleeptime_adj    = SLEEPTIME_ADJ;
    eeprom.emptyloops       = EMPTYLOOPS;
    eeprom.senddelay        = SENDDELAY;
    eeprom.max_sendcount    = MAX_SENDCOUNT;
    eeprom.max_stopcount    = MAX_STOPCOUNT;
    eeprom.volt_fac         = VOLT_FAC;
    eeprom.volt_off         = VOLT_OFF;
    eeprom.low_volt_level   = LOW_VOLT_LEVEL;
    eeprom.low_volt_sendint = LOW_VOLT_SENDINT;
    EEPROM.put(0, eeprom);
  }
#if defined(SERIAL_DEBUG)
  Serial.begin(115200);
  printf_begin();
#endif
  SPI.begin();
#if defined(DALLAS_18B20)
  sensor.begin(); 
#endif
#if defined(BOSCH_SENSOR)
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
#if defined(SERIAL_DEBUG_TXRX)
  radio.printDetails();
#endif    
  delay(1000);
  digitalWrite(STATUSLED,STATUSLED_OFF); 
#if defined(HAS_DISPLAY)
#if defined(DISPLAY_5110)
  myGLCD.InitLCD();
  myGLCD.setContrast(eeprom.contrast);
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
  pingTest();
  send_register();
}

#if defined(HAS_DISPLAY)
void monitor(uint32_t delaytime) {
#if defined(MONITOR)  
  const char string_0[] PROGMEM = "SW Version: ";
  const char string_1[] PROGMEM = "Temp: ";
  const char string_2[] PROGMEM = "Ubatt: ";
  const char string_3[] PROGMEM = "Loops: ";
  const char string_4[] PROGMEM = "Send: ";
  const char string_5[] PROGMEM = "RF24 Nw: ";
  const char string_6[] PROGMEM = "Node: ";
  const char string_7[] PROGMEM = "Channel: ";
  const char string_8[] PROGMEM = "Kontrast: ";
#if defined(DISPLAY_5110)
  myGLCD.setFont(SmallFont);
  myGLCD.print(string_0, 10, 5);
  myGLCD.printNumI(SWVERSION, 35, 30);
  myGLCD.update();
  sleep4ms(delaytime);
  delay(10);
  myGLCD.clrScr();
  get_sensordata();
  myGLCD.print(string_1, 0, 0);
  myGLCD.printNumF(temp,1, 30, 0);
  cur_voltage = vcc.Read_Volts()*eeprom.volt_fac+eeprom.volt_off;
  float mes_voltage = vcc.Read_Volts();
  myGLCD.print(string_2, 0, 10);
  myGLCD.printNumF(cur_voltage,1, 40, 10);
  myGLCD.printNumF(mes_voltage,1, 65, 10);
  myGLCD.print(string_3, 0, 20);
  myGLCD.printNumI(eeprom.sleeptime_sec, 45, 20);
  myGLCD.print("/", 60, 20);
  myGLCD.printNumI(eeprom.emptyloops, 70, 20);
  myGLCD.print(string_4, 0, 30);
  myGLCD.printNumI(eeprom.senddelay, 0, 40);
  myGLCD.print("/", 20, 40);
  myGLCD.printNumI(eeprom.max_sendcount, 30, 40);
  myGLCD.print("/", 60, 40);
  myGLCD.printNumI(eeprom.max_stopcount, 70, 40);
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
  myGLCD.printNumI(eeprom.contrast, 55, 0);
  myGLCD.setFont(BigNumbers);
  for (int i=0; i<100; i+=5) {
    myGLCD.printNumI(i, 20, 20);        
    myGLCD.setContrast(i);
    myGLCD.update();
    delay(300);
  }
  sleep4ms(5000);
  myGLCD.setContrast(eeprom.contrast);
  delay(10);
  myGLCD.clrScr();
#endif  
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

void payloadInitData(void) {
  s_payload.data1 = 0;
  s_payload.data2 = 0;
  s_payload.data3 = 0;
  s_payload.data4 = 0;
  s_payload.data5 = 0;
  s_payload.data6 = 0;
}

#if defined(SERIAL_DEBUG_PAYLOAD)
void printPayloadData(uint32_t pldata) {
    Serial.print("(");
    Serial.print(getChannel(pldata));
    Serial.print("/");
// Überarbeiten !!!!!!!!
// Datentyp ist abhängig vom Channel !!!!    
    Serial.print(getValue_f(pldata));
    Serial.print(")");
}
#endif

void printPayload(payload_t* pl) {
#if defined(SERIAL_DEBUG_PAYLOAD)
    Serial.print("I:");
    Serial.print(pl->msg_id);
    Serial.print(" T:");
    Serial.print(pl->msg_type);
    Serial.print(" O:");
    Serial.print(pl->orderno);
    Serial.print(" ");
    printPayloadData(pl->data1));
    printPayloadData(pl->data2));
    printPayloadData(pl->data3));
    printPayloadData(pl->data4));
    printPayloadData(pl->data5));
    printPayloadData(pl->data6));
    Serial.println();
#endif
}

void payload_data(uint8_t pos, uint8_t channel, float value) {
  switch (pos) {
    case 1:
      s_payload.data1 = calcTransportValue_f(channel, value);
    break;
    case 2:
      s_payload.data2 = calcTransportValue_f(channel, value);
    break;
    case 3:
      s_payload.data3 = calcTransportValue_f(channel, value);
    break;
    case 4:
      s_payload.data4 = calcTransportValue_f(channel, value);
    break;
    case 5:
      s_payload.data5 = calcTransportValue_f(channel, value);
    break;
    case 6:
      s_payload.data6 = calcTransportValue_f(channel, value);
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
  prep_data(3, PAYLOAD_TYPE_PING_POW_MAX, PAYLOAD_FLAG_LASTMESSAGE, 0, true);
  radio.setPALevel( RF24_PA_HIGH) ;
  prep_data(3, PAYLOAD_TYPE_PING_POW_HIGH, PAYLOAD_FLAG_LASTMESSAGE, 0, true);
  radio.setPALevel( RF24_PA_LOW) ;
  prep_data(3, PAYLOAD_TYPE_PING_POW_LOW, PAYLOAD_FLAG_LASTMESSAGE, 0, true);
  radio.setPALevel( RF24_PA_MIN) ;
  prep_data(3, PAYLOAD_TYPE_PING_POW_MIN, PAYLOAD_FLAG_LASTMESSAGE, 0, true);
  radio.setPALevel( PA_Level ); 
  exec_pingTest = false;
}


void send_register(void) {
  s_payload.data1 = calcTransportValue_f(REG_VOLTFAC, eeprom.volt_fac);
  s_payload.data2 = calcTransportValue_f(REG_VOLTOFF, eeprom.volt_off);
  s_payload.data3 = calcTransportValue_f(REG_LOWVOLTLEV, eeprom.low_volt_level);
  s_payload.data4 = calcTransportValue_i(REG_SLEEPTIMEADJ, eeprom.sleeptime_adj);
  s_payload.data5 = calcTransportValue_ui(REG_LOWVOLTINT, eeprom.low_volt_sendint);
  s_payload.data6 = calcTransportValue_ui(REG_SW, SWVERSION);
  prep_data(3,PAYLOAD_TYPE_INIT,PAYLOAD_FLAG_EMPTY,0,false);
  s_payload.data1 = calcTransportValue_ui(REG_DISPLAY, (eeprom.brightnes<<8 | eeprom.contrast) );
  s_payload.data2 = calcTransportValue_ui(REG_SLEEPTIME, eeprom.sleeptime_sec);
  s_payload.data3 = calcTransportValue_ui(REG_EMPTYLOOPS, eeprom.emptyloops);
  s_payload.data4 = calcTransportValue_ui(REG_SENDDELAY, eeprom.senddelay);
  s_payload.data5 = calcTransportValue_ui(REG_SNDCNTN, eeprom.max_sendcount);
  s_payload.data6 = calcTransportValue_ui(REG_SNDCNTS, eeprom.max_stopcount);
  prep_data(3,PAYLOAD_TYPE_INIT,PAYLOAD_FLAG_LASTMESSAGE,0,false);
  exec_RegTrans = false;
}

void prep_data(uint8_t numloopcount, uint8_t msg_type, uint8_t msg_flags, ONR_DATTYPE orderno, bool null_data) {
  if (null_data) payloadInitData();
  s_payload.node_id = RF24NODE;
  s_payload.msg_id = 1;
  s_payload.msg_type = msg_type;
  s_payload.msg_flags = msg_flags;
  s_payload.orderno = orderno;
  if ( send_data(numloopcount) ) process_data();
}

bool send_data(uint8_t maxSendLoopCount) {
  bool retval = false;
  while ( s_payload.msg_id <= maxSendLoopCount ) {
    radio.stopListening();
#if defined(SERIAL_DEBUG_TXRX)
    Serial.print("TX => I:");
    Serial.print(s_payload.msg_id);
    Serial.print(" T:");
    Serial.print(s_payload.msg_type);
    Serial.print(" O:");
    Serial.print(s_payload.orderno);
    Serial.print(" ");
    if (radio.write(&s_payload, sizeof(s_payload))) {
      Serial.println("OK");
    } else {
      Serial.println("fail");
    } 
#else
    radio.write(&s_payload, sizeof(s_payload));
#endif         
    radio.startListening();
    s_payload.msg_id++;
    if ( receive_data() ) {
        s_payload.msg_id = maxSendLoopCount +1;
        retval = true;
    }
  }
  return retval;
}

bool receive_data(void) {
    bool retval = false;
    uint8_t loopcount = 0;
    while (loopcount < 8) {
      loopcount++;
      delay(eeprom.senddelay >> 3);     
      if ( radio.available() ) {
        radio.read(&r_payload, sizeof(r_payload));
        if (r_payload.node_id == RF24NODE ) {
#if defined(SERIAL_DEBUG_TXRX)
          Serial.print("RX I:");
          Serial.print(r_payload.msg_id);
          Serial.print(" T:");
          Serial.println(r_payload.msg_type);
          Serial.print(" O:");
          Serial.println(r_payload.orderno);
#endif    
          retval = true;
          loopcount = 8;
        }
      }
    }
    return retval;
}

void process_data(void) {
  switch (r_payload.msg_type) {
    case PAYLOAD_TYPE_HB_RESP:
      prep_data(eeprom.max_stopcount,PAYLOAD_TYPE_HB_STOP,PAYLOAD_FLAG_LASTMESSAGE,r_payload.orderno,true);
      break;
    case PAYLOAD_TYPE_DATSTOP:
      // go sleep !!
      break;
    case PAYLOAD_TYPE_DAT:  {
        if (r_payload.data1 > 0) { s_payload.data1 = action_loop(r_payload.data1); } else { s_payload.data1 = 0; }
        if (r_payload.data2 > 0) { s_payload.data2 = action_loop(r_payload.data2); } else { s_payload.data2 = 0; }
        if (r_payload.data3 > 0) { s_payload.data3 = action_loop(r_payload.data3); } else { s_payload.data3 = 0; }
        if (r_payload.data4 > 0) { s_payload.data4 = action_loop(r_payload.data4); } else { s_payload.data4 = 0; }
        if (r_payload.data5 > 0) { s_payload.data5 = action_loop(r_payload.data5); } else { s_payload.data5 = 0; }
        if (r_payload.data6 > 0) { s_payload.data6 = action_loop(r_payload.data6); } else { s_payload.data6 = 0; }
        prep_data(eeprom.max_sendcount,PAYLOAD_TYPE_DATRESP,PAYLOAD_FLAG_LASTMESSAGE,r_payload.orderno,false);
    }      
  }
}
  
void exec_jobs(void) {
  // Test if there are some jobs to do
  if (exec_pingTest) {
    pingTest();
    delay(1000);
  }
  if (exec_RegTrans) {
    send_register();
    delay(1000);
  }
}

void loop(void) {
  delay(10);  
  cur_voltage = vcc.Read_Volts()*eeprom.volt_fac+eeprom.volt_off;
  low_voltage_flag = (cur_voltage <= eeprom.low_volt_level);
  payloadInitData();
#if defined(HBNODE)  
#if defined(HAS_DISPLAY)
    if (low_voltage_flag) display_sleep(true);
#endif
  if ((! low_voltage_flag) || (last_send > eeprom.low_volt_sendint)) {
    if (last_send > eeprom.low_volt_sendint) last_send = 0;
#if defined(HAS_DISPLAY)
    if ( ! monitormode ) {
      draw_battery(BATT_X0,BATT_Y0,cur_voltage);
      draw_therm(THERM_X0, THERM_Y0);
      draw_hb_countdown((uint8_t) 8 * (1- ((float)loopcount / eeprom.emptyloops)) );
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
      
      // Empty FiFo Buffer from old transmissions
      while ( radio.available() ) {
        radio.read(&r_payload, sizeof(r_payload));
        delay(10);
      }

//      payloadInitHeader(PAYLOAD_TYPE_HB, PAYLOAD_FLAG_LASTMESSAGE, 0);
#if defined(SERIAL_DEBUG_TXRX)
          delay(100);
          Serial.println("WakeUp");
#endif
#if defined(SERIAL_DEBUG_SENSOR)
          Serial.print("Volt: ");
          Serial.println(cur_voltage);
          Serial.print("Temp: ");
          Serial.println(temp);
          Serial.print("Pres: ");
          Serial.println(pres);
#endif  
      payload_data(1, 101, cur_voltage);
#if defined(DALLAS_18B20)
      payload_data(2, 1, temp);
#endif
#if defined(BOSCH_SENSOR)
      if (sensor.hasTemperature() ) payload_data(2, 1, temp);
      if (sensor.hasPressure() )    payload_data(3, 2, pres);
      if (sensor.hasHumidity() )    payload_data(4, 3, humi);
#endif
//      if ( sendData(eeprom.sendloopcount) ) processData(eeprom.receiveloopcount);
      uint8_t msg_flags = PAYLOAD_FLAG_LASTMESSAGE;
      if ( low_voltage_flag ) msg_flags |= PAYLOAD_FLAG_NEEDHELP; 
      prep_data(eeprom.max_sendcount,PAYLOAD_TYPE_HB,PAYLOAD_FLAG_LASTMESSAGE,0,false);

      exec_jobs();
      radio.stopListening();
#if defined(SERIAL_DEBUG_TXRX)
      Serial.println("Sleep");
      delay(100);
#endif
#if defined(HAS_DISPLAY)
      wipe_antenna(ANT_X0, ANT_Y0);
#endif
      radio.powerDown();
    }
  }  
#if defined(HAS_DISPLAY)
  if ( monitormode ) {
    monitor(2);
  } else {
#endif
//ToDo prüfen und ggf. überarbeiten
  long int tempsleeptime = (long int)(eeprom.sleeptime_sec + eeprom.sleeptime_adj + sleeptime_kor) * 1000;
  sleeptime_kor = 0;  
  sleep4ms(tempsleeptime);
#if defined(HAS_DISPLAY)
  }
#endif
  last_send += eeprom.sleeptime_sec;
  loopcount++;
  if (loopcount > eeprom.emptyloops) loopcount=0;
// Ende ToDo  
#else
// Always On Node from here  
  if (receive_data()) {
    process_data(); 
    exec_jobs();
  }
#endif
}
