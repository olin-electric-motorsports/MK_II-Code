#define F_CPU (4000000L)
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "ds18b20.h"

int main1 () {
    //initialize temperature
    int16_t temp1;
    //int bintemp;
    DDRC |= _BV(PC7);

    temp1 = 5;
    while(1) {
        // Reading and converting from Sensor 1
        //Start conversion (without ROM matching)
        //PORTC &= ~_BV(PC7);
        _delay_ms( 1000 );
        ds18b20convert( &PORTC, &DDRC, &PINC, ( _BV(PC4) ), NULL );

        //Delay (sensor needs time to perform conversion)
        _delay_ms( 1000 );

        //Read temperature (without ROM matching)
        ds18b20read( &PORTC, &DDRC, &PINC, (_BV(PC4)), NULL, &temp1 );

        // Reading and converting from Sensor 2
        //Delay (sensor needs time to perform conversion)
        _delay_ms( 1000 );
        //Read temperature (without ROM matching)
        PORTC |= _BV(PC7);
        // Control pump
        if(temp1 == 0){
          PORTC |=_BV(PC7);
        }
        else {
        PORTC ^=  _BV(PC7);
          //PORTC &= ~ _BV(PC7);; // turn bit to ond. shifts on.
          _delay_ms( 200 );
        PORTC ^=  _BV(PC7);
        _delay_ms( 200 );
        PORTC ^=  _BV(PC7);

        }
    }
}
