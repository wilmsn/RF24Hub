/* test_dataformat.c
*
* A testtool for the lib dataformat.*
*
*/
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include "dataformat.h"
//#include "secrets.h"

#define FEHLERTOLLERANZ   0.01

  bool verbose_set = false;

void help(char* prgname) {
    printf ("Usage:: %s [-v][-f/i/u/a] [--] <start> [stop [step]]\n", prgname);
    printf ("        -v Show all results\n");
    printf ("        -f Use FLOAT\n");
    printf ("        -i Use INTEGER\n");
    printf ("        -u Use Unsigned INTEGER\n");
    printf ("        -a Use automatic format (DEFAULT)\n");
}

void test_uint(uint16_t zahl) {
  char val_str[20];
  uint16_t erg1, erg2; 
  uint32_t data1, data2;
  float delta1, delta2;
  float fehler1, fehler2;
  snprintf(val_str,19,"%u", zahl);
  if (verbose_set) printf("Use unsigned integer\n");
  data1 = calcTransportValue(1, val_str);
  data2 = calcTransportValue(1, zahl);
  if (! getValue(data1, &erg1) ) printf("String: False returened from 'getValue(%u, %u)'\n", data1, erg1);
  if (! getValue(data2, &erg2) ) printf("Wert:   False returened from 'getValue(%u, %u)'\n", data2, erg2);
  if (data1 != data2) printf("Unterschiede zwischen kodierung über Wert und String\n");
  delta1 = erg1-zahl;
  delta2 = erg2-zahl;
  if ( zahl != 0 ) {
    fehler1 = (float)(erg1-zahl)/zahl;
    fehler2 = (float)(erg2-zahl)/zahl;
  } else {
    fehler1 = erg1;
    fehler2 = erg1;
  }      
  if (fehler1 < 0) fehler1 *= -1;
  if (fehler2 < 0) fehler2 *= -1;
  if (fehler1 > FEHLERTOLLERANZ) printf("ZF: %u Zahl: %u\n", getDataType(data1) >> ZF_SHIFT_FORMAT, data1 & ZF_ZAHL_WERT_INT);
  if (fehler2 > FEHLERTOLLERANZ) printf("ZF: %u Zahl: %u\n", (data2 & ZF_FORMAT) >> ZF_SHIFT_FORMAT, data2 & ZF_ZAHL_WERT_INT);
  if ( fehler1 > FEHLERTOLLERANZ || fehler2 > FEHLERTOLLERANZ || verbose_set) {
    printf("Wert:%s, Ergebnis:%u/%u  Delta:%f/%f  Fehler:%f/%f\n", val_str, erg1, erg2, delta1, delta2, fehler1, fehler2);
  }
}

void test_int(int16_t zahl) {
  char val_str[20];
  int16_t erg1, erg2; 
  uint32_t data1, data2;
  float delta1, delta2;
  float fehler1, fehler2;
  snprintf(val_str,19,"%d", zahl);
  if (verbose_set) printf("Use integer\n");
  data1 = calcTransportValue(1, val_str);
  data2 = calcTransportValue(1, zahl);
  if (! getValue(data1, &erg1) ) printf("String: False returened from 'getValue(%u, %d)'\n", data1, erg1);
  if (! getValue(data2, &erg2) ) printf("Wert:   False returened from 'getValue(%u, %d)'\n", data2, erg2);
  if (data1 != data2) printf("Unterschiede zwischen kodierung über Wert und String\n");
  delta1 = erg1-zahl;
  delta2 = erg2-zahl;
  if ( zahl != 0 ) {
    fehler1 = (float)(erg1-zahl)/zahl;
    fehler2 = (float)(erg2-zahl)/zahl;
  } else {
    fehler1 = erg1;
    fehler2 = erg1;
  }      
  if (fehler1 < 0) fehler1 *= -1;
  if (fehler2 < 0) fehler2 *= -1;
  if (fehler1 > FEHLERTOLLERANZ) printf("ZF: %u Zahl: %d\n", getDataType(data1), data1 & ZF_ZAHL_WERT_INT);
  if (fehler2 > FEHLERTOLLERANZ) printf("ZF: %u Zahl: %d\n", getDataType(data2), data2 & ZF_ZAHL_WERT_INT);
  if ( fehler1 > FEHLERTOLLERANZ || fehler2 > FEHLERTOLLERANZ || verbose_set) {
    printf("Wert:%s, Ergebnis:%d/%d  Delta:%f/%f  Fehler:%f/%f\n", val_str, erg1, erg2, delta1, delta2, fehler1, fehler2);
  }
}

