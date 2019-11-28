/*
A thermometer.
Can be used with a display or only as a sensor without display
*/
//****************************************************
//          Define node general settings
#define RF24NODE        025
#define RF24CHANNEL     90
// Delay between 2 transmission in ms
#define RF24SENDDELAY 50
// Delay between 2 transmission in ms
#define RF24RECEIVEDELAY 50
// Sleeptime in ms !!
#define RF24SLEEPTIME   112300
// Max Number of Heartbeart Messages to send !!
#define RF24SENDLOOPCOUNT 10
// Max Number of  !!
#define RF24RECEIVELOOPCOUNT 10
// number of empty loop after sending data
#define RF24EMPTYLOOPCOUNT  9
// Voltage Faktor will be divided by 100 (Integer !!)!!!!
#define VOLTAGEFACTOR 119
// Change the versionnumber to store new values in EEPROM
// Set versionnumber to "0" to disable 
#define EEPROM_VERSION 5
//             END node general settings 
//*****************************************************
//       Define used temerature sensor here
#define DALLAS_18B20
//#define BME280
//             END temerature sensor
//*****************************************************
//       Define if we have a Display and which
//       disable all for temeraturesensor only
//#define HAS_DISPLAY
//#define DISPLAY_5110
//             END Display
//*****************************************************
// 4 voltages for the battery (empty ... full)
#define U0 3.6
#define U1 3.7
#define U2 3.8
#define U3 3.9
#define U4 4.0
// set X0 and Y0 of battery symbol ( is 10 * 5 pixel )
#define BATT_X0 74
#define BATT_Y0 0
// set X0 and Y0 of antenna symbol ( is 10 * 10 pixel )
#define ANT_X0 74
#define ANT_Y0 6
// set X0 and Y0 of thermometer symbol ( is 3 * 6 pixel )
#define THERM_X0 74
#define THERM_Y0 17
// set X0 and Y0 of waiting symbol ( is 6 * 6 pixel )
#define WAIT_X0 78
#define WAIT_Y0 17
// The CE Pin of the Radio module
#define RADIO_CE_PIN 10
// The CS Pin of the Radio module
#define RADIO_CSN_PIN 9
// The pin of the statusled
#define STATUSLED 3
#define STATUSLED_ON LOW
#define STATUSLED_OFF HIGH
#define ONE_WIRE_BUS 8
#define RECEIVEDELAY 100

// ------ End of configuration part ------------

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <sleeplib.h>
#include <Vcc.h>
#include <EEPROM.h>

#if defined(DISPLAY_5110)
#include <LCD5110_Graph.h>
#endif

#if defined(DALLAS_18B20)
#include <OneWire.h>
#include <DallasTemperature.h>
#endif

#if defined(BME280)
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#endif


// ----- End of Includes ------------------------

Vcc vcc(1.0);

ISR(WDT_vect) { watchdogEvent(); }

#if defined(DISPLAY_5110)
LCD5110 myGLCD(7,6,5,2,4);
extern uint8_t SmallFont[];
extern uint8_t BigNumbers[];
#endif
#if defined(DALLAS_18B20)
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS); 
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensor(&oneWire);
float temp;
#endif
#if defined(BME280)
Adafruit_BME280 sensor;
float temp, pres, humi;
#endif


// Structure of our payload
struct payload_t {
  uint16_t  orderno;      // the orderno as primary key for our message for the nodes
  uint16_t  flags;        // a field for varies flags
                          // flags are defined as:
                          // 0x01: if set: last message, node goes into sleeptime1 else: goes into sleeptime2
  uint8_t   sensor1;      // internal address of sensor1
  uint8_t   sensor2;      // internal address of sensor2
  uint8_t   sensor3;      // internal address of sensor3
  uint8_t   sensor4;      // internal address of sensor4
  float     value1;       // value of sensor1
  float     value2;       // value of sensor2
  float     value3;       // value of sensor3
  float     value4;       // value of sensor4
};
payload_t payload;    

struct eeprom_t {
   uint32_t sleeptime;
   uint16_t sendloopcount;
   uint16_t receiveloopcount;
   uint16_t emptyloopcount;
   uint16_t voltagefactor;
   uint16_t node;
   uint8_t  channel;
   uint8_t  versionnumber;
};
eeprom_t eeprom;

RF24NetworkHeader   rxheader;
RF24NetworkHeader   txheader(0);
boolean             display_down = false;
boolean             low_voltage_flag = false;
//Some Var for restore after sleep of display
#if defined(HAS_DISPLAY)
float               field1_val, field2_val, field3_val, field4_val;
#endif
float               cur_voltage;
uint16_t            loopcount;
uint16_t            receiveloopcount;
uint16_t            sendloopcount;
long int            sleep_kor_time;


