
/*
A Node for the rf24hub.
Can be used as thermometer with a display or only as a sensor without display

Build in Parts (selectable):
Bosch Temperature/Pressure/Humidity Sensor like BMP085/BMP180/BMP280/BME280
Dallas Temperature Sensor 18B20
Display Nokia 5110

On Branch: master  !!!!!


*/
//****************************************************
// My definitions for my nodes based on this sketch
// Select only one at one time !!!!
#define SOLARNODE
//#define AUSSENTHERMOMETER
//#define AUSSENTHERMOMETER2
//#define SCHLAFZIMMERTHERMOMETER
//#define BASTELZIMMERTHERMOMETER
//#define BASTELZIMMERTHERMOMETER_SW
//#define KUECHETHERMOMETER // noch mit Bug in 205
//#define WOHNZIMMERTHERMOMETER
//#define ANKLEIDEZIMMERTHERMOMETER
//#define KUGELNODE1
//#define KUGELNODE2
//#define GAESTEZIMMERTHERMOMETER
//----Testnodes-----
//#define FEUCHTESENSOR_170
//#define TESTNODE_UNO
//#define TESTNODE
//****************************************************
// Default settings are in "default.h" now !!!!!
#include "defaults.h"
// Default settings for the individual nodes are in "Node_settings.h"
#include "Node_settings.h"
//-----------------------------------------------------
//*****************************************************
// ------ End of configuration part ------------


#include <avr/pgmspace.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <SPI.h>
#include <sleeplib.h>
#include <Vcc.h>
#include <EEPROM.h>
#include "dataformat.h"
#include "version.h"
#include "config.h"
#include "rf24_config.h"

#if defined(DEBUG_SERIAL)
#include "printf.h"
#endif

#if defined(DISPLAY_5110)
#define HAS_DISPLAY
#include "LCD5110.h"
#endif

#if defined(SENSOR_18B20)
#include <OneWire.h>
#include <DallasTemperature.h>
#endif

#if defined(SENSOR_BOSCH)
#include <BMX_sensor.h>
#endif

#if defined(NEOPIXEL)
#include <Adafruit_NeoPixel.h>
#endif
// ----- End of Includes ------------------------

Vcc vcc(1.0);

ISR(WDT_vect) { watchdogEvent(); }

#if defined(HAS_DISPLAY)
bool displayIsSleeping = false;
#if defined(DISPLAY_5110)
LCD5110 lcd(N5110_RST,N5110_CE,N5110_DC,N5110_DIN,N5110_CLK);
#endif
#endif

#if defined(SENSOR_DUMMY)
float temp;
#endif

#if defined(SENSOR_18B20)
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensor(&oneWire);
DeviceAddress sensorAddress;
float temp;
#endif

#if defined(SENSOR_BOSCH)
BMX_SENSOR sensor;
float temp, pres, humi;
#endif

