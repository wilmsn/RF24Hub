// The CE Pin of the Radio module
#define RADIO_CE_PIN 10
// The CS Pin of the Radio module
#define RADIO_CSN_PIN 9
#define ONE_WIRE_BUS 8

// ------ End of configuration part ------------

#include <RF24.h>
#include <SPI.h>
#include <sleeplib.h>
#include <Vcc.h>
#include <rf24hub.h>
#include <EEPROM.h>
#include <BMP280.h>

const float VccCorrection = 1.0/1.0;  // Measured Vcc by multimeter divided by reported Vcc

Vcc vcc(VccCorrection);
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
 
BMP280 bmp(0x76); 

ISR(WDT_vect) { watchdogEvent(); }

payload_type2_t payload;
header_t  header;

struct message_t {
  header_t  header;
  payload_type2_t payload;
};
message_t  message;

struct eeprom_data_t {
  configDataRf24_t configDataRf24;
  configNode_t configNode;
};
eeprom_data_t eeprom_data;

int write_count;

float       cur_voltage;
uint32_t    tv1, tv2, tv3, tv4;
uint16_t    msg_id;
uint16_t    flags;

// Usage: radio(CE_pin, CS_pin)
RF24 radio(RADIO_CE_PIN,RADIO_CSN_PIN);
//MyNetwork network(radio);

void setup(void) {
  int eeAddress = 0;  
  EEPROM.get(eeAddress, eeprom_data);
  msg_id = 1;
  flags = 0;
  SPI.begin();
  //****
  // put anything else to init here
  //****
  bmp.begin();
  //####
  // end aditional init
  //####
  radio.begin();
  radio.setChannel(eeprom_data.configDataRf24.rf24Channel);
  radio.setPALevel(RF24_PA_MAX);
//  radio.setPALevel(RF24_PA_MIN);
  radio.setRetries(15,15);
  switch ( eeprom_data.configDataRf24.rf24Speed ) {
    case 0:
      radio.setDataRate(RF24_250KBPS);
    break;   
    case 1:
      radio.setDataRate(RF24_1MBPS);
    break;   
    case 2:
      radio.setDataRate(RF24_2MBPS);
    break;   
  } 
  radio.openWritingPipe(eeprom_data.configDataRf24.address2GW);
  radio.openReadingPipe(0,eeprom_data.configDataRf24.address2GW);
  radio.openReadingPipe(1,eeprom_data.configDataRf24.address2Node);
  // Start the radio listening for data
  radio.startListening();
  delay(200);
  bool do_transmit = true;
// Init of Node
// --> later
}

void loop(void) {
  bmp.startSingleMeasure();
  tv1=calcTransportValue_f(1,bmp.readTemperature());
  tv2=calcTransportValue_f(2,bmp.readPressure());
  tv3=calcTransportValue_f(3,bmp.readPressureAtSealevel(91.0));
  tv4=calcTransportValue_f(101,vcc.Read_Volts());
  message.header.network_id = eeprom_data.configNode.network_id;
  message.header.node_id = eeprom_data.configNode.node_id;
  message.header.msg_id = msg_id++;
  message.header.flags = flags;
  message.payload.sensor1 = tv1;
  message.payload.sensor2 = tv2;
  message.payload.sensor3 = tv3;
  message.payload.sensor4 = tv4;
  radio.stopListening();
  write_count=0;
  while (write_count < 10) {  
    if (radio.write( &message, sizeof(message) )){
       write_count=write_count+11;
    }
  }
  radio.startListening();                  
// TODO
  if ( radio.available() ) {
    radio.read(&message,sizeof(message));
  }
  radio.powerDown();
  sleep4ms( eeprom_data.configNode.sleeptime * 1000 );
  radio.powerUp();
  
}