// nRF24L01(+) radio attached using Getting Started board 
// Usage: radio(CE_pin, CS_pin)
RF24 radio(RADIO_CE_PIN,RADIO_CSN_PIN);

// Network uses that radio
RF24Network network(radio);

void get_sensordata(void) {
#if defined(DALLAS_18B20)
  sensor.requestTemperatures(); // Send the command to get temperatures
  sleep4ms(800);
  delay(10);
  temp=sensor.getTempCByIndex(0);
#endif
#if defined(BME280)
  sensor.takeForcedMeasurement();
  sleep4ms(800);
  delay(10);
  temp=sensor.readTemperature();
  pres=pow(((95*0.0065)/(sensor.readTemperature()+273.15)+1),5.257)*sensor.readPressure()/100.0;
  humi=sensor.readHumidity();
#endif
}

float action_loop(unsigned char channel, float value) {
  float retval = value;
    switch (channel) {
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
        if (value < 0.5) {
          digitalWrite(STATUSLED,STATUSLED_OFF); 
        } else  {
          digitalWrite(STATUSLED,STATUSLED_ON);
        }
       break;
      case 41:
        // Display Sleepmode ON <-> OFF
        display_sleep(value < 0.5);
       break;
#endif
      case 101:  
      // battery voltage
        retval = cur_voltage;
        break;      
      case 111:
      // sleeptime in ms!
        if (value > 0.5 || value < -0.5) {
          eeprom.sleeptime=(uint32_t)value;
          EEPROM.put(0, eeprom);
        } else {
          retval = (float)eeprom.sleeptime;
        }
        break;
      case 112:
      // sendloopcount - number sendloop befor giving up 
        if (value > 0.5 || value < -0.5) {
          eeprom.sendloopcount=(uint16_t)value;
          EEPROM.put(0, eeprom);
        } else {
          retval = (float)eeprom.sendloopcount;
        }
        break;
      case 113:
      // receiveloopcount - number of receivloops befor giving up.
        if (value > 0.5 || value < -0.5) {
          eeprom.receiveloopcount=(uint16_t)value;
          EEPROM.put(0, eeprom);
        } else {
          retval = (float)eeprom.receiveloopcount;
        }
        break;
      case 114:
      // emptyloopcount - only loop 0 will transmit all other loops will only read and display
        if (value > 0.5 || value < -0.5) {
          eeprom.emptyloopcount=(uint16_t)value;
          EEPROM.put(0, eeprom);
        } else {
          retval = (float)eeprom.emptyloopcount;
        }
        break;
      case 115:
      // sleep korrektion faktor in ms - will only be used once!
        if (value > 0.5 || value < -0.5) {
          sleep_kor_time = (long int)value;
        } else {
          retval = sleep_kor_time;
        }
        break;
      case 116:
      // Voltage devider - will be divided by 100
        if (value > 0.5 || value < -0.5) {
          eeprom.voltagefactor=(uint16_t)value;
          EEPROM.put(0, eeprom);
        } else {
          retval = (float)eeprom.voltagefactor;
        }
        break;
    }  
    return retval;
}  

void setup(void) {
  unsigned long last_send=millis();
  pinMode(STATUSLED, OUTPUT);     
  digitalWrite(STATUSLED,STATUSLED_ON); 
  EEPROM.get(0, eeprom);
  if (eeprom.versionnumber != EEPROM_VERSION && EEPROM_VERSION > 0) {
    eeprom.versionnumber = EEPROM_VERSION;
    eeprom.sleeptime = RF24SLEEPTIME;
    eeprom.sendloopcount = RF24SENDLOOPCOUNT;
    eeprom.receiveloopcount = RF24RECEIVELOOPCOUNT;
    eeprom.emptyloopcount = RF24EMPTYLOOPCOUNT;
    eeprom.voltagefactor = VOLTAGEFACTOR;
    eeprom.node = RF24NODE;
    eeprom.channel = RF24CHANNEL; 
    EEPROM.put(0, eeprom);
  }
  SPI.begin();
  radio.begin();
  sensor.begin(); 
#if defined(DISPLAY_5110)
  myGLCD.InitLCD();
  myGLCD.setContrast(65);
  myGLCD.clrScr();
#endif
#if defined(HAS_DISPLAY)
  draw_antenna(ANT_X0, ANT_Y0);
#endif
  network.begin(eeprom.channel, eeprom.node);
  radio.setDataRate( RF24_250KBPS );
  radio.setPALevel( RF24_PA_MAX ) ;
  delay(100);
  digitalWrite(STATUSLED,STATUSLED_OFF); 
  delay(1000);
  loopcount = 0;
}