#if defined(NEOPIXEL)
Adafruit_NeoPixel neopixels(NEOPIXEL, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
uint8_t neopixel_r;
uint8_t neopixel_g;
uint8_t neopixel_b;
#endif

#if defined(SOLARZELLE)
float    u_sol;
float    pow_sol;
uint8_t  batt_mod1;
uint8_t  batt_mod2;
#endif

#if defined(LOAD_BALANCER)
float u_batt1;
#endif

payload_t r_payload, s_payload;    

uint8_t  rf24_node2hub[] = RF24_NODE2HUB;
uint8_t  rf24_hub2node[] = RF24_HUB2NODE;

struct eeprom_t {
   uint16_t versionnumber;
   uint16_t senddelay;
   float    volt_fac;
   float    volt_off;
   float    low_volt_level;
   uint16_t lowVoltLoops;
   uint16_t sleeptime_sec;
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
#if defined(HAS_DISPLAY)
float               field1_val, field2_val, field3_val, field4_val;
boolean             display_on = true;
#endif


// nRF24L01(+) radio attached using Getting Started board 
// Usage: radio(CE_pin, CS_pin)
RF24 radio(RADIO_CE_PIN,RADIO_CSN_PIN);

void pingTest(void);
void sendRegister(void);

void get_voltage(void) {
  vcc_mess = vcc.Read_Volts();
  cur_voltage = (vcc_mess*eeprom.volt_fac)+eeprom.volt_off;
  low_voltage_flag = (eeprom.low_volt_level > 1.5) && (cur_voltage <= eeprom.low_volt_level);
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
}

void get_sensordata(void) {
// Sensor Dummy
#if defined(SENSOR_DUMMY)
    temp=DUMMY_TEMP;
#if defined(DEBUG_SERIAL_SENSOR)
    Serial.print("Temp: ");
    Serial.print(temp);
#endif
#endif
// ENDE: Sensor Dummy

// Sensor Dallas 18B20
#if defined(SENSOR_18B20)
  sensor.requestTemperatures(); // Send the command to get temperatures
  delay(DS18B20_DELAYTIME);
  temp=sensor.getTempCByIndex(0);
#if defined(DEBUG_SERIAL_SENSOR)
    Serial.print("Temp: ");
    Serial.print(temp);
#endif
#endif
// ENDE: Sensor Dallas 18B20

// Sensor Bosch BMP180; BMP280; BME280 
#if defined(SENSOR_BOSCH)
#if defined(DEBUG_SERIAL_SENSOR)
  if (sensor.isBMP180()) Serial.println("BMP180");
  if (sensor.isBMP280()) Serial.println("BMP280");
  if (sensor.isBME280()) Serial.println("BME280"); 
#endif
  sensor.startSingleMeasure();
  if (sensor.hasTemperature() ) {
    temp = sensor.getTemperature();
#if defined(DEBUG_SERIAL_SENSOR)
    Serial.print("Temp: ");
    Serial.println(temp);
#endif
  }
  if (sensor.hasPressure() ) {
    pres = sensor.getPressureAtSealevel(ALTITUDEABOVESEALEVEL);
#if defined(DEBUG_SERIAL_SENSOR)
    Serial.print("Pres: ");
    Serial.println(pres);
#endif
  }
  if (sensor.hasHumidity() )  {
    humi = sensor.getHumidity();
#if defined(DEBUG_SERIAL_SENSOR)
    Serial.print("Humi: ");
    Serial.println(humi);
#endif
  }
#endif
// ENDE: Sensor Bosch BMP180; BMP280; BME280
#if defined(SOLARZELLE)
  u_sol = (float)analogRead(SOLARZELLE) / 1024.0 * vcc_mess;
  pow_sol += u_sol / R_SOLAR * u_sol / 60.0 / 60.0 * (float)eeprom.sleeptime_sec;   // In WStd
#if defined(DEBUG_SERIAL_SENSOR)
    Serial.print("U_sol: ");
    Serial.println(u_sol);
#endif
#endif
#if defined(LOAD_BALANCER)
  u_batt1 = (float)analogRead(LOAD_BALANCER_BATT) / 1024.0 * vcc_mess;
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
#if defined(HAS_DISPLAY)
      case 21:
      {
        // Set field 1
        field1_val = getValue_f(data);;
        print_field(field1_val,1);
      }
      break;
      case 22:
      {
        // Set field 2
        field2_val = getValue_f(data);;
        print_field(field2_val,2);
      }
      break;
      case 23:
      {
        // Set field 3
        field3_val = getValue_f(data);;
        print_field(field3_val,3);
      }
      break;
      case 24:
      {
        // Set field 4
        field4_val = getValue_f(data);;
        print_field(field4_val,4);
      }
      break;
      case 51:
      {
        // Displaylight ON <-> OFF
        if ( getValue_ui(data) & 0x01 ) {
          digitalWrite(STATUSLED,STATUSLED_ON); 
        } else  {
          digitalWrite(STATUSLED,STATUSLED_OFF);
        }
      }
      break;
      case 52:
      {
        // Display Sleepmode ON <-> OFF
        display_sleep( getValue_ui(data) == 0x00 );
      }
      break;
#endif
#if defined(LOAD_BALANCER)
      case 41:
      {
        if ( getValue_i(data) == 0 ) {
          pinMode(LOAD_BALANCER_BATT, INPUT);
        }
        if ( getValue_i(data) == 1 ) {
          pinMode(LOAD_BALANCER_BATT, OUTPUT);
          digitalWrite(LOAD_BALANCER_BATT, HIGH);
        } 
        if ( getValue_i(data) == 2 ) {
          pinMode(LOAD_BALANCER_BATT, OUTPUT);
          digitalWrite(LOAD_BALANCER_BATT, LOW);
        } 
      }
      break;
#endif
#if defined(SOLARZELLE)
      case 42:
      {
        if ( getValue_i(data) == 0 ) {
          pow_sol = 0;;
        }
      }
      break;
#endif
#if defined (RELAIS_1)
      case 51:
      {
        // Relais_1 ON <-> OFF
        if ( getValue_ui(data) & 0x01 ) {
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
        // Relais_2 ON <-> OFF
        if ( getValue_ui(data) > 0x00 ) {
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
        // Relais_3 ON <-> OFF
        if ( getValue_ui(data) & 0x01 ) {
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
        // Relais_4 ON <-> OFF
        if ( getValue_ui(data) & 0x01 ) {
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
        uint16_t pixeldata = getValue_ui(data);
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
      case REG_BATT: 
      { 
      // battery voltage
#if defined(DEBUG_SERIAL_PROC)
    Serial.print("Processing: Volt: ");
    Serial.println(cur_voltage);
#endif  
        data = calcTransportValue_f(REG_BATT, cur_voltage);
      }
      break;      
      case REG_TRANSREG:
      {  
        exec_RegTrans = true;
      }
      break;      
      case REG_MONITOR:
      {
        monitormode = (getValue_ui(data) & 1);
      }
      break;
      case REG_DISPLAY: 
      {
        uint16_t val = getValue_ui(data);
        uint8_t contrast = (val & 0x00FF);
        uint8_t brightnes = (val>>8);
        if (contrast > 0 && contrast < 101) {
#if defined(DISPLAY_5110)
          lcd.setContrast(eeprom.contrast);
#endif
          eeprom.contrast = contrast;
          EEPROM.put(0, eeprom);
        }
      }
      break;
      case REG_SLEEPTIME: 
      {
        // sleeptime in sec!
        uint16_t val = getValue_ui(data);
        if ( val > 9 && val < 32401) {
          eeprom.sleeptime_sec = val;
          EEPROM.put(0, eeprom);
        }
      }
      break;
      case REG_SLEEP4MS_FAC: 
      {
        // sleeptime adjust in sec!
        int16_t val = getValue_i(data);
        if (val >= 500 && val <= 2000) {
          eeprom.sleep4ms_fac = val;
          EEPROM.put(0, eeprom);
        }
      }
      break;
      case REG_EMPTYLOOPS: 
      {
        // emptyloops - number of loops without sending to hub / messure and display only!
        uint8_t val = getValue_ui(data);
        if (val < 21) {
          eeprom.emptyloops=val;
          EEPROM.put(0, eeprom);
        }
      }
      break;
      case REG_SLEEPTIMEKOR: 
      {
        // sleeptime_kor: onetime adjust of sleeptime, will be reset to 0 after use 
        int16_t val = getValue_i(data);
        if (val > -1001 && val < 1001) {
          sleeptime_kor = val;
        }
      }
      break;
      case REG_SENDDELAY: 
      {
        // senddelay in millisec.
        uint16_t val = getValue_ui(data);
        if (val > 49 && val < 1001) {
          eeprom.senddelay = val;
          EEPROM.put(0, eeprom);
        }
      }
      break;
      case REG_SNDCNTN: 
      {
        // max_sendcount: numbers of attempts to send for normal messages
        uint16_t val = getValue_ui(data);
        if (val > 0 && val < 21) {
          eeprom.max_sendcount = val;
          EEPROM.put(0, eeprom);
        }
      }
      break;
      case REG_SNDCNTS: 
      {
      // max_stopcount: numbers of attempts to send for stop messages
        uint16_t val = getValue_ui(data);
        if (val > 0 && val < 21) {
          eeprom.max_stopcount = val;
          EEPROM.put(0, eeprom);
        }
      }
      break;
      case REG_VOLTFAC: 
      {
        // Volt_fac - V = Vmess * Volt_fac
        float val = getValue_f(data);
        if (val >= 0.1 && val <= 10) {
          eeprom.volt_fac = val;
          EEPROM.put(0, eeprom);
        }
      }
      break;
      case REG_VOLTOFF: 
      {
        // Volt_off - V = (Vmess * Volt_fac) + Volt_off
        float val = getValue_f(data);
        if (val >= -10 && val <= 10) {
          eeprom.volt_off = val;
          EEPROM.put(0, eeprom);
        }
      }
      break;
      case REG_LOWVOLTLEV: 
      {
        // Low Voltage Level
        float val = getValue_f(data);
        if (val >= 1 && val <= 5) {
          eeprom.low_volt_level = val;
          EEPROM.put(0, eeprom);
        }
      }
      break;
      case REG_LOWVOLTLOOPS: 
      {
        // Low Voltage send interval
        uint16_t val = getValue_ui(data);
        if ( val < 1001) {
          eeprom.lowVoltLoops = val;
          EEPROM.put(0, eeprom);
        }
      }
      break;
      case REG_PALEVEL: 
      {
        // PA Level
        uint16_t val = getValue_ui(data);
        if (val == 9) exec_pingTest = true;
        if (val > 0 && val < 5) {
          eeprom.pa_level = val;
          // Speichern im EEPROM (noch) nicht nötig
        }
      }
      break;
      case REG_SW:
      {
        data = calcTransportValue_f(REG_SW, SWVERSION);
      }
      break;
    }  
    return data; 
}  

void setup(void) {
  delay(500);
  pinMode(STATUSLED, OUTPUT);     
  digitalWrite(STATUSLED,STATUSLED_ON); 
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
#if defined(SOLARZELLE)
  pinMode(SOLARZELLE, INPUT);
  batt_mod1 = 0;
  batt_mod2 = 0;
#endif
#if defined(LOAD_BALANCER)
  pinMode(LOAD_BALANCER_BATT, INPUT);
#endif
#if defined(DISCHARGE_PIN)
  pinMode(DISCHARGE_PIN, OUTPUT);
  digitalWrite(DISCHARGE_PIN, HIGH); 
#endif
  EEPROM.get(0, eeprom);
  if (eeprom.versionnumber != EEPROM_VERSION && EEPROM_VERSION > 0) {
    eeprom.versionnumber    = EEPROM_VERSION;
    eeprom.brightnes        = BRIGHTNES;
    eeprom.contrast         = CONTRAST;
    eeprom.sleeptime_sec    = SLEEPTIME_SEC;
    eeprom.sleep4ms_fac     = SLEEP4MS_FAC;
    eeprom.emptyloops       = EMPTYLOOPS;
    eeprom.senddelay        = SENDDELAY;
    eeprom.max_sendcount    = MAX_SENDCOUNT;
    eeprom.max_stopcount    = MAX_STOPCOUNT;
    eeprom.volt_fac         = VOLT_FAC;
    eeprom.volt_off         = VOLT_OFF;
    eeprom.low_volt_level   = LOW_VOLT_LEVEL;
    eeprom.lowVoltLoops     = LOW_VOLT_LOOPS;
    EEPROM.put(0, eeprom);
  }  
#if defined(DEBUG_SERIAL)
  Serial.begin(115200);
  printf_begin();
#endif
  SPI.begin();
#if defined(SENSOR_18B20)
  sensor.begin(); 
  for(byte i=0; i<sensor.getDeviceCount(); i++) {
      if(sensor.getAddress(sensorAddress, i)) {
        sensor.setResolution(sensorAddress, DS18B20_RESOLUTION);
      }
    }

#endif
#if defined(SENSOR_BOSCH)
  sensor.begin(); 
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
  delay(1000);
  digitalWrite(STATUSLED,STATUSLED_OFF); 
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
#if defined(HAS_DISPLAY)
#if defined(DISPLAY_5110)
  lcd.begin();
  lcd.setContrast(eeprom.contrast);
  lcd.setFont(LCD5110::small);
  lcd.clear();
  lcd.println();
  lcd.print("Node: ");
  lcd.println(RF24NODE);
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
  draw_antenna(ANT_X0, ANT_Y0);
  draw_therm(THERM_X0, THERM_Y0);
  draw_hb_countdown(8);
#endif
  loopcount = 0;
// on init send config to hub
  pingTest();
  sendRegister();
}

// Start of HAS_DISPLAY Block
#if defined(HAS_DISPLAY)
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
  delay(10);
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
  delay(10);
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
  delay(10);
  lcd.clear();
#endif  
#endif
}

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
//        get_sensordata();
        draw_temp(temp);
        if (field1_val != 0) print_field(field1_val,1);
        if (field2_val != 0) print_field(field2_val,2);
        if (field3_val != 0) print_field(field3_val,3);
        if (field4_val != 0) print_field(field4_val,4);
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
    if (t > -0.01) {
      lcd.print(" ");
    }
    if (t < 10) {
      lcd.print(" ");
    }
    lcd.print(t,1);
    lcd.print("*");
#endif
  }
}

void print_field(float val, int field) {
  uint8_t x0, y0, x1, y1;
  x1=41;
  y1=10;
//  if ( display_on ) {
#if defined(DISPLAY_5110)
    switch (field) {
      case 1: x0=0; y0=27;  break;
      case 2: x0=42; y0=27; break;
      case 3: x0=0; y0=37;  break;
      case 4: x0=42; y0=37; break;
    }
    lcd.drawRect(x0,y0,x1,y1,true,true,false);
    lcd.setFont(LCD5110::small);
    lcd.setCursor(x0+7,y0+2);
    if ( val > 99.9 ) {
      if (val > 999) { 
       lcd.print(val,0);
      } else {
       lcd.print(" "); 
       lcd.print(val,0);
      }    
    } else {
      if (val > 9.999) {
        lcd.print(val,1);
      } else {
        lcd.print(val,2);
      }      
    }
#endif    
//  }
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
// End of HAS_DISPLAY Block

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

void payload_data(uint8_t pos, uint8_t channel, float value) {
  switch (pos) {
    case 1:
      s_payload.data1 = calcTransportValue_f(channel, value);
    break;
    case 2:
      s_payload.data2 = calcTransportValue_f(channel, value);
    break;
    case 3:
      s_payload.data3 = calcTransportValue_f(channel, value);
    break;
    case 4:
      s_payload.data4 = calcTransportValue_f(channel, value);
    break;
    case 5:
      s_payload.data5 = calcTransportValue_f(channel, value);
    break;
    case 6:
      s_payload.data6 = calcTransportValue_f(channel, value);
    break;    
  }
}

/*
 * Ping Test durchführen
 * Ergebnis ist die geringste Sendeleistung die vom Hub erkannt wurde
 */

void pingTest(void) {
  uint8_t PA_Level = radio.getPALevel();
  radio.setPALevel( RF24_PA_MAX) ;
  do_transmit(3, PAYLOAD_TYPE_PING_POW_MAX, PAYLOAD_FLAG_LASTMESSAGE, 0, 251);
  radio.setPALevel( RF24_PA_HIGH) ;
  do_transmit(3, PAYLOAD_TYPE_PING_POW_HIGH, PAYLOAD_FLAG_LASTMESSAGE, 0, 252);
  radio.setPALevel( RF24_PA_LOW) ;
  do_transmit(3, PAYLOAD_TYPE_PING_POW_LOW, PAYLOAD_FLAG_LASTMESSAGE, 0, 253);
  radio.setPALevel( RF24_PA_MIN) ;
  do_transmit(3, PAYLOAD_TYPE_PING_POW_MIN, PAYLOAD_FLAG_LASTMESSAGE, 0, 254);
  radio.setPALevel( RF24_PA_MAX) ;
  do_transmit(3, PAYLOAD_TYPE_PING_END, PAYLOAD_FLAG_LASTMESSAGE, 0, 255);
  radio.setPALevel( PA_Level ); 
  exec_pingTest = false;
}

void sendRegister(void) {
  s_payload.data1 = calcTransportValue_f(REG_VOLTFAC, eeprom.volt_fac);
  s_payload.data2 = calcTransportValue_f(REG_VOLTOFF, eeprom.volt_off);
  s_payload.data3 = calcTransportValue_f(REG_LOWVOLTLEV, eeprom.low_volt_level);
  s_payload.data4 = calcTransportValue_ui(REG_SLEEP4MS_FAC, eeprom.sleep4ms_fac);
  s_payload.data5 = calcTransportValue_ui(REG_LOWVOLTLOOPS, eeprom.lowVoltLoops);
  s_payload.data6 = calcTransportValue_ui(REG_SW, SWVERSION);
  do_transmit(3,PAYLOAD_TYPE_INIT,PAYLOAD_FLAG_EMPTY,0, 241);
// Hub needs some time to prcess data !!!  
  delay(1000);
  s_payload.data1 = calcTransportValue_ui(REG_DISPLAY, (eeprom.brightnes<<8 | eeprom.contrast) );
  s_payload.data2 = calcTransportValue_ui(REG_SLEEPTIME, eeprom.sleeptime_sec);
  s_payload.data3 = calcTransportValue_ui(REG_EMPTYLOOPS, eeprom.emptyloops);
  s_payload.data4 = calcTransportValue_ui(REG_SENDDELAY, eeprom.senddelay);
  s_payload.data5 = calcTransportValue_ui(REG_SNDCNTN, eeprom.max_sendcount);
  s_payload.data6 = calcTransportValue_ui(REG_SNDCNTS, eeprom.max_stopcount);
  do_transmit(3,PAYLOAD_TYPE_INIT,PAYLOAD_FLAG_LASTMESSAGE,0, 242);
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
    heartbeatno++;
    if ( heartbeatno > 200 ) heartbeatno = 1;
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
      delay(10);
#if defined(DEBUG_SERIAL_RADIO)
      Serial.print("TX: ");
      printPayload(&s_payload);
#endif
      radio.write(&s_payload, sizeof(s_payload));
      delay(10);
      radio.startListening(); 
      delay(10);
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
  if (exec_pingTest) {
    pingTest();
    delay(200);
  }
  if (exec_RegTrans) {
    sendRegister();
    delay(200);
  }
}

void loop(void) {
  delay(10);  
  payloadInitData();
  get_voltage();

  if (low_voltage_flag) {   // Low Voltage Handling
#if defined(HAS_DISPLAY)
    display_sleep(true);
#endif
    if ( loopcount > eeprom.lowVoltLoops ) {
      loopcount = 0;
    }
    if ( loopcount == 0 ) get_sensordata();
  } else { // regular Voltage Handling
#if defined(HAS_DISPLAY)
    display_sleep(false);
#endif
    if ( loopcount > eeprom.emptyloops ) loopcount = 0;
#if defined(HAS_DISPLAY)
    draw_battery(BATT_X0,BATT_Y0,cur_voltage);
    draw_therm(THERM_X0, THERM_Y0);
    draw_hb_countdown((uint8_t) 8 * (1- ((float)loopcount / eeprom.emptyloops)) );
    if ( loopcount == 0) draw_antenna(ANT_X0, ANT_Y0);
#if defined(DISPLAY_5110)
    lcd.draw();
#endif
#endif
    get_sensordata();
#if defined(HAS_DISPLAY)
    draw_temp(temp);
    wipe_therm(THERM_X0, THERM_Y0);
#endif
  } // END regular Voltage Handling
  if ( loopcount == 0) {
#if defined(DEBUG_SERIAL_RADIO)
    delay(100);
    Serial.println("Radio WakeUp");
#endif
    radio.powerUp();
    delay(10);
    radio.startListening();
    radio.openReadingPipe(1,rf24_hub2node);
    delay(10);

    // Empty FiFo Buffer from old transmissions
    while ( radio.available() ) {
      radio.read(&r_payload, sizeof(r_payload));
      delay(10);
    }
    uint8_t pos=1;
    payload_data(pos, 101, cur_voltage);
    pos++;
#if defined(SENSOR_DUMMY)
    payload_data(pos, 1, temp);
    pos++;
#endif
#if defined(SENSOR_18B20)
    payload_data(pos, 1, temp);
    pos++;
#endif
#if defined(SENSOR_BOSCH)
    if (sensor.hasTemperature() ) payload_data(pos, 1, temp);
    pos++;
    if (sensor.hasPressure() )    payload_data(pos, 2, pres);
    pos++
    if (sensor.hasHumidity() )    payload_data(pos, 3, humi);
    pos++;
#endif
#if defined(SOLARZELLE)
    payload_data(pos,5,u_sol);
    pos++;
#if defined(DEBUG_SERIAL_RADIO)
    Serial.print("U_sol: ");
    Serial.println(u_sol);
#endif
    payload_data(pos,6,pow_sol);
    pos++;
#if defined(DEBUG_SERIAL_RADIO)
    Serial.print("Pow_sol: ");
    Serial.println(pow_sol);
#endif
    payload_data(pos,8,batt_mod1+batt_mod2);
    pos++;
#if defined(DEBUG_SERIAL_RADIO)
    Serial.print("Batterie Modus: ");
    Serial.println(batt_mod1+batt_mod2);
#endif
#endif
#if defined(LOAD_BALANCER)
    payload_data(pos,7,u_batt1);
    pos++;
#if defined(DEBUG_SERIAL_RADIO)
    Serial.print("U_batt1: ");
    Serial.println(u_batt1);
#endif
#endif
    uint8_t msg_flags = PAYLOAD_FLAG_LASTMESSAGE;
    if ( low_voltage_flag ) msg_flags |= PAYLOAD_FLAG_NEEDHELP; 
    do_transmit(eeprom.max_sendcount, PAYLOAD_TYPE_HB, msg_flags, 0, 0);
    exec_jobs();
/*
#if defined(USE_BATTERIE)
    if (vcc_mess < USE_BATTERIE) {
#endif
*/
    radio.stopListening();
    delay(10);
    radio.powerDown();
#if defined(DEBUG_SERIAL_RADIO)
    Serial.println("Radio Sleep");
    delay(100);
#endif
/*
#if defined(USE_BATTERIE)
    }
#endif
*/
#if defined(HAS_DISPLAY)
    wipe_antenna(ANT_X0, ANT_Y0);
#endif
  }
#if defined(DISPLAY_5110)
  lcd.draw();
#endif
  long int tempsleeptime = eeprom.sleeptime_sec;  // regelmaessige Schlafzeit in Sek.
  tempsleeptime += sleeptime_kor;                 // einmalige Korrektur in Sek. (-1000 ... +1000)
  if ( eeprom.sleep4ms_fac > 499 && eeprom.sleep4ms_fac < 2001 ) {
    tempsleeptime *= eeprom.sleep4ms_fac;           // Umrechnung in Millisek.
  } else {
    tempsleeptime *= 1000;
  }
  sleeptime_kor = 0;  
/*
#if defined(USE_BATTERIE)
  if (vcc_mess < USE_BATTERIE) {
    batt_mod1 = 0;
#endif
*/
    sleep4ms(tempsleeptime);
/*
#if defined(USE_BATTERIE)
  } else {
    batt_mod1 = 1;
    delay(tempsleeptime);
  }
#endif
#if defined(DISCHARGE_PIN)
  if ( vcc_mess > DISCHARGE_U ) {
    digitalWrite(DISCHARGE_PIN, LOW);
    batt_mod2 = 2;
  } else {
    digitalWrite(DISCHARGE_PIN, HIGH);
    batt_mod2 = 0;
  }
#endif
*/
  loopcount++;
}
