#define F_CPU (4000000L)
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "can_api.h"

//uint8_t FLAGS = 0x00;

int main (void) {
    sei(); //alllow interrupts
    // Set PB5,PB6,PC0 to output
    DDRB |= _BV(PB5) | _BV(PB6); //program LEDs
    PORTB ^= _BV(PB5); //have them alternate
    DDRB &= ~_BV(PB3); //BSPD Current Sense
    DDRC |= _BV(PC0); //program LED
    PORTC &= ~_BV(PC0); //have the LED startup off

    CAN_init(0,0); //turn on CAN
    CAN_Rx(0, IDT_GLOBAL, IDT_GLOBAL_L, IDM_single);

    //PCICR |= _BV(PCIE0); //enable 0th mask register for interrupts
    //PCMSK0 |= _BV(PCINT3); //enable interrupts for INT3
    

    while(1) {
        // Toggle PB5,PB6,PC0
        PORTB ^= _BV(PB5);

        if(PINB & _BV(PB3)){
            PORTC |= _BV(PC0);
            //FLAGS &= ~1;
        }
        else {
          PORTC &= ~_BV(PC0);  
        }

        // Give a delay to the toggle so it is visible
        _delay_ms(500);
    }

}

ISR(CAN_INT_vect){
    CANPAGE = 0x00; //reset the CAN page
    uint8_t msg = CANMSG; //grab the first byte of the CAN message
    PORTB ^= _BV(PB6);
    CAN_Rx(0, IDT_GLOBAL, IDT_GLOBAL_L, IDM_single); //setup to receive again
}

// ISR(PCINT3_vect){
//     FLAGS |= 1;
// }
