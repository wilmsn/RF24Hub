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


int main (int argc, char **argv) {
  float start, ende, schritt, erg, delta;
  uint32_t data, mant;
  bool verbose;

  /* Kommandozeile auswerten */
  if ( (argc != 4 && argc != 5) && !( argc == 5 && argv[1][0] == '-' && argv[1][1] == 'v') ) {
    printf ("Usage:: %s [-v] <start> <stop> <step> \n      -v Show all results\n\n",
        argv[0] );
    exit (EXIT_FAILURE);
  }

  if ( argc == 4 ) {
    start = atof(argv[1]);
    ende = atof(argv[2]);
    schritt = atof(argv[3]);
    verbose = false;
  } else {
    start = atof(argv[2]);
    ende = atof(argv[3]);
    schritt = atof(argv[4]);
    verbose = true;
  }
//  start = -1000000.0;
//  ende = 1000000.0;
//  schritt = 0.1;

  for(float wert=start; wert < ende; wert += schritt) {
      data = calcTransportValue_f(1,wert); 
      mant = data & ZF_ZAHL_WERT;
      erg  = getValue_f(data);
      delta = erg-wert;
      if (verbose) {
          printf("Wert: %f, Mantisse: %u, Ergebnis: %f      Delta:%f    Fehler:%f\n", wert,  mant, erg, erg-wert,(erg-wert)/wert);
      } else {
        if ( delta/wert > 0.0001 || delta/wert < -0.0001 )
          printf("Wert: %f, Mantisse: %u, Ergebnis: %f      Delta:%f    Fehler:%f\n", wert,  mant, erg, erg-wert,(erg-wert)/wert);
     }
  }
  printf("Programmende \n");
  return EXIT_SUCCESS;
}
