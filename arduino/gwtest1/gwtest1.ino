/*
Testnode
*/
// Define a valid radiochannel here
#define RADIOCHANNEL 10
// Nodenumber: Has to be unique inside the network
#define NODE 431
#define NETWORK 123
// The CE Pin of the Radio module
#define RADIO_CE_PIN 10
// The CS Pin of the Radio module
#define RADIO_CSN_PIN 9
// The pin of the statusled
#define STATUSLED 3
#define STATUSLED_ON HIGH
#define STATUSLED_OFF LOW


// ------ End of configuration part ------------
#include <RF24.h>
#include <SPI.h>
#include <Vcc.h>
#include "printf.h"
#include "zahlenformat.h"

const float VccCorrection = 1.0/1.0;  // Measured Vcc by multimeter divided by reported Vcc
float       cur_voltage;
Vcc vcc(VccCorrection);

//ISR(WDT_vect) { watchdogEvent(); }



uint8_t rx_address1[] = { 0x34, 0x12, 0xcc, 0xcc, 0xcc};
uint8_t  tx_address[] = { 0x76, 0x98, 0xcc, 0xcc, 0xcc};

struct payload_t {
  uint16_t    network_id;
  uint16_t    node_id;
  uint16_t    msg_id;
  uint16_t    flags;
  uint32_t    sensor1;
  uint32_t    sensor2;
};

struct payload_t payload;

// nRF24L01(+) radio attached using Getting Started board 
// Usage: radio(CE_pin, CS_pin)
RF24 radio(RADIO_CE_PIN,RADIO_CSN_PIN);

void setup() {

  Serial.begin(9600);
  printf_begin();
  pinMode(STATUSLED, OUTPUT);     
  digitalWrite(STATUSLED,STATUSLED_ON); 
  SPI.begin();
  radio.begin();
  //****
  // put anything else to init here
  //****
  cur_voltage = vcc.Read_Volts();
  //####
  // end aditional init
  //####
  radio.setChannel(RADIOCHANNEL);
  radio.setDataRate( RF24_250KBPS );
  radio.setPALevel( RF24_PA_MAX ) ;
  radio.setRetries(15, 15);
  radio.openWritingPipe(tx_address);
  radio.openReadingPipe(1,rx_address1);
  delay(1000);
  digitalWrite(STATUSLED,STATUSLED_OFF); 
  radio.printDetails();
  payload.network_id = NETWORK;
  payload.node_id = NODE;
  payload.msg_id = 1;
  payload.flags = 0;
  payload.sensor1 = 0;
  payload.sensor2 = 0;
}

void loop() {
      radio.stopListening();                                
      Serial.print(F("Data sent msg_nr:"));
      Serial.print(payload.msg_id);
      if (radio.write( &payload, sizeof(payload) )) {
        Serial.println("+++++++++> ok"); 
      } else {
        Serial.println("---------> error"); 
      }
      radio.startListening();   
      payload.msg_id++;                        
    delay(1000);
}
