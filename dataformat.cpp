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

char* unpackTransportValue(uint32_t data, char* buf) {
    uint8_t dataTyp = getDataTyp( getChannel(data) );
    switch ( dataTyp ) {
        case 0:
            sprintf(buf,"%d",0);
        break;            
        case 1:  
        {
            float myval = getValue_f(data);
            if ( myval > 500 ) {
                sprintf(buf,"%.1f", myval);
            } else {
                if ( myval > 9.9 ) {
                    sprintf(buf,"%.2f", myval);
                } else {
                    sprintf(buf,"%.3f", myval);
                }   
            }
        }
        break;
        case 2:
        {
            sprintf(buf,"%d",getValue_i(data));
        }
        break;
        case 3:
        {
            sprintf(buf,"%u",getValue_ui(data));
        }
        break;
        case 4:
            // ToDo Wort kann ein kompletter Text sein, das in verschiedene Channels zerlegt wird
            //      Max Länge 20*3=60 Zeichen
        break;
    }
    return buf;
}

#if defined(__linux__) || defined(ESP8266)

uint32_t packTransportValue(uint8_t channel, char* value) {
    uint32_t retval = 0;
    uint8_t dataTyp = getDataTyp(channel);
    char* pEnd; 
    switch ( dataTyp ) {
        case 0:
        {
            retval = 0;
        }
        case 1:
        {
            float val_f = strtof(value, &pEnd);
            retval = calcTransportValue_f(channel, val_f);
        }
        break;
        case 2:
        {
            int16_t val_i = (int16_t)strtol(value, &pEnd, 10);
            retval = calcTransportValue_i(channel, val_i);
        }
        break;
        case 3:
        {
            uint16_t val_ui = (uint16_t)strtoul(value, &pEnd, 10);
            retval = calcTransportValue_ui(channel, val_ui);
        }
        break;
        case 4:
            // ToDo Wort kann ein kompletter Text sein, das in verschiedene Channels zerlegt wird
            //      Max Länge 20*3=60 Zeichen
        break;
    }
    return retval;
}
#endif

/***************************************************
 * Extrahiert den Datentyp auf Basis des verwendeten 
 * Channels 
 ***************************************************/
uint8_t getDataTyp(uint8_t channel) {
    uint8_t retval;
    switch ( channel ) {
        case 0:
        // invalid data !!!!!    
        case 126 ... 255:
            retval = 0;
            break;
        case 1 ... 40:
        case 101 ... 105:  
        // datatyp is float    
            retval = 1;
            break;
        case 41 ... 50:
        case 106 ... 110:
        // datatyp is int16_t    
            retval = 2;
            break;
        case 51 ... 60:
        case 111 ... 125:
        // datatyp is uint16_t    
            retval = 3;
            break;
        case 61 ... 80:
        // datatyp is char[3]    
            retval = 4;
            // ToDo Wort kann ein kompletter Text sein, das in verschiedene Channels zerlegt wird
            //      Max Länge 20*3=60 Zeichen
        break;
    }
    return retval;
}

/***************************************************
 * Extrahiert die Sensornummer aus dem Transportwert
 ***************************************************/
uint8_t getChannel(uint32_t data) {
  data &= ZF_SENSOR_NO;
  data >>= 25;
  return data;
}

/***************************************************
 * Extrahiert den Sensorwert aus dem Transportwert
 * Hier: Float
 ***************************************************/
float getValue_f(uint32_t data) {
  uint32_t exponent = (data & ZF_EXPO_WERT) >> 19;
  bool expo_negativ = data & ZF_EXPO_NEGATIV;
  bool zahl_negativ = data &  ZF_ZAHL_NEGATIV;
  float retval;
  retval = data & ZF_ZAHL_WERT;
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
 * Hier: Integer max. 16 bit
 ***************************************************/
int16_t getValue_i(uint32_t data) {
  int retval;
  if ( data & ZF_ZAHL_NEGATIV ) {
    retval = (data & ZF_ZAHL_WERT_INT) * -1;
  } else {
    retval = data & ZF_ZAHL_WERT_INT;
  }  
  return retval;
}

/***************************************************
 * Extrahiert den Sensorwert aus dem Transportwert
 * Hier: unsigned Integer max. 16 bit
 ***************************************************/
uint16_t getValue_ui(uint32_t data) {
  uint16_t retval;
  retval = data & ZF_ZAHL_WERT_UINT;
  return retval;
}

/******************************************************
 * Verpackt die Sensornummer und den Messwert zu einem 
 * TransportWert des Datentyps uint32_t.
 * Sensor: gültige Werte zwischen 1..127
 * Value: gültige Werte: -1*10^19 .. 1*10^19
 ******************************************************/
uint32_t calcTransportValue_f(uint8_t channel, float value) {  
  float _val = value;
  uint32_t exponent=0;
  bool expo_negativ = false;
  uint32_t result = 0;
  result = channel;
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
uint32_t calcTransportValue_ui(uint8_t channel, uint16_t value) {  
  uint32_t result = 0;
  result = channel;
  result <<= 25; 
  result |= (uint32_t) value;
  return result; 
}

/******************************************************
 * Verpackt die Sensornummer und den Messwert zu einem 
 * TransportWert des Datentyps uint32_t.
 * Sensor: gültige Werte zwischen 1..127
 * Value: gültige Werte: 0 .. 65535
 ******************************************************/
uint32_t calcTransportValue_i(uint8_t channel, int16_t value) {  
  uint32_t result = 0;
  result = channel;
  result <<= 25; 
  if ( value & 0b1000000000000000 ) result |= ZF_ZAHL_NEGATIV;
  result |= (value & ZF_ZAHL_WERT_INT);
  return result; 
}

uint32_t calcTransportValue_c(uint8_t channel, char* value, uint16_t* pos) {
  uint32_t result = 0;
  uint32_t c1 = value[*pos]<<16;
  uint32_t c2 = value[*pos+1]<<8;
  uint32_t c3 = value[*pos+2];
  result = channel;
  result <<= 25; 
  result = result | c1 | c2 | c3;  
  return result;  
}

