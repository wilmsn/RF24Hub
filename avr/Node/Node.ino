
/**
 * @file Node.ino
 * @brief A generic Arduino Program / Template to build the Software für ATMega Nodes
 * 
 * A Node for the rf24hub.
 * Can be configurated to you needs
 * 
 * Build in Parts (cut and pasteable): see page Parts.txt
 * 
 * On Branch: V3.0  !!!!!
 * 
 */
//****************************************************
// Select / enable your in "Node_settings" defined Nodes here
// Select only one at one time !!!!
// Node 200
//#define AUSSENTHERMOMETER1
// Node 201
//#define AUSSENTHERMOMETER2
// Node 202
//#define SOLARNODE1
// Node 203
//#define SOLARNODE2
// Node 100
//#define BASTELZIMMERTHERMOMETER
// Node 101
//#define NODE_101
// Node 102
//#define SCHLAFZIMMERTHERMOMETER
// Node 103
//#define KUECHENTHERMOMETER
// Node 104
//#define GAESTEZIMMERTHERMOMETER
// Node 105
//#define BASTELZIMMERTHERMOMETER_SW
// Node 106
//#define KUGELNODE1
// Node 107
//#define KUGELNODE2
// Node 108
//#define MW_WOHNZIMMER
// Node 110
//#define ANKLEIDEZIMMERTHERMOMETER    
//----Testnodes-----
//#define TESTNODE
// ------ End of configuration part ------------
//****************************************************
// Default settings and settings for the individual nodes are in "Node_settings.h"
#include "Node_settings.h"
//-----------------------------------------------------
//*****************************************************


#include <avr/pgmspace.h>
#include <SPI.h>
#include <sleeplib.h>
#include <Vcc.h>
#include <EEPROM.h>
#include "dataformat.h"
#include "version.h"
#include "config.h"
#include "rf24_config.h"

#if defined(RF24NODE)
#include <nRF24L01.h>
#include <RF24.h>
#endif

#if defined(DEBUG_SERIAL)
#include "printf.h"
#endif

#if defined(DISPLAY_5110)
#include "LCD5110.h"
#endif

#if defined(SENSOR_18B20)
#include <OneWire.h>
#include <DallasTemperature.h>
#endif

#if defined(SENSOR_BOSCH)
#include <BMX_sensor.h>
#endif

#if defined(SENSOR_HTU2X)
#include <htu2x_sensor.h>
#endif

#if defined(SENSOR_AHT20)
#include <aht20_sensor.h>
#endif

#if defined(NEOPIXEL)
#include <Adafruit_NeoPixel.h>
#endif
// ----- End of Includes ------------------------

Vcc vcc(1.0);
float u_ref;

ISR(WDT_vect) { watchdogEvent(); }

#if defined(DISPLAY_ALL)
bool displayIsSleeping = false;
#if defined(DISPLAY_5110)
LCD5110 lcd(N5110_RST,N5110_CE,N5110_DC,N5110_DIN,N5110_CLK);
#endif
#endif

#if defined(SENSOR_DUMMY)
float temp_dummy;
#endif

#if defined(SENSOR_18B20)
OneWire oneWire(SENSOR_18B20); 
DallasTemperature s_18b20(&oneWire);
DeviceAddress sensorAddress;
float temp_18b20;
#endif

#if defined(SENSOR_BOSCH)
BMX_SENSOR bosch;
float temp_bosch, pres_bosch, humi_bosch;
#endif

#if defined(SENSOR_HTU2X)
HTU2X htu2x;
float temp_htu2x, humi_htu2x;
#endif

#if defined(SENSOR_AHT20)
AHT20 aht20;
float temp_aht20, humi_aht20;
#endif

