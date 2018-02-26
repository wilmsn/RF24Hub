/*

A node template

*/
// Define a valid radiochannel here
#define RADIOCHANNEL 10
// This node: Use octal numbers starting with "0": "041" is child 4 of node 1
#define NODE 04
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
#include <Vcc.h>

ISR(WDT_vect) { watchdogEvent(); }
Vcc vcc(1.0);

//****
// some includes for your sensor(s) here
//****

//####
//end aditional includes
//####

// Structure of our payload
struct payload_t {
  uint16_t   orderno;
  char       value[10];
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
unsigned int sleeptime2 = 60;
// Time to sleep after wakeup with radio on
unsigned int sleeptime3 = 1;
// Time to keep the network up if it was busy
unsigned int sleeptime4 = 5;
boolean init_finished = false;
boolean init_transmit = true;
float networkuptime = 0;
float temp;
float voltagefactor = 1;
float cur_voltage;
String valuestr="";


// nRF24L01(+) radio attached using Getting Started board 
// Usage: radio(CE_pin, CS_pin)
RF24 radio(RADIO_CE_PIN,RADIO_CSN_PIN);

// Network uses that radio
RF24Network network(radio);


void action_loop(void) {
    txheader.type=rxheader.type;
    switch (rxheader.type) {
      case 1: {
        // Temperature
//        dtostrf(get_temp(),7,2,payload.value);
       break; }
      case 21:
        // Set field 1
//        field1_val=atof(payload.value);
//        print_field(field1_val,1);
       break;
       case 101:  
      // battery voltage
        float ubatt;
        ubatt = vcc.Read_Volts();
        dtostrf(ubatt,7,2,payload.value);
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
          init_finished = true; //( payload.value > 0.5);
          break;
//        default:
        // Default: just send the paket back - no action here  
    }  
    network.write(txheader,&payload,sizeof(payload));
}  

void setup(void) {
  unsigned long last_send=millis();
  pinMode(STATUSLED, OUTPUT);
  digitalWrite(STATUSLED,STATUSLED_ON);
  SPI.begin();
  //****
  // put anything else to init here
  //****

  //####
  // end aditional init
  //####
  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
//  radio.setRetries(15,2);
  network.begin(RADIOCHANNEL, NODE);
//  radio.setDataRate(RF24_250KBPS);
  delay(200);
  // initialisation beginns
  bool do_transmit = true;
  while ( ! init_finished ) {
    if ( (last_send + 1000 < millis()) && do_transmit ) {
      txheader.type=119;
      payload.orderno=0;
      network.write(txheader,&payload,sizeof(payload));
      last_send = millis();
    }
    network.update();
    if ( last_send + 10000 < millis()) { do_transmit = true; }
    if ( network.available() ) {
      do_transmit = false;
      network.read(rxheader,&payload,sizeof(payload));
      init_transmit=false;
      action_loop();
      last_send = millis();
    }
  }
  digitalWrite(STATUSLED,STATUSLED_OFF);
  sleepmode=sleep4;
  networkuptime = 0;    
  display_down=false;
}

void sleep12(unsigned int sleeptime) {
  if ( radiomode == radio_sleep ) {
    radio.stopListening();
    wipe_antenna(ANT_X0, ANT_Y0);
    radio.powerDown();
  }
  sleep4ms((unsigned int)(sleeptime)); 
  if ( radiomode == radio_sleep ) {
    radio.powerUp();
    draw_antenna(ANT_X0, ANT_Y0);
    radio.startListening();
  }
}

void loop(void) {
  uint8_t n_update = 0;
  n_update = network.update();
  if ( n_update > 0 ) {
    sleepmode = sleep4;
    networkuptime = 0;
  }
  if ( network.available() ) {
    sleepmode = sleep4;
    networkuptime = 0;
    network.read(rxheader,&payload,sizeof(payload));
    Serial.print("Testnode02 received");
    Serial.println(rxheader.type);
    action_loop();
  }
  if ( networkuptime < 0.1 ) {
  //*****************
  // Put anything you want to run frequently here
  //*****************  

  //#################
  // END run frequently
  //#################
  }
  sleep4ms(100);
  networkuptime += 0.1;    
  switch (sleepmode) {
    case sleep1:
      sleep12((unsigned int)(sleeptime1*1000)); 
      sleepmode = sleep3;
      next_sleepmode = sleep2;
      networkuptime = 0;    
    break;
    case sleep2:
      sleep12((unsigned int)(sleeptime2*1000)); 
      sleepmode = sleep3;
      next_sleepmode = sleep2;
      networkuptime = 0;    
    break;
    case sleep3:
      if ( networkuptime > sleeptime3) sleepmode = next_sleepmode;    
    break;
    case sleep4:
      if ( networkuptime > sleeptime4) sleepmode = sleep1;        
    break;
  } 
}


