# RF24Hub
A gateway and controler for Arduino nodes with nrf24l01 (demo nodes included)
Main features:
 - sould run on all linux based systems with a connected nrf24l01 (i use raspberry pi version 2)
 - runs as a bachground service (deamon) or in interactive mode
 - controls the communication to the nodes using RF24Network
 - all informations are stored in a MariaDB database
 - can be controled by telnet commands
 - can send telnet commands to a third party controler (i use FHEM)
 - configuration via config file

More details (german only): https://wilmie.myhome-server.de/wiki/index.php?title=RF24Hub

Environment:
============
- Server: Raspberry PI
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
0. prerequisite

   MariaDB is installed
   You created a user and a database to be used with rf24hub

1. Go to your development directory

   cd ~/entw

2.  Clone the RF24 Repo

    git clone https://github.com/nRF24/RF24.git RF24

3.  Change to the RF24 folder and compile it

    cd RF24
    sudo make install

4. Clone the RF24Network Repo

    git clone https://github.com/nRF24/RF24Network.git RF24Network

5. Change to the RF24Network folder and compile it

    cd RF24Network

    sudo make install

6. Clone the sensorhub Repro

   git clone https://github.com/wilmsn/RF24Hub.git RF24Hub

7. Change to the sensorhub folder and compile it

   cd RF24Hub

   make

   install tables from create_tab.sql

8. Do a test run

   Have a least one node ready for test
   Add the configuration of this node to the table node inside the database
   Add at least one sensor to the table sensor

   ./rf24hubd -v9  #Just stop it with ctrl-c

9. If everything works, install it

   sudo make install

Now its up to you:
==================
Feel free to fork, use or modify it.
=======

