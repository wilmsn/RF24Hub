#include "zahlenformat.h"

uint8_t getSensor(uint32_t val) {
  val &= ZF_SENSOR_NO;
  val >>= 25;
  return val;
}

float getValue(uint32_t val) {
  uint32_t exponent = (val & ZF_EXPO_WERT) >> 20;
  bool expo_negativ = val & ZF_EXPO_NEGATIV;
  bool zahl_negativ = val &  ZF_ZAHL_NEGATIV;
  float retval;
  retval = val & ZF_ZAHL_WERT;
  if ( expo_negativ ) {
    for (uint8_t i=exponent;i>0;i--) {
      retval/=10.0;
    }
  } else {
    for (uint8_t i=exponent;i>0;i--) {
      retval*=10.0;
    }    
  }
  return retval;
}

uint16_t getValue_uint(uint32_t val) {
  uint16_t retval;
  retval = val & ZF_ZAHL_WERT;
  return retval;
}

uint32_t calcTransportValue(uint8_t sensor, float value) {  
  float _val = value;
  uint32_t exponent=0;
  bool expo_negativ = false;
  uint32_t result = 0;
  result = sensor;
  result <<= 25; 
  if ( abs(value) > 0 ) {
    bool negativ = value < 0;
    if ( negativ ) {
      result |= ZF_ZAHL_NEGATIV;
      _val=abs(_val);
    }
    while ( _val < 10000.0 ) {
      expo_negativ = true;
      exponent++;
      _val*=10.0;
    }
    if ( expo_negativ ) {
      result |= ZF_EXPO_NEGATIV;
    }
    while ( _val > 100000.0 ) {
      exponent++;
      _val/=10.0;
    }
    exponent <<= 20;
    result |= exponent;
    result |= (uint32_t) _val;
  }
  return result; 
}

uint32_t calcTransportValue_uint(uint8_t sensor, uint16_t value) {  
  uint32_t result = 0;
  result = sensor;
  result <<= 25; 
  result |= (uint32_t) value;
  return result; 
}
