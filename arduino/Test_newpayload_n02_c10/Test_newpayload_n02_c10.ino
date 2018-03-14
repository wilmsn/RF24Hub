// Define a valid radiochannel here
#define RADIOCHANNEL 10
// This node: Use octal numbers starting with "0": "041" is child 4 of node 1
#define NODE 02
// The CE Pin of the Radio module
#define RADIO_CE_PIN 10
// The CS Pin of the Radio module
#define RADIO_CSN_PIN 9
// The pin of the statusled
#define SLEEP delay
//#define SLEEP sleep4ms
//define some sleeptime as default values
#define SLEEPTIME1 10
#define SLEEPTIME2 10
#define SLEEPTIME3 2
#define SLEEPTIME4 5
#define STATUSLED LED_BUILTIN
#define STATUSLED_ON HIGH
#define STATUSLED_OFF LOW

// ------ End of configuration part ------------

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <sleeplib.h>
#include <Vcc.h>
#include "printf.h"
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include "Adafruit_Si7021.h"


const float VccCorrection = 1.0/1.0;  // Measured Vcc by multimeter divided by reported Vcc

Vcc vcc(VccCorrection);
Adafruit_BMP085 bmp;
Adafruit_Si7021 si7021 = Adafruit_Si7021();

ISR(WDT_vect) { watchdogEvent(); }

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

RF24NetworkHeader rxheader;
RF24NetworkHeader txheader(0);
// all sleeptime* values in seconds 
// Time for the fist sleep after an activity of this node
float       sleeptime1 = SLEEPTIME1;
// Time for the 2. to N. sleeploop
float       sleeptime2 = SLEEPTIME2;
// Time to sleep after wakeup with radio on
float       sleeptime3 = SLEEPTIME3;
// Time to keep the network up if it was busy
float       sleeptime4 = SLEEPTIME4;
boolean     init_finished = false;
boolean     init_transmit = true;
float       networkuptime = 0;
uint16_t    orderno_p1, orderno_p2;

// Usage: radio(CE_pin, CS_pin)
RF24 radio(RADIO_CE_PIN,RADIO_CSN_PIN);

// Network uses that radio
RF24Network network(radio);


float action_loop(unsigned char sensor, float value) {
  float retval;
      switch (sensor) {
        case 1:
        //Temperature from BMP085
        retval = bmp.readTemperature();
        break;
        case 2:
        //Temperature from BMP085
        retval = pow(((95*0.0065)/(bmp.readTemperature()+273.15)+1),5.257)*bmp.readPressure()/100.0;
        break;
        case 3:
        //Humidity from SI7021
        retval = si7021.readHumidity();
        break;
        case 4:
        //Temperature from SI7021
        retval = si7021.readTemperature();
        break;
        case 21:
        //****
        // insert here: action = payload.value
        break;
        case 101:
          // battery voltage => vcc.Read_Volts();
          retval = vcc.Read_Volts();
        break;
        case 111:
          // sleeptimer1
          sleeptime1=value;
          retval=value;
          Serial.println("111 detected");
        break;
        case 112:
          // sleeptimer2
          sleeptime2=value;
          retval=value;
         Serial.println("112 detected");
        break;
        case 113:
          // sleeptimer3
          sleeptime3=value;
          retval=value;
          Serial.println("113 detected");
        break;
        case 114:
          // sleeptimer4
          sleeptime4=value;
          retval=value;
          Serial.println("114 detected");
          break;
        case 115:
          // radio on (=1) or off (=0) when sleep
          Serial.print("115 detected ");
          if ( value > 0.5) {
            radiomode=radio_listen; 
            Serial.println("Radio always on");
          } else {
            radiomode=radio_sleep;
            Serial.println("Radio sleeps");
          }
          retval=value;
        break;
        case 116:
          // Voltage factor
          vcc.m_correction = value;
          retval=value;
          Serial.println("116 detected");
        break; 
        case 118:
        // init_finished (=1)
          retval=1;
          init_finished = (1 == 1); //( payload.value > 0.5);
          Serial.println("118 detected");
          break;
//        default:
        // Default: just send the paket back - no action here  
    }
    return retval;
}

