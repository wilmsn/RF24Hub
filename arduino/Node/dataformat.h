/**
 * @file dataformat.h
 * @brief As we use our own dataformat it is defined here
 * 
 * Stellt die Funktionen bereit um das Zahlenformat
 * für die Übertragung aufzubereiten und nach der
 * Übertragung wieder zu dekodieren
 * 
 * Genauigkeit: 1 von 100.000 = 0,01 Promille
 * 
 */
#ifndef _DATAFORMAT_H_   /* Include guard */
#define _DATAFORMAT_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "rf24_config.h"
//#include "config.h"

/**
 * @defgroup datafmt Definitions to the own dataformat
 * @addtogroup datafmt
 * @brief All precompiler macros for the dataformat start with "ZF_"
 * @{
 */

/// Rückgabewerte der Funktion getDataType(data)
/// Hier: float
#define ZF_FLOAT            0

/// Rückgabewerte der Funktion getDataType(data)
/// Hier: Signed Integer 16 bit
#define ZF_INT              1

/// Rückgabewerte der Funktion getDataType(data)
/// Hier: Unsigned Integer 16 bit
#define ZF_UINT             2

/// Rückgabewerte der Funktion getDataType(data)
/// Hier: Char
#define ZF_CHAR             3

/// Rückgabewerte der Funktion getDataType(data)
/// Hier: Unbekannt
#define ZF_UNKNOWN          9

/// Anzahl der Rechtsschiebungen um den Channel als normale Zahl darzustellen
/// channel = (data & ZF_CHANNEL) >> ZF_SHIFT_CHANNEL
#define ZF_SHIFT_CHANNEL    23

/// Anzahl der Rechtsschiebungen um den Exponenten (bei float) als normale Zahl darzustellen
/// exponent = (data & ZF_EXPO_WERT) >> ZF_SHIFT_EXPO
#define ZF_SHIFT_EXPO       17

/// Anzahl der Rechtsschiebungen um den Channel als normale Zahl darzustellen
/// dataformat = (data & ZF_FORMAT) >> ZF_FORMAT
#define ZF_SHIFT_FORMAT     30

/// Datenformat float zur Bildung von data im eigenen Format
#define ZF_FORMAT_FLOAT     0b00000000000000000000000000000000

/// Datenformat integer zur Bildung von data im eigenen Format
#define ZF_FORMAT_INT       0b01000000000000000000000000000000

/// Datenformat unsigned integer zur Bildung von data im eigenen Format
#define ZF_FORMAT_UINT      0b10000000000000000000000000000000

/// Datenformat char zur Bildung von data im eigenen Format
#define ZF_FORMAT_CHAR      0b11000000000000000000000000000000

/// Digits in denen beim eigenen Zahlenformat der Datentyp gespeichert wird 
#define ZF_FORMAT           0b11000000000000000000000000000000

/// Digits in denen beim eigenen Zahlenformat der Channel gespeichert wird 
#define ZF_CHANNEL          0b00111111100000000000000000000000

/// Digit in dem beim eigenen Zahlenformat das Vorzeichen der Zahl (1 = negativ) gespeichert wird 
#define ZF_ZAHL_NEGATIV     0b00000000010000000000000000000000

/// Digit in dem beim eigenen Zahlenformat das Vorzeichen des Exponenten (1 = negativ) gespeichert wird 
#define ZF_EXPO_NEGATIV     0b00000000001000000000000000000000

/// Digits in denen beim eigenen Zahlenformat der Betrag des Exponenten gespeichert wird 
#define ZF_EXPO_WERT        0b00000000000111100000000000000000

/// Digits in denen beim eigenen Zahlenformat für den Datentyp Float der Betrag der Mantisse gespeichert wird 
#define ZF_ZAHL_WERT_FLOAT  0b00000000000000001111111111111111

/// Digits in denen beim eigenen Zahlenformat für den Datentyp Integer der Betrag der Zahl gespeichert wird 
#define ZF_ZAHL_WERT_INT    0b00000000000000000111111111111111

/// Digits in denen beim eigenen Zahlenformat für den Datentyp Unsigned Integer der Betrag der Zahl gespeichert wird 
#define ZF_ZAHL_WERT_UINT   0b00000000000000001111111111111111

/**
 * @brief Beschreibung des verwendeten Zahlenformates zur Übertragung (TransportWert)
 * Es handelt sich um ein 32 Bit unsigned Integer Format.
 * Der Sensorwert wird mit einer Genauigkeit von 17 Bit 
 * (131072 max, genutzt 100000 = 5 Stellen = 0,01 Promille) gespeichert.
 * Zahlenformat X * 10^Y
 * Format des Sensorwertes
 * Bitreihenfolge: Bit 1 (MSB) ... Bit 32 (LSB)
 * 
 * 1) FLOAT
 * Bit 1..2:    Verwendetes Zahlenformat (0b00)
 * Bit 3..9:    Sensornummer (1..127)
 * Bit 10:      Vorzeichen (0=positiv; 1=negativ)
 * Bit 11:      Vorzeichen Exponent (0=10^X; 1=10^-X)
 * Bit 12..15   Exponent (0..15)
 * Bit 16..32   Mantisse (0..100000)
 * 
 * 2) INTEGER
 * Bit 1..2:    Verwendetes Zahlenformat (0b01)
 * Bit 3..9:    Sensornummer (1..127)
 * Bit 10:      Vorzeichen (0=positiv; 1=negativ)
 * Bit 11..17:  ungenutzt
 * Bit 18..32   15 bit unsigned Integer (0..32767)
 * 
 * 3) UNSIGNED INTEGER
 * Bit 1..2:    Verwendetes Zahlenformat (0b10)
 * Bit 3..9:    Sensornummer (1..127)
 * Bit 11..16:  ungenutzt
 * Bit 17..32   16 bit unsigned Integer (0..65536)
 * 
 * 4) CHARACTER (2 Characters)
 * Bit 1..2:    Verwendetes Zahlenformat (0b11)
 * Bit 3..9:    Sensornummer (1..127)
 * Bit 11..16:  ungenutzt
 * Bit 17..32   2*8 bit Char
 *
 */

