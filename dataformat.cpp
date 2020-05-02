#include "dataformat.h"

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
uint8_t getChannel(uint32_t val) {
  val &= ZF_SENSOR_NO;
  val >>= 25;
  return val;
}

/***************************************************
 * Extrahiert den Sensorwert aus dem Transportwert
 * Hier: Float
 ***************************************************/
float getValue_f(uint32_t val) {
  uint32_t exponent = (val & ZF_EXPO_WERT) >> 19;
  bool expo_negativ = val & ZF_EXPO_NEGATIV;
  bool zahl_negativ = val &  ZF_ZAHL_NEGATIV;
  float retval;
  retval = val & ZF_ZAHL_WERT;
  if ( expo_negativ ) {
    for (uint8_t i=exponent;i>0;i--) {
      retval /= 10.0;
    }
  } else {
    for (uint8_t i=exponent;i>0;i--) {
      retval *= 10.0;
    }    
  }
  if ( zahl_negativ ) {
      retval *= -1.0;
  }
  return retval;
}


/***************************************************
 * Extrahiert den Sensorwert aus dem Transportwert
 * Hier: Float
 ***************************************************/
uint16_t getValue_i(uint32_t val) {
  uint16_t retval;
  retval = val & ZF_ZAHL_WERT;
  return retval;
}

/******************************************************
 * Verpackt die Sensornummer und den Messwert zu einem 
 * TransportWert des Datentyps uint32_t.
 * Sensor: gültige Werte zwischen 1..127
 * Value: gültige Werte: -1*10^19 .. 1*10^19
 ******************************************************/
uint32_t calcTransportValue_f(uint8_t sensor, float value) {  
  float _val = value;
  uint32_t exponent=0;
  bool expo_negativ = false;
  uint32_t result = 0;
  result = sensor;
  result <<= 25; 
  if ( value > 0.00001 || value < -0.00001 ) {
    bool negativ = value < 0.0;
    if ( negativ ) {
      result |= ZF_ZAHL_NEGATIV;
      _val=abs(_val);
    }
    while ( _val < 50000.0 ) {
      expo_negativ = true;
      exponent++;
      _val*=10.0;
    }
    if ( expo_negativ ) {
      result |= ZF_EXPO_NEGATIV;
    }
    while ( _val > 500000.0 ) {
      exponent++;
      _val/=10.0;
    }
    exponent <<= 19;
    result |= exponent;
    result |= (uint32_t) _val;
  }
  return result; 
}

/******************************************************
 * Verpackt die Sensornummer und den Messwert zu einem 
 * TransportWert des Datentyps uint32_t.
 * Sensor: gültige Werte zwischen 1..127
 * Value: gültige Werte: 0 .. 65535
 ******************************************************/
uint32_t calcTransportValue_i(uint8_t sensor, uint16_t value) {  
  uint32_t result = 0;
  result = sensor;
  result <<= 25; 
  result |= (uint32_t) value;
  return result; 
}
