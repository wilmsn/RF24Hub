/*
A thermometer for inside.
V3: Upgrade to Lowpower Library; display of a battery symbol

*/
// Define a valid radiochannel here
#define RADIOCHANNEL 90
// This node: Use octal numbers starting with "0": "041" is child 4 of node 1
#define NODE 01
// The CE Pin of the Radio module
#define RADIO_CE_PIN 10
// The CS Pin of the Radio module
#define RADIO_CSN_PIN 9
//define some sleeptime as default values
#define SLEEPTIME1 10
#define SLEEPTIME2 10
#define SLEEPTIME3 2
#define SLEEPTIME4 5
// The pin of the statusled
#define STATUSLED A1
#define STATUSLED_ON LOW
#define STATUSLED_OFF HIGH

// The outputpin for batterycontrol for the voltagedivider
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

// ------ End of configuration part ------------

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <sleeplib.h>
#include <Vcc.h>
//****
// some includes for your sensor(s) here
//****
#include <LCD5110_Graph.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

float temp, pres, humi;
boolean display_down = false;

const float VccCorrection = 1.0/1.0;  // Measured Vcc by multimeter divided by reported Vcc
Vcc vcc(VccCorrection);
Adafruit_BME280 bme;

ISR(WDT_vect) { watchdogEvent(); }

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
 
LCD5110 myGLCD(7,6,5,2,4);

extern uint8_t SmallFont[];
extern uint8_t BigNumbers[];

//####
//end aditional includes
//####

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

enum radiomode_t { radio_sleep, radio_listen } radiomode = radio_sleep;
enum sleepmode_t { sleep1, sleep2, sleep3, sleep4} sleepmode = sleep1, next_sleepmode = sleep2;

RF24NetworkHeader   rxheader;
RF24NetworkHeader   txheader(0);
// all sleeptime* values in seconds 
// Time for the fist sleep after an activity of this node
float               sleeptime1 = SLEEPTIME1;
// Time for the 2. to N. sleeploop
float               sleeptime2 = SLEEPTIME2;
// Time to sleep after wakeup with radio on
float               sleeptime3 = SLEEPTIME3;
// Time to keep the network up if it was busy
float               sleeptime4 = SLEEPTIME4;
boolean             init_finished = false;
unsigned int        networkup = 0;
uint16_t            orderno_p1, orderno_p2;
boolean             low_voltage_flag = false;
//Some Var for restore after sleep of display
float               field1_val, field2_val, field3_val, field4_val;
float               cur_voltage;
uint8_t             n_update = 0;


// nRF24L01(+) radio attached using Getting Started board 
// Usage: radio(CE_pin, CS_pin)
RF24 radio(RADIO_CE_PIN,RADIO_CSN_PIN);

// Network uses that radio
RF24Network network(radio);

void display_sleep(boolean dmode) {
  display_down = dmode;
  if ( ! low_voltage_flag ) {
    if ( dmode ) { // Display go to sleep
      myGLCD.enableSleep(); 
    } else {
      myGLCD.disableSleep(); 
        print_field(field1_val,1);
        print_field(field2_val,2);
        print_field(field3_val,3);
        print_field(field4_val,4);
    }
  }  
}

float action_loop(unsigned char channel, float value) {
  float retval = value;
    switch (channel) {
        case 1:
        //Temperature from BME280
        retval=temp;
        break;
        case 2:
        //Temperature from BME280
        retval=pres;
        break;
        case 3:
        //Humidity from BME280
        retval=humi;
        break;
      case 21:
        // Set field 1
        field1_val = value;
        print_field(field1_val,1);
       break;
      case 22:
        // Set field 2
        field2_val=value;
        print_field(field2_val,2);
       break;
      case 23:
        // Set field 3
        field3_val=value;
        print_field(field3_val,3);
       break;
      case 24:
        // Set field 4
        field4_val=value;
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
      case 101:  
      // battery voltage
        retval = cur_voltage;
        break;      
      case 110:
      // contrast of LCD Display
        myGLCD.setContrast(value);
        break;
      case 111:
      // sleeptimer1
        sleeptime1 = value;
        break;
      case 112:
      // sleeptimer2
        sleeptime2 = value;
        break;
      case 113:
      // sleeptimer3
        sleeptime3 = value;
        break;
      case 114:
      // sleeptimer4
        sleeptime4 = value;
        break;
      case 115:
      // radio on (=1) or off (=0) when sleep
        if ( value > 0.5) radiomode=radio_listen; else radiomode=radio_sleep;
        break;
      case 116:
      // Voltage devider
        vcc.m_correction = value;
        break;
      case 118:
      // init_finished (=1)
        init_finished = ( true );
        break;
    }  
    return retval;
}  

