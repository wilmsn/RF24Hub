#define RF24CHANNEL     90
#define RF24NODE        03
#define RF24SLEEPTIME1  300
#define RF24SLEEPTIME2  300
#define RF24SLEEPTIME3  1
#define RF24SLEEPTIME4  1

#define STATUSLED 3

#include <EEPROM.h>

struct eeprom_t {
   uint16_t node;
   uint8_t  channel;
   uint16_t sleeptime1;
   uint16_t sleeptime2;
   uint16_t sleeptime3;
   uint16_t sleeptime4;
};
eeprom_t eeprom;


void setup() {
  eeprom.node = RF24NODE;
  eeprom.channel = RF24CHANNEL;
  eeprom.sleeptime1 = RF24SLEEPTIME1;
  eeprom.sleeptime2 = RF24SLEEPTIME2;
  eeprom.sleeptime3 = RF24SLEEPTIME3;
  eeprom.sleeptime4 = RF24SLEEPTIME4;
  delay(500);  
  for (int i = 0 ; i < EEPROM.length() ; i++) {
    EEPROM.write(i, 0);
  }
  delay(500);
  EEPROM.put(0, eeprom);
  pinMode(STATUSLED, OUTPUT);

}

void loop() {
  digitalWrite(STATUSLED, HIGH);   
  delay(1000);               
  digitalWrite(STATUSLED, LOW);    
  delay(1000);              
}
