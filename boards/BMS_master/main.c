/* CAN Rx */
#define F_CPU (4000000L)
#include <avr/io.h>
#include <util/delay.h>
#include "can_api.h"

uint8_t flag = 0x00;

ISR(CAN_INT_vect)
{
    // Reset CANPAGE (A necessary step; don't worry about it
    CANPAGE = 0x00;

    // msg now contains the first byte of the CAN message.
    // Repeat this call to receive the other bytes.
    uint8_t msg = CANMSG;

    flag = 0x01; //Set a flag to flash the light

    // Set the chip to wait for another message.
    CAN_Rx(0, IDT_GLOBAL, IDT_GLOBAL_L, IDM_single);
}

int main (void) {
    DDRB |= _BV(PB5) | _BV(PB2);

    //Write shutdown control high
    PORTB |= _BV(PB2);

    //PWM init
    //Output compare pin is OC1B, so we need OCR1B as our counter
    TCCR1B |= _BV(CS10); //Clock prescale set to max speed
    TCCR1A |= _BV(COM1B1) | _BV(WGM00); //Enable the right pwm compare and mode
    TCCR1A &= ~_BV(COM1B0); //Make sure other PWM is off
    DDRC |= _BV(PC1); //Enable

    OCR1B = (uint8_t) 0; //Set counter to a random value?

    // Initialize CAN
    //CAN_init(0, 0);

    // Tell the CAN system to wait for a message.
    //CAN_Rx(0, IDT_GLOBAL, IDT_GLOBAL_L, IDM_single);

    while(1) {
        // Wait indefinitely for a message to come.
        PORTB ^= _BV(PB5);
        _delay_ms(200);
    }

}

//Pin is PC1A
//Also known as OC1B
