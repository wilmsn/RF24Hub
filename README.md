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