void setup(void) {
  unsigned long last_send=millis();
  pinMode(STATUSLED, OUTPUT);     
  digitalWrite(STATUSLED,STATUSLED_ON); 
  SPI.begin();
  radio.begin();
  //****
  // put anything else to init here
  //****
  myGLCD.InitLCD();
  myGLCD.setContrast(65);
  myGLCD.clrScr();
  bme.begin();
  cur_voltage = vcc.Read_Volts();
  get_bme280();
  draw_battery(BATT_X0, BATT_Y0,cur_voltage);
  draw_wait(WAIT_X0,WAIT_Y0,17);
  draw_antenna(ANT_X0, ANT_Y0);
  //####
  // end aditional init
  //####
  network.begin(RADIOCHANNEL, NODE);
  radio.setDataRate( RF24_250KBPS );
  radio.setPALevel( RF24_PA_MAX ) ;
  //radio.setRetries(1,15);
  // initialisation beginns
  bool do_transmit = true;
  while ( ! init_finished ) {
    // send only one message every second
    if ( do_transmit &&  last_send + 1000 < millis() ) {
      txheader.type=119;
      payload.orderno=0;
      payload.sensor1=119;
      payload.value1=0;
      network.write(txheader,&payload,sizeof(payload));
      last_send = millis();
    }
    network.update();
    if ( last_send + 100000 < millis()) { do_transmit = true; }
    if ( network.available() ) {
      do_transmit = false;
      network.read(rxheader,&payload,sizeof(payload));
      payload.value1 = action_loop(payload.sensor1, payload.value1);
      payload.value2 = action_loop(payload.sensor2, payload.value2);
      payload.value3 = action_loop(payload.sensor3, payload.value3);
      payload.value4 = action_loop(payload.sensor4, payload.value4);
      last_send = millis();
      txheader.type=1;
      network.write(txheader,&payload,sizeof(payload));
    }
  }
  delay(100);
  digitalWrite(STATUSLED,STATUSLED_OFF); 
  sleepmode=sleep4;
  networkup = 0;    
}

void draw_therm(byte x, byte y) {
  if ( ! display_down ) {
    myGLCD.drawRect(x+1,y,x+1,y+3);
    myGLCD.drawRect(x,y+4,x+2,y+5);
  }
}

void wipe_therm(byte x, byte y) {
  if ( ! display_down ) {
    for (byte i=x; i<x+3; i++) {
      for (byte j=y; j<y+6; j++) {
        myGLCD.clrPixel(i,j);
      }
    }
  }
}

void draw_temp(float t) {
  int mytemp, temp_i, temp_dez_i; 
  if ( ! display_down ) {
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
  }
}

void print_field(float val, int field) {
  int x0, y0;
  if ( ! display_down ) {
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
  }
}

void draw_battery_filled(int x, int y) {
  myGLCD.setPixel(x,y); 
  myGLCD.setPixel(x,y+1); 
  myGLCD.setPixel(x,y+2); 
  myGLCD.setPixel(x+1,y); 
  myGLCD.setPixel(x+1,y+1); 
  myGLCD.setPixel(x+1,y+2); 
}

void draw_battery(int x, int y, float u) {
  if ( ! display_down ) {
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
  }
}

void draw_antenna(int x, int y) {
  if ( ! display_down ) {
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
  }
}   
 
void wipe_antenna(int x, int y) {
  if ( ! display_down ) {
    for (int i=x; i<x+10; i++) {
      for (int j=y; j<y+10; j++) {
        myGLCD.clrPixel(i,j);
      }
    }
    myGLCD.update();
  }
}  
  
