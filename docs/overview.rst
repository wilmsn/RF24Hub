Overview
********

Project Overview: 
=================

.. image:: overview.png

Components
==========

Hub
---

The 'Hub' is the element that controls the whole network.
It can be any server witch runs linux

* i use a **Raspberry Pi**

Used Software:

* MariaDB as Database
* rf24hubd as a Deamon to control the communication

Gateway
-------

The gateway builds the interface bwtween the IP-Network and the 2.4GHZ Network.
I can run on any device with an IP-Interface where you can connect an nrf24l01 - I use a Raspberry Pi and an ESP8266 for it.

Used Software:

* rf24gwd (on Linux) or an Arduino Sketch (on ESP8266)

Node
----

The Node ist the thing which hosts all sensors and actors - I use an self build Arduino for it.

Used Software:

* Arduino Sketch

