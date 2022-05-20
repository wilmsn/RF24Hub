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
#include "dataformat.h"
#include "secrets.h"

int main (int argc, char **argv) {
  float start, ende, schritt;
  uint32_t data, mant, expo;
  bool verbose;
  bool use_float = true;
  bool use_int = false;
  bool use_uint = false;
  
  /* Kommandozeile auswerten */
  if ( !(argc == 4 || argc == 5 || argc == 6) && !( (argc == 5 || argc == 6) && argv[1][0] == '-' && argv[1][1] == 'v') ) {
    printf ("Usage:: %s [-v][-f/i/u] <start> <stop> <step> \n      -v Show all results\n      -f Use FLOAT\n      -i Use INTEGER\n      -u Use Unsigned INTEGER\n         Default is FLOAT\n\n",
        argv[0] );
    exit (EXIT_FAILURE);
  }
  if (argv[1][0] == '-' && argv[1][1] == 'v') {
    verbose = true; 
  }
  if ( (argv[1][0] == '-' && argv[1][1] == 'i') || (argv[2][0] == '-' && argv[2][1] == 'i') ) {
    use_int = true; 
    use_float = false; 
  }
  if ( (argv[1][0] == '-' && argv[1][1] == 'u') || (argv[2][0] == '-' && argv[2][1] == 'u') ) {
    use_uint = true; 
    use_float = false; 
  }
  if ( (argv[1][0] == '-' && argv[1][1] == 'f') || (argv[2][0] == '-' && argv[2][1] == 'f') ) {
    use_float = true; 
  }


  if ( argc == 4 ) {
    start = atof(argv[1]);
    ende = atof(argv[2]);
    schritt = atof(argv[3]);
  }
  if ( argc == 5 ) {
    start = atof(argv[2]);
    ende = atof(argv[3]);
    schritt = atof(argv[4]);
  }
  if ( argc == 6 ) {
    start = atof(argv[3]);
    ende = atof(argv[4]);
    schritt = atof(argv[5]);
  }

  if (use_float) {
    float erg, delta;
    for(float wert=start; wert < ende; wert += schritt) {
      data = calcTransportValue_f(mykey,1,wert); 
      mant = (data ^ mykey) & ZF_ZAHL_WERT_FLOAT;
      expo = ((data ^ mykey) & ZF_EXPO_WERT) >> 17;
      erg  = getValue_f(mykey,data);
      delta = erg-wert;
      if (verbose) {
          printf("Wert: %f, Mantisse: %u, Exponent: %u, Ergebnis: %f      Delta:%f    Fehler:%f\n", wert, mant, expo, erg, erg-wert, wert != 0? (erg-wert)/wert: erg);
      } else {
        if ( delta/wert > 0.0001 || delta/wert < -0.0001 )
          printf("Wert: %f, Mantisse: %u, Exponent: %u, Ergebnis: %f      Delta:%f    Fehler:%f\n", wert, mant, expo, erg, erg-wert,wert != 0? (erg-wert)/wert:erg);
     }
    }
  }
  if (use_int) {
    int erg, delta;
    for(int wert=start; wert < ende; wert += schritt) {
      data = calcTransportValue_i(mykey,2,wert); 
      mant = (data ^ mykey) & ZF_ZAHL_WERT_INT;
      erg  = getValue_i(mykey,data);
      delta = erg-wert;
      if (verbose) {
          printf("Wert: %i, Transp: %u, Ergebnis: %i      Delta:%i\n", wert, mant, erg, delta);
      } else {
        if ( delta != 0 )
          printf("Wert: %i, Transp: %u, Ergebnis: %i      Delta:%i\n", wert, mant, erg, delta);
     }
    }
  }
  if (use_uint) {
    unsigned int erg, delta;
    for(unsigned int wert=start; wert < ende; wert += schritt) {
      data = calcTransportValue_ui(mykey,3,wert); 
      mant = (data ^ mykey) & ZF_ZAHL_WERT_UINT;
      erg  = getValue_ui(mykey,data);
      delta = erg-wert;
      if (verbose) {
          printf("Wert: %u, Transp: %u, Ergebnis: %u      Delta:%i\n", wert, mant, erg, erg-wert);
      } else {
        if ( delta != 0 )
          printf("Wert: %u, Transp: %u, Ergebnis: %u      Delta:%i\n", wert, mant, erg, erg-wert);
      }
    }
  }
    
  printf("Programmende \n");
  return EXIT_SUCCESS;
}
