#define F_CPU (4000000L)
#include <avr/io.h>
#include <util/delay.h>

int main (void) {
    // Set PE1 to output
    // Use pin 10 to light up an LED
    DDRC |= _BV(PC0);

    while(1) {
        // Toggle PE1 (pin 10)
        // Toggles power to pin 10 to create a "blink"
        PORTC ^= _BV(PC0);
        

        // Give a delay to the toggle so it doesn't infinitely toggle
        _delay_ms(500);
    }
}
