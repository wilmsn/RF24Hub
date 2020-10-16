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

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#define ZF_SENSOR_NO        0b11111110000000000000000000000000
#define ZF_ZAHL_NEGATIV     0b00000001000000000000000000000000
#define ZF_EXPO_NEGATIV     0b00000000100000000000000000000000
#define ZF_EXPO_WERT        0b00000000011110000000000000000000
#define ZF_ZAHL_WERT        0b00000000000001111111111111111111
#define ZF_ZAHL_WERT_INT    0b00000000000000000111111111111111
#define ZF_ZAHL_WERT_UINT   0b00000000000000001111111111111111

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
 * Bit 14..32   Mantisse (0..100000)
 ***********************************/

char* unpackData(uint32_t data, char* buf);

/***************************************************
 * Extrahiert den Datentyp auf Basis des verwendeten 
 * Channels 
 * Dabei gilt folgende Zuordnung:
 * 0 => ungueltiger Wert
 * 1 => float Value
 * 2 => integer Value
 * 3 => unsigned int Value
 * 4 => Character
 ***************************************************/
uint8_t getDataTyp(uint8_t channel);

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
 * Hier: Integer (15 Bit + Vorzeichen)
 ***************************************************/
int16_t getValue_i(uint32_t val);

/***************************************************
 * Extrahiert den Sensorwert aus dem Transportwert
 * Hier: unsigned int (16 Bit)
 ***************************************************/
uint16_t getValue_ui(uint32_t val);

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
uint32_t calcTransportValue_ui(uint8_t sensor, uint16_t value);  

/******************************************************
 * Verpackt die Sensornummer und den Messwert zu einem 
 * TransportWert des Datentyps uint32_t.
 * Sensor: gültige Werte zwischen 1..127
 * Value: gültige Werte: -32,768 to 32,767
 ******************************************************/
uint32_t calcTransportValue_i(uint8_t sensor, int16_t value);

/******************************************************
 * Verpackt die Sensornummer und die ersten 3 Zeichen
 * von value zu einem TransportWert 
 * des Datentyps uint32_t.
 * Sensor: gültige Werte zwischen 1..127
 * Value: 1 bis 3 char, der Rest wird ignoriert
 * Pos: bei Übergabe Startposition, 
 *      bei Rückgabe Position des ersten 
 *                   nicht verarbeteten Zeichens
 ******************************************************/
uint32_t calcTransportValue_c(uint8_t sensor, char* value, uint16_t* pos);


#endif