#if defined(HAS_DISPLAY)
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
#endif
  }
}

void wipe_therm(byte x, byte y) {
  if ( ! display_down ) {
    for (byte i=x; i<x+3; i++) {
      for (byte j=y; j<y+6; j++) {
#if defined(DISPLAY_5110)
        myGLCD.clrPixel(i,j);
#endif
      }
    }
  }
}

void draw_temp(float t) {
  int mytemp, temp_i, temp_dez_i; 
  if ( ! display_down ) {
#if defined(DISPLAY_5110)
    if (t < 0) {
      mytemp=t*-1;
      myGLCD.drawRect(0,10,9,12);
    } else {
      mytemp=t;
    }      
    temp_i=(int)temp;
    temp_dez_i=t*10-temp_i*10;
    for(byte i=0; i<74; i++) {
      for (byte j=0; j<25; j++) {
        myGLCD.clrPixel(i,j);
      }
    }
    if (temp<99) {
      myGLCD.setFont(BigNumbers);
      myGLCD.printNumI(temp_i, 10, 0);
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
  
void loop(void) {
  delay(10);  
  cur_voltage = vcc.Read_Volts()*((float)eeprom.voltagefactor)/100.0;
  low_voltage_flag = (cur_voltage <= U0);
  if (! low_voltage_flag) {
#if defined(HAS_DISPLAY)
    draw_battery(BATT_X0,BATT_Y0,cur_voltage);
    draw_therm(THERM_X0, THERM_Y0);
#endif
    get_sensordata();
#if defined(HAS_DISPLAY)
    draw_temp(temp);
    wipe_therm(THERM_X0, THERM_Y0);
#endif
    if ( loopcount == 0) {
      // Clear the RX buffer !!!
      while ( network.update() ) {}
      while ( network.available() ) {
        network.read(rxheader,&payload,sizeof(payload));
        network.update();
      }
      radio.flush_rx();      
#if defined(HAS_DISPLAY)
      draw_antenna(ANT_X0, ANT_Y0);
#endif
      radio.powerUp();
      radio.startListening();
      delay(10);
      payload.orderno = 0;
      payload.sensor1 = 1;
      payload.value1 = temp;
      payload.sensor2 = 101;
      payload.value2 = cur_voltage;
#if defined(DALLAS_18B20)
      payload.sensor3 = 0;
      payload.value3 = 0;
      payload.sensor4 = 0;
      payload.value4 = 0;
#endif
#if defined(BME280)
      payload.sensor3 = 2;
      payload.value3 = pres;
      payload.sensor4 = 3;
      payload.value4 = humi;
#endif
      txheader.type = 51;
      receiveloopcount = 0;
      sendloopcount = 0;
      while ( sendloopcount < eeprom.sendloopcount ) {
        network.update();
        if ( network.available() ) {
          sendloopcount = eeprom.sendloopcount;
          while ( receiveloopcount < eeprom.receiveloopcount ) {
            if ( network.available() ) {
              network.read(rxheader,&payload,sizeof(payload));
              if ( rxheader.type == 52 ) {
                receiveloopcount = eeprom.receiveloopcount;
              } else {
                payload.value1 = action_loop(payload.sensor1, payload.value1);
                payload.value2 = action_loop(payload.sensor2, payload.value2);
                payload.value3 = action_loop(payload.sensor3, payload.value3);
                payload.value4 = action_loop(payload.sensor4, payload.value4);
              }
              if ((payload.flags & 0x01) == 0x01 ) {
                receiveloopcount = eeprom.receiveloopcount;
              }
              txheader.type = rxheader.type;
              network.write(txheader,&payload,sizeof(payload));
              network.update();
              delay(RF24RECEIVEDELAY);
              network.update();
            }
            receiveloopcount++;
          }
        } else {
            network.write(txheader,&payload,sizeof(payload));    
            delay(RF24SENDDELAY);
        }
        sendloopcount++;
      }
    }
    radio.stopListening();
#if defined(HAS_DISPLAY)
    wipe_antenna(ANT_X0, ANT_Y0);
#endif
    radio.powerDown();
  } else {
#if defined(HAS_DISPLAY)
    display_sleep(true);
#endif
  }
  if ( sleep_kor_time != 0 ) {
    long int tempsleeptime = eeprom.sleeptime + sleep_kor_time;
    sleep4ms(tempsleeptime);
    sleep_kor_time=0;
  } else {
    sleep4ms(eeprom.sleeptime);
  } 
  loopcount++;
  if (loopcount > eeprom.emptyloopcount) loopcount=0;
}
