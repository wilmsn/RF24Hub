# RF24Hub
A gateway and controler for Arduino nodes with nrf24l01 (demo nodes included)
Main features:
 - the controler sould run on all linux based systems 
 - the gateway should run on a system where the nrf24l01 is connected (i tested raspberry pi, ESP8266)
 - runs as a background service (deamon) or in interactive mode
 - controls the communication to the nodes using RF24 library
 - all informations are stored in a MariaDB database
 - can be controled by telnet commands
 - can send telnet commands to a third party controler (i use FHEM)
 - configuration via config file

Attention: This Brunch is just a test of the transmitter/receiver in rf24 technologie!!!!!
==============================================================

This test uses the folowing payload structure:

struct payload_t {
  uint8_t     node_id;         
  uint8_t     msg_id;          
  uint8_t     msg_type;        
  uint8_t     msg_flags;       
  uint8_t     orderno;         
  uint8_t     network_id;      
  uint8_t     reserved1;      
  uint8_t     reserved2;      
  uint32_t    data1;         
  uint32_t    data2;         
  uint32_t    data3;         
  uint32_t    data4;         
  uint32_t    data5;         
  uint32_t    data6;         
};
 
This backend and Node are compatible with branch "no_network" !!!!!
It will be used for test purposes only.
