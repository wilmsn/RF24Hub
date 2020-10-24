Class Documentation
*******************

Class Cfg
---------
In der Class Cfg sind Funktionen zur Programmkonfiguration gebündelt.

.. doxygenclass:: Cfg
   :project: RF24HUB
   :members:


Class Node
----------
In der Class Node sind folgende Inhalte zu finden:

* Ein speicherbasierte, verkettete Liste mit allen benötigten Informationen zu einem Node. Diese wird beim Programmaufruf aus der Datenbank befüllt. Bei einem normalen Programmende werden die relevanten Daten in die Datenbank zurückgeschrieben.

* Funktionen zum Umgang mit Nodes

.. doxygenclass:: Node
   :project: RF24HUB
   :members: 

Class Sensor
------------
In der Class Sensor sind folgende Inhalte zu finden:

* Ein speicherbasierte, verkettete Liste mit allen benötigten Informationen zu einem Sensor. Diese wird beim Programmaufruf aus der Datenbank befüllt. Bei einem normalen Programmende werden die relevanten Daten in die Datenbank zurückgeschrieben.

* Funktionen zum Umgang mit Sensoren

.. doxygenclass:: Sensor
   :project: RF24HUB
   :members:

Class Order
-----------
Eine Order ist eine Sammlung von "Set" Befehlen für Sensoren. Bei der derzeitigen Payloadstruktur können bis zu 6 Set Befehle 
in einer Order gepackt werden.

In der Class Order sind folgende Inhalte zu finden:

* Ein speicherbasierte, verkettete Liste mit allen benötigten Informationen zu einem Order(=Datenlieferanfrage an einen Node). 

Eine Order kann durch 2 Ereignisse erzeugt werden:

#. Eintreffender Heartbeat und Vorhandesein von Einträgen für diesen Node im Orderbuffer.

#. Empfang einer "Setlast" Anweisung für einen Always On Node. 

* Funktionen zum Umgang mit den Orders

.. doxygenclass:: Order
   :project: RF24HUB
   :members:

Class OrderBuffer
-----------------
Ein Orderbuffer beinhaltet genau eine Set Anweisung an einen Sensor.
Ein Orderbuffer wird immer dann angelegt wenn eine "Set Sensor xyz wert" eingeht.

In der Class OrderBuffer sind folgende Inhalte zu finden:

* Ein speicherbasierte, verkettete Liste mit allen benötigten Informationen zu einem OrderBuffer(=Set Anweisung an einen Sensor). 

* Funktionen zum Umgang mit den Orders

.. doxygenclass:: OrderBuffer
   :project: RF24HUB
   :members:

Class Database
-----------------

.. doxygenclass:: Database
   :project: RF24HUB
   :members:

   
   
