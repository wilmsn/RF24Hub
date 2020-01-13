#include "common.h"

uint8_t getChannel(uint32_t val) {
  val &= ZF_SENSOR_NO;
  val >>= 25;
  return val;
}

float getValue_f(uint32_t val) {
  uint32_t exponent = (val & ZF_EXPO_WERT) >> 19;
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

uint16_t getValue_i(uint32_t val) {
  uint16_t retval;
  retval = val & ZF_ZAHL_WERT;
  return retval;
}

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

uint32_t calcTransportValue_i(uint8_t sensor, uint16_t value) {  
  uint32_t result = 0;
  result = sensor;
  result <<= 25; 
  result |= (uint32_t) value;
  return result; 
}

//-------------------------------------------------------------

/*
 * trim: get rid of trailing and leading whitespace...
 *       ...including the annoying "\n" from fgets()
 */
char * trim (char * s) {
  /* Initialize start, end pointers */
  char *s1 = s, *s2 = &s[strlen (s) - 1];
  /* Trim and delimit right side */
  while ( (isspace (*s2)) && (s2 >= s1) )
    s2--;
  *(s2+1) = '\0';
  /* Trim left side */
  while ( (isspace (*s1)) && (s1 < s2) )
    s1++;
  /* Copy finished string */
  strcpy (s, s1);
  return s;
}

uint64_t mymillis(void) {
	struct timeval tv;
	uint64_t timebuf;
	gettimeofday(&tv, NULL);
	timebuf = (((tv.tv_sec & 0x000FFFFFFFFFFFFF) * 1000) + (tv.tv_usec / 1000));
	return timebuf;
}

bool decodeVerbose(uint16_t* oldLevel, char* verboseSet) {
    bool retval = false;
    char cmp_addVBtn[]="+telnet",
         cmp_rmVBtn[]="-telnet",
         cmp_addVBrf24[]="+rf24",
         cmp_rmVBrf24[]="-rf24",
         cmp_addVBob[]="+obuffer",
         cmp_rmVBob[]="-obuffer",
         cmp_addVBorder[]="+order",
         cmp_rmVBorder[]="-order",
         cmp_addVBOBcont[]="+obcont",
         cmp_rmVBOBcont[]="-obcont",
         cmp_addVBOcont[]="+ocont",
         cmp_rmVBOcont[]="-ocont",
         cmp_addVBsql[]="+sql",
         cmp_rmVBsql[]="-sql",
         cmp_addVBother[]="+other",
         cmp_rmVBother[]="-other";
    if (strcmp(verboseSet,cmp_addVBOBcont) == 0) {
        *oldLevel |= VERBOSECONTENTOBUFFER;
        retval = true;
    }
    if (strcmp(verboseSet,cmp_rmVBOBcont) == 0) {
        *oldLevel ^= VERBOSECONTENTOBUFFER;
        retval = true;
    }
    if (strcmp(verboseSet,cmp_addVBOcont) == 0) {
        *oldLevel |= VERBOSECONTENTORDER;
        retval = true;
    }
    if (strcmp(verboseSet,cmp_rmVBOcont) == 0) {
        *oldLevel ^= VERBOSECONTENTORDER;
        retval = true;
    }
    if (strcmp(verboseSet,cmp_addVBtn) == 0) {
        *oldLevel |= VERBOSETELNET;
        retval = true;
    }
    if (strcmp(verboseSet,cmp_rmVBtn) == 0) {
        *oldLevel ^= VERBOSETELNET;
        retval = true;
    }
    if (strcmp(verboseSet,cmp_addVBrf24) == 0) {
        *oldLevel |= VERBOSERF24;
        retval = true;
    }
    if (strcmp(verboseSet,cmp_rmVBrf24) == 0) {
        *oldLevel ^= VERBOSERF24;
        retval = true;
    }    
    if (strcmp(verboseSet,cmp_addVBsql) == 0) {
        *oldLevel |= VERBOSESQL;
        retval = true;
    }
    if (strcmp(verboseSet,cmp_rmVBsql) == 0) {
        *oldLevel ^= VERBOSESQL;
        retval = true;
    }    
    if (strcmp(verboseSet,cmp_addVBob) == 0) {
        *oldLevel |= VERBOSEOBUFFER;
        retval = true;
    }
    if (strcmp(verboseSet,cmp_rmVBob) == 0) {
        *oldLevel ^= VERBOSEOBUFFER;
        retval = true;
    }    
    if (strcmp(verboseSet,cmp_addVBorder) == 0) {
        *oldLevel |= VERBOSEORDER;
        retval = true;
    }
    if (strcmp(verboseSet,cmp_rmVBorder) == 0) {
        *oldLevel ^= VERBOSEORDER;
        retval = true;
    }    
    if (strcmp(verboseSet,cmp_addVBother) == 0) {
        *oldLevel |= VERBOSEOTHER;
        retval = true;
    }
    if (strcmp(verboseSet,cmp_rmVBother) == 0) {
        *oldLevel ^= VERBOSEOTHER;
        retval = true;
    }    
    return retval;
}
