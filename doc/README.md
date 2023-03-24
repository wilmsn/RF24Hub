RF24Hub
=====
 
A gateway and controler for Arduino nodes with nrf24l01 (demo nodes included)

Main features:

- The Hub should run on every Linux System
- The Gateway sould run on Linux or ESP32/ESP8266 Systems with a connected nrf24l01 transceiver
- On Linux: runs as a bachground service (deamon) or in interactive mode
- Controls the communication to the nodes using the RF24 library
- all informations are stored in a MariaDB database (easy to change to another database system)
- can be controled by telnet commands
- can send telnet commands to a third party controler (i use FHEM)
- configuration via config file
- a node will run aprox. 1..2 years on 2 AA Batteries

![alt text](https://raw.githubusercontent.com/wilmsn/RF24Hub/master/doc/overview.png "RF24Hub overview")

## Environment:
- Server: Linux host (eg. Raspberry PI or your Linux computer)
- Gateway: Raspberry Pi or ESP8266/ESP32
- Nodes: "Arduino" like build with minimal cost based on ATmega328P

## Aim of the project:
Build up a network of sensors and store and process the data on a server.
With:
- minimal hardware cost
- only free software
- low energy cost

## Guides
- [Quick Start](doc/quickstartguide.html)

- [Database](doc/dbguide.html)

- [Avr Node](doc/avrguide.html)

- [ESP Node](doc/espguide.html)

## Documentation Parts
- [Linux Programs](linux/index.html)

- [Avr Node](avr/index.html)

- [ESP Node](esp/index.html)


**Now its up to you:**
**Feel free to fork, use or modify it.**

