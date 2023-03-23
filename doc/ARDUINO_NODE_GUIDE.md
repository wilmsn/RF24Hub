# A howto to build an Avr node from the internal template
## A minimalistic Node without attached Sensor
### Node Schematic
![Layout minimalistic node](https://raw.githubusercontent.com/wilmsn/RF24Hub/master/doc/Avr_minimal_Schaltplan.png  "Schematic minimalistic Node")
### Node configuration
- Open the Node Sketch (avr/Node/Node.ino) in your favorite IDE (Arduino IDE works fine for me).
- Go to the file Node.ino
- In the lines above "// ------ End of configuration part ------------" we select the node to build. All other konfigurationlines **must** be commented out (with "//" at the beginning of the line)!
- look for **"//#define TESTNODE"** and remove the slashs (uncomment it)
Now you can compile it.

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

![Arduino Settings](https://raw.githubusercontent.com/wilmsn/RF24Hub/master/doc/Arduino_IDE_settings.png  "Arduino settings")

Important:

- We use internal clock (8 Mhz or less)
- We do not need a bootloader