void test_float(float zahl) {
  char val_str[20];
  float erg1, erg2; 
  uint32_t data1, data2;
  float delta1, delta2;
  float fehler1, fehler2;
  snprintf(val_str,19,"%f", zahl);
  if (verbose_set) printf("Use float\n");
  data1 = calcTransportValue(1, val_str);
  data2 = calcTransportValue(1, zahl);
  if (! getValue(data1, &erg1) ) printf("String: False returened from 'getValue(%u, %f)'\n", data1, erg1);
  if (! getValue(data2, &erg2) ) printf("Wert:   False returened from 'getValue(%u, %f)'\n", data2, erg2);
  if (data1 != data2) {
      printf("Unterschiede zwischen kodierung über String und Wert\n");
      printf("String: data1: 0b"); printBits(sizeof(data1),&data1);
      printf("Wert:   data2: 0b"); printBits(sizeof(data2),&data2);
  }
  delta1 = erg1-zahl;
  delta2 = erg2-zahl;
  if ( zahl != 0 ) {
    fehler1 = (float)(erg1-zahl)/zahl;
    fehler2 = (float)(erg2-zahl)/zahl;
  } else {
    fehler1 = erg1;
    fehler2 = erg1;
  }      
  if ( (delta1/zahl > 0.0001 || delta1/zahl < -0.0001) || (delta2/zahl > 0.0001 || delta2/zahl < -0.0001) || verbose_set) {
    printf("Wert:%s, Ergebnis:%f/%f  Delta:%f/%f  Fehler:%f/%f\n", val_str, erg1, erg2, delta1, delta2, fehler1, fehler2);
  }
}

int main (int argc, char **argv) {
  float start, ende, schritt;
  bool format_set = false;
  bool ende_set = false;
  bool schritt_set = false;
  bool use_auto = true;
  bool use_float = false;
  bool use_int = false;
  bool use_uint = false;
  
  /* Kommandozeile auswerten */
  while (1) {
    int result = getopt(argc, argv, "vfaius:e:d:");
    if (result == -1) break; /* end of list */
    switch (result) {
      case 'v':
	  verbose_set = true;
	  break;
      case 'a':
	  if ( ! format_set ) {
	      use_auto = true;
	      format_set = true;
	  }
	  break;
      case 'f':
	  if ( ! format_set ) {
	      use_float = true;
	      use_auto = false;
	      format_set = true;
	  }
	  break;
      case 'i':
	  if ( ! format_set ) {
	      use_int = true;
	      use_auto = false;
	      format_set = true;
	  }
	  break;
      case 'u':
	  if ( ! format_set ) {
	      use_uint = true;
	      use_auto = false;
	      format_set = true;
	  }
	  break;
      case 'h':
      default:
	  help(argv[0]);
    }
  }

  
  if (optind < argc) {
    start = atof(argv[optind++]);
  }
  if (optind < argc) {
    ende = atof(argv[optind++]);
    ende_set = true;
  }
  if (optind < argc) {
    schritt = atof(argv[optind++]);
    schritt_set = true;
  }
  
  if (! ende_set) ende = start;
  if (! schritt_set) schritt = 1;
  
  if (verbose_set) {
    printf("Start:%f Ende:%f Schritt:%f ",start, ende, schritt);
    if (use_auto ) printf("Zahlenformat: auto\n");
    if (use_int ) printf("Zahlenformat: int\n");
    if (use_uint ) printf("Zahlenformat: uint\n");
    if (use_float ) printf("Zahlenformat: float\n");
  }    
  if (use_auto) {
    for(float wert = start; wert <= ende; wert += schritt) {
// Testen ob Wert auch als INT oder UINT übergeben werden kann
      if (wert > 0 && wert == (uint16_t)wert) {
	  test_uint( (uint16_t)wert );
      } else {
	if (wert < 0 && wert == (int16_t)wert) {
	  test_int( (int16_t)wert );
	} else {
	  test_float( wert );
	}
      }
    }
  }
  if (use_float) {
    for(float wert = start; wert <= ende; wert += schritt) {
      test_float( wert );
    }
  }
  if (use_int) {
    for(int16_t wert = start; wert <= ende; wert += schritt) {
      test_int( wert );
    }
  }
  if (use_uint) {
    for(uint16_t wert=start; wert <= ende; wert += schritt) {
      test_uint( wert );
    }
  }
    
  printf("Programmende \n");
  return EXIT_SUCCESS;
}