void setup(void) {
  unsigned long last_send=millis();
  pinMode(STATUSLED, OUTPUT);
  digitalWrite(STATUSLED,STATUSLED_ON);
  Serial.begin(115200);
  Serial.println("Programstart Testnode02");
  printf_begin();
  SPI.begin();
  //****
  // put anything else to init here
  //****
  bmp.begin();
  si7021.begin();
  //####
  // end aditional init
  //####
  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
//  radio.setRetries(15,2);
  network.begin(RADIOCHANNEL, NODE);
//  radio.setDataRate(RF24_250KBPS);
  delay(200);
  radio.printDetails();
  radiomode=radio_listen;
  init_finished = false;
  // initialisation beginns
  bool do_transmit = true;
  while ( ! init_finished ) {
    if ( (last_send + 1000 < millis()) && do_transmit ) {
      Serial.println("Testnode02 send 119");
      txheader.type=119;
      payload.orderno=0;
      payload.sensor1=119;
      payload.value1=0;
      network.write(txheader,&payload,sizeof(payload));
      last_send = millis();
    }
    network.update();
    if ( last_send + 10000 < millis()) { do_transmit = true; }
    if ( network.available() ) {
      do_transmit = false;
      network.read(rxheader,&payload,sizeof(payload));
      Serial.print("Testnode02 received ");
      Serial.print(rxheader.type);
      Serial.println(" ");
//      Serial.println(payload.value);
      init_transmit=false;
//      action_loop();
      payload.value1=action_loop(payload.sensor1, payload.value1);
      payload.value2=action_loop(payload.sensor2, payload.value2);
      payload.value3=action_loop(payload.sensor3, payload.value3);
      payload.value4=action_loop(payload.sensor4, payload.value4);
      txheader.type=60;
      network.write(txheader,&payload,sizeof(payload));
      last_send = millis();
    }
  }
  delay(500);
  digitalWrite(STATUSLED,STATUSLED_OFF);
  Serial.println("Init Testnode01 finished");
  sleepmode=sleep4;
  networkuptime = 0;    
  Serial.print("Humidity:    "); Serial.print(si7021.readHumidity(), 2);
  Serial.print("\tTemperature: "); Serial.println(si7021.readTemperature(), 2);
}
 
void sleep12(unsigned int sleeptime) {
  if ( radiomode == radio_sleep ) {
    radio.stopListening();
    radio.powerDown();
  }
  SLEEP((unsigned int)(sleeptime)); 
  if ( radiomode == radio_sleep ) {
    radio.powerUp();
    radio.startListening();
  }
}

void loop(void) {
  uint8_t n_update = 0;
  digitalWrite(STATUSLED,STATUSLED_ON);
  Serial.print("Testnode 02 active: ");
  Serial.println(networkuptime);
  n_update = network.update();
  if ( radio.rxFifoFull() ) { Serial.println("#####> rxFiFoFull !!!"); }
  if ( radio.failureDetected ) { Serial.println("#####> Failure detected !!!"); }
  
  Serial.print("Network update:");
  Serial.println(n_update);
  if ( network.available() ) {
    sleepmode = sleep4;
    networkuptime = 0;
    if ((payload.flags & 0x01) == 0x01 ) {
      next_sleepmode = sleep1;
    } else {
      next_sleepmode = sleep2;
    }
    network.read(rxheader,&payload,sizeof(payload));
    Serial.print("Testnode02 received: ");
    Serial.print(rxheader.type);
    Serial.print(" ");
    Serial.print(payload.sensor1);
    Serial.print(" ");
    Serial.print(payload.value1);
    Serial.print(" ");
    Serial.print(payload.sensor2);
    Serial.print(" ");
    Serial.print(payload.value2);
    Serial.print(" ");
    Serial.print(payload.sensor3);
    Serial.print(" ");
    Serial.print(payload.value3);
    Serial.print(" ");
    Serial.print(payload.sensor4);
    Serial.print(" ");
    Serial.println(payload.value4);
    payload.value1 = action_loop(payload.sensor1, payload.value1);
    payload.value2 = action_loop(payload.sensor2, payload.value2);
    payload.value3 = action_loop(payload.sensor3, payload.value3);
    payload.value4 = action_loop(payload.sensor4, payload.value4);
    txheader.type=rxheader.type;
    network.write(txheader,&payload,sizeof(payload));    
  } else if ( n_update > 0 ) {
    Serial.print("Durchgangsverkehr Channel: ");
    Serial.println(n_update);
    sleepmode = sleep4;
    next_sleepmode = sleep2;
    networkuptime = 0;
  }
  if ( networkuptime < 0.1 ) {
  //*****************
  // Put anything you want to run frequently here
  //*****************  
  //#################
  // END run frequently
  //#################
  }
  SLEEP(100);
  networkuptime += 0.1;    
  digitalWrite(STATUSLED,STATUSLED_OFF);
  switch (sleepmode) {
    case sleep1:
      Serial.println("Enter Sleepmode 1");
      sleep12((unsigned int)(sleeptime1*1000)); 
      sleepmode = sleep3;
      next_sleepmode = sleep2;
      networkuptime = 0;    
    break;
    case sleep2:
      Serial.println("Enter Sleepmode 2");
      sleep12((unsigned int)(sleeptime2*1000)); 
      sleepmode = sleep3;
      next_sleepmode = sleep2;
      networkuptime = 0;    
    break;
    case sleep3:
      Serial.println("Enter Sleepmode 3");
      if ( networkuptime > sleeptime3) sleepmode = next_sleepmode;    
    break;
    case sleep4:
      Serial.println("Enter Sleepmode 4");
      if ( networkuptime > sleeptime4) sleepmode = next_sleepmode;        
    break;
  } 
}
