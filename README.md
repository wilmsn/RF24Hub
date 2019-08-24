# RF24Hub
A gateway and controler for Arduino nodes with nrf24l01 (demo nodes included)
Main features:
 - the controler sould run on all linux based systems 
 - the gateway should run on a system where the nrf24l01 is connected (i tested raspberry pi, ESP8266)
 - runs as a background service (deamon) or in interactive mode
 - controls the communication to the nodes using RF24 library
 - all informations are stored in a MariaDB database
 - can be controled by telnet commands
 - can send telnet commands to a third party controler (i use FHEM)
 - configuration via config file

Attention: This Brunch uses a different payload structure!!!!!
==============================================================
More details (german only): https://wilmie.myhome-server.de/wiki/index.php?title=RF24Hub

Environment:
============
- Server: Raspberry PI
- Gateway: ESP8266
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

2.  Clone the RF24 Repo

    git clone https://github.com/nRF24/RF24.git RF24

3.  Change to the RF24 folder and compile it

    cd RF24
    sudo make install

4. Clone the RF24Hub Repro

   git clone https://github.com/wilmsn/RF24Hub.git RF24Hub

5. Change to the RF24Hub folder and compile it

   cd RF24Hub

   make

   install tables from create_tab.sql

6. Do a test run

   Have a least one node ready for test
   Add the configuration of this node to the table node inside the database
   Add at least one sensor to the table sensor

   sudo ./rf24hubd -v9  #Just stop it with ctrl-c

7. If everything works, install it

   sudo make install

Now its up to you:
==================
Feel free to fork, use or modify it.
=======

