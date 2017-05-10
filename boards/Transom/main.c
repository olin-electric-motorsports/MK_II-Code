#define F_CPU (4000000L)
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "can_api.h"

ISR(CAN_INT_vect){
    // Reset CANPAGE (A necessary step; don't worry about it
    CANPAGE = 0x00;
    //CAM message
    uint8_t msg = CANMSG;

    if (msg == 0xF1){
      PORTC |= _BV(PC0);
    }

    // Set the chip to wait for another message.
    CAN_Rx(0, IDT_GLOBAL, IDT_GLOBAL_L, IDM_single);
}

int main (void) {

    sei();
    // Initialize CAN
    CAN_init(0, 0);

    // Set the array msg to contain 3 bytes
    uint8_t msg[] = {0xF1, 0x00, 0x00};

    //Recieve Message
    CAN_Rx(0, IDT_GLOBAL, IDT_GLOBAL_L, IDM_single);

    // Set PE1 to output
    // Use pin 10 to light up an LED
    DDRC |= _BV(PC6);
    DDRC |= _BV(PC0);

    while(1) {

      PORTC ^= _BV(PC6); //D1
      //flashing LED
      _delay_ms(200);
    }
}
