#include "dataformat.h"

uint32_t calcTransportValue_f(uint32_t key, uint8_t channel, float value) {
  float _val = value;
  uint32_t exponent = 0;
  bool expo_negativ = false;
  uint32_t result = 0;
  result = channel;
  result <<= ZF_SHIFT_CHANNEL; 
  if ( value > 0.00001 || value < -0.00001 ) {
    bool negativ = value < 0.0;
    if ( negativ ) {
      result |= ZF_ZAHL_NEGATIV;
      _val=abs(_val);
    }
    while ( _val < 10000.0 ) {
      expo_negativ = true;
      exponent++;
      _val *= 10.0;
    }
    if ( expo_negativ ) {
      result |= ZF_EXPO_NEGATIV;
    }
    while ( _val > 100000.0 ) {
      exponent++;
      _val/=10.0;
    }
    exponent <<= ZF_SHIFT_EXPO;
    result |= ZF_FORMAT_FLOAT;
    result |= exponent;
    result |= (uint32_t) _val;
  }
  return result ^ key; 
}

uint32_t calcTransportValue_ui(uint32_t key, uint8_t channel, uint16_t value) {  
  uint32_t result = 0;
  result = channel;
  result <<= ZF_SHIFT_CHANNEL; 
  result |= ZF_FORMAT_UINT;
  result |= (uint32_t) value;
  return result ^ key; 
}

uint32_t calcTransportValue_i(uint32_t key, uint8_t channel, int16_t value) {  
  uint32_t result = 0;
  result = channel;
  result <<= ZF_SHIFT_CHANNEL; 
  result |= value;
  result |= ZF_FORMAT_INT;
  return result ^ key; 
}

uint32_t calcTransportValue_c(uint8_t channel, char* value, uint16_t* pos) {
  uint32_t result = 0;
  uint32_t c1 = value[*pos]<<16;
  uint32_t c2 = value[*pos+1]<<8;
  uint32_t c3 = value[*pos+2];
  result = channel;
  result <<= ZF_SHIFT_CHANNEL; 
  result = result | c1 | c2 | c3;  
  return result;  
}

float getValue_f(uint32_t key, uint32_t data) {
  uint32_t mydata = data ^ key;
  uint32_t exponent = (mydata & ZF_EXPO_WERT) >> 17;
  bool expo_negativ = mydata & ZF_EXPO_NEGATIV;
  bool zahl_negativ = mydata &  ZF_ZAHL_NEGATIV;
  float retval;
  retval = mydata & ZF_ZAHL_WERT_FLOAT;
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

int16_t getValue_i(uint32_t key, uint32_t data) {
  int retval;
  uint32_t mydata = data ^ key;
  retval = mydata & ZF_ZAHL_WERT_INT;
  return retval;
}

uint16_t getValue_ui(uint32_t key, uint32_t data) {
  uint16_t retval;
  uint32_t mydata = data ^ key;
  retval = mydata & ZF_ZAHL_WERT_UINT;
  return retval;
}

uint8_t getDataType(uint32_t key, uint32_t data) {
  uint32_t mydata = data ^ key;
  uint8_t retval;
  retval = (mydata & ZF_FORMAT) >> ZF_SHIFT_FORMAT;
  return retval;
}

uint8_t getChannel(uint32_t key, uint32_t data) {
  uint32_t mydata = data ^ key;
  uint8_t retval;
  retval = (mydata & ZF_CHANNEL) >> ZF_SHIFT_CHANNEL;
  return retval;
}

char* unpackTransportValue(uint32_t key, uint32_t data, char* buf) {
    uint8_t dataType = getDataType(key, data);
    switch ( dataType ) {
        case 0:  
        {
            float myval = getValue_f(key, data);
            if ( myval > 500 ) {
#if defined(__linux__) || defined(ESP8266)
                sprintf(buf,"%.1f", myval);
#else
                dtostrf(myval, 4, 0, buf);
#endif
            } else {
                if ( myval > 9.9 ) {
#if defined(__linux__) || defined(ESP8266)
                    sprintf(buf,"%.2f", myval);
#else
                dtostrf(myval, 4, 1, buf);
#endif
                } else {
#if defined(__linux__) || defined(ESP8266)
                    sprintf(buf,"%.3f", myval);
#else
                dtostrf(myval, 4, 2, buf);
#endif
                }   
            }
        }
        break;
        case 1:
        {
            sprintf(buf,"%d",getValue_i(key, data));
        }
        break;
        case 2:
        {
            sprintf(buf,"%u",getValue_ui(key, data));
        }
        break;
        case 3:
            // ToDo Wort kann ein kompletter Text sein, das in verschiedene Channels zerlegt wird
            //      Max Länge 20*3=60 Zeichen
        break;
    }
    return buf;
}

#if defined(__linux__) || defined(ESP8266)
/*
uint32_t packTransportValue(uint8_t channel, char* value) {
    uint32_t retval = 0;
    uint8_t dataType = getDataType(channel);
    char* pEnd; 
    switch ( dataType ) {
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
*/
#endif
