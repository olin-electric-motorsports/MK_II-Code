#define F_CPU (1000000L)
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "ds18b20.h"

int testTemp(void) {
    //initialize temperature
    int temp;
    int bintemp;


    while(1) {
        //Start conversion (without ROM matching)
        ds18b20convert( &PORTC, &DDRC, &PINC, ( 1 << 0 ), NULL );

        //Delay (sensor needs time to perform conversion)
        _delay_ms( 1000 );

        //Read temperature (without ROM matching)
        ds18b20read( &PORTC, &DDRC, &PINC, ( 1 << 0 ), NULL, &temp );

            //Somehow use data stored in `temp` variable

    // fdconvert from interger to binary
    bintemp = decimalToBinary(temp)
    }
    return 0
}

long decimalToBinary(int n) {
    int remainder;
 long binary = 0, i = 1;
    while(n != 0) {
        remainder = n%2;
        n = n/2;
        binary= binary + (remainder*i);
        i = i*10;
    }
    return binary;
}
