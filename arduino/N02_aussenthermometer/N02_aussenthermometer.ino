// Define a valid radiochannel here
#define RADIOCHANNEL 90
// This node: Use octal numbers starting with "0": "041" is child 4 of node 1
#define NODE 02
// The CE Pin of the Radio module
#define RADIO_CE_PIN 10
// The CS Pin of the Radio module
#define RADIO_CSN_PIN 9
// The pin of the statusled
#define STATUSLED 7
#define STATUSLED_ON HIGH
#define STATUSLED_OFF LOW
#define SLEEPTIME1 10
#define SLEEPTIME2 10
#define SLEEPTIME3 2
#define SLEEPTIME4 5

// ------ End of configuration part ------------

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <sleeplib.h>
#include <Vcc.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>


const float VccCorrection = 1.0/1.0;  // Measured Vcc by multimeter divided by reported Vcc

Vcc vcc(VccCorrection);
Adafruit_BME280 bme;

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
float               sleeptime1 = SLEEPTIME1;
// Time for the 2. to N. sleeploop
float               sleeptime2 = SLEEPTIME2;
// Time to sleep after wakeup with radio on
float               sleeptime3 = SLEEPTIME3;
// Time to keep the network up if it was busy
float               sleeptime4 = SLEEPTIME4;
boolean             init_finished = false;
unsigned int        networkup = 0;
float               temp;
//Some Var for restore after sleep of display
float               cur_voltage;
uint8_t             n_update = 0;

// Usage: radio(CE_pin, CS_pin)
RF24 radio(RADIO_CE_PIN,RADIO_CSN_PIN);

// Network uses that radio
RF24Network network(radio);


float action_loop(unsigned char channel, float value) {
  float retval = value;
    switch (channel) {
        case 1:
        //Temperature from BME280
        retval=bme.readTemperature();
        break;
        case 2:
        //Temperature from BME280
        retval=pow(((95*0.0065)/(bme.readTemperature()+273.15)+1),5.257)*bme.readPressure()/100.0;
        break;
        case 3:
        //Humidity from BME280
        retval=bme.readHumidity();
        break;
        case 101:
          // battery voltage => vcc.Read_Volts();
          retval=vcc.Read_Volts();
        break;
        case 111:
          // sleeptimer1
          sleeptime1=value;
        break;
        case 112:
          // sleeptimer2
          sleeptime2=value;
        break;
        case 113:
          // sleeptimer3
          sleeptime3=value;
        break;
        case 114:
          // sleeptimer4
          sleeptime4=value;
          break;
        case 115:
          // radio on (=1) or off (=0) when sleep
          if ( value > 0.5) radiomode=radio_listen; else radiomode=radio_sleep;
        break;
        case 116:
          // Voltage factor
          vcc.m_correction = value;
        break; 
        case 118:
        // init_finished (=1)
          init_finished = true; //( payload.value > 0.5);
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
  SPI.begin();
  //****
  // put anything else to init here
  //****
  bme.begin();
  //####
  // end aditional init
  //####
  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
//  radio.setRetries(15,2);
  network.begin(RADIOCHANNEL, NODE);
  radio.setDataRate(RF24_250KBPS);
  delay(200);
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

void sleep12(unsigned int sleeptime) {
  if ( radiomode == radio_sleep ) {
    radio.stopListening();
    radio.powerDown();
  }
  sleep4ms((unsigned int)(sleeptime)); 
  if ( radiomode == radio_sleep ) {
    radio.powerUp();
    radio.startListening();
  }
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
    bme.takeForcedMeasurement();
    payload.value1 = action_loop(payload.sensor1, payload.value1);
    payload.value2 = action_loop(payload.sensor2, payload.value2);
    payload.value3 = action_loop(payload.sensor3, payload.value3);
    payload.value4 = action_loop(payload.sensor4, payload.value4);
    txheader.type=rxheader.type;
    network.write(txheader,&payload,sizeof(payload));    
  }
  sleep4ms(100);
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