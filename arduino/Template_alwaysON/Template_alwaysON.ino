// How long do we try to init this node 100000(in ms) = 100 sec. 
#define MAXINIT 100000
// The CE Pin of the Radio module
#define RADIO_CE_PIN 10
// The CS Pin of the Radio module
#define RADIO_CSN_PIN 9
// The pin of the statusled
#define STATUSLED 3
#define STATUSLED_ON LOW
#define STATUSLED_OFF HIGH

// ------ End of configuration part ------------

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <sleeplib.h>
#include <Vcc.h>
#include <EEPROM.h>

const float VccCorrection = 1.0/1.0;  // Measured Vcc by multimeter divided by reported Vcc

Vcc vcc(VccCorrection);

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

struct eeprom_t {
   uint16_t node;
   uint8_t  channel;
   uint16_t sleeptime1;
   uint16_t sleeptime2;
   uint16_t sleeptime3;
   uint16_t sleeptime4;
};
eeprom_t eeprom;

RF24NetworkHeader rxheader;
RF24NetworkHeader txheader(0);

float               cur_voltage;
boolean             init_finished = false;

// Usage: radio(CE_pin, CS_pin)
RF24 radio(RADIO_CE_PIN,RADIO_CSN_PIN);

// Network uses that radio
RF24Network network(radio);


float action_loop(unsigned char channel, float value) {
  float retval = value;
    switch (channel) {
        case 1:
        break;    
        case 21:
        break;
        case 101:
          // battery voltage => vcc.Read_Volts();
          retval=vcc.Read_Volts();
        break;
        case 111:
          // sleeptimer1 ==> NOT USED
        break;
        case 112:
          // sleeptimer2 ==> NOT USED
        break;
        case 113:
          // sleeptimer3 ==> NOT USED
        break;
        case 114:
          // sleeptimer4 ==> NOT USED
          break;
        case 115:
          // radio on (=1) or off (=0) when sleep ==> NOT USED
        break;
        case 116:
          // Voltage factor
          vcc.m_correction = value;
        break; 
        case 118:
          // init_finished (value=1)
          if ( value > 0.5 ) init_finished = true; 
        break;
//        default:
        // Default: just send the paket back - no action here  
      }
    return retval;
}  

void setup(void) {
  unsigned long last_send=millis();
  unsigned long init_start=millis();
  pinMode(STATUSLED, OUTPUT);
  digitalWrite(STATUSLED,STATUSLED_ON);
  EEPROM.get(0, eeprom);
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
  network.begin(eeprom.channel, eeprom.node);
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
      if ( payload.sensor1 > 0 ) payload.value1 = action_loop(payload.sensor1, payload.value1);
      if ( payload.sensor2 > 0 ) payload.value2 = action_loop(payload.sensor2, payload.value2);
      if ( payload.sensor3 > 0 ) payload.value3 = action_loop(payload.sensor3, payload.value3);
      if ( payload.sensor4 > 0 ) payload.value4 = action_loop(payload.sensor4, payload.value4);
      last_send = millis();
      txheader.type=1;
      network.write(txheader,&payload,sizeof(payload));
    }
    if ( millis() - init_start > MAXINIT ) init_finished = true;
  }
  digitalWrite(STATUSLED,STATUSLED_OFF); 
  delay(100);
}

void loop(void) {
  network.update();
  if ( network.available() ) {
    network.read(rxheader,&payload,sizeof(payload));
    if ( payload.sensor1 > 0 ) payload.value1 = action_loop(payload.sensor1, payload.value1);
    if ( payload.sensor2 > 0 ) payload.value2 = action_loop(payload.sensor2, payload.value2);
    if ( payload.sensor3 > 0 ) payload.value3 = action_loop(payload.sensor3, payload.value3);
    if ( payload.sensor4 > 0 ) payload.value4 = action_loop(payload.sensor4, payload.value4);
    txheader.type=rxheader.type;
    network.write(txheader,&payload,sizeof(payload));    
  }
  delay(100);
}
