#ifndef rf24hub_h
#define rf24hub_h

#include "Arduino.h"

#include "rf24hub_data_types.h"

/*
struct MyNetworkHeader
{
  uint16_t    network_id;
  uint16_t    node_id;
  uint16_t    msg_id;
  uint8_t     type;
  uint8_t     flags;
  MyNetworkHeader() {}
};


class MyNetwork
{
  public:
  MyNetwork( RF24& _radio );
  void begin(uint8_t _channel);  
  bool available(void);
  uint16_t peek(MyNetworkHeader& header);
  uint16_t read(MyNetworkHeader& header, void* message, uint16_t maxlen);
  bool write(MyNetworkHeader& header,const void* message, uint16_t len);
  uint8_t frame_buffer[MAX_FRAME_SIZE];   
  private:

    
    
};
*/

extern "C"
{
#include "zahlenformat.h"
}


#endif
