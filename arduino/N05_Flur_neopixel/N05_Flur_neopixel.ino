// How long do we try to init this node 100000(in ms) = 100 sec. 
#define MAXINIT 100000
// Define a valid radiochannel here
#define RADIOCHANNEL 90
// This node: Use octal numbers starting with "0": "041" is child 4 of node 1
#define NODE 05
// The CE Pin of the Radio module
#define RADIO_CE_PIN 10
// The CS Pin of the Radio module
#define RADIO_CSN_PIN 9
// The pin of the statusled
#define ONE_WIRE_BUS 8

#define PIN_PIXELS 7
#define NUM_PIXELS 46
#define R_PIXEL_DEFAULT 100
#define G_PIXEL_DEFAULT 100
#define B_PIXEL_DEFAULT 100


// ------ End of configuration part ------------

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
//#include <sleeplib.h>
#include <Vcc.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_NeoPixel.h>


const float VccCorrection = 1.0/1.0;  // Measured Vcc by multimeter divided by reported Vcc

Vcc vcc(VccCorrection);

//ISR(WDT_vect) { watchdogEvent(); }

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
 
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// Neopixels
Adafruit_NeoPixel pixels(NUM_PIXELS, PIN_PIXELS, NEO_GRB + NEO_KHZ800);
uint8_t r_pixel;
uint8_t g_pixel;
uint8_t b_pixel;


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

enum radiomode_t { radio_sleep, radio_listen } radiomode = radio_sleep;
enum sleepmode_t { sleep1, sleep2, sleep3, sleep4} sleepmode = sleep1, next_sleepmode = sleep2;

RF24NetworkHeader rxheader;
RF24NetworkHeader txheader(0);
boolean             init_finished = false;
float               temp;
//Some Var for restore after sleep of display
float               cur_voltage;

// Usage: radio(CE_pin, CS_pin)
RF24 radio(RADIO_CE_PIN,RADIO_CSN_PIN);

// Network uses that radio
RF24Network network(radio);


float action_loop(unsigned char channel, float value) {
  float retval = value;

    switch (channel) {
        case 1:
            get_temp();
            retval=temp;
        break;    
        case 21: {
          pixels.clear();
          if ( value > 0.5) {
            for(int i=0; i<NUM_PIXELS; i++) {
              pixels.setPixelColor(i, pixels.Color(r_pixel, g_pixel, b_pixel));
            }
          } else {
          }
          pixels.show();   
        }
        break;
        case 31:
            r_pixel = (uint8_t) value;
        break;
        case 32:
            g_pixel = (uint8_t) value;
        break;
        case 33:
            b_pixel = (uint8_t) value;
        break;
        case 101:
          // battery voltage => vcc.Read_Volts();
          retval=vcc.Read_Volts();
        break;
        case 111:
          // sleeptimer1
          //sleeptime1=value;
        break;
        case 112:
          // sleeptimer2
          //sleeptime2=value;
        break;
        case 113:
          // sleeptimer3
          //sleeptime3=value;
        break;
        case 114:
          // sleeptimer4
          //sleeptime4=value;
          break;
        case 115:
          // radio on (=1) or off (=0) when sleep
          if ( value > 0.5) radiomode=radio_listen; else radiomode=radio_sleep;
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
  SPI.begin();
  //****
  // put anything else to init here
  //****
  sensors.begin(); 
  sensors.setResolution(10);
  get_temp();
  r_pixel = R_PIXEL_DEFAULT;
  g_pixel = G_PIXEL_DEFAULT;
  b_pixel = B_PIXEL_DEFAULT;
  pixels.begin();
  //####
  // end aditional init
  //####
  radio.begin();
  radio.setPALevel(RF24_PA_MAX);
//  radio.setRetries(15,2);
  network.begin(RADIOCHANNEL, NODE);
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
  delay(100);
}

void get_temp(void) {
  sensors.requestTemperatures(); // Send the command to get temperatures
  temp=sensors.getTempCByIndex(0);
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
