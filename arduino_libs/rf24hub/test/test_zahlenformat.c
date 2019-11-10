#include <stdio.h>
#include "zahlenformat.c"

void printit(uint8_t sens, float wert) {
    uint32_t tv;
    tv = calcTransportValue_f(sens, wert);
    if ( wert < 100 ) {
    printf("Sensor: %u\tZahl: %f\t\t=> TV:\t%08x\t=> Rückgabewerte: Sensor\t%u\tWert: %f\n", sens, wert, tv, getSensor(tv), getValue_f(tv) );
    } else {        
    printf("Sensor: %u\tZahl: %f\t=> TV:\t%08x\t=> Rückgabewerte: Sensor\t%u\tWert: %f\n", sens, wert, tv, getSensor(tv), getValue_f(tv) );
    }
}    

int main() {
	float zahl1;
	uint8_t sensor1;
     
    zahl1 = 0.0000123;
    sensor1 = 88;
    printit(sensor1,zahl1);
    
    zahl1 = 0.000123;
    sensor1 = 44;
    printit(sensor1,zahl1);
    
    zahl1 = 0.00123;
    sensor1 = 99;
    printit(sensor1,zahl1);
    
    zahl1 = 0.0123;
    sensor1 = 1;
    printit(sensor1,zahl1);
    
    zahl1 = 1.123;
    sensor1 = 125;
    printit(sensor1,zahl1);

    zahl1 = 55.123;
    sensor1 = 9;
    printit(sensor1,zahl1);

    zahl1 = 555.123;
    sensor1 = 33;
    printit(sensor1,zahl1);

    zahl1 = 5555.123;
    sensor1 = 33;
    printit(sensor1,zahl1);

    zahl1 = 55555.123;
    sensor1 = 33;
    printit(sensor1,zahl1);

    zahl1 = 555551.123;
    sensor1 = 33;
    printit(sensor1,zahl1);

    zahl1 = 5555512.123;
    sensor1 = 33;
    printit(sensor1,zahl1);

    zahl1 = 55555123.123;
    sensor1 = 33;
    printit(sensor1,zahl1);

    return 0;
}
