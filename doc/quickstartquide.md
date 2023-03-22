# RF24Hub Quick Start Guide
## 0. Prerequisite
- MariaDB is installed
- Developmenttools are installed
- Arduino-IDE (or any other tool to compile the SW for the ATMEGA) is installed
- **Option 1**:
- - The nrf24l01 transceiver module is connected to your server/host
- **Option 2:**
- - Build an ESP32 or ESP8266 Node with the nrf24l01 transceiver module as Gateway
## 1. Go to your development directory
    cd ~/entw
## 2. (Option 1 only) Clone, Compile and Install the RF24 Repo</h2>

	git clone https://github.com/nRF24/RF24.git RF24
	cd RF24
	make && sudo make install
	
## 3. Clone and Compile the RF24Hub Repro

	git clone https://github.com/wilmsn/RF24Hub.git RF24Hub
	cd RF24Hub/linux
	#Option 1
	make
	#Option 2
	make rf24hubd

## 3a (Option 2 only) Build an ESP gateway

[Build an ESP-Node with Gateway](https://wilmsn.github.io/rf24hub/doc/espguide.html)

## 4. Setup Database

[Have a look at the DB-Guide](https://wilmsn.github.io/rf24hub/arduinoguide.html)

## 5. Do a test run
Edit the config file to your needs/environment: 

	rf24hub.cfg

**Start the Hub:**

	sudo ./rf24hubd -v +rf24 -c rf24hub.cfg # Just stop it with ctrl-c

**Option 1:**
Open a second terminal and go to the same dir as before and start the Gateway:

	sudo ./rf24gwd -v +rf24 -c rf24hub.cfg # Just stop it with ctrl-c
	
**Option 2:**
Power on the ESP Gateway.

If you see datapackets on the hub (and in Option 1 on the Gateway) everything looks good:

	[2022.11.05 19:31:16.152] G:103>H N:201 T:51 m:1 F:0x01 O:0 H:143 (79/3.164)(1/9.439)(2/1019.3)(3/88.80)(0/0.000)(0/0.000)
	
## 6. If everything works, install it

	#Please be sure to copy rf24hub.cfg to /etc/rf24hub/rf24hub.cfg
	sudo mkdir /etc/rf24hub
	sudo cp ./rf24hub.cfg /etc/rf24hub/rf24hub.cfg
	sudo make install
	
Restart the deamon after that:

	sudo systemctl stop rf24hub
	sudo systemctl start rf24hub
	
**Option 1:**
**Also restart the Gateway!**

	sudo systemctl stop rf24gw
	sudo systemctl start rf24gw
	    
**Enjoy!**


