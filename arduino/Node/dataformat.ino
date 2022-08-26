#include "dataformat.h"

// some forward declaration
float getValue_f(uint32_t data);
int16_t getValue_i(uint32_t data);
uint16_t getValue_ui(uint32_t data);
// END some forward declaration

// This function is only for debugging
void printBits(size_t const size, void const * const ptr) {
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;
    
    for (i = size-1; i >= 0; i--) {
        for (j = 7; j >= 0; j--) {
            byte = (b[i] >> j) & 1;
            printf("%u", byte);
        }
    }
    puts("");
}
// END This function is only for debugging


uint32_t calcTransportValue(uint8_t channel, float value) {
  float _val = value;
  uint32_t ui_val = 0;
  uint32_t exponent = 0;
  bool expo_negativ = false;
  uint32_t result = 0;
  result = ((uint32_t)channel << ZF_SHIFT_CHANNEL);
  if ( value > 0.00001 || value < -0.00001 ) {
    bool negativ = value < 0.0;
    if ( negativ ) {
      result |= ZF_ZAHL_NEGATIV;
      _val *= -1;
    }
    while ( _val < 6553.6 ) {
      expo_negativ = true;
      exponent++;
      _val *= 10.0;
    }
    if ( expo_negativ ) {
      result |= ZF_EXPO_NEGATIV;
    }
    while ( _val > 65536.0 ) {
      exponent++;
      _val /= 10.0;
    }
    ui_val = 1 * _val;
    ui_val &= ZF_ZAHL_WERT_FLOAT;
    exponent <<= ZF_SHIFT_EXPO;
    result |= ZF_FORMAT_FLOAT;
    result |= exponent;
    result |= ui_val;
  }
  return result; 
}

uint32_t calcTransportValue(uint8_t channel, uint16_t value) {  
  uint32_t result = 0;
  result = (uint32_t)channel << ZF_SHIFT_CHANNEL;
  result &= ZF_CHANNEL;
  result |= ((uint32_t)value & ZF_ZAHL_WERT_UINT);
  result |= ZF_FORMAT_UINT;
  return result; 
}

uint32_t calcTransportValue(uint8_t channel, int16_t value) {  
  uint32_t result = 0;
  result = (uint32_t)channel << ZF_SHIFT_CHANNEL;
  result &= ZF_CHANNEL;
  result |= ((uint32_t)value & ZF_ZAHL_WERT_INT);
  result |= ZF_FORMAT_INT;
  return result; 
}

uint32_t calcTransportValue(uint8_t channel, char value1, char value2) {
  uint32_t result = 0;
  uint32_t c2 = value2<<8;
  uint32_t c1 = value1;
  result = channel;
  result <<= ZF_SHIFT_CHANNEL; 
  result &= ZF_CHANNEL;
  result = result | ZF_FORMAT_CHAR | c1 | c2;  
  return result;  
}

#if defined(__linux__) || defined(ESP8266)
uint32_t calcTransportValue(uint8_t channel, char* value, uint8_t dataType) {
    uint32_t retval = 0;
    char* pEnd; 
    switch ( dataType ) {
        case ZF_FLOAT:
        {
            float val_f = strtof(value, &pEnd);
            retval = calcTransportValue(channel, val_f);
        }
        break;
        case ZF_INT:
        {
            int16_t val_i = (int16_t)strtol(value, &pEnd, 10);
            retval = calcTransportValue(channel, val_i);
        }
        break;
        case ZF_UINT:
        {
            uint16_t val_ui = (uint16_t)strtoul(value, &pEnd, 10);
            retval = calcTransportValue(channel, val_ui);
        }
        break;
        case ZF_CHAR:
            // ToDo Wort kann ein kompletter Text sein, das in verschiedene Channels zerlegt wird
            //      Max Länge 20*3=60 Zeichen
        break;
    }
    return retval;
}

uint32_t calcTransportValue(uint8_t channel, char* value) {
    uint8_t dataType = ZF_UNKNOWN;
    for (size_t i=0; i<strlen(value); i++) {
	if ( (value[i] >= 'a' && value[i] <= 'z') || (value[i] >= 'A' && value[i] <= 'Z') ) dataType = ZF_CHAR;
    }
    if ( dataType == ZF_CHAR || dataType == ZF_UNKNOWN ) {
	if ( value[0] >= '0' && value[0] <= '9' ) dataType = ZF_UINT;
    }
    if ( dataType == ZF_CHAR || dataType == ZF_UINT || dataType == ZF_UNKNOWN ) {
	if ( value[0] == '-' ) dataType = ZF_INT;
    }    
    for (size_t i=0; i<strlen(value); i++) {
	if ( dataType == ZF_CHAR || dataType == ZF_UINT || dataType == ZF_INT ) {
	    if ( value[i] == '.' ) dataType = ZF_FLOAT;
	}
    }
    char* pEnd; 
    float val_f = strtof(value, &pEnd);
    if ( ((val_f > 32768) && (dataType == ZF_INT)) || ((val_f > 65536) && (dataType == ZF_UINT)) ) dataType = ZF_FLOAT;
    return calcTransportValue(channel, value, dataType);
}

