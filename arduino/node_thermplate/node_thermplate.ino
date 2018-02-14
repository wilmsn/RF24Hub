/*
A thermometer for inside.
V3: Upgrade to Lowpower Library; display of a battery symbol

*/
// Define a valid radiochannel here
#define RADIOCHANNEL 10
// This node: Use octal numbers starting with "0": "041" is child 4 of node 1
#define NODE 02
// The CE Pin of the Radio module
#define RADIO_CE_PIN 10
// The CS Pin of the Radio module
#define RADIO_CSN_PIN 9
// The pin of the statusled
#define STATUSLED 3
#define STATUSLED_ON LOW
#define STATUSLED_OFF HIGH
#define ONE_WIRE_BUS 8

// ------ End of configuration part ------------

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <sleeplib.h>
// use Arduino_Vcc from github.com/wilmsn
// there are some modifications to run with my nodes
#include <Vcc.h>

//****
// some includes for your sensor(s) here
//****

ISR(WDT_vect) { watchdogEvent(); }

Vcc vcc(1.0);

//####
//end aditional includes
//####

// Structure of our payload
struct payload_t {
  uint16_t   orderno;
  char    value[10];
};

payload_t payload;    

enum radiomode_t { radio_sleep, radio_listen } radiomode;
enum sleepmode_t { sleep1, sleep2, sleep3, sleep4} sleepmode = sleep1, next_sleepmode = sleep2;


RF24NetworkHeader rxheader;
RF24NetworkHeader txheader(0);
// all sleeptime* values in seconds 
// Time for the fist sleep after an activity of this node
unsigned int sleeptime1 = 60;
// Time for the 2. to N. sleeploop
unsigned int sleeptime2 = 10;
// Time to sleep after wakeup with radio on
unsigned int sleeptime3 = 1;
// Time to keep the network up if it was busy
unsigned int sleeptime4 = 5;
unsigned int init_loop_counter = 0;
unsigned int loop_counter = 0;
boolean init_finished = false;
boolean init_transmit = true;
float networkuptime = 0;
float voltagefactor = 1;
//Some Var for restore after sleep of display


// nRF24L01(+) radio attached using Getting Started board 
// Usage: radio(CE_pin, CS_pin)
RF24 radio(RADIO_CE_PIN,RADIO_CSN_PIN);

// Network uses that radio
RF24Network network(radio);


void action_loop(void) {
    txheader.type=rxheader.type;
    switch (rxheader.type) {
      //Example for reading a sensor
      // You can use any "rxheader.type" from 1 to 100 for your sensors and actors
      // 101 to 118 is reserved to control the node
      case 1: {
        // if a sensor delivers float (eg. Temperature)
        //dtostrf(get_temp(),7,2,payload.value);
        //Remember: payload.value is an array of char !!!
      break; }
      //Example for setting an actor
      case 21:
        // Set field 1
        // myactor=atof(payload.value);
      break;
        case 101:
          // battery voltage
          dtostrf(vcc.Read_Volts(),7,2,payload.value);
        break;
        case 111:
          // sleeptimer1
          sleeptime1=atoi(payload.value);
        break;
        case 112:
          // sleeptimer2
          sleeptime2=atoi(payload.value);
        break;
        case 113:
          // sleeptimer3
          sleeptime3=atoi(payload.value);
        break;
        case 114:
          // sleeptimer4
          sleeptime4=atoi(payload.value);
          break;
        case 115:
          // radio on (=1) or off (=0) when sleep
          if ( atof(payload.value) > 0.5) radiomode=radio_listen; else radiomode=radio_sleep;
        break;
        case 116:
         // Voltage devider
          vcc.m_correction = atof(payload.value);
        break; 
        case 118:
        // init_finished (=1)
          init_finished = (1 == 1); //( payload.value > 0.5);
          break;
//        default:
        // Default: just send the paket back - no action here  
    }  
    network.write(txheader,&payload,sizeof(payload));
}  

void setup(void) {
  pinMode(STATUSLED, OUTPUT);     
  digitalWrite(STATUSLED,STATUSLED_ON); 
  SPI.begin();
  radio.begin();
  //****
  // put anything else to init here
  //****
  //####
  // end aditional init
  //####
  network.begin(RADIOCHANNEL, NODE);
  radio.setDataRate(RF24_1MBPS);
//  radio.setRetries(15,2); // delay 4000us, 2 retries
  // initialisation beginns
  while ( ! init_finished ) {
    if ( init_transmit && init_loop_counter < 1 ) {
      txheader.type=119;
      payload.orderno=0;
      sprintf(payload.value,"0");
      network.write(txheader,&payload,sizeof(payload));
      init_loop_counter=10;
    }
    network.update();
    if ( network.available() ) {
      network.read(rxheader,&payload,sizeof(payload));
      init_transmit=false;
      init_loop_counter=0;
      action_loop(); 
    }
    delay(30);
    init_loop_counter--;
    //just in case of initialisation is interrupted
    if (init_loop_counter < -1000) init_transmit=true;
  }
  digitalWrite(STATUSLED,STATUSLED_OFF); 
}

void loop(void) {
  network.update();
  if ( network.available() ) {
    sleepmode = sleep4;
    networkuptime = 0;
    network.read(rxheader,&payload,sizeof(payload));
    action_loop();
  }
  sleep4ms(200);
  networkuptime += 0.2;    
  switch (sleepmode) {
    case sleep1:
      if ( radiomode == radio_sleep ) {
        radio.stopListening();
        radio.powerDown();
      }
      sleep4ms((unsigned int)(sleeptime1*1000)); 
      sleepmode = sleep3;
      next_sleepmode = sleep2;
      networkuptime = 0;    
      if ( radiomode == radio_sleep ) {
        radio.powerUp();
        radio.startListening();
      }
    break;
    case sleep2:
      if ( radiomode == radio_sleep ) {
        radio.stopListening();
        radio.powerDown();
      }
      sleep4ms((unsigned int)(sleeptime2*1000)); 
      sleepmode = sleep3;
      next_sleepmode = sleep2;
      networkuptime = 0;    
      if ( radiomode == radio_sleep ) {
        radio.powerUp();
        radio.startListening();
      }
    break;
    case sleep3:
      if ( networkuptime > sleeptime3) sleepmode = next_sleepmode;    
    break;
    case sleep4:
      if ( networkuptime > sleeptime3) sleepmode = sleep1;        
    break;
  } 
}