/**
 * Diese Funktion ist nur für das Debugging des Zahlenformates bestimmt !!!!!
 */
void printBits(size_t const size, void const * const ptr);

/**
 * Verpackt die Sensornummer und den Messwert zu einem 
 * TransportWert des Datentyps uint32_t.
 * @param channel Der Channel, gültige Werte zwischen 1..127
 * @param value: gültige Werte: -1*10^19 .. 1*10^19
 * @return Der Transportwert
 */
uint32_t calcTransportValue(uint8_t channel, float value);

/**
 * Verpackt die Sensornummer und den Messwert zu einem 
 * TransportWert des Datentyps uint32_t.
 * @param channel Der Channel, gültige Werte zwischen 1..127
 * @param value: Der Wert, gültige Werte: 0 .. 65535
 * @return Der Transportwert
 */
uint32_t calcTransportValue(uint8_t channel, uint16_t value);  

/**
 * Verpackt die Sensornummer und den Messwert zu einem 
 * TransportWert des Datentyps uint32_t.
 * @param channel gültige Werte zwischen 1..127
 * @param value: gültige Werte: -32,768 to 32,767
 * @return Der Transportwert
 */
uint32_t calcTransportValue(uint8_t channel, int16_t value);

/**
 * Verpackt die Sensornummer und 2 Zeichen zu einem TransportWert 
 * des Datentyps uint32_t.
 * @param channel gültige Werte zwischen 1..127
 * @param value1: Ein beliebiger Character (8bit)
 * @param value2: Ein beliebiger Character (8bit)
 * @return Der Transportwert
 ******************************************************/
uint32_t calcTransportValue(uint8_t channel, char* value1, char* value2);

/**
 * Entpackt die übertragenen Daten
 * @note Der übergebene **buf** muss gross genug sein um den Rückgabewert aufzunehmen
 * 
 */
char* unpackTransportValue(uint32_t data, char* buf);

//#if defined(__linux__)

/**
 * Diese Funktion packt die Daten entsprechend des übergebenen Datatypes ein.
 * @note Diese Funktion läuft wegen den verwendetetn C Funktionen nur auf Linux Systemen
 * @param channel Der Channel
 * @param value Der Wert als String
 * @param dataType Der Datentyp
 * @return Den gepackten Transportwert
 */
uint32_t calcTransportValue(uint8_t channel, char* value, uint8_t dataType);

/**
 * Diese Funktion packt die Daten ein.
 * Ist der Value eine Fließkommazahl (enthält einen "." als Dezimaltrenner) wird der Datatype FLOAT verwendet,
 * sonst int16.
 * @note Diese Funktion läuft wegen den verwendetetn C Funktionen nur auf Linux Systemen
 * @param channel Der Channel
 * @param value Der Wert als String
 * @return Den gepackten Transportwert
 */
uint32_t calcTransportValue(uint8_t channel, char* value);

//#endif

/**
 * Extrahiert den Datentyp aus den Transportdaten 
 * Dabei gilt folgende Zuordnung:
 * 0 => float Value
 * 1 => integer Value
 * 2 => unsigned int Value
 * 3 => Character
 * @param data Der Transportwert
 * @return Der verwendete Datentyp
 */
uint8_t getDataType(uint32_t data);

/**
 * Extrahiert die Channel aus dem Transportwert
 * @param data Der Transportwert
 * @return Der verwendete Channel
 */
uint8_t getChannel(uint32_t data);

/**
 * Extrahiert den Sensorwert aus dem Transportwert
 * Hier: Float
 * @param data Der Transportwert
 * @param zahl Ein Zeiger auf eine FLOAT Variable, hier wird das Ergebnis hinterlegt.
 * @return "true" wenn im Transportwert ein float eingepackt war, wurde eine Konvertierung durchgeführt wird ein "false" zurückgegeben.
 */
bool getValue(uint32_t data, float* zahl);

/**
 * Extrahiert den Sensorwert aus dem Transportwert
 * Hier: Integer (15 Bit + Vorzeichen)
 * @param data Der Transportwert
 * @param zahl Ein Zeiger auf eine int16_t Variable, hier wird das Ergebnis hinterlegt.
 * @return "true" wenn im Transportwert ein int16_t eingepackt war, wurde eine Konvertierung durchgeführt wird ein "false" zurückgegeben.
 */
bool getValue(uint32_t data, int16_t* zahl);

/**
 * Extrahiert den Sensorwert aus dem Transportwert
 * Hier: unsigned int (16 Bit)
 * @param data Der Transportwert
 * @param zahl Ein Zeiger auf eine uint16_t Variable, hier wird das Ergebnis hinterlegt.
 * @return "true" wenn im Transportwert ein uint16_t eingepackt war, wurde eine Konvertierung durchgeführt wird ein "false" zurückgegeben.
 */
bool getValue(uint32_t data, uint16_t* zahl);

#endif
