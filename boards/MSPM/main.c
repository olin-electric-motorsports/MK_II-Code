#define F_CPU (4000000L)
#include <avr/io.h>
// #include <util/delay.h>
#include <avr/interrupt.h>
// #include "can_api.h"

ISR(PCINT0_vect) {
    // Set state of led at pin 25 due to
    // interrupt on pin 17
    if(PINC & _BV(PC4)) {
      PORTC |= _BV(PC7);
    } else {
      PORTC &= ~_BV(PC7);
    }
}

ISR(PCINT1_vect) {
    // Set state of led at pin 29 due to
    // interrupt on pin 16
    if(PINB & _BV(PB2)) {
      PORTD |= _BV(PD0);
    } else {
      PORTD &= ~_BV(PD0);
    }
}

ISR(PCINT2_vect) {
    // Set state of LEDs at pins 26, 27, and 28 due to
    // interrupts on pins 13, 14, and 15 respectively
    if(PIND & _BV(PD5)) {
      PORTB |= _BV(PB5);
    } else {
      PORTB &= ~_BV(PB5);
    }

    if(PIND & _BV(PD6)) {
      PORTB |= _BV(PB6);
    } else {
      PORTB &= ~_BV(PB6);
    }

    if(PIND & _BV(PD7)) {
      PORTB |= _BV(PB7);
    } else {
      PORTB &= ~_BV(PB7);
    }
}

int main (void) {
    sei(); // Enable global interrupts

    // Set PC7, PB5, PB6, PB7, and PD0 to output (pins 25, 26, 27, 28, and 29 respectively)
    DDRC |= _BV(PC7);
    DDRB |= _BV(PB5) | _BV(PB6) | _BV(PB7);
    DDRD |= _BV(PD0);

    // Set PD5, PD6, PD7, PB2, and PC4 to input (pins 13, 14, 15, 16, and 17 respectively)
    DDRD &= ~_BV(PD5) & ~_BV(PD6) & ~_BV(PD7);
    DDRB &= ~_BV(PB2);
    DDRC &= ~_BV(PC4);

    // Set up interrupts
    PCICR |= _BV(PCIE0) | _BV(PCIE1) | _BV(PCIE2); // enable mask registers for interrupts
    PCMSK0 |= _BV(PCINT2); // PC4
    PCMSK1 |= _BV(PCINT12); // PB2
    PCMSK2 |= _BV(PCINT21) | _BV(PCINT22) | _BV(PCINT23); // PD5, PD6, AND PD7 respectively

    for(;;){}
}
