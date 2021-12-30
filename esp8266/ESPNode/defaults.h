// Part 1: global defaults
#define MAGICNO                 1435
#define HOSTNAME                "testnode"
// Infosize wird für den info_str benötigt. Hier werden z.B. die JSON Strings gespeichert.
// Der derzeit größte String ist die Framebufferabbildung (32*24+1 Byte)!!!
#define INFOSIZE                1000
#define LOGGER_NUMLINES         25
#define LOGGER_LINESIZE         250

// MQTT
#define MQTT
#define MQTT_SERVER             "rpi2.fritz.box"
#define MQTT_NODENAME           "testnode"
#define MQTT_CMD                "cmnd"
#define TOPIC_PART1_SIZE        5
#define TOPIC_PART2_SIZE        20
#define TOPIC_PART3_SIZE        20
#define STATINTERVAL            300000
#define TELEINTERVAL            900000
#define TOPIC_BUFFER_SIZE       50
#define MQTT_CMD                "cmnd"
#define TOPIC_PART1_SIZE        5
#define TOPIC_PART2_SIZE        20
#define TOPIC_PART3_SIZE        20

// Sensors and Actors
#define NOSENSOR
#define SWITCH1_DEFAULT         LOW
#define SWITCH2_DEFAULT         LOW
#define SWITCH3_DEFAULT         LOW
#define SWITCH4_DEFAULT         LOW
#define ONE_WIRE_BUS            4



// Part 2: individual defaults
// LEDMATRIX 
#define LEDMATRIX_CLK           5  /* D1 */
#define LEDMATRIX_CS            2  /* D4 */
#define LEDMATRIX_DIN           0  /* D3 */
#define LEDMATRIX_DEVICES_X     4
#define LEDMATRIX_DEVICES_Y     3
#define LINE_SIZE               10
#define GRAFIK_SIZE             40
#define TEXT_SIZE               15
#define LED_MATRIX_ANIM_DELAY   100

// for rf24gw
#define RF24_HUB_SERVER              "192.168.178.212"
#define RF24_HUB_UDP_PORTNO          7004
#define RF24_GW_UDP_PORTNO           7003
#define RF24_RADIO_CE_PIN            15
#define RF24_RADIO_CSN_PIN           16
#define RF24_GW_VERBOSECRITICAL          0b0000000000000001
#define RF24_GW_VERBOSESTARTUP           0b0000000000000010
#define RF24_GW_VERBOSERF24              0b0000000010000000
#define RF24_GW_STARTUPVERBOSELEVEL      0b0000000000000011 

// for witty cloud
#define WITTY_RGB_BL            13
#define WITTY_RGB_GN            12
#define WITTY_RGB_RT            15
#define WITTY_INT_LED           2
#define WITTY_LDR               A0

// for Neopixel
#define NEOPIXELPIN             2
