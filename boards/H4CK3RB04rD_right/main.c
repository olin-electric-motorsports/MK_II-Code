/* RIGHT */
#define F_CPU (4000000L)
#include <avr/io.h>
#include <util/delay.h>
#include "can_api.h"

ISR(CAN_INT_vect)
{

    CANPAGE = 0x00; // Reset canpage
    uint8_t msg = CANMSG;
    if (msg == 0x11){
        PORTB |= _BV(PB6);
        //_delay_ms(1);
        //PORTB &= ~_BV(PB6);
    } else {
        PORTD |= _BV(PD0);
        //_delay_ms(1);
        //PORTD &= ~_BV(PD0);
    }
    
    CAN_Rx(0, IDT_GLOBAL, IDT_GLOBAL_L, IDM_single);
}

int main (void) {
    // Set PE1 to output
    // Use pin 10 to light up an LED
    DDRB |= _BV(PB5) | _BV(PB6);
    DDRD |= _BV(PD0);
    CAN_init(0, 0);

    CAN_Rx(0, IDT_GLOBAL, IDT_GLOBAL_L, IDM_single);
    while(1) {
        // Toggle PE1 (pin 10)
        // Toggles power to pin 10 to create a "blink"
        //PORTB ^= _BV(PB5);

        // Give a delay to the toggle so it doesn't infinitely toggle
        //_delay_ms(500);
    }
}
