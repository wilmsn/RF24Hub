/*
Testnode
*/
// Define a valid radiochannel here
#define RADIOCHANNEL 90
// Nodenumber: Has to be unique inside the network
#define NODENUMBER 431
// The CE Pin of the Radio module
#define RADIO_CE_PIN 10
// The CS Pin of the Radio module
#define RADIO_CSN_PIN 9
// The pin of the statusled
#define STATUSLED 3
#define STATUSLED_ON HIGH
#define STATUSLED_OFF LOW
#define ONE_WIRE_BUS 8


// ------ End of configuration part ------------
#include <EEPROM.h>
#include <RF24.h>
#include <SPI.h>
#include <Vcc.h>
#include "printf.h"
#include "zahlenformat.h"
//****
// some includes for your sensor(s) here
//****
#include <OneWire.h>
#include <DallasTemperature.h>
//####
//end aditional includes
//####

const float VccCorrection = 1.0/1.0;  // Measured Vcc by multimeter divided by reported Vcc
float       cur_voltage;
Vcc vcc(VccCorrection);

//ISR(WDT_vect) { watchdogEvent(); }

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
 
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);


uint8_t addresses[][6] = {"aaaaa"};


int eeAddress = 0;
struct ee_data_t {
  uint16_t    network_id;
  uint16_t    node_id;

  
};
struct ee_data_t ee_data;

struct udp_data_t {
  uint16_t    network_id;
  uint16_t    node_id;
  uint16_t    msg_id;
  uint16_t    flags;
  uint32_t    sensor1;
  uint32_t    sensor2;
};

struct udp_data_t udp_s_data, udp_r_data;

// nRF24L01(+) radio attached using Getting Started board 
// Usage: radio(CE_pin, CS_pin)
RF24 radio(RADIO_CE_PIN,RADIO_CSN_PIN);

int loopcount = 1;



uint32_t action_loop(uint32_t sensorValue) {
  uint8_t channel = getSensor(sensorValue);
  uint32_t retval;
    switch (channel) {
      case 1: {
        // Temperature
        retval = calcTransportValue(1,get_temp());
       break; }
      case 21: {
        // LED
        uint16_t value=getValue_uint(sensorValue);
        if ( value == 1 ) {
          digitalWrite(STATUSLED,STATUSLED_ON); 
        } else {
          digitalWrite(STATUSLED,STATUSLED_OFF); 
        }
       break; }
      case 101:  
      // battery voltage
        cur_voltage = vcc.Read_Volts();
//        retval = uint16Value(cur_voltage);
        break;      
      case 111:
      // sleeptimer1
//        sleeptime1 = value;
        break;
      case 112:
      // sleeptimer2
//        sleeptime2 = value;
        break;
      case 113:
      // sleeptimer3
//        sleeptime3 = value;
        break;
      case 114:
      // sleeptimer4
//        sleeptime4 = value;
        break;
      case 115:
      // radio on (=1) or off (=0) when sleep
//        if ( value > 0.5) radiomode=radio_listen; else radiomode=radio_sleep;
        break;
      case 116:
      // Voltage devider
//        vcc.m_correction = value;
        break;
      case 118:
      // init_finished (=1)
//        init_finished = ( true );
        break;
    }  
    return retval;
}  

float get_temp(void) {
  float temp;
  sensors.requestTemperatures(); // Send the command to get temperatures
  temp=sensors.getTempCByIndex(0);
  return temp;
}

void setup() {
  EEPROM.get( eeAddress, ee_data );
  Serial.begin(115200);
  printf_begin();
  pinMode(STATUSLED, OUTPUT);     
  digitalWrite(STATUSLED,STATUSLED_ON); 
  SPI.begin();
  radio.begin();
  //****
  // put anything else to init here
  //****
  sensors.begin(); 
  get_temp();
  cur_voltage = vcc.Read_Volts();
  //####
  // end aditional init
  //####
  radio.setChannel(RADIOCHANNEL);
  //radio.setDataRate( RF24_250KBPS );
  radio.setPALevel( RF24_PA_MAX ) ;
  radio.setRetries(15, 15);
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1,addresses[0]);
  delay(1000);
  digitalWrite(STATUSLED,STATUSLED_OFF); 
  udp_s_data.msg_id=1;
  udp_s_data.network_id=ee_data.network_id;
  udp_s_data.node_id=ee_data.node_id;
  udp_s_data.sensor1=0;
  udp_s_data.sensor2=0;
  loopcount=0;
  radio.printDetails();
/*  
  Serial.print("Temperatur: "); 
  Serial.println(get_temp());
  uint32_t tv=calcTransportValue(1,get_temp());
  Serial.print("Transport Value: "); 
  Serial.println(tv, BIN);
  
  Serial.print("Sensor: "); 
  Serial.print(getSensor(tv)); 
  Serial.print(" Wert: "); 
  Serial.println(getValue(tv)); 
  Serial.println("----------------------------------"); 
  Serial.println("Test Sensor 44 Value 123456");
  tv=calcTransportValue(44,123456.0);
  Serial.print("Transport Value: "); 
  Serial.println(tv, BIN);
  
  Serial.print("Sensor: "); 
  Serial.print(getSensor(tv)); 
  Serial.print(" Wert: "); 
  Serial.println(getValue(tv)); 
  Serial.println("----------------------------------"); 
*/  
}

void loop() {
    if (loopcount > 1000) {
      udp_s_data.sensor1=calcTransportValue(1,get_temp());
      udp_s_data.sensor2=calcTransportValue(101,vcc.Read_Volts());
      radio.stopListening();                                
      Serial.print(F("Data sent msg_nr:"));
      Serial.print(udp_s_data.msg_id);
      if (radio.write( &udp_s_data, sizeof(udp_s_data) )) {
        Serial.println("+++++++++> ok"); 
      } else {
        Serial.println("---------> error"); 
      }
      udp_s_data.msg_id++;
      radio.startListening();                           
      loopcount=0;
    }
    if ( radio.available() ){                           
      radio.read( &udp_r_data, sizeof(udp_r_data) );
      printf(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
      printf("Network_number: %u \n",udp_r_data.network_id);
      printf("Node_number: %u \n",udp_r_data.node_id);
      printf("Msg_number: %u \n",udp_r_data.msg_id);
/*      printf("Sensor1_id: %u \n",udp_r_data.sensor1);
      printf("Sensor1_value: %f \n",udp_r_data.value1);
      printf("Sensor2_id: %u \n",udp_r_data.sensor2_id);
      printf("Sensor2_value: %f \n",udp_r_data.value2); */
      printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
      if ( udp_r_data.sensor1 > 0 ) action_loop(udp_r_data.sensor1);
      if ( udp_r_data.sensor2 > 0 ) action_loop(udp_r_data.sensor2);
    }
    delay(10);
    loopcount++;
}
