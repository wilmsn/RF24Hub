# A howto to build an Avr node from the internal template

**RF24Hub startpage: [wilmsn.github.io/rf24hub](index.html)**

## A minimalistic Node without attached Sensor

### Node Schematic

![Layout minimalistic node](https://wilmsn.github.io/rf24hub/Avr_minimal_Schaltplan.png  "Schematic minimalistic Node")

### Node configuration

- Open the Node Sketch (avr/Node/Node.ino) in your favorite IDE (Arduino IDE works fine for me).

- Go to the file Node.ino

- In the lines above 

	// ------ End of configuration part ------------
	
 we select the node to build. All other konfigurationlines **must** be commented out (with "//" at the beginning of the line)!

- look for **"//#define TESTNODE"** and remove the slashs (uncomment it)

Here are the configurations behind TESTNODE (file Node_settings.h)

	#if defined(TESTNODE)
	#define RF24NODE                      100
	#define SLEEPTIME                     60
	#define EMPTYLOOPS                    0
	#define SENSOR_DUMMY
	#define DUMMY_TEMP                    33.3
	#endif

So we get a Node with id=100 that will send every 60 seconds a dummy value of "33.3" on channel "1"

### Board settings

I use "Mini Core" as the board template. To install it look at "https://github.com/MCUdude/MiniCore" for details.
Once the Mini Core is installed use this settings:

![Arduino Settings](https://wilmsn.github.io/rf24hub/Arduino_IDE_settings.png  "Arduino settings")

Important:

- We use internal clock (8 Mhz or less)
- We do not need a bootloader

### Compiling and uploading

If all the settings are done you can compile the sketch.

For transfering the compiled code onto the AVR microcontroler i use the USBASP Programmer. But there are lots of alternatives (eg. to use Arduino Uno as a programmer - google for it).

## Build a node to your needs

If you want to build your own node there are two posibilities to do it:

1. If all of the sensors you used are already implemented you can configure it easily.

2. If you used a new, unimplemented sensor you have to implement it first.

### Node configuration 

- Open the Node Sketch (avr/Node/Node.ino) in your favorite IDE (Arduino IDE works fine for me).

- Go to the file Node_settings.ino

* First set up your Node configuration
* Set up the basics:
* * Give the node a unique name, here: MYFIRSTNODE
* * Define a unique ID inside the network, here: 200
The result ooks like this:

~~~c++
//***********************************************
//    Individual settings
//-----------------------------------------------------
#if defined(MYFIRSTNODE)
#define RF24NODE            200
#endif
//-----------------------------------------------------
~~~

* Scroll down Node_settings.ino to check if a sensor is implemented.

In this case we will add a temerature sensor 18b20

Here is the definition from Node_settings.ino:

~~~c++
// Dallas 18B20 Sensor
#ifdef SENSOR_18B20
// Resolution for DS18B20 sensor
#ifndef SENSOR_18B20_RESOLUTION
#define SENSOR_18B20_RESOLUTION 9
#endif
// Delaytime for 18B20 measurement
#ifndef SENSOR_18B20_DELAYTIME
#define SENSOR_18B20_DELAYTIME  100
#endif
// Channel for temperature
#ifndef TEMP_18B20_CHANNEL
#define TEMP_18B20_CHANNEL     1
#endif
#endif
~~~

So the only thing we have to add to the node definition is:

~~~c++
#define SENSOR_18B20
~~~

Now we have configured the 18b20 sensor which will send the temperature on channel 1,has a resolution of 9 (see datasheet) and has a conversation delay of 100ms (see datasheet)

The resulting node configuration is:

~~~c++
//***********************************************
//    Individual settings
//-----------------------------------------------------
#if defined(MYFIRSTNODE)
#define RF24NODE            200
#define SENSOR_18B20
#endif
//-----------------------------------------------------
~~~

If you want to change the resulution or the delaytime just add the corespondending line from the configuratuin (#define ...) to your node configuration. This will overwrite the defaults!

### Implementing a new sensor

In this step you should be able to programm in c++.

You will need a library that can handle your sensor!

Usually there are some steps that have to be programmed.

In the following lines i show the steps for the 18b20 sensor.

1. Define a name for your sensor and get it running on Arduino board

The name will be **SENSOR_18B20**

2. Goto **Node.ino** and include the library

~~~c++
#if defined(SENSOR_18B20)
#include <OneWire.h>
#include <DallasTemperature.h>
#endif
~~~

3. Define Initial treatment of the sensor at startup

Define variables and objects:

~~~c++
#if defined(SENSOR_18B20)
OneWire oneWire(SENSOR_18B20); 
DallasTemperature s_18b20(&oneWire);
DeviceAddress sensorAddress;
float temp_18b20;
#endif
~~~

~~~c++
void setup(void) {


#if defined(SENSOR_18B20)
  s_18b20.begin(); 
  s_18b20.setWaitForConversion(false);
  for(byte i=0; i<s_18b20.getDeviceCount(); i++) {
      if(s_18b20.getAddress(sensorAddress, i)) {
        s_18b20.setResolution(sensorAddress, SENSOR_18B20_RESOLUTION);
      }
    }
#endif

}
~~~

4. Define cyclic actions

~~~c++
void get_sensordata(void) {

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

}
~~~