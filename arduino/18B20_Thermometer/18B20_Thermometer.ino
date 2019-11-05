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
#include <OneWire.h>
#include <DallasTemperature.h>
#include "printf.h"
#include <rf24hub.h>
#include <EEPROM.h>

const float VccCorrection = 1.0/1.0;  // Measured Vcc by multimeter divided by reported Vcc

Vcc vcc(VccCorrection);
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
 
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

ISR(WDT_vect) { watchdogEvent(); }

payload_t payload;

struct eeprom_data_t {
  configDataRf24_t configDataRf24;
  configNode_t configNode;
};
eeprom_data_t eeprom_data;

enum radiomode_t { radio_sleep, radio_listen } radiomode = radio_sleep;
enum sleepmode_t { sleep1, sleep2, sleep3, sleep4} sleepmode = sleep1, next_sleepmode = sleep2;

float       cur_voltage;
float       sensor1;
uint16_t    msg_id;
uint16_t    flags;

// Usage: radio(CE_pin, CS_pin)
RF24 radio(RADIO_CE_PIN,RADIO_CSN_PIN);

void setup(void) {
  int eeAddress = 0;  
  EEPROM.get(eeAddress, eeprom_data);
  msg_id = 1;
  flags = 0;
  SPI.begin();
  //****
  // put anything else to init here
  //****
  sensors.begin(); 
  Serial.begin(9600);
  printf_begin();
  //####
  // end aditional init
  //####
  radio.begin();
  radio.setChannel(eeprom_data.configDataRf24.rf24Channel);
  radio.setPALevel(RF24_PA_MAX);
//  radio.setPALevel(RF24_PA_MIN);
//  radio.setRetries(15,2);
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
Serial.print(eeprom_data.configDataRf24.address2GW[0],HEX);
Serial.print(eeprom_data.configDataRf24.address2GW[1],HEX);
Serial.print(eeprom_data.configDataRf24.address2GW[2],HEX);
Serial.print(eeprom_data.configDataRf24.address2GW[3],HEX);
Serial.println(eeprom_data.configDataRf24.address2GW[4],HEX);
Serial.print(eeprom_data.configDataRf24.address2Node[0],HEX);
Serial.print(eeprom_data.configDataRf24.address2Node[1],HEX);
Serial.print(eeprom_data.configDataRf24.address2Node[2],HEX);
Serial.print(eeprom_data.configDataRf24.address2Node[3],HEX);
Serial.println(eeprom_data.configDataRf24.address2Node[4],HEX);
  
  radio.openWritingPipe(eeprom_data.configDataRf24.address2GW);
  radio.openReadingPipe(0,eeprom_data.configDataRf24.address2GW);
  radio.openReadingPipe(1,eeprom_data.configDataRf24.address2Node);
  // Start the radio listening for data
  radio.startListening();
  delay(200);
  radio.printDetails();
  bool do_transmit = true;
// Init of Node
// --> later
  Serial.println("18B20 Thermometer");
}

float get_temp(void) {
  float retval;
  sensors.requestTemperatures(); // Send the command to get temperatures
  delay(500);
  retval=sensors.getTempCByIndex(0);
  Serial.print("18B20 Messung: ");
  Serial.print(retval);
  Serial.println(" °C");
  return retval;
}

void loop(void) {
  sensor1=calcTransportValue_f(1,get_temp());
  payload.network_id = eeprom_data.configNode.network_id;
  payload.node_id = eeprom_data.configNode.node_id;
  payload.msg_id = msg_id++;
  payload.flags = flags;
  payload.sensor1 = sensor1;
  payload.sensor2 = 0;
  radio.stopListening();   
  if (!radio.write( &payload, sizeof(payload) )){
       Serial.println(F("Transmition failed"));
  }
  radio.startListening();                  

  if ( radio.available() ) {
    radio.read(&payload,sizeof(payload));
  }
  radio.powerDown();
  delay(1000);
  radio.powerUp();
  
}
