#define DISPLAY_SDA D3
#define DISPLAY_SCL D2
#define NRF24_MOSI D7
#define NRF24_MISO D6
#define NRF24_SCK D5
#define NRF24_CE D1
#define NRF24_CSN D0
#define RADIOCHANNEL 90
#define NODE 00


#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include <SSD1306_Logwriter.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <wifi_settings.h>

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

SSD1306_OLED display;
RF24 radio(NRF24_CE,NRF24_CSN);
RF24Network network(radio);

RF24NetworkHeader rxheader;

void setup() {
  SPI.begin();
  display.begin(DISPLAY_SDA, DISPLAY_SCL);
  display.setFont(2); // use 8*6 font
  display.println("RF24Gateway");

  display.print("Connect to Wifi ");
  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    display.print(".");
  }
  display.println("");
  display.println("WiFi connected");
  display.println("IP address: ");
  display.println(WiFi.localIP());

  radio.begin();
  radio.setPALevel(RF24_PA_MIN);
  network.begin(RADIOCHANNEL, NODE);
  radio.setDataRate(RF24_250KBPS);
  delay(200);
  display.println("Radio started on");
  display.print("channel ");
  display.println(RADIOCHANNEL);

  uint16_t mypipe;
  mypipe = network.addressOfPipe(01,1);
  display.print("PipeAdress:"); display.println(mypipe);   

}

void loop() {
    network.update();
    if ( network.available() ) {
      network.read(rxheader,&payload,sizeof(payload));
       display.print("S: "); display.print(payload.sensor1); display.print(" V: "); display.println(payload.value1);
    }

}
