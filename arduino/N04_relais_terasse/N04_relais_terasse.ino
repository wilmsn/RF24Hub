// Define a valid radiochannel here
#define RADIOCHANNEL 90
// This node: Use octal numbers starting with "0": "041" is child 4 of node 1
#define NODE 04
// Sleeptime during the loop in ms -> if 0 ATMega always busy
#define RADIO_CE_PIN 10
// The CS Pin of the Radio module
#define RADIO_CSN_PIN 9
//define some sleeptime as default values
#define SLEEPTIME1 1
#define SLEEPTIME2 1
#define SLEEPTIME3 5
#define SLEEPTIME4 5
// The pin of the statusled
//#define STATUSLED A2
#define STATUSLED A2
#define STATUSLED_ON HIGH
#define STATUSLED_OFF LOW
// The pin of the relais
#define RELAIS1 4
#define RELAIS2 5
#define RELAIS_ON LOW
#define RELAIS_OFF HIGH

// The outputpin for batterycontrol for the voltagedivider
#define VMESS_OUT 5
// The inputpin for batterycontrol
#define VMESS_IN A0
// Sleeptime when network is busy

// ------ End of configuration part ------------

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <sleeplib.h>
#include <Vcc.h>

ISR(WDT_vect) { watchdogEvent(); }
const float VccCorrection = 1.0/1.0;  // Measured Vcc by multimeter divided by reported Vcc
Vcc vcc(VccCorrection);

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

// Usage: radio(CE_pin, CS_pin)
RF24 radio(RADIO_CE_PIN,RADIO_CSN_PIN);

// Network uses that radio
RF24Network network(radio);

float action_loop(unsigned char channel, float value) {
  float retval = value;
    switch (channel) {
      case 1:
        //****
        // insert here: payload.value=[result from sensor]
       break;
      case 21:
        if ( value > 0.5 ) {
          digitalWrite(RELAIS1, RELAIS_ON);
        } else {
          digitalWrite(RELAIS1, RELAIS_OFF);
        }
       break;
      case 22:
        if ( value > 0.5 ) {
          digitalWrite(RELAIS2, RELAIS_ON);
        } else {
          digitalWrite(RELAIS2, RELAIS_OFF);
        }
       break;
      case 31:
        //****
        // insert here: action = payload.value
        // Switch the StatusLED ON or OFF
        if ( value > 0.5 ) {
          digitalWrite(STATUSLED,STATUSLED_ON);
        } else {
          digitalWrite(STATUSLED,STATUSLED_OFF);
        }
       break;
      case 101:
      // battery voltage
        retval = cur_voltage;
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
        if ( value > 0.5) radiomode = radio_listen; else radiomode = radio_sleep;
        break;
      case 116:
      // Voltage devider
        vcc.m_correction = value;
        break;
      case 118:
      // init_finished (=1)
        init_finished = ( value > 0.5);
        break;
//      default:
      // Default: just send the paket back - no action here  
    }  
    return retval;
}  

void setup(void) {
  unsigned long last_send=millis();
  pinMode(STATUSLED, OUTPUT);
  pinMode(RELAIS1, OUTPUT);
  pinMode(RELAIS2, OUTPUT);
  digitalWrite(STATUSLED,STATUSLED_ON);
  digitalWrite(RELAIS1,RELAIS_ON);
  digitalWrite(RELAIS2,RELAIS_OFF);
  SPI.begin();
  radio.begin();
  cur_voltage = vcc.Read_Volts();
  network.begin(RADIOCHANNEL, NODE);
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MAX);
  // initialisation beginns
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
  digitalWrite(STATUSLED,STATUSLED_OFF);
  digitalWrite(RELAIS1,RELAIS_OFF);
}

void sleep12(float sleeptime) {
  if ( radiomode == radio_sleep ) {
    radio.stopListening();
    radio.powerDown();
  }
  sleep4s(sleeptime); 
  if ( radiomode == radio_sleep ) {
    radio.powerUp();
    radio.startListening();
  }
  //*****************
  // Put anything you want to run frequently here
  //*****************  
  cur_voltage = vcc.Read_Volts();
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
  }
  delay(80);
  networkup++;    
  switch (sleepmode) {
    case sleep1:
      sleep12(sleeptime1); 
      sleepmode = sleep3;
      next_sleepmode = sleep2;
      networkup = 0;    
    break;
    case sleep2:
      sleep12(sleeptime2); 
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
