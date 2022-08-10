# RF24Hub
A gateway and controler for Arduino nodes with nrf24l01 (demo nodes included)
Main features:
 - sould run on all linux based systems with a connected nrf24l01 (i use raspberry pi version 2 and 4)
 - runs as a bachground service (deamon) or in interactive mode
 - controls the communication to the nodes using RF24Network
 - all informations are stored in a MariaDB database
 - can be controled by telnet commands
 - can send telnet commands to a third party controler (i use FHEM)
 - configuration via config file

![alt text](https://raw.githubusercontent.com/wilmsn/RF24Hub/master/doc/overview.png "RF24Hub overview")

More details (german only): http://wilmsn.github.io/rf24hub

Environment:
============
- Server: Raspberry PI
- Gateway: Raspberry Pi or ESP8266
- Nodes: "Arduino" like build with minimal cost based on ATmega328P

Aim of the project:
===================
Build up a network of sensors and store and process the data on a server.
With:
- minimal hardware cost
- only free software
- low energy cost

Quick start guide:
=================
0. Prerequisite

   MariaDB is installed
   You created a user and a database to be used with rf24hub

1. Go to your development directory

   cd ~/entw

2. Clone the RF24 Repo

   git clone https://github.com/nRF24/RF24.git RF24

3. Change to the RF24 folder and compile it

   cd RF24
   sudo make install

4. Clone the RF24Hub Repro

   git clone https://github.com/wilmsn/RF24Hub.git RF24Hub

5. Change to the RF24Hub folder and compile it

   cd RF24Hub

   make

   install tables from create_tab.sql

8. Do a test run

   Have a least one node ready for test
   Add the configuration of this node to the table node inside the database
   Add at least one sensor to the table sensor
   Edit the config file: rf24hub.cfg

   Start the Hub:

   sudo ./rf24hubd -c rf24hub.cfg # Just stop it with ctrl-c

   Open a second terminal and go to the same dir as before

   Start the Gateway:

   sudo ./rf24gwd -v +rf24 -c rf24hub.cfg # Just stop it with ctrl-c

9. If everything works, install it

   sudo make install

   Please be sure to edit "/etc/rf24hub/rf24hub.cfg" to your needs and restart the deamon after that:
   "sudo systemctl stop rf24hub; sudo systemctl start rf24hub" after that

Now its up to you:
==================
Feel free to fork, use or modify it.
=======

