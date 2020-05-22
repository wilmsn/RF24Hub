/*************************************
 * Stellt die Funktionen bereit um das Zahlenformat
 * für die Übertragung aufzubereiten und nach der
 * Übertragung wieder zu dekodieren
 * 
 * Genauigkeit: 1 von 100.000 = 0,01 Promille
 * 
 *************************************/
#ifndef _DATAFORMAT_H_   /* Include guard */
#define _DATAFORMAT_H_

#if defined (ARDUINO)
#else
#include <stdint.h>
#include <cstring>
#include <ctype.h> 
//#include <time.h>
//#include <sys/time.h>
#include "rf24hub_config.h"
#endif

#include <stdlib.h>
#include <stdbool.h>

#define ZF_SENSOR_NO        0b11111110000000000000000000000000
#define ZF_ZAHL_NEGATIV     0b00000001000000000000000000000000
#define ZF_EXPO_NEGATIV     0b00000000100000000000000000000000
#define ZF_EXPO_WERT        0b00000000011110000000000000000000
#define ZF_ZAHL_WERT        0b00000000000001111111111111111111

/************************************
 * Das Ergebnis wird mit einer Genauigkeit von 16 Bit 
 * (131072 max, genutzt 100000 = 5 Stellen = 0,01 Promille) gespeichert.
 * Zahlenformat X * 10^Y
 * Format des Sensorwertes
 * Bitreihenfolge: Bit 1 (MSB) ... Bit 32 (LSB)
 * Bit 1..7:    Sensornummer (1..127)
 * Bit 8:       Vorzeichen (0=positiv; 1=negativ)
 * Bit 9:       Vorzeichen Exponent (0=10^X; 1=10^-X)
 * Bit 10..13   Exponent (0..15)
 * Bit 14..15   Reserviert / ungenutzt
 * Bit 16..32   Mantisse (0..10000)
 ***********************************/


/***************************************************
 * Extrahiert die Sensornummer aus dem Transportwert
 ***************************************************/
uint8_t getChannel(uint32_t val);

/***************************************************
 * Extrahiert den Sensorwert aus dem Transportwert
 * Hier: Float
 ***************************************************/
float getValue_f(uint32_t val);

/***************************************************
 * Extrahiert den Sensorwert aus dem Transportwert
 * Hier: Float
 ***************************************************/
uint16_t getValue_i(uint32_t val);

/******************************************************
 * Verpackt die Sensornummer und den Messwert zu einem 
 * TransportWert des Datentyps uint32_t.
 * Sensor: gültige Werte zwischen 1..127
 * Value: gültige Werte: -1*10^19 .. 1*10^19
 ******************************************************/
uint32_t calcTransportValue_f(uint8_t sensor, float value);

/******************************************************
 * Verpackt die Sensornummer und den Messwert zu einem 
 * TransportWert des Datentyps uint32_t.
 * Sensor: gültige Werte zwischen 1..127
 * Value: gültige Werte: 0 .. 65535
 ******************************************************/
uint32_t calcTransportValue_i(uint8_t sensor, uint16_t value);  

#endif