void draw_wait(byte x, byte y, int waitcount ) {
  if ( ! display_down ) {
    for (byte i=x; i<x+6; i++) {
      for(byte j=y; j<y+6; j++) {
        myGLCD.clrPixel(i,j);
      }
    }
    if (waitcount > 1) myGLCD.setPixel(x+3,y+2);
    if (waitcount > 2) myGLCD.setPixel(x+3,y+3);
    if (waitcount > 3) myGLCD.setPixel(x+2,y+3);
    if (waitcount > 4) myGLCD.setPixel(x+2,y+2);
    if (waitcount > 5) myGLCD.setPixel(x+3,y  );
    if (waitcount > 6) myGLCD.setPixel(x+4,y+1);
    if (waitcount > 7) myGLCD.setPixel(x+5,y+2);
    if (waitcount > 8) myGLCD.setPixel(x+5,y+3);
    if (waitcount > 9) myGLCD.setPixel(x+4,y+4);
    if (waitcount > 10) myGLCD.setPixel(x+3,y+5);
    if (waitcount > 11) myGLCD.setPixel(x+2,y+5);
    if (waitcount > 12) myGLCD.setPixel(x+1,y+4);
    if (waitcount > 13) myGLCD.setPixel(x  ,y+3);
    if (waitcount > 14) myGLCD.setPixel(x  ,y+2);
    if (waitcount > 15) myGLCD.setPixel(x+1,y+1);
    if (waitcount > 16) myGLCD.setPixel(x+2,y  );
  }
}

void get_bme280(void) {
    bme.takeForcedMeasurement();
    temp=bme.readTemperature();
    pres=pow(((95*0.0065)/(bme.readTemperature()+273.15)+1),5.257)*bme.readPressure()/100.0;
    humi=bme.readHumidity();
    draw_temp(temp);
}

void sleep12(unsigned int sleeptime) {
  draw_wait(WAIT_X0,WAIT_Y0,0);
//  print_field(0,4);
  if ( radiomode == radio_sleep ) {
    radio.stopListening();
    wipe_antenna(ANT_X0, ANT_Y0);
    radio.powerDown();
  }
  sleep4ms(sleeptime); 
  if ( radiomode == radio_sleep ) {
    radio.powerUp();
    radio.startListening();
    draw_antenna(ANT_X0, ANT_Y0);
  }
  //*****************
  // Put anything you want to run frequently here
  //*****************  
  cur_voltage = vcc.Read_Volts();
  get_bme280();
  draw_battery(BATT_X0,BATT_Y0,cur_voltage);
  //#################
  // END run frequently
  //#################
}

void loop(void) {
  
  n_update = network.update();
  if ( network.available() ) {
    sleepmode = sleep4;
    networkup = 0;
    if ((payload.flags & 0x01) == 0x01 ) {
      next_sleepmode = sleep1;
    } else {
      next_sleepmode = sleep2;
    }
    network.read(rxheader,&payload,sizeof(payload));
    if ( payload.sensor1 == 1 || payload.sensor1 == 2 || payload.sensor1 == 3 || 
         payload.sensor2 == 1 || payload.sensor2 == 2 || payload.sensor2 == 3 ||
         payload.sensor3 == 1 || payload.sensor3 == 2 || payload.sensor3 == 3 ||
         payload.sensor4 == 1 || payload.sensor4 == 2 || payload.sensor4 == 3 ) {
            bme.takeForcedMeasurement();
    }
    payload.value1 = action_loop(payload.sensor1, payload.value1);
    payload.value2 = action_loop(payload.sensor2, payload.value2);
    payload.value3 = action_loop(payload.sensor3, payload.value3);
    payload.value4 = action_loop(payload.sensor4, payload.value4);
    txheader.type=rxheader.type;
    network.write(txheader,&payload,sizeof(payload));    
  } 
  if ( n_update > 0 ) {
    sleepmode = sleep4;
    networkup = 0;
    draw_wait(WAIT_X0,WAIT_Y0,17);
  }
  delay(80);
  networkup++;    
  switch (sleepmode) {
    case sleep1:
      sleep12((unsigned int)(sleeptime1*1000)); 
      sleepmode = sleep3;
      next_sleepmode = sleep2;
      networkup = 0;    
    break;
    case sleep2:
      sleep12((unsigned int)(sleeptime2*1000)); 
      sleepmode = sleep3;
      next_sleepmode = sleep2;
      networkup = 0;    
    break;
    case sleep3:
      if ( networkup > 10*sleeptime3) sleepmode = next_sleepmode;    
    break;
    case sleep4:
      if ( networkup > 10*sleeptime4) sleepmode = next_sleepmode;        
    break;
  } 
}

