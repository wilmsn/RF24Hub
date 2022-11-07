RF24Hub {#mainpage}
=======

A gateway and controler for Arduino nodes with nrf24l01 (demo nodes included)
Main features:
- The Hub shoul run on every Linux System
- The Gateway sould run on a Linux Sased or ESP32/ESP8266 Systems with a connected nrf24l01
- On Linux: runs as a bachground service (deamon) or in interactive mode
- Controls the communication to the nodes using RF24Network
- all informations are stored in a MariaDB database (easy to change to another database system)
- can be controled by telnet commands
- can send telnet commands to a third party controler (i use FHEM)
- configuration via config file

![alt text](https://raw.githubusercontent.com/wilmsn/RF24Hub/master/doc/overview.png "RF24Hub overview")

More details (mainly german only): http://wilmsn.github.io/rf24hub

## Environment:
- Server: Raspberry PI
- Gateway: Raspberry Pi or ESP8266
- Nodes: "Arduino" like build with minimal cost based on ATmega328P

## Aim of the project:
Build up a network of sensors and store and process the data on a server.
With:
- minimal hardware cost
- only free software
- low energy cost


[Quick Start Guide](https://wilmsn.github.io/rf24hub/doc/quickstartguide.html)


**Now its up to you:**
**Feel free to fork, use or modify it.**

