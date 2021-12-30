// The CE Pin of the Radio module
#define RADIO_CE_PIN    10
// The CS Pin of the Radio module
#define RADIO_CSN_PIN   9
// The pin of the statusled
#define STATUSLED 3
#define STATUSLED_ON  HIGH
#define STATUSLED_OFF LOW
// Relais
#define RELAIS_ON     HIGH
#define RELAIS_OFF    LOW
// A dummy temperature
#define DUMMY_TEMP    33.3
// One Wire Bus
#define ONE_WIRE_BUS  8
// Resolution for DS18B20 sensor
#define DS18B20_RESOLUTION 9
// Delaytime ffor 18B20 measurement
#define DS18B20_DELAYTIME  100
// Altitude above sealevel
#define ALTITUDEABOVESEALEVEL   95.0
// Neopixel
// Arduino Pin where the Data-Pin from the Neopixels is connected to
#define NEOPIXEL_PIN 7
// Default Color Values for the Neopixel
#define NEOPIXEL_R_DEFAULT 100
#define NEOPIXEL_G_DEFAULT 100
#define NEOPIXEL_B_DEFAULT 100
// Sleeptime in Seconds !! 
// (valid: 10 ... 32.400)
#define SLEEPTIME_SEC   120
// adjust the length of a second (aprox. in ms) - will be used as a factor to sleep4ms()
// (valid: 500 ... 2000)
#define SLEEP4MS_FAC   1000
// number of empty loop after sending data
// (valid: 0...20)
#define EMPTYLOOPS      0
// waiting time between 2 transmissions in ms
// (valid 100 ... 1000)
#define SENDDELAY       200
// Max number of attempts to send for a nomal message!!
// (valid 1 ... 20)
#define MAX_SENDCOUNT   10
// Max number of attempts to send for a stop message!!
// (valid 1 ... 20)
#define MAX_STOPCOUNT   3
// Voltage faktor will be multiplied to the messured value !!!!
// (valid 0.1 ... 10)
#define VOLT_FAC        1.0
// Voltage offset will be added to messured value * volt_fac !!!
// (valid -10 ... 10)
#define VOLT_OFF        0
// Define low voltage level on processor
// below that level the thermometer will be switched off 
// until the battery will be reloaded
#define LOW_VOLT_LEVEL 2
// In case of low Voltage send every X Seconds
// 90 => every 3 hours (sleeptime 120)
#define LOW_VOLT_LOOPS  90
// 5 voltages for the battery (empty ... full)
#define U0 3.6
#define U1 3.7
#define U2 3.8
#define U3 3.9
#define U4 4.0
//
// Definitions for Nokia 5110 Display
//
// DISPLAY Nokia 5110 is 84 * 48 Pixel
#define N5110_CLK         7
#define N5110_DIN         6
#define N5110_DC          5
#define N5110_RST         2
#define N5110_CE          4
// Kontrast of the display
#define CONTRAST 65
// Brightnes of the display - unused until now
#define BRIGHTNES 0
//  012345678901234567890123456789012345678901234567890123456789012345678901234567890123
// 0                                                                           BBBBBBBBB     B = Batterie
// 1                                                                          BBBBBBBBBB
// 2                                                                          BBBBBBBBBB
// 3                                                                          BBBBBBBBBB
// 4                                                                           BBBBBBBBB
// 5
// 6                                                                          AAAAAAAAAA     A = Antenna
// 7                                                                          AAAAAAAAAA
// 8                                                                          AAAAAAAAAA
// 9                                                                          AAAAAAAAAA
// 0                                                                          AAAAAAAAAA
// 1                                                                          AAAAAAAAAA
// 2                                                                          AAAAAAAAAA
// 3                                                                          AAAAAAAAAA
// 4                                                                          AAAAAAAAAA
// 5                                                                          AAAAAAAAAA
// 6
// 7                                                                           TT   HHHH      T = Thermometer
// 8                                                                           TT   HHHH 
// 9                                                                           TT   HHHH      H = Heatbeat countdown
// 0                                                                           TT   HHHH      
// 1                                                                           TT   HHHH
// 2                                                                          TTTT  HHHH
// 3                                                                          T  T  HHHH
// 4                                                                          TTTT  HHHH
// 5
// 6
// 7------------------------------------------------------------------------------------
// 8|                                        ||                                        |
// 9|                                        ||                                        |
// 0|                                        ||                                        |
// 1|                                        ||                                        |
// 2|        Field 1                         ||           Field 2                      |
// 3|                                        ||                                        |
// 4|                                        ||                                        |
// 5|                                        ||                                        |
// 6|                                        ||                                        |
// 7------------------------------------------------------------------------------------
// 8|                                        ||                                        |
// 9|                                        ||                                        |
// 0|                                        ||                                        |
// 1|        Field 3                         ||           Field 4                      |
// 2|                                        ||                                        |
// 3|                                        ||                                        |
// 4|                                        ||                                        |
// 5|                                        ||                                        |
// 6|                                        ||                                        |
// 7------------------------------------------------------------------------------------
// Define the location of the display symbols
// set X0 and Y0 of battery symbol ( is 10 * 5 pixel )
#define BATT_X0 74
#define BATT_Y0 0
// set X0 and Y0 of antenna symbol ( is 10 * 10 pixel )
#define ANT_X0 74
#define ANT_Y0 6
// set X0 and Y0 of thermometer symbol ( is 3 * 6 pixel )
#define THERM_X0 74
#define THERM_Y0 17
// set X0 and Y0 of HB countdown symbol ( is 4 * 10 pixel )
#define HB_X0 80
#define HB_Y0 17
//
// END Definition Nokia 5110 Display
//
// Register of RF24 Nodes
//
