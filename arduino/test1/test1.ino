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
#define STATUSLED LED_BUILTIN
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



//uint8_t rx_address1[] = { 0xcc, 0xcc, 0xcc, 0xcc, 0xcc};
//uint8_t  tx_address[] = { 0xcc, 0xcc, 0xcc, 0xcc, 0xcc};
uint8_t  address1[] = { 0xf0, 0xcc, 0xcc, 0xcc, 0xcc};
uint8_t  address2[] = { 0x33, 0xcc, 0xcc, 0xcc, 0xcc};
bool test=false;
int loopcount = 0;

struct payload_t {
  uint8_t     node_id;         
  uint8_t     msg_id;          
  uint8_t     msg_type;        
  uint8_t     msg_flags;       
  uint8_t     orderno;         
  uint8_t     network_id;      
  uint8_t     reserved1;      
  uint8_t     reserved2;      
  uint32_t    data1;         
  uint32_t    data2;         
  uint32_t    data3;         
  uint32_t    data4;         
  uint32_t    data5;         
  uint32_t    data6;         
};

struct payload_t payload, payload1;

uint8_t channel;
float value = 0;

// nRF24L01(+) radio attached using Getting Started board 
// Usage: radio(CE_pin, CS_pin)
RF24 radio(RADIO_CE_PIN,RADIO_CSN_PIN);

void setup() {

  Serial.begin(9600);
  printf_begin();
  SPI.begin();
  radio.begin();
  cur_voltage = vcc.Read_Volts();
  radio.setChannel(10);
  radio.setDataRate( RF24_1MBPS );
  radio.setPALevel( RF24_PA_MAX ) ;
  radio.setRetries(15, 15);
  radio.setAutoAck(true);
  radio.enableDynamicPayloads();
  radio.openWritingPipe(address1);
  radio.openReadingPipe(1,address2);
  delay(1000);
  radio.printDetails();
  payload.network_id = NETWORK;
  payload.node_id = NODE;
  payload.msg_id = 1;
  payload.msg_type = 51;
  payload.msg_flags = 0;
  payload.orderno = 0;
  payload.data1 = 0;
  payload.data2 = 0;
  payload.data3 = 0;
  payload.data4 = 0;
  payload.data5 = 0;
  payload.data6 = 0;
  loopcount = 0;
}

void loop() {
      
    while ( radio.available() ){
        radio.read( &payload1, sizeof(payload1) );
          Serial.println(">>Msg received"); 
    }
    if ( loopcount == 100 ) {
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
      loopcount=0;
    }
    delay(50);            
    loopcount++;            
}
