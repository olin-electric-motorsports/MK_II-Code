#define F_CPU (1000000L)
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "ds18b20.h"

int testPump(void) {
    //initialize temperature
    int temp1;
    int temp2;
    int bintemp;
    DDRC|= _BV(PC7);


    while(1) {
        // Reading and converting from Sensor 1
        //Start conversion (without ROM matching)
        ds18b20convert( &PORTC, &DDRC, &PINC, ( _BV(PC4) ), NULL );

        //Delay (sensor needs time to perform conversion)
        _delay_ms( 1000 );

        //Read temperature (without ROM matching)
        ds18b20read( &PORTC, &DDRC, &PINC, (_BV(PC4)), NULL, &temp1 );

        // Reading and converting from Sensor 2
        ds18b20convert( &PORTC, &DDRC, &PINC, (_BV(PC5)), NULL );

        //Delay (sensor needs time to perform conversion)
        _delay_ms( 1000 );

        //Read temperature (without ROM matching)
        ds18b20read( &PORTC, &DDRC, &PINC, ( _BV(PC5)), NULL, &temp2 );

        // Control pump
        if(( (temp1 + temp2) / 2)) > 0{
          PORTC &= ~_BV(PC7);
        }
        else {
          PORTC |= _BV(PC7);
        }

    }
    return 0
}
