# RF24Hub Linux Components


Documentation startpage: [rf24hub on github.io](../index.html)


## rf24hubd
Der RF24Hub (rf24hubd) ist das zentrale Programm zur Steuerung der Kommunikation mit den Nodes und zur Speicherung der Sensordaten.
[Flussplan rf24hub](../rf24hub_flow.html) 

## rf24gwd
Der RF24GW (rf24gwd) nimmt die Funkdaten des RF24 Netzes entgegen, wandelt sie in einen UDP Datensatz um und leitet diesen mittels TCP/IP Netzwerk an den Hub weiter. UDP Datensätze vom Hub werden in Gegenrichtung an das RF24 Netz weitergeleitet. Im Gateway ist keine verarbeitende Logik eingebaut.
