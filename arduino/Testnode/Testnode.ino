/*
A very minimalistic Node for Testing only.

!!!!! On Branch payload_v3 !!!!!!!

*/
// The CE Pin of the Radio module
#define RADIO_CE_PIN 10
// The CS Pin of the Radio module
#define RADIO_CSN_PIN 9
// 
#define RF24NODE 100
//****************************************************
#define RF24CHANNEL     91
// Delay between 2 transmission in ms
#define RF24SENDDELAY 50
// Delay between 2 transmission in ms
#define RF24RECEIVEDELAY 50
// Sleeptime in ms !! 
// (valid: 10.000 ... 3.600.000)
#define RF24SLEEPTIME   60000
// Max Number of Heartbeart Messages to send !!
#define RF24SENDLOOPCOUNT 10
// Max Number of Receiveloops !!
#define RF24RECEIVELOOPCOUNT 10
// number of empty loop after sending data
// valid: 0...9
#define RF24EMPTYLOOPCOUNT  0
// Voltage Faktor will be divided by 100 (Integer !!)!!!!
#define STATUSLED               3
#define STATUSLED_ON            HIGH
#define STATUSLED_OFF           LOW
#define RECEIVEDELAY            100
#define MSGTYPEND               0
#define MSGTYPHB1               51
#define MSGTYPORQ1              71
#define MSGTYPORP1              81

//-----------------------------------------------------
//*****************************************************
// ------ End of configuration part ------------

#include <avr/pgmspace.h>
#include <RF24.h>
#include <SPI.h>
#include <sleeplib.h>
#include <Vcc.h>
#include <EEPROM.h>
#include "zahlenformat.h"
#include "printf.h"

// ----- End of Includes ------------------------

Vcc vcc(1.0);

ISR(WDT_vect) { watchdogEvent(); }

float temp;

// Structure of our payload
struct payload_t {
 uint8_t     node_id;         
 uint8_t     type;        
 uint8_t     flags;       
 uint8_t     orderno;         
 uint8_t     reserved1;      
 uint8_t     reserved2;      
 uint8_t     reserved3;      
 uint8_t     reserved4;      
 uint32_t    data1;         
 uint32_t    data2;         
 uint32_t    data3;         
 uint32_t    data4;         
 uint32_t    data5;         
 uint32_t    data6;         
};
payload_t payload;    


float               cur_voltage;
uint16_t            loopcount;
uint16_t            receiveloopcount;
uint16_t            sendloopcount;
long int            sleep_kor_time;
uint32_t            last_send;
uint8_t             msg_id;


// nRF24L01(+) radio attached using Getting Started board 
// Usage: radio(CE_pin, CS_pin)
RF24 radio(RADIO_CE_PIN,RADIO_CSN_PIN);
uint8_t rx_address1[] = { 0x33, 0xcc, 0xcc, 0xcc, 0xcc};
uint8_t  tx_address[] = { 0xf0, 0xcc, 0xcc, 0xcc, 0xcc};


void get_sensordata(void) {
  temp=22.2;
}

//float action_loop(uint32_t data) {
//  uint8_t channel = getChannel(data);
//  float value = getValue_f(data);
//  float retval = value;
//    switch (channel) {
//      default:
//      // still empty
//    }
//    return calcTransportValue_f(channel,retval);
//}  

void setup(void) {
  SPI.begin();
  Serial.begin(115200);
  printf_begin();
  Serial.println(F("Thermometer"));
  radio.begin();
  radio.setChannel( 91 );
  radio.setDataRate( RF24_250KBPS );
//  radio.setPALevel( RF24_PA_MAX ) ;
//  radio.setPALevel( RF24_PA_HIGH ) ;
  radio.setPALevel( RF24_PA_LOW ) ;
//  radio.setPALevel( RF24_PA_MIN ) ;
  radio.setRetries(15, 15);
  radio.openWritingPipe(tx_address);
  radio.openReadingPipe(1,rx_address1);
  radio.setAutoAck(true);
  radio.enableDynamicPayloads();
  delay(100);
  radio.printDetails();
  loopcount = 0;
  last_send = 0;
  msg_id = 1;
}

  
void loop(void) {
  delay(10);  
  cur_voltage = vcc.Read_Volts();
  get_sensordata();
//  Clear the RX buffer !!!
//  radio.flush_rx();
  if ( loopcount > 100 ) {   
    payload.node_id= RF24NODE;  
    payload.type = MSGTYPHB1;
    payload.flags = 0;
    payload.orderno = 0;
    payload.data1 = calcTransportValue_f(101, cur_voltage);
    payload.data2 = calcTransportValue_f(1,temp);
    payload.data3 = 0;
    payload.data4 = 0;
    payload.data5 = 0;
    payload.data6 = 0;
    radio.stopListening();
    Serial.print(F("Now transmitting Heartbeat "));
    if (radio.write(&payload,sizeof(payload))) {
      Serial.println(F("OK "));
    } else {
      Serial.println(F("failed "));
    }
    radio.startListening();
    loopcount = 0;
  }
  while ( radio.available() ){
    radio.read( &payload, sizeof(payload) );
    Serial.print("Msg received N:");
    Serial.print(payload.node_id);
    Serial.print(" T:");
    Serial.print(payload.type);
    Serial.print(" F:");
    Serial.print(payload.flags);
    Serial.print(" O:");
    Serial.print(payload.orderno);
    Serial.print(" Data (C/V): ("); 
    Serial.print(getChannel(payload.data1)); 
    Serial.print("/"); 
    Serial.print(getValue_f(payload.data1)); 
    Serial.print(") ("); 
    Serial.print(getChannel(payload.data2)); 
    Serial.print("/"); 
    Serial.print(getValue_f(payload.data2));
    Serial.print(") ("); 
    Serial.print(getChannel(payload.data3)); 
    Serial.print("/"); 
    Serial.print(getValue_f(payload.data3));  
    Serial.print(") ("); 
    Serial.print(getChannel(payload.data4)); 
    Serial.print("/"); 
    Serial.print(getValue_f(payload.data4));
    Serial.print(") ("); 
    Serial.print(getChannel(payload.data5)); 
    Serial.print("/"); 
    Serial.print(getValue_f(payload.data5));
    Serial.print(") ("); 
    Serial.print(getChannel(payload.data6)); 
    Serial.print("/"); 
    Serial.print(getValue_f(payload.data6));
    Serial.println(")");
    if (payload.orderno > 0 ) {
      if ( payload.type == MSGTYPORQ1 ) { 
        payload.type = MSGTYPORP1;
        radio.stopListening();
        Serial.print(F("Sending back "));
        if (radio.write(&payload,sizeof(payload))) {
          Serial.println(F("OK "));
        } else {
          Serial.println(F("failed "));
        }
        radio.startListening();
      }
    }
  }
  delay(50);            
  loopcount++;              
}
