// ADC input and PWM output
// Try modifying this to only turn on the LED after a threshold.
// Or do something more interesting.

#define F_CPU (1000000L)
#include <avr/io.h>

int main (void) {
    //Enable ADC, set prescalar to 128 (slow down ADC clock)
    ADCSRA |= _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);

    //Enable internal reference voltage
    ADCSRB &= _BV(AREFEN);

    //Set internal reference voltage as AVcc
    ADMUX |= _BV(REFS0);

    //Reads by default from ADC0 (pin 11); this line
    //  is redundant.
    ADMUX |= _BV( 0x00 );

    //No prescaling on PWM clock
    TCCR0B |= _BV(CS00);

    //Set up phase-correct PWM on OC0B
    TCCR0A |= _BV(COM0B1) | _BV(WGM00);

    //Reset the other PWM pin
    TCCR0A &= ~_BV(COM0B0);

    //set OC0B (PE1, pin 10) as output.
    DDRE |= _BV(PE1);

    while(1) {
        //Read from ADC
        ADCSRA |=  _BV(ADSC);
        //Wait for ADC reading
        while(bit_is_set(ADCSRA, ADSC));
        uint16_t reading = ADC;

        OCR0B = (uint8_t) (reading >> 2);
    }
}
