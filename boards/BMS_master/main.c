#define F_CPU (1000000L)
#include <avr/io.h>
#include <util/delay.h>

int main (void) {
    // Set PB5,PB6,PC0 to output
    DDRB |= _BV(PB5);
    DDRB |= _BV(PB6);
    DDRC |= _BV(PC0);

    while(1) {
        // Toggle PB5,PB6,PC0
        PORTB ^= _BV(PB5);
        PORTB ^= _BV(PB6);
        PORTC ^= _BV(PC0);
        

        // Give a delay to the toggle so it is visible
        _delay_ms(500);
    }
}