#endif


bool getValue(uint32_t data, float* zahl) {
  bool retval = false;
  if ( getDataType(data) == ZF_FLOAT ) { 
    *zahl = getValue_f(data);
    retval = true;
  }
  if ( getDataType(data) == ZF_INT ) { 
    *zahl = (float)getValue_i(data);
    retval = false;
  }
  if ( getDataType(data) == ZF_UINT ) { 
    *zahl = (float)getValue_ui(data);
    retval = false;
  }
  return retval;
}

bool getValue(uint32_t data, int16_t * zahl) {
  bool retval = false;
  if ( getDataType(data) == ZF_FLOAT ) { 
    *zahl = (int16_t)getValue_f(data);
    retval = false;
  }
  if ( getDataType(data) == ZF_INT ) { 
    *zahl = getValue_i(data);
    retval = true;
  }
  if ( getDataType(data) == ZF_UINT ) { 
    *zahl = (int16_t)getValue_ui(data);
    retval = false;
  }
  return retval;
}

bool getValue(uint32_t data, uint16_t* zahl) {
  bool retval = false;
  if ( getDataType(data) == ZF_FLOAT ) { 
    *zahl = (uint16_t)getValue_f(data);
    retval = false;
  }
  if ( getDataType(data) == ZF_INT ) { 
    *zahl = (uint16_t)getValue_i(data);
    retval = false;
  }
  if ( getDataType(data) == ZF_UINT ) { 
    *zahl = getValue_ui(data);
    retval = true;
  }
  return retval;
}

uint8_t getDataType(uint32_t data) {
  uint8_t retval;
  retval = ((data & ZF_FORMAT) >> ZF_SHIFT_FORMAT);
  return retval;
}

uint8_t getChannel(uint32_t data) {
  uint8_t retval;
  retval = (data & ZF_CHANNEL) >> ZF_SHIFT_CHANNEL;
  return retval;
}

char* unpackTransportValue(uint32_t data, char* buf) {
    uint8_t dataType = getDataType(data);
    switch ( dataType ) {
        case ZF_FLOAT:  
        {
            float myval;
	    getValue(data, &myval);
            if ( myval > 500 ) {
#if defined(__linux__) || defined(ESP8266)
                snprintf(buf,9,"%.1f", myval);
#else
                dtostrf(myval, 4, 0, buf);
#endif
            } else {
                if ( myval > 9.9 ) {
#if defined(__linux__) || defined(ESP8266)
                    snprintf(buf,9,"%.2f", myval);
#else
                dtostrf(myval, 4, 1, buf);
#endif
                } else {
#if defined(__linux__) || defined(ESP8266)
                    snprintf(buf,9,"%.3f", myval);
#else
                dtostrf(myval, 4, 2, buf);
#endif
                }   
            }
        }
        break;
        case ZF_INT:
        {
	    int16_t myval;
	    getValue(data, &myval);
            sprintf(buf,"%d",myval);
        }
        break;
        case ZF_UINT:
        {
	    uint16_t myval;
	    getValue(data, &myval);
            sprintf(buf,"%u",myval);
        }
        break;
        case ZF_CHAR:
            // ToDo Wort kann ein kompletter Text sein, das in verschiedene Channels zerlegt wird
            //      Max Länge 20*3=60 Zeichen
        break;
    }
    return buf;
}

/***************************************
 * getValue_? sind nur modulintern und deshalb
 * nicht in dataformat.h deklariert !!!!!
 **************************************/
float getValue_f(uint32_t data) {
  float zahl;
  if ( getDataType(data) == ZF_FLOAT ) { 
    uint32_t exponent = (data & ZF_EXPO_WERT) >> 17;
    bool expo_negativ = data & ZF_EXPO_NEGATIV;
    bool zahl_negativ = data &  ZF_ZAHL_NEGATIV;
    zahl = data & ZF_ZAHL_WERT_FLOAT;
    if ( expo_negativ ) {
      for (uint8_t i=exponent;i>0;i--) {
	zahl /= 10.0;
      }
    } else {
      for (uint8_t i=exponent;i>0;i--) {
	zahl *= 10.0;
      }    
    }
    if ( zahl_negativ ) {
      zahl *= -1.0;
    }
  }
  return zahl;
}

int16_t getValue_i(uint32_t data) {
  int16_t zahl;
  if ( getDataType(data) == ZF_INT ) { 
    zahl = data & ZF_ZAHL_WERT_INT;
  }
  return zahl;
}

uint16_t getValue_ui(uint32_t data) {
  uint16_t zahl;
  if ( getDataType(data) == ZF_UINT ) { 
    zahl = data & ZF_ZAHL_WERT_UINT;
  }
  return zahl;
}

