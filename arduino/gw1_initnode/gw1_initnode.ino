#include <EEPROM.h>

int eeAddress = 0;
struct ee_data_t {
  uint16_t    network_id;
  uint16_t    node_id;

  
};
struct ee_data_t ee_data;


void setup() {
   ee_data.network_id=2712;
   ee_data.node_id=51;

   EEPROM.put( eeAddress, ee_data );
}

void loop() {

}