#if defined(NEOPIXEL)
Adafruit_NeoPixel neopixels(NEOPIXEL, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
uint8_t neopixel_r;
uint8_t neopixel_g;
uint8_t neopixel_b;
#endif

#if defined(SOLARZELLE1)
float    u_sol1;
float    u_sol1_sum;
#endif

#if defined (BATT_MONITOR)
uint8_t  batt_mod1;
uint8_t  batt_mod2;
uint8_t  batt_mod3;
uint8_t  batt_mod4;
uint8_t  batt_mod5;
uint8_t  batt_mod1_s;
uint8_t  batt_mod2_s;
uint8_t  batt_mod3_s;
uint8_t  batt_mod4_s;
uint8_t  batt_mod5_s;
#endif

#if defined(SOLARZELLE2)
float    u_sol2;
#endif

#if defined (LOAD_BALLANCER)
float    u_batt1;
float    u_batt2;
#endif

payload_t r_payload, s_payload;    
uint8_t payloadNo = 0;

uint8_t  rf24_node2hub[] = RF24_NODE2HUB;
uint8_t  rf24_hub2node[] = RF24_HUB2NODE;

struct eeprom_t {
   uint16_t versionnumber;
   uint16_t senddelay;
   float    volt_fac;
   float    volt_off;
   float    volt_lv;
   float    volt_lb;
   float    volt_dis1;
   float    volt_dis2;
   float    volt_dis3;
   uint16_t sleeptime_lv;
   uint16_t sleeptime;
   uint16_t sleep4ms_fac;
   uint8_t  emptyloops;
   uint8_t  max_sendcount;
   uint8_t  max_stopcount;
   uint8_t  contrast;
   uint8_t  brightnes;
   uint8_t  pa_level;
};
eeprom_t eeprom;

boolean             low_voltage_flag = false;
boolean             exec_pingTest = false;
boolean             exec_RegTrans = false;
boolean             set_default = false;
float               cur_voltage;
float               vcc_mess;
uint16_t            loopcount;
uint16_t            max_loopcount;
int                 sleeptime_kor;
uint8_t             last_orderno = 0;
uint8_t             msg_id = 0;
uint8_t             heartbeatno=0;
boolean             monitormode = false;

//Some Var for restore after sleep of display
#if defined(DISPLAY_ALL)
float               field1_val, field2_val, field3_val, field4_val;
boolean             display_on = true;
#endif


// nRF24L01(+) radio attached using Getting Started board 
// Usage: radio(CE_pin, CS_pin)
#if defined(RF24NODE)
RF24 radio(RADIO_CE_PIN,RADIO_CSN_PIN);
#endif

void get_sensordata(void) {

// Read Voltage and calculate battery
  u_ref = vcc.Read_Volts();
#if defined(BATTERY_READ_EXTERNAL)
  vcc_mess = (float)analogRead(BATTERY_READ_EXTERNAL) / 1024.0 * u_ref 
              * (float)(BATTERY_VOLTAGEDIVIDER_R1 + BATTERY_VOLTAGEDIVIDER_R2) / (float)BATTERY_VOLTAGEDIVIDER_R2;
#else
  vcc_mess = u_ref;
#endif
  cur_voltage = ( vcc_mess * eeprom.volt_fac ) + eeprom.volt_off;
  low_voltage_flag = (eeprom.volt_lv > 1.5) && (cur_voltage <= eeprom.volt_lv);
#if defined(DEBUG_SERIAL_SENSOR)
   Serial.print("Volt (gemessen): ");
   Serial.println(vcc_mess);
   Serial.print("Volt Faktor: ");
   Serial.println(eeprom.volt_fac);
   Serial.print("Volt Offset: ");
   Serial.println(eeprom.volt_off);
   Serial.print("Volt: ");
   Serial.println(cur_voltage);
   Serial.print("Low Voltage Flag: ");
   if ( low_voltage_flag ) {       
     Serial.println("set");
   } else {
     Serial.println("not set");
   }
#endif  
// Ende Voltage

// Sensor Dummy
#if defined(SENSOR_DUMMY)
    temp_dummy=DUMMY_TEMP;
#if defined(DEBUG_SERIAL_SENSOR)
    Serial.print("Temp: ");
    Serial.print(temp);
#endif
#endif
// ENDE: Sensor Dummy

// Sensor Dallas 18B20
#if defined(SENSOR_18B20)
  s_18b20.requestTemperatures(); // Send the command to get temperatures
  sleep4ms(SENSOR_18B20_DELAYTIME);
  delay(2);
  temp_18b20=s_18b20.getTempCByIndex(0);
#define DISPLAY_TEMP temp_18b20
#if defined(DEBUG_SERIAL_SENSOR)
    Serial.print("Temp: ");
    Serial.print(temp_18b20);
#endif
#endif
// ENDE: Sensor Dallas 18B20

// Sensor Bosch BMP180; BMP280; BME280 
#if defined(SENSOR_BOSCH)
#if defined(DEBUG_SERIAL_SENSOR)
  if (bosch.isBMP180()) Serial.println("BMP180");
  if (bosch.isBMP280()) Serial.println("BMP280");
  if (bosch.isBME280()) Serial.println("BME280"); 
#endif
  bosch.startSingleMeasure();
  if (bosch.hasTemperature() ) {
    temp_bosch = bosch.getTemperature();
#define DISPLAY_TEMP temp_bosch
#if defined(DEBUG_SERIAL_SENSOR)
    Serial.print("Temp: ");
    Serial.println(temp_bosch);
#endif
  }
  if (bosch.hasPressure() ) {
    pres_bosch = bosch.getPressureAtSealevel(ALTITUDEABOVESEALEVEL);
#if defined(DEBUG_SERIAL_SENSOR)
    Serial.print("Pres: ");
    Serial.println(pres_bosch);
#endif
  }
  if (bosch.hasHumidity() )  {
    humi_bosch = bosch.getHumidity();
#define DISPLAY_HUMI humi_bosch
#if defined(DEBUG_SERIAL_SENSOR)
    Serial.print("Humi: ");
    Serial.println(humi_bosch);
#endif
  }
#endif
// ENDE: Sensor Bosch BMP180; BMP280; BME280

// Sensor HTU2X
#if defined(SENSOR_HTU2X)
  htu2x.startSingleMeasure();
  temp_htu2x = htu2x.getTemperature();
#define DISPLAY_TEMP temp_htu2x
#if defined(DEBUG_SERIAL_SENSOR)
    Serial.print("Temp: ");
    Serial.println(temp_htu2x);
#endif
  humi_htu2x = htu2x.getHumidity();
#define DISPLAY_HUMI humi_htu2x
#if defined(DEBUG_SERIAL_SENSOR)
    Serial.print("Humi: ");
    Serial.println(humi_htu2x);
#endif
#endif
// ENDE: Sensor HTU2X

// Sensor AHT20
#if defined(SENSOR_AHT20)
  aht20.startSingleMeasure();
  temp_aht20 = aht20.getTemperature();
#define DISPLAY_TEMP temp_aht20
#if defined(DEBUG_SERIAL_SENSOR)
    Serial.print("Temp: ");
    Serial.println(temp_aht20);
#endif
  humi_aht20 = aht20.getHumidity();
#define DISPLAY_HUMI humi_aht20
#if defined(DEBUG_SERIAL_SENSOR)
    Serial.print("Humi: ");
    Serial.println(humi_aht20);
#endif
#endif
// ENDE: Sensor AHT20

// Solarzelle
#if defined(SOLARZELLE1)
  u_sol1 = (float)analogRead(SOLARZELLE1) / 1024.0 * vcc_mess;
  u_sol1_sum += u_sol1;
#if defined(DEBUG_SERIAL_SENSOR)
    Serial.print("U_sol: ");
    Serial.println(u_sol);
#endif
#endif
#if defined(SOLARZELLE2)
  u_sol2 = (float)analogRead(SOLARZELLE2) / 1024.0 * vcc_mess;
#if defined(DEBUG_SERIAL_SENSOR)
    Serial.print("U_sol2: ");
    Serial.println(u_sol2);
#endif
// Ende Solarzelle

#endif
#if defined(LOAD_BALLANCER)
  pinMode(LOAD_BALLANCER_PIN, INPUT);
  delay(10);
  u_batt1 = (float)analogRead(LOAD_BALLANCER_PIN) / 1024.0 * vcc_mess;
  u_batt2 = vcc_mess - u_batt1;
#endif
}

uint32_t action_loop(uint32_t data) {
  uint32_t retval = 0;
  int      intval;
  uint8_t  channel = getChannel(data);
#if defined(DEBUG_SERIAL_PROC)
    Serial.print("Processing: Channel: ");
    Serial.println(getChannel(data));
#endif  
    switch (channel) {
#if defined(DISPLAY_ALL)
#if defined(DISPLAY_LAYOUT_DEFAULT)
      case 21:
      {
        // Set field 1
        float val;
        getValue(data, &val);;
        print_field(val,1);
      }
      break;
      case 22:
      {
        // Set field 2
        float val;
        getValue(data, &val);
        print_field(val,2);
      }
      break;
      case 23:
      {
        // Set field 3
        float val;
        getValue(data, &val);
        print_field(val,3);
      }
      break;
      case 24:
      {
        // Set field 4
        float val;
        getValue(data, &val);
        print_field(val,4);
      }
      break;
#endif
      case 51:
      {
        uint16_t val;
        getValue(data, &val);
        // Displaylight ON <-> OFF
        if ( val & 0x01 ) {
          digitalWrite(STATUSLED,STATUSLED_ON); 
        } else  {
          digitalWrite(STATUSLED,STATUSLED_OFF);
        }
      }
      break;
      case 52:
      {
        // Display Sleepmode ON <-> OFF
        uint16_t val;
        getValue(data, &val);
        display_sleep( val == 0x00 );
      }
      break;
#endif
#if defined (RELAIS_1)
      case 51:
      {
        uint16_t val;
        getValue(data, &val)
        // Relais_1 ON <-> OFF
        if ( val & 0x01 ) {
#if defined (DEBUG_SERIAL_SENSOR)
          Serial.println("Relais 1 ein");
#endif               
          digitalWrite(RELAIS_1, RELAIS_ON); 
        } else  {
#if defined (DEBUG_SERIAL_SENSOR)
          Serial.println("Relais 1 aus");
#endif               
          digitalWrite(RELAIS_1, RELAIS_OFF);
        }
      }
      break;
#endif
#if defined (RELAIS_2)
      case 52:
      {
        uint16_t val;
        getValue(data, &val)
        // Relais_2 ON <-> OFF
        if ( val & 0x01 ) {
#if defined (DEBUG_SERIAL_SENSOR)
          Serial.println("Relais 2 ein");
#endif               
          digitalWrite(RELAIS_2, RELAIS_ON); 
        } else  {
#if defined (DEBUG_SERIAL_SENSOR)
          Serial.println("Relais 2 aus");
#endif               
          digitalWrite(RELAIS_2, RELAIS_OFF);
        }
      }
      break;
#endif
#if defined (RELAIS_3)
      case 53:
      {
        uint16_t val;
        getValue(data, &val)
        // Relais_3 ON <-> OFF
        if ( val & 0x01 ) {
#if defined (DEBUG_SERIAL_SENSOR)
          Serial.println("Relais 3 ein");
#endif               
          digitalWrite(RELAIS_3,RELAIS_ON); 
        } else  {
#if defined (DEBUG_SERIAL_SENSOR)
          Serial.println("Relais 3 aus");
#endif               
          digitalWrite(RELAIS_3,RELAIS_OFF);
        }
      }
      break;
#endif
#if defined (RELAIS_4)
      case 54:
      {
        uint16_t val;
        getValue(data, &val)
        // Relais_4 ON <-> OFF
        if ( val & 0x01 ) {
#if defined (DEBUG_SERIAL_SENSOR)
          Serial.println("Relais 4 ein");
#endif               
          digitalWrite(RELAIS_4,RELAIS_ON); 
        } else  {
#if defined (DEBUG_SERIAL_SENSOR)
          Serial.println("Relais 4 aus");
#endif               
          digitalWrite(RELAIS_4,RELAIS_OFF);
        }
      }
      break;
#endif
#if defined (NEOPIXEL)
      case 51:
      {
        uint16_t pixeldata;
        getValue(data, &pixeldata);
        uint8_t pixelcount = 0;
        neopixel_b = (uint8_t)((pixeldata & 0b0000000000011111)<<3);
        neopixel_g = (uint8_t)((pixeldata & 0b0000001111100000)>>2);
        neopixel_r = (uint8_t)((pixeldata & 0b0111110000000000)>>7);
#if defined (DEBUG_SERIAL_SENSOR)
          Serial.print("Neopixel Data:");
          Serial.println(pixeldata);
          Serial.print("Rot:");
          Serial.print(neopixel_r);
          Serial.print(" Grün:");
          Serial.print(neopixel_g);
          Serial.print(" Blau:");
          Serial.println(neopixel_b);
#endif               
        neopixels.clear();
        do {
            neopixels.setPixelColor(pixelcount, neopixels.Color(neopixel_r, neopixel_g, neopixel_b));
          pixelcount++;
        }
        while(pixelcount < NEOPIXEL);     
        neopixels.show();
      }
      break;  
#endif
      case REG_TRANSREG:
      {  
        exec_RegTrans = true;
      }
      break;      
      case REG_TRANSPA:
      {  
        exec_pingTest = true;
      }
      break;
      case REG_DEFAULT:
      {
        set_default = true;
      }
      case REG_SLEEPTIME:
      {
        // sleeptime in sec!
        uint16_t val;
        getValue(data, &val);
        if ( val >= 10 && val <= 32400) {
          eeprom.sleeptime = val;
          EEPROM.put(0, eeprom);
        }
      }
      break;
      case REG_SLEEP4MS_FAC:
      {
        // sleeptime adjust in sec!
        int16_t val;
        getValue(data, &val);
        if (val >= 500 && val <= 2000) {
          eeprom.sleep4ms_fac = val;
          EEPROM.put(0, eeprom);
        }
      }
      break;
      case REG_EMPTYLOOPS:
      {
        // emptyloops - number of loops without sending to hub / messure and display only!
        uint16_t val;
        getValue(data, &val);
        if (val < 21) {
          eeprom.emptyloops=(uint8_t)val;
          EEPROM.put(0, eeprom);
        }
      }
      break;
      case REG_SLEEPTIME_KOR:
      {
        // sleeptime_kor: onetime adjust of sleeptime, will be reset to 0 after use 
        int16_t val;
        getValue(data, &val);
        if (val > -1001 && val < 1001) {
          sleeptime_kor = val;
        }
      }
      break;
      case REG_SENDDELAY:
      {
        // senddelay in millisec.
        uint16_t val;
        getValue(data, &val);
        if (val > 49 && val < 1001) {
          eeprom.senddelay = val;
          EEPROM.put(0, eeprom);
        }
      }
      break;
      case REG_MAX_SENDCOUNT:
      {
        // max_sendcount: numbers of attempts to send for normal messages
        uint16_t val;
        getValue(data, &val);
        if (val > 0 && val < 21) {
          eeprom.max_sendcount = val;
          EEPROM.put(0, eeprom);
        }
      }
      break;
      case REG_MAX_STOPCOUNT:
      {
      // max_stopcount: numbers of attempts to send for stop messages
        uint16_t val;
        getValue(data, &val);
        if (val > 0 && val < 21) {
          eeprom.max_stopcount = val;
          EEPROM.put(0, eeprom);
        }
      }
      break;
      case REG_VOLT_FAC:
      {
        // Volt_fac - V = Vmess * Volt_fac
        float val;
        getValue(data, &val);
        if (val >= 0.1 && val <= 10) {
          eeprom.volt_fac = val;
          EEPROM.put(0, eeprom);
        }
      }
      break;
      case REG_VOLT_OFF:
      {
        // Volt_off - V = (Vmess * Volt_fac) + Volt_off
        float val;
        getValue(data, &val);
        if (val >= -10 && val <= 10) {
          eeprom.volt_off = val;
          EEPROM.put(0, eeprom);
        }
      }
      break;
      case REG_VOLT_LV:
      {
        // Low Voltage Level
        float val;
        getValue(data, &val);
        if (val >= 1 && val <= 5) {
          eeprom.volt_lv = val;
          EEPROM.put(0, eeprom);
        }
      }
      break;
      case REG_SLEEPTIME_LV:
      {
        // Low Voltage send interval
        uint16_t val;
        getValue(data, &val);
        if ( val > 9 && val < 32401) {
          eeprom.sleeptime_lv = val;
          EEPROM.put(0, eeprom);
        }
      }
      break;
#if defined(RF24NODE)
      case REG_PALEVEL:
      {
        // PA Level
        uint16_t val;
        getValue(data, &val);
        if (val > 0 && val < 5) {
          eeprom.pa_level = val;
          radio.setPALevel( eeprom.pa_level) ;
          EEPROM.put(0, eeprom);
        }
      }
      break;
#endif
      case REG_SW:
      {
        data = calcTransportValue(REG_SW, SWVERSION);
      }
      break;
#if defined(DISPLAY_ALL)
      case REG_CONTRAST:
      {
        uint16_t val;
        getValue(data, &val);
        if ( val >= 0 && val <= 100 ) {
          eeprom.contrast = val;
          EEPROM.put(0, eeprom);
        }
        data = calcTransportValue(REG_CONTRAST, eeprom.contrast);
      }
      break;
      case REG_BRIGHTNES:
      {
        uint16_t val;
        getValue(data, &val);
        if ( val >= 0 && val <= 100 ) {
          eeprom.brightnes = val;
          EEPROM.put(0, eeprom);
        }
        data = calcTransportValue(REG_BRIGHTNES, eeprom.brightnes);
      }
      break;
#endif
#if defined(LOAD_BALLANCER)
      case REG_LOAD_BALLANCER:
      {
        float val;
        getValue(data, &val);
        if ( val >= 0.01 && val <= 1 ) {
          eeprom.volt_lb = val;
          EEPROM.put(0, eeprom);
        }
        data = calcTransportValue(REG_LOAD_BALLANCER, eeprom.volt_lb);
      }
      break;
#endif
#if defined(DISCHARGE1)
      case REG_DISCHARGE_LEV1:
      {
        float val;
        getValue(data, &val);
        if ( val >= 1 && val <= 20 ) {
          eeprom.volt_dis1 = val;
          EEPROM.put(0, eeprom);          
        }
        data = calcTransportValue(REG_DISCHARGE_LEV1, eeprom.volt_dis1);
      }
      break;
#endif
#if defined(DISCHARGE2)
      case REG_DISCHARGE_LEV2:
      {
        float val;
        getValue(data, &val);
        if ( val >= 1 && val <= 20 ) {
          eeprom.volt_dis2 = val;
          EEPROM.put(0, eeprom);
        }
        data = calcTransportValue(REG_DISCHARGE_LEV2, eeprom.volt_dis2);
      }
      break;
#endif
#if defined(DISCHARGE3)
      case REG_DISCHARGE_LEV3:
      {
        float val;
        getValue(data, &val);
        if ( val >= 1 && val <= 20 ) {
          eeprom.volt_dis3 = val;
          EEPROM.put(0, eeprom);
        }
        data = calcTransportValue(REG_DISCHARGE_LEV3, eeprom.volt_dis3);
      }
      break;
#endif
    }  
    return data; 
}  

void init_eeprom(bool reset_eeprom) {
// EEPROM_VERSION == 0 disables EEPROM !!!!
  EEPROM.get(0, eeprom);
  if (eeprom.versionnumber != EEPROM_VERSION || EEPROM_VERSION == 0 || reset_eeprom) {
    eeprom.versionnumber    = EEPROM_VERSION;
#if defined(DISPLAY_ALL)    
    eeprom.brightnes        = BRIGHTNES;
    eeprom.contrast         = CONTRAST;
#else
    eeprom.brightnes        = 0;
    eeprom.contrast         = 0;
#endif
    eeprom.sleeptime        = SLEEPTIME;
    eeprom.sleep4ms_fac     = SLEEP4MS_FAC;
    eeprom.emptyloops       = EMPTYLOOPS;
    eeprom.senddelay        = SENDDELAY;
    eeprom.max_sendcount    = MAX_SENDCOUNT;
    eeprom.max_stopcount    = MAX_STOPCOUNT;
    eeprom.volt_fac         = VOLT_FAC;
    eeprom.volt_off         = VOLT_OFF;
    eeprom.volt_lv          = VOLT_LV;
#if defined(RF24NODE)
    eeprom.pa_level         = PA_LEVEL;
#else
    eeprom.pa_level         = 0;
#endif
#if defined(LOAD_BALLANCER)
    eeprom.volt_lb          = LOAD_BALLANCER;
#else
    eeprom.volt_lb          = 0;
#endif
#if defined(DISCHARGE1)
    eeprom.volt_dis1        = DISCHARGE1;
#else
    eeprom.volt_dis1        = 0;
#endif
#if defined(DISCHARGE2)
    eeprom.volt_dis2        = DISCHARGE2;
#else
    eeprom.volt_dis2        = 0;
#endif
#if defined(DISCHARGE3)
    eeprom.volt_dis3        = DISCHARGE3;
#else
    eeprom.volt_dis3        = 0;
#endif
    eeprom.sleeptime_lv     = SLEEPTIME_LV;
    if (EEPROM_VERSION > 0) EEPROM.put(0, eeprom);
  }  
}

void setup(void) {
  delay(500);
#if defined(STATUSLED)
  pinMode(STATUSLED, OUTPUT);
  digitalWrite(STATUSLED,STATUSLED_ON);
#endif

// Init EEPROM
init_eeprom(false);

#if defined(DISCHARGE3_PIN)
  pinMode(DISCHARGE3_PIN, OUTPUT);
  digitalWrite(DISCHARGE3_PIN,LOW);
#endif
#if defined(RELAIS_1)
  pinMode(RELAIS_1, OUTPUT);
  digitalWrite(RELAIS_1,RELAIS_ON);
#endif
#if defined(RELAIS_2)
  pinMode(RELAIS_2, OUTPUT);
  digitalWrite(RELAIS_2,RELAIS_ON);
#endif
#if defined(RELAIS_3)
  pinMode(RELAIS_3, OUTPUT);
  digitalWrite(RELAIS_3,RELAIS_ON);
#endif
#if defined(RELAIS_4)
  pinMode(RELAIS_4, OUTPUT);
  digitalWrite(RELAIS_4,RELAIS_ON);
#endif
#if defined(SOLARZELLE1)
  pinMode(SOLARZELLE1, INPUT);
#endif
#if defined(SOLARZELLE2)
  pinMode(SOLARZELLE2, INPUT);
#endif
#if defined(LOAD_BALLANCER)
  pinMode(LOAD_BALLANCER, INPUT);
#endif
#if defined(DEBUG_SERIAL)
  Serial.begin(115200);
  printf_begin();
#endif
  SPI.begin();
#if defined(SENSOR_18B20)
  s_18b20.begin(); 
  s_18b20.setWaitForConversion(false);
  for(byte i=0; i<s_18b20.getDeviceCount(); i++) {
      if(s_18b20.getAddress(sensorAddress, i)) {
        s_18b20.setResolution(sensorAddress, SENSOR_18B20_RESOLUTION);
      }
    }

#endif
#if defined(SENSOR_BOSCH)
  bosch.begin(); 
#endif
#if defined(SENSOR_AHT20)
  aht20.begin();
#endif
#if defined(SENSOR_HTU2X)
  htu2x.begin();
#endif
#if defined(NEOPIXEL)
  neopixel_r = NEOPIXEL_R_DEFAULT;
  neopixel_g = NEOPIXEL_G_DEFAULT;
  neopixel_b = NEOPIXEL_B_DEFAULT;
  neopixels.begin();
  neopixels.clear();
  for(int i=0; i<NEOPIXEL; i++) {
    neopixels.setPixelColor(i, neopixels.Color(neopixel_r, neopixel_g, neopixel_b));
  }
  neopixels.show();   
#endif
#if defined(RF24NODE)
  radio.begin();
  radio.setChannel(RF24_CHANNEL);
  radio.setDataRate(RF24_SPEED);
  radio.setPALevel(RF24_PA_MAX);
  radio.setRetries(0, 0);
  radio.setAutoAck(false);
  radio.disableDynamicPayloads();
  radio.setPayloadSize(32);
  radio.setCRCLength(RF24_CRC_16);
  radio.openWritingPipe(rf24_node2hub);
  radio.openReadingPipe(1,rf24_hub2node);
#if defined(DEBUG_SERIAL_RADIO)
  radio.printDetails();
#endif
#endif  //RF24NODE    
  delay(100);
#if defined(RELAIS_1)
  digitalWrite(RELAIS_1,RELAIS_OFF); 
#endif
#if defined(RELAIS_2)
  digitalWrite(RELAIS_2,RELAIS_OFF); 
#endif
#if defined(RELAIS_3)
  digitalWrite(RELAIS_3,RELAIS_OFF); 
#endif
#if defined(RELAIS_4)
  digitalWrite(RELAIS_4,RELAIS_OFF); 
#endif
#if defined(NEOPIXEL)
  neopixels.clear();
  neopixels.show();   
#endif
#if defined(DISPLAY_ALL)
#if defined(DISPLAY_5110)
  lcd.begin();
  lcd.setContrast(eeprom.contrast);
  lcd.setFont(LCD5110::small);
  lcd.clear();
  lcd.println();
#if defined(RF24NODE)
  lcd.print("Node: ");
  lcd.println(RF24NODE);
#endif
  lcd.println();
  lcd.print("SW: ");
  lcd.println(SWVERSION);
  lcd.draw();
  delay(1000);
  lcd.clear();
#endif
#if defined(MONITOR)
  monitor(1000);
#endif
#if defined(RF24NODE)
  draw_antenna(ANT_X0, ANT_Y0);
#endif
  draw_therm(THERM_X0, THERM_Y0);
  draw_hb_countdown(8);
#endif
  loopcount = 0;
#if defined(STATUSLED)
  digitalWrite(STATUSLED,STATUSLED_OFF); 
#endif
// on init send config to hub
#if defined(RF24NODE)
  pingTest();
  sendRegister();
#endif
}

// Start of DISPLAY_ALL Block
#if defined(DISPLAY_ALL)

void monitor(uint32_t delaytime) {
#if defined(MONITOR)  
  const char string_0[] PROGMEM = "SW Version: ";
  const char string_1[] PROGMEM = "Temp: ";
  const char string_2[] PROGMEM = "Ubatt: ";
  const char string_3[] PROGMEM = "Loops: ";
  const char string_4[] PROGMEM = "Send: ";
  const char string_5[] PROGMEM = "RF24 Nw: ";
  const char string_6[] PROGMEM = "Node: ";
  const char string_7[] PROGMEM = "Channel: ";
  const char string_8[] PROGMEM = "Kontrast: ";
#if defined(DISPLAY_5110)
  lcd.setFont(LCD5110::small);
  lcd.clear();
  lcd.println();
  lcd.println(string_0);
  lcd.println();
  lcd.print("    ");
  lcd.print(SWVERSION);
  lcd.draw();
  sleep4ms(delaytime);
  delay(1);
  lcd.clear();
  get_sensordata();
  lcd.print(string_1);
  lcd.println(temp,1);
  get_voltage();
  lcd.print(string_2);
  lcd.print(cur_voltage,1);
  lcd.print("/");
  lcd.println(vcc.Read_Volts(),1);
  lcd.print(string_3);
  lcd.print(eeprom.sleeptime_sec);
  lcd.print("/");
  lcd.println(eeprom.emptyloops);
  lcd.print(string_4);
  lcd.print(eeprom.senddelay);
  lcd.print("/");
  lcd.print(eeprom.max_sendcount);
  lcd.print("/");
  lcd.print(eeprom.max_stopcount);
  lcd.draw();
  sleep4ms(delaytime);
  delay(1);
  lcd.clear();
  lcd.println(string_5);
  lcd.print(string_6);
  lcd.println(RF24NODE);
  lcd.print(string_7);
  lcd.print(RF24_CHANNEL);
  lcd.draw();
  sleep4ms(delaytime);
  lcd.clear();
  lcd.print(string_8);
  lcd.print(eeprom.contrast);
  lcd.setFont(LCD5110::big);
  for (int i=0; i<100; i+=5) {
    lcd.setCursor(25,20);
    lcd.print(i);        
    lcd.setContrast(i);
    lcd.draw();
    delay(300);
  }
  sleep4ms(5000);
  lcd.setContrast(eeprom.contrast);
  delay(1);
  lcd.clear();
#endif  
#endif

} // END monitor()

void display_sleep(boolean displayGotoSleep) {
  if ( displayIsSleeping != displayGotoSleep ) {
    if ( displayGotoSleep ) { // Display go to sleep
#if defined(DISPLAY_5110)
      lcd.off(); 
#endif
      displayIsSleeping = true;
    } else {
      if ( ! low_voltage_flag ) {  
#if defined(DISPLAY_5110)
        lcd.on(); 
#endif
        displayIsSleeping = false;
        draw_temp(DISPLAY_TEMP);
#if defined(DISPLAY_LAYOUT_TEMPHUMI)
        draw_temp(DISPLAY_HUMI);
#endif
#if defined(DISPLAY_LAYOUT_DEFAULT)
        if (field1_val != 0) print_field(field1_val,1);
        if (field2_val != 0) print_field(field2_val,2);
        if (field3_val != 0) print_field(field3_val,3);
        if (field4_val != 0) print_field(field4_val,4);
#endif
      }
    }
  }  
}

void draw_hb_countdown(uint8_t watermark) {
  if ( display_on ) {
    if ( watermark > 8 ) watermark = 8;
#if defined(DISPLAY_5110)
    lcd.drawRect(HB_X0,HB_Y0,4,8,false,true,false);
    lcd.drawRect(HB_X0, HB_Y0 + 8 - watermark, 4, watermark, true, true, true);
#endif
  }
}

void wipe_therm(uint8_t x, uint8_t y) {
  if ( display_on ) {
#if defined(DISPLAY_5110)
    lcd.drawRect(x,y,3,7,false,true,false);
#endif
  }
}

void draw_therm(uint8_t x, uint8_t y) {
// Ein Thermometersymbol 4*8 Pixel   
  if ( display_on ) {
#if defined(DISPLAY_5110)
    lcd.drawRect(x+1,y,1,3);
    lcd.drawRect(x,y+4,3,2);
#endif
  }
}

void draw_temp(float t) {
  if ( display_on ) {
#if defined(DISPLAY_5110)
    lcd.drawRect(0,0,72, 24,false,true,false);
    lcd.setFont(LCD5110::big);
    lcd.setCursor(0,0);
#endif
    if (t > -0.01) {
#if defined(DISPLAY_5110)
      lcd.print(" ");
#endif
    }
    if (t < 10) {
#if defined(DISPLAY_5110)
      lcd.print(" ");
#endif
    }
#if defined(DISPLAY_5110)
    lcd.print(t,1);
    lcd.print("*");
#endif
  }
}

void draw_humi(float h) {
  if ( display_on ) {
#if defined(DISPLAY_5110)
    lcd.drawRect(0,25,83,47,false,true,false);
    lcd.setFont(LCD5110::medium);
    lcd.setCursor(50,30);
    lcd.print(h,0);
    lcd.print("%");
#endif
  }
}

void print_field(float val, int field) {
  uint8_t x0, y0, x1, y1;
  x1=41;
  y1=10;
    switch (field) {
      case 1: x0=0; y0=27;  break;
      case 2: x0=42; y0=27; break;
      case 3: x0=0; y0=37;  break;
      case 4: x0=42; y0=37; break;
    }
#if defined(DISPLAY_5110)
    lcd.drawRect(x0,y0,x1,y1,true,true,false);
    lcd.setFont(LCD5110::small);
    lcd.setCursor(x0+7,y0+2);
#endif    
    if ( val > 99.9 ) {
      if (val > 999) { 
#if defined(DISPLAY_5110)
       lcd.print(val,0);
#endif    
      } else {
#if defined(DISPLAY_5110)
       lcd.print(" "); 
       lcd.print(val,0);
#endif    
      }    
    } else {
      if (val > 9.999) {
#if defined(DISPLAY_5110)
        lcd.print(val,1);
#endif    
      } else {
#if defined(DISPLAY_5110)
        lcd.print(val,2);
#endif    
      }      
    }
}

void draw_battery(int x, int y, float u) {
  if ( display_on ) {
#if defined(DISPLAY_5110)
// Das Batteriesymbol ist 10*5 Pixel
    // Clear the drawing field
    //lcd.clrRect(x,y,9,5);
    // Drawing a symbol of an battery
    // Size: 10x5 pixel
    // at position x and y
    lcd.drawRect(x+2,y,7,4,true,true,false);
    lcd.drawRect(x,y+1,1,2,true,true,false);
    if ( u > U1 ) {
      lcd.drawRect(x+8,y+1,1,2,true,true,true); 
    } else {
      lcd.setPixel(x+3,y);
      lcd.setPixel(x+4,y+1);
      lcd.setPixel(x+5,y+2);
      lcd.setPixel(x+6,y+3);
      lcd.setPixel(x+7,y+4);
    }
    if ( u > U2 ) lcd.drawRect(x+6,y+1,1,2,true,true,true);
    if ( u > U3 ) lcd.drawRect(x+4,y+1,1,2,true,true,true);
    if ( u > U4 ) lcd.drawRect(x+2,y+1,1,2,true,true,true);
#endif
  }
}

#if defined(RF24NODE)
void draw_antenna(int x, int y) {
  if ( display_on ) {
#if defined(DISPLAY_5110)
    // Drawing a symbol of an antenna
    // Size: 10x10 pixel
    // at position x and y
    lcd.setPixel(x+7,y+0);
    lcd.setPixel(x+1,y+1);
    lcd.setPixel(x+8,y+1);
    lcd.setPixel(x+0,y+2);
    lcd.setPixel(x+3,y+2);
    lcd.setPixel(x+6,y+2);
    lcd.setPixel(x+9,y+2);
    lcd.setPixel(x+0,y+3);
    lcd.setPixel(x+2,y+3);
    lcd.setPixel(x+7,y+3);
    lcd.setPixel(x+9,y+3);
    lcd.setPixel(x+0,y+4);
    lcd.setPixel(x+2,y+4);
    lcd.setPixel(x+4,y+4);
    lcd.setPixel(x+5,y+4);
    lcd.setPixel(x+7,y+4);
    lcd.setPixel(x+9,y+4);
    lcd.setPixel(x+0,y+5);
    lcd.setPixel(x+2,y+5);
    lcd.setPixel(x+4,y+5);
    lcd.setPixel(x+5,y+5);
    lcd.setPixel(x+7,y+5);
    lcd.setPixel(x+9,y+5);
    lcd.setPixel(x+0,y+6);
    lcd.setPixel(x+3,y+6);
    lcd.setPixel(x+4,y+6);
    lcd.setPixel(x+5,y+6);
    lcd.setPixel(x+6,y+6);
    lcd.setPixel(x+9,y+6);
    lcd.setPixel(x+1,y+7);
    lcd.setPixel(x+4,y+7);
    lcd.setPixel(x+5,y+7);
    lcd.setPixel(x+8,y+7);
    lcd.setPixel(x+4,y+8);
    lcd.setPixel(x+5,y+8);
    lcd.setPixel(x+4,y+9);
    lcd.setPixel(x+5,y+9);
#endif
  }
}   
 
void wipe_antenna(int x, int y) {
  if ( display_on ) {
#if defined(DISPLAY_5110)
    lcd.drawRect(x,y,9,9,false,true,false);
#endif
  }
}  
#endif
#endif //RF24NODE

void display_refresh() {
#if defined(DISPLAY_5110)
    lcd.draw();
#endif
}
// End of DISPLAY_ALL Block

void payloadInitData(void) {
  s_payload.data1 = 0;
  s_payload.data2 = 0;
  s_payload.data3 = 0;
  s_payload.data4 = 0;
  s_payload.data5 = 0;
  s_payload.data6 = 0;
}

#if defined(DEBUG_SERIAL_RADIO)
void printPayloadData(uint32_t pldata) {
    char buf[20];
    Serial.print("(");
    Serial.print(getChannel(pldata));
    Serial.print("/");
    Serial.print(unpackTransportValue(pldata,buf));
    Serial.print(")");
}

void printPayload(payload_t* pl) {
    Serial.print(" N:");
    Serial.print(pl->node_id);
    Serial.print(" I:");
    Serial.print(pl->msg_id);
    Serial.print(" T:");
    Serial.print(pl->msg_type);
    Serial.print(" O:");
    Serial.print(pl->orderno);
    Serial.print(" ");
    printPayloadData(pl->data1);
    printPayloadData(pl->data2);
    printPayloadData(pl->data3);
    printPayloadData(pl->data4);
    printPayloadData(pl->data5);
    printPayloadData(pl->data6);
    Serial.println();
}
#endif

uint8_t mk_flags(bool last_msg) {
  uint8_t retval = PAYLOAD_FLAG_EMPTY;
  if (low_voltage_flag) retval |= PAYLOAD_FLAG_LOWVOLTAGE;
  if (last_msg) retval |= PAYLOAD_FLAG_LASTMESSAGE;
  return retval;
}

#if defined(RF24NODE)
void payload_data(uint8_t* pos, uint8_t channel, float value) {
  if ( (*pos) == 7 ) {
    do_transmit(eeprom.max_sendcount, payloadNo == 0 ? PAYLOAD_TYPE_HB : PAYLOAD_TYPE_HB_F, mk_flags(false), payloadNo, heartbeatno);
// Hub needs some time to prcess data !!!  
    delay(500);
    payloadInitData();
    (*pos) = 1;
    payloadNo++;
  }
  switch (*pos) {
    case 1:
      s_payload.data1 = calcTransportValue(channel, value);
    break;
    case 2:
      s_payload.data2 = calcTransportValue(channel, value);
    break;
    case 3:
      s_payload.data3 = calcTransportValue(channel, value);
    break;
    case 4:
      s_payload.data4 = calcTransportValue(channel, value);
    break;
    case 5:
      s_payload.data5 = calcTransportValue(channel, value);
    break;
    case 6:
      s_payload.data6 = calcTransportValue(channel, value);
    break;    
  }
  (*pos)++;
}

/*
 * Ping Test durchführen
 * Ergebnis ist die geringste Sendeleistung die vom Hub erkannt wurde
 */

void pingTest(void) {
  radio.setPALevel( RF24_PA_MAX) ;
  do_transmit(3, PAYLOAD_TYPE_PING_POW_MAX, mk_flags(false), 0, 251);
  radio.setPALevel( RF24_PA_HIGH) ;
  do_transmit(3, PAYLOAD_TYPE_PING_POW_HIGH, mk_flags(false), 0, 252);
  radio.setPALevel( RF24_PA_LOW) ;
  do_transmit(3, PAYLOAD_TYPE_PING_POW_LOW, mk_flags(false), 0, 253);
  radio.setPALevel( RF24_PA_MIN) ;
  do_transmit(3, PAYLOAD_TYPE_PING_POW_MIN, mk_flags(false), 0, 254);
  radio.setPALevel( RF24_PA_MAX) ;
  do_transmit(3, PAYLOAD_TYPE_PING_END, mk_flags(false), 0, 255);
  radio.setPALevel( eeprom.pa_level ); 
  exec_pingTest = false;
}

void sendRegister(uint32_t data, uint8_t* pos, uint8_t* hbno) {
  if ((*pos) == 1) payloadInitData();
  switch (*pos) {
    case 1: s_payload.data1 = data;  break;
    case 2: s_payload.data2 = data;  break;
    case 3: s_payload.data3 = data;  break;
    case 4: s_payload.data4 = data;  break;
    case 5: s_payload.data5 = data;  break;
    case 6: s_payload.data6 = data;  break;
  }
  (*pos)++;
  if ((*pos) == 7) {
    do_transmit(3,PAYLOAD_TYPE_INIT,mk_flags(false),0, *hbno);
    (*hbno)++;
    (*pos) = 1;
// Hub needs some time to prcess data !!!  
    sleep4ms(1000);
    delay(1);
  }
}

void sendRegister(void) {
  uint8_t pos = 1;
  uint8_t hbno = 241;
  sendRegister(calcTransportValue(REG_VOLT_FAC, eeprom.volt_fac),&pos,&hbno);
  sendRegister(calcTransportValue(REG_VOLT_OFF, eeprom.volt_off),&pos,&hbno);
  sendRegister(calcTransportValue(REG_VOLT_LV, eeprom.volt_lv),&pos,&hbno);
  sendRegister(calcTransportValue(REG_SLEEP4MS_FAC, eeprom.sleep4ms_fac),&pos,&hbno);
  sendRegister(calcTransportValue(REG_SLEEPTIME_LV, eeprom.sleeptime_lv),&pos,&hbno);
  sendRegister(calcTransportValue(REG_SW, SWVERSION),&pos,&hbno);
  sendRegister(calcTransportValue(REG_SLEEPTIME, eeprom.sleeptime),&pos,&hbno);
  sendRegister(calcTransportValue(REG_EMPTYLOOPS, eeprom.emptyloops),&pos,&hbno);
  sendRegister(calcTransportValue(REG_SENDDELAY, eeprom.senddelay),&pos,&hbno);
  sendRegister(calcTransportValue(REG_MAX_SENDCOUNT, eeprom.max_sendcount),&pos,&hbno);
  sendRegister(calcTransportValue(REG_MAX_STOPCOUNT, eeprom.max_stopcount),&pos,&hbno);
  sendRegister(calcTransportValue(REG_PALEVEL, eeprom.pa_level),&pos,&hbno);
#if defined(DISPLAY_5110)
  sendRegister(calcTransportValue(REG_CONTRAST, eeprom.contrast),&pos,&hbno);
#endif
#if defined(DISPLAY_XXXX)
  sendRegister(calcTransportValue(REG_BRIGHTNES, eeprom.brightnes),&pos,&hbno);
#endif
#if defined(LOAD_BALLANCER)
  sendRegister(calcTransportValue(REG_LOAD_BALLANCER, eeprom.volt_lb),&pos,&hbno);
#endif
#if defined(DISCHARGE1)
  sendRegister(calcTransportValue(REG_DISCHARGE_LEV1, eeprom.volt_dis1),&pos,&hbno);
#endif
#if defined(DISCHARGE2)
  sendRegister(calcTransportValue(REG_DISCHARGE_LEV2, eeprom.volt_dis2),&pos,&hbno);
#endif
#if defined(DISCHARGE3)
  sendRegister(calcTransportValue(REG_DISCHARGE_LEV3, eeprom.volt_dis3),&pos,&hbno);
#endif
  if ( pos > 1 ) do_transmit(3,PAYLOAD_TYPE_INIT,PAYLOAD_FLAG_EMPTY,0, hbno);
  exec_RegTrans = false;
}

void prep_data(uint8_t msg_type, uint8_t msg_flags, ONR_DATTYPE orderno, uint8_t myheartbeatno) {
  s_payload.node_id = RF24NODE;
  s_payload.msg_id = 0;
  s_payload.msg_type = msg_type;
  s_payload.msg_flags = msg_flags;
  s_payload.orderno = orderno;
  if (myheartbeatno > 0) {
    s_payload.heartbeatno = myheartbeatno;
  } else {
    s_payload.heartbeatno = heartbeatno;
  }
}

void do_transmit(uint8_t max_tx_loopcount, uint8_t msg_type, uint8_t msg_flags, ONR_DATTYPE orderno, uint8_t myheartbeatno) {
// ToDo: Verarbeitung des MSG_FLAGS !!!!
    unsigned long start_ts;
    uint8_t tx_loopcount = 0;
    bool doLoop = true;
#if defined(DEBUG_SERIAL_RADIO)
    unsigned long temp_ts = 0;
#endif
    start_ts = millis();
    prep_data(msg_type, msg_flags, orderno, myheartbeatno);
    while ( tx_loopcount < max_tx_loopcount ) {
      s_payload.msg_id++;
      radio.stopListening();
      delay(1);
#if defined(DEBUG_SERIAL_RADIO)
      Serial.print("TX: ");
      printPayload(&s_payload);
#endif
      radio.write(&s_payload, sizeof(s_payload));
      delay(1);
      radio.startListening(); 
      delay(1);
      start_ts = millis();
      doLoop = true;
#if defined(DEBUG_SERIAL_RADIO)
      Serial.print("RX: ");
      Serial.println(start_ts);
#endif
      while ( (millis() < (start_ts + eeprom.senddelay) ) && doLoop ) {
#if defined(DEBUG_SERIAL_RADIO)
        if ( temp_ts + 100 < millis() ) {
          temp_ts = millis();
          Serial.print(".");
        }
#endif
        if ( radio.available() ) {
#if defined(DEBUG_SERIAL_RADIO)
          Serial.print("RX: ");
#endif
          radio.read(&r_payload, sizeof(r_payload));
#if defined(DEBUG_SERIAL_RADIO)
          printPayload(&r_payload);
          if (r_payload.node_id != RF24NODE) {
            Serial.println("Wrong Node, Message dropped!");
          }
          if (r_payload.orderno == last_orderno) {
            Serial.println("Ordernumber already processed!");
          }
#endif
          if (r_payload.node_id == RF24NODE && r_payload.orderno != last_orderno) {
            last_orderno = r_payload.orderno;
            switch(r_payload.msg_type) {
              case PAYLOAD_TYPE_DAT:
                if (r_payload.data1 > 0) { s_payload.data1 = action_loop(r_payload.data1); } else { s_payload.data1 = 0; }
                if (r_payload.data2 > 0) { s_payload.data2 = action_loop(r_payload.data2); } else { s_payload.data2 = 0; }
                if (r_payload.data3 > 0) { s_payload.data3 = action_loop(r_payload.data3); } else { s_payload.data3 = 0; }
                if (r_payload.data4 > 0) { s_payload.data4 = action_loop(r_payload.data4); } else { s_payload.data4 = 0; }
                if (r_payload.data5 > 0) { s_payload.data5 = action_loop(r_payload.data5); } else { s_payload.data5 = 0; }
                if (r_payload.data6 > 0) { s_payload.data6 = action_loop(r_payload.data6); } else { s_payload.data6 = 0; }
                prep_data(PAYLOAD_TYPE_DATRESP,PAYLOAD_FLAG_LASTMESSAGE,r_payload.orderno, 0);
                tx_loopcount = 0; 
                if (r_payload.msg_flags & PAYLOAD_FLAG_LASTMESSAGE ) {
                  // Wenn das LASTMESSAGEFLAG gesetzt ist sollte nur noch eine Endmessage kommen
                  // ==> Zeit wird verkürzt
                  max_tx_loopcount = eeprom.max_stopcount;
                } else {
                  max_tx_loopcount = eeprom.max_sendcount;
                }
                doLoop = false;;
              break;
              case PAYLOAD_TYPE_HB_RESP:
              case PAYLOAD_TYPE_HB_F_RESP:
              case PAYLOAD_TYPE_DATSTOP:
                  tx_loopcount = max_tx_loopcount; 
                  doLoop = false;
              break;  
            }
          }
        }  //radio.available
      }
      tx_loopcount++;
    }
}

void exec_jobs(void) {
  // Test if there are some jobs to do
  if (set_default) {
    init_eeprom(true);
    exec_RegTrans = true;
  }
  if (exec_pingTest) {
    pingTest();
    delay(200);
  }
  if (exec_RegTrans) {
    sendRegister();
    delay(200);
  }
}
#endif

void batt_monitor() {
#if defined (BATT_MONITOR)
#if defined(DISCHARGE1)
  if (vcc_mess > eeprom.volt_dis1) {
    batt_mod1 = 1;
    batt_mod1_s = 1;
  } else {
    batt_mod1 = 0;
  }
#if defined(DISCHARGE2)
  if ( vcc_mess > eeprom.volt_dis2 ) {
    digitalWrite(STATUSLED,STATUSLED_ON);
    batt_mod2 = 2;
    batt_mod2_s = 2;
  } else {
    digitalWrite(STATUSLED,STATUSLED_OFF);
    batt_mod2 = 0;
  }
#if defined(DISCHARGE3)
  if ( vcc_mess > eeprom.volt_dis3 ) {
    digitalWrite(DISCHARGE3_PIN,HIGH); 
    batt_mod3 = 4;
    batt_mod3_s = 4;
  } else {
    digitalWrite(DISCHARGE3_PIN,LOW); 
    batt_mod3 = 0;
  }
#endif  
#endif
#if defined(LOAD_BALLANCER)
  if ( (u_batt2 - u_batt1) > eeprom.volt_lb && vcc_mess > eeprom.volt_dis1 ) {
    pinMode(LOAD_BALLANCER, OUTPUT);
    delay(1);
    digitalWrite(LOAD_BALLANCER, HIGH);
    batt_mod4 = 8;
    batt_mod4_s = 8;
  } else {
    batt_mod4 = 0;
  }
  if ( (u_batt1 - u_batt2) > eeprom.volt_lb && vcc_mess > eeprom.volt_dis1 ) {
    pinMode(LOAD_BALLANCER, OUTPUT);
    delay(1);
    digitalWrite(LOAD_BALLANCER, LOW);
    batt_mod5 = 16;
    batt_mod5_s = 16;
  } else {
    batt_mod5 = 0;
  }
#endif
#endif
#endif
}

void loop(void) {
  delay(1);
  payloadNo = 0;
  heartbeatno++;
  if ( heartbeatno > 200 ) heartbeatno = 1;
  payloadInitData();
  get_sensordata();

  if (low_voltage_flag) {
// Low Voltage Handling
#if defined(DISPLAY_ALL)
    display_sleep(true);
#endif

  } else {

// regular Voltage Handling
#if defined(DISPLAY_ALL)
    display_sleep(false);
#endif
    if ( loopcount > eeprom.emptyloops ) loopcount = 0;
#if defined(DISPLAY_ALL)
    draw_battery(BATT_X0,BATT_Y0,cur_voltage);
    draw_therm(THERM_X0, THERM_Y0);
    draw_hb_countdown((uint8_t) 8 * (1- ((float)loopcount / eeprom.emptyloops)) );
#if defined(RF24NODE)
    if ( loopcount == 0) draw_antenna(ANT_X0, ANT_Y0);
#endif
    display_refresh();
    draw_temp(DISPLAY_TEMP);
#if defined(DISPLAY_LAYOUT_TEMPHUMI)
    draw_humi(DISPLAY_HUMI);
#endif
    wipe_therm(THERM_X0, THERM_Y0);
#endif
    batt_monitor();
  } // END regular Voltage Handling
  if ( loopcount == 0) {
#if defined(DEBUG_SERIAL_RADIO)
    delay(100);
    Serial.println("Radio WakeUp");
#endif
#if defined(RF24NODE)
    radio.powerUp();
    delay(1);
    radio.startListening();
    radio.openReadingPipe(1,rf24_hub2node);
    delay(1);

    // Empty FiFo Buffer from old transmissions
    while ( radio.available() ) {
      radio.read(&r_payload, sizeof(r_payload));
      delay(10);
    }
    uint8_t pos=1;
    payload_data(&pos, SENSOR_BATT, cur_voltage);
#if defined(SENSOR_DUMMY)
    payload_data(&pos, TEMP_DUMMY_CHANNEL, temp_dummy);
#endif
#if defined(SENSOR_18B20)
    payload_data(&pos, TEMP_18B20_CHANNEL, temp_18b20);
#endif
#if defined(SENSOR_BOSCH)
    if ( bosch.hasTemperature() ) payload_data(&pos, TEMP_BOSCH_CHANNEL, temp_bosch);
    if ( bosch.hasPressure() )  payload_data(&pos, PRES_BOSCH_CHANNEL, pres_bosch);
    if ( bosch.hasHumidity() )  payload_data(&pos, HUMI_BOSCH_CHANNEL, humi_bosch);
#endif
#if defined(SENSOR_HTU2X)
    payload_data(&pos, TEMP_HTU2X_CHANNEL, temp_htu2x);
    payload_data(&pos, HUMI_HTU2X_CHANNEL, humi_htu2x);
#endif
#if defined(SENSOR_AHT20)
    payload_data(&pos, TEMP_AHT20_CHANNEL, temp_aht20);
    payload_data(&pos, HUMI_AHT20_CHANNEL, humi_aht20);
#endif
#if defined(SOLARZELLE1)
    payload_data(&pos, SOLARZELLE1_CHANNEL, u_sol1_sum/(float)(eeprom.emptyloops+1));
    u_sol1_sum = 0;
#if defined(SOLARZELLE2)
    payload_data(&pos,SOLARZELLE2_CHANNEL,u_sol2);
#endif
#if defined(LOAD_BALLANCER) || defined(DISCHARGE1) || defined(DISCHARGE2)
    payload_data(&pos,BATT1_CHANNEL,u_batt1);
#endif
    uint16_t batt_mod = batt_mod1_s | batt_mod2_s | batt_mod3_s | batt_mod4_s | batt_mod5_s;
    payload_data(&pos,BATT_MOD_CHANNEL, batt_mod);
    batt_mod1_s = 0;
    batt_mod2_s = 0;
    batt_mod3_s = 0;
    batt_mod4_s = 0;
    batt_mod5_s = 0;
#endif    
    do_transmit(eeprom.max_sendcount, payloadNo == 0 ? PAYLOAD_TYPE_HB : PAYLOAD_TYPE_HB_F, mk_flags(true), 0, 0);
    exec_jobs();
    radio.stopListening();
    delay(1);
    radio.powerDown();
#if defined(DEBUG_SERIAL_RADIO)
    Serial.println("Radio Sleep");
    delay(100);
#endif
#if defined(DISPLAY_ALL)
    wipe_antenna(ANT_X0, ANT_Y0);
#endif
#endif   //RF24NODE
  }
#if defined(DISPLAY_ALL)
  display_refresh();
#endif
  long int tempsleeptime = eeprom.sleeptime;  // regelmaessige Schlafzeit in Sek.
  if (low_voltage_flag) tempsleeptime = eeprom.sleeptime_lv;
  tempsleeptime += sleeptime_kor;                 // einmalige Korrektur in Sek.
  tempsleeptime *= eeprom.sleep4ms_fac;           // Umrechnung in Millisek.
  sleeptime_kor = 0;  
#if defined(DISCHARGE1)
  if (batt_mod1 == 1) {
    unsigned long mymillis = millis();
    while ( millis() - mymillis < tempsleeptime ) {
      delay(1000);
      batt_monitor();
      if ( millis() < mymillis ) mymillis = 0;
    }
    //delay(tempsleeptime);
  } else {
#endif
    sleep4ms(tempsleeptime);
#if defined(DISCHARGE1)
  }
#endif
  loopcount++;
}
