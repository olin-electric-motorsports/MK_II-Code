/* CAN Tx */
#define F_CPU (4000000L)
#include <avr/io.h>
#include <util/delay.h>
#include "can_api.h"

int main (void) {
    // Set PE1 to output
    // Use pin 10 to light up an LED
    DDRD |= _BV(PD7) | _BV(PD6) | _BV(PD5);
    PORTD |= _BV(PD7);
    CAN_init(0, 0);


    uint8_t msg[] = { 0x11, 0x66, 0x0a };
    while(1) {
        PORTD ^= _BV(PD6);
        // Toggle PE1 (pin 10)
        // Toggles power to pin 10 to create a "blink"
        //PORTD ^= _BV(PD6);

        // Give a delay to the toggle so it doesn't infinitely toggle
        _delay_us(200);
        if( bit_is_set(CANGSTA, TXBSY) ) {
            PORTD |= _BV(PD5);
        } else {
            //PORTD &= ~_BV(PD5);
        }

        CAN_Tx( 0, IDT_GLOBAL, IDT_GLOBAL_L, msg );

    }
}
