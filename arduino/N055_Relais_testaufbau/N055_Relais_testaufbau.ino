// How long do we try to init this node 100000(in ms) = 100 sec. 
#define MAXINIT 100000
// Define a valid radiochannel here
#define RADIOCHANNEL 90
// This node: Use octal numbers starting with "0": "041" is child 4 of node 1
#define NODE 055
// The CE Pin of the Radio module
#define RADIO_CE_PIN 10
// The CS Pin of the Radio module
#define RADIO_CSN_PIN 9
// The pin of the statusled
#define STATUSLED 3
#define STATUSLED_ON HIGH
#define STATUSLED_OFF LOW
// sleeptime 1...4 is NOT used for always on NODES
#define SLEEPTIME1 5
#define SLEEPTIME2 5
#define SLEEPTIME3 2
#define SLEEPTIME4 5
#define ONE_WIRE_BUS 8

#define RELAIS1 3
#define RELAIS_ON HIGH
#define RELAIS_OFF LOW

#define LED_MATRIX_DIN 5
#define LED_MATRIX_CLK 6
#define LED_MATRIX_CS 4
#define LED_MATRIX_NUM_DEV 4
#define LED_CHAR_SPACE 6

// ------ End of configuration part ------------

#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <sleeplib.h>
#include <Vcc.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LED_Matrix.h>

byte sprite[][8]={ 
           {B00011000,
            B00100100,
            B00100100,
            B00011000,
            B01111110,
            B00011000,
            B00100100,
            B01000010},

          { B00011000,
            B00100100,
            B00100100,
            B00011010,
            B01111100,
            B00011000,
            B01100100,
            B00000010},

          { B00011000,
            B00100100,
            B00100100,
            B00011010,
            B00111100,
            B01011000,
            B00110100,
            B00000100},

          { B00011000,
            B00100100,
            B00100100,
            B00011010,
            B00111100,
            B01011000,
            B00011000,
            B00011000},

          { B00011000,
            B00100100,
            B00100100,
            B00011010,
            B00111100,
            B01011000,
            B00010100,
            B00010000},

          { B00011000,
            B00100100,
            B00100100,
            B00011000,
            B00111110,
            B01011000,
            B00010100,
            B00010100}
};

const float VccCorrection = 1.0/1.0;  // Measured Vcc by multimeter divided by reported Vcc

LED_Matrix matrix(LED_MATRIX_DIN,LED_MATRIX_CLK,LED_MATRIX_CS,LED_MATRIX_NUM_DEV);

Vcc vcc(VccCorrection);

ISR(WDT_vect) { watchdogEvent(); }

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);
 
// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

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
// all sleeptime* values in seconds 
// Time for the fist sleep after an activity of this node
float               sleeptime1 = SLEEPTIME1;
// Time for the 2. to N. sleeploop
float               sleeptime2 = SLEEPTIME2;
// Time to sleep after wakeup with radio on
float               sleeptime3 = SLEEPTIME3;
// Time to keep the network up if it was busy
float               sleeptime4 = SLEEPTIME4;
boolean             init_finished = false;
float               networkuptime = 0.0;
float               temp;
//Some Var for restore after sleep of display
float               cur_voltage;
uint8_t             n_update = 0;
int                 std_value = 0;
int                 std_value_old = 0;

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
        case 21:
            if ( value > 0.5 ) {
              digitalWrite(RELAIS1,RELAIS_ON);
            } else {
              digitalWrite(RELAIS1,RELAIS_OFF);  
            }
        break;
        case 31: 
           std_value = (int) value;
        break;
        case 41:
           for(int address = 0; address < LED_MATRIX_NUM_DEV ; address++ ) {
             matrix.setIntensity((byte)value);
           } 
        break;        
        case 101:
          // battery voltage => vcc.Read_Volts();
          retval=vcc.Read_Volts();
        break;
        case 111:
          // sleeptimer1
          sleeptime1=value;
        break;
        case 112:
          // sleeptimer2
          sleeptime2=value;
        break;
        case 113:
          // sleeptimer3
          sleeptime3=value;
        break;
        case 114:
          // sleeptimer4
          sleeptime4=value;
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
  pinMode(STATUSLED, OUTPUT);
  digitalWrite(STATUSLED,STATUSLED_ON);
  SPI.begin();
  //****
  // put anything else to init here
  //****
  sensors.begin(); 
  sensors.setResolution(10);
  get_temp();
  matrix.begin();
  matrix.setIntensity(5);
  int pos=4;
  matrix.printChar(pos,'i');
  pos+=LED_CHAR_SPACE;
  matrix.printChar(pos,'n');
  pos+=LED_CHAR_SPACE;
  matrix.printChar(pos,'i');
  pos+=LED_CHAR_SPACE;
  matrix.printChar(pos,'t');
  matrix.display();
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
  digitalWrite(STATUSLED,STATUSLED_OFF); 
  sleepmode=sleep4;
  action_loop(41,12);
  networkuptime = 0.0;    
  delay(100);
}

void get_temp(void) {
  sensors.requestTemperatures(); // Send the command to get temperatures
  temp=sensors.getTempCByIndex(0);
}

void drawSprite( byte* sprite, int x, int y, int width, int height )
{
  byte mask = B10000000;
  for( int iy = 0; iy < height; iy++ )
  {
    for( int ix = 0; ix < width; ix++ )
    {
      matrix.setPixel(x + ix, y + iy, (bool)(sprite[iy] & mask ));
      mask = mask >> 1;
    }
    mask = B10000000;
  }
}

void loop(void) {
  n_update = network.update();
  if ( network.available() ) {
    network.read(rxheader,&payload,sizeof(payload));
    if ( payload.sensor1 > 0 ) payload.value1 = action_loop(payload.sensor1, payload.value1);
    if ( payload.sensor2 > 0 ) payload.value2 = action_loop(payload.sensor2, payload.value2);
    if ( payload.sensor3 > 0 ) payload.value3 = action_loop(payload.sensor3, payload.value3);
    if ( payload.sensor4 > 0 ) payload.value4 = action_loop(payload.sensor4, payload.value4);
    txheader.type=rxheader.type;
    network.write(txheader,&payload,sizeof(payload));    
    if ( std_value != std_value_old ) {
      std_value_old = std_value;
      for (int x1 = 0; x1 < 6; x1++) {
        for (int x2 = 0; x2 < 6; x2++) {
          drawSprite( (byte*)&sprite[x2], x1 * 6 + x2, 0, 8, 8 );
          matrix.display();
          delay(100);
        }
      }
      byte _std = std_value / 100;
      byte _min = std_value % 100;
      byte std_h, std_l, min_h,min_l;
      std_h = _std/10;
      std_l = _std - std_h * 10;
      min_h = _min/10;
      min_l = _min - min_h * 10;
      matrix.printChar(2,'0'+std_h);
      matrix.printChar(8,'0'+std_l);
      matrix.printChar(14,':');
      matrix.printChar(19,'0'+min_h);
      matrix.printChar(25,'0'+min_l);
      matrix.display();
    }
  }
  delay(100);
}
