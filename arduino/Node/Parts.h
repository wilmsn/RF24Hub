// do not include this file
// onl use it for cut and paste
//-----------------------------
// Minimum Node Definition
// for Node "MYNODE" => 
// replace the name to your needs

#if defined(MYNODE)
// Every Node needs a numeric identifier between 10 and 250
#define RF24NODE        17
#endif

// Thats the bare minimum
// You can add everything from below

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                  BASIC Node Configuration (all optional)
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Set the sleeptime of a node between 2 mesurements
// In this example the node will sleep 2 minutes
#define SLEEPTIME      120
// To fine adjust the sleeptime increase or degrease this 
#define SLEEP4MS_FAC   1000
// Emptyloops are loops without sending data to the hub
// In this example the node will send every 20 minutes
#define EMPTYLOOPS      9
// The delay time between two radio transmissions 
#define SENDDELAY       200
// The maximum number of radio transmissions for data messages before giving up
#define MAX_SENDCOUNT   3
// The maximum number of radio transmissions for stop messages before giving up
#define MAX_STOPCOUNT   3

// Battery measurement
//  Use external measurement of battery voltage on defined Pin 
#define BATTERY_READ_EXTERNAL     <A-pin>
// If the external voltage is higher than the VCC of the ATMega you will need
// 2 resistor to divide it. 
// R1 is connected between ground and the analog input pin
// R2 is connected between Vcc and the analog input pin
// Values are in KOhm
#define BATTERY_VOLTAGEDIVIDER_R1     0
#define BATTERY_VOLTAGEDIVIDER_R2     470
// Adjust the battery measurement
// Use a factor
#define VOLT_FAC        1.0
// Use a offset
#define VOLT_OFF        0

// Low Voltage Management
// Set the limit voltage for it, low voltage handling is below
#define VOLT_LV         2
// In Low Power State we use a different sleeptime 
#define SLEEPTIME_LV    1440

// Battery Management
// If the akku is loaded by solarcell we have 2 methods to consume power
// that the akku will not be overcharged
// 1. Step: The ATMega will NOT be send to low power mode if the voltage is above
#define DISCHARGE1          2.7
// 2. Step: The battery will additionly be discharged through the status LED (swiched on) if the voltage is above
#define DISCHARGE2          2.75
// 3. Step: The battery will additionly be discharged through a resistor on port DISCHARGE3_PIN against GND
#define DISCHARGE3          2.8
#define DISCHARGE3_PIN      5

// Load Ballancing
// If the node uses 2 battery cells in series the charging voltage of both batteries should not differ to much
// The load ballancer pin will be connected to a resistor ( eg. 470 Ohm ) and the resistor goes between the battery cells
// The load ballancer needs a analog input and will only work above the DISCHARGE1 voltage
#define LOAD_BALLANCER_PIN   A3
// The maximun difference voltage between the cells will be defined here
#define LOAD_BALLANCER       0.2


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                  Sensors (Input Devices)
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// 1: A dummy Sensor for Test Purposes - no HW needed
// Will act as a temperature Sensor
// mandatory:
#define SENSOR_DUMMY
// optional set the temperature
#define DUMMY_TEMP           33.3

// 2. A Dallas 18B20 Temperature Sensor
// mandatory: Set the Pin the Sensor is connected
#define SENSOR_18B20         8
// optional
// Set the resolution 9 ... 12 (see datasheet for details)
#define SENSOR_18B20_RESOLUTION 9
// Set the delaytime to process the measurement (see datasheet for details)
#define SENSOR_18B20_DELAYTIME  100

// 3. A Bosch BMP180, BMP280 or BME280 Sensor
// mandatory:
#define SENSOR_BOSCH
// optional
// For calculating the Presure at sea level we need the altitude
#define ALTITUDEABOVESEALEVEL   95.0
// define a channel for the temperature (default is 1)
#define TEMP_BOSCH_CHANNEL            4
// define a channel for the pressure (default is 2)
#define PRES_BOSCH_CHANNEL            5
// define a channel for the humidity (default is 3)
#define HUMI_BOSCH_CHANNEL            6

// 4. AHT20 Temperature and Humidity Sensor 
// mandatory:
#define SENSOR_AHT20
// optional
// define a channel for the temperature (default is 1)
#define TEMP_AHT20_CHANNEL            2
// define a channel for the humidity (default is 2)
#define HUMI_AHT20_CHANNEL            3

// 5. HTU2X Temperature and Humidity Sensor 
// mandatory:
#define SENSOR_HTU2X
// optional
// define a channel for the temperature (default is 1)
#define TEMP_HTU2X_CHANNEL            7
// define a channel for the humidity (default is 2)
#define HUMI_HTU2X_CHANNEL            8

// 6. A Solar Cell for measuring the solar power
// mandatory: Set the Analog-Pin the Solarcell is connected
// SOLARZELLE1 is connected with a resistor in parallel
// Umax (solarcell) must be less than Vcc or you have to use a voltage divider!!!!
#define SOLARZELLE1        <A-pin>
// SOLARZELLE2 is used to charge the internal akku
#define SOLARZELLE2        <A-pin>

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                  Actors (Output Devices)
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Status LED
// mandatory: Set the Pin the Status LED is connected
#define STATUSLED       <pin>
// optional: the logic level to switch on and off
#define STATUSLED_ON    HIGH
#define STATUSLED_OFF   LOW

// Display
// A Nokia 5110 Display
// connected to pin: CLK 7; DIN 6; DC 5; RST 2; CE 4
#define DISPLAY_5110
// optional: 
// set the contrast
#define CONTRAST 65

// Design Pattern for all Displays (all optional)
// The default layout has the Temperature in the top and 4 generic field in the bottom (default)
#define DISPLAY_LAYOUT_DEFAULT
// This Layout has the Temperature in the top and the Humidity in the bottom
#define DISPLAY_LAYOUT_TEMPHUMI

// Relais up to 4 pices
// mandatory: Set the Pin the Relais is connected
#define RELAIS_1    5
#define RELAIS_2    6
#define RELAIS_3    7
#define RELAIS_4    8
// optional: the logic level to switch on and off
#define RELAIS_ON     HIGH
#define RELAIS_OFF    LOW
