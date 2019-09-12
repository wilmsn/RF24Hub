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

// Structure of our payload
// Maximum is 32 byte !!
struct payload_t {
  uint16_t  nodeno;       // the nodenumber
  uint16_t  msgid;        // an internal message ID
  uint16_t  flags;        // Flags for internal use
  uint8_t   sensor1;      // internal address of sensor1
  uint8_t   sensor2;      // internal address of sensor2
  uint8_t   sensor3;      // internal address of sensor3
  uint8_t   sensor4;      // internal address of sensor4
  float     value1;       // value of sensor1
  float     value2;       // value of sensor2
  float     value3;       // value of sensor3
  float     value4;       // value of sensor4
};

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
  uint8_t     sensor1_id;
  uint8_t     sensor2_id;
  float       value1;
  float       value2;
};

struct udp_data_t udp_s_data, udp_r_data;

// nRF24L01(+) radio attached using Getting Started board 
// Usage: radio(CE_pin, CS_pin)
RF24 radio(RADIO_CE_PIN,RADIO_CSN_PIN);

int loopcount = 1;

float action_loop(unsigned char channel, float value) {
  float retval = value;
    switch (channel) {
      case 1: {
        // Temperature
        retval = get_temp();
       break; }
      case 21: {
        // LED
        if ( value > 0.5 ) {
          digitalWrite(STATUSLED,STATUSLED_ON); 
        } else {
          digitalWrite(STATUSLED,STATUSLED_OFF); 
        }
       break; }
      case 101:  
      // battery voltage
        cur_voltage = vcc.Read_Volts();
        retval = cur_voltage;
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
  Serial.print("temp: ");
  Serial.println(temp);
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
  radio.setPALevel( RF24_PA_MIN ) ;
  radio.setRetries(15, 15);
  radio.openWritingPipe(addresses[0]);
  radio.openReadingPipe(1,addresses[0]);
  delay(1000);
  digitalWrite(STATUSLED,STATUSLED_OFF); 
  udp_s_data.msg_id=1;
  udp_s_data.network_id=ee_data.network_id;
  udp_s_data.node_id=ee_data.node_id;
  udp_s_data.sensor1_id=0;
  udp_s_data.value1=0;
  udp_s_data.sensor2_id=0;
  udp_s_data.value2=0;
  loopcount=0;
  radio.printDetails();
}

void loop() {
    if (loopcount > 1000) {
      udp_s_data.sensor1_id=1;
      udp_s_data.value1=get_temp();
      Serial.print("Temperatur = ");
      Serial.println( udp_s_data.value1 );
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
      printf("Sensor1_id: %u \n",udp_r_data.sensor1_id);
      printf("Sensor1_value: %f \n",udp_r_data.value1);
      printf("Sensor2_id: %u \n",udp_r_data.sensor2_id);
      printf("Sensor2_value: %f \n",udp_r_data.value2);
      printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");
      if ( udp_r_data.sensor1_id > 0 ) action_loop(udp_r_data.sensor1_id,udp_r_data.value1);
      if ( udp_r_data.sensor2_id > 0 ) action_loop(udp_r_data.sensor2_id,udp_r_data.value2);
    }
    delay(10);
    loopcount++;
}
