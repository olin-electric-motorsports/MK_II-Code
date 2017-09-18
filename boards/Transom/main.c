#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "can_api.h"

// ISR to flash brake light
ISR(CAN_INT_vect){

    uint8_t msg[CAN_IDT_TRANSOM_L];
    // CAN ready to recieve the first message
    CAN_read_received(0, CAN_IDT_TRANSOM_L, msg);

    if (msg[0] == 0xFF){ //if the message contains 0xFF in the first spot of array instead of 0x00
        PORTC |= _BV(PC6); //turn on the brake lightpin PC0 is not zero (aka if brake is pressed at all)
    }
    else{ //if the brake is not pressed
        PORTC &= ~(_BV(PC6)); //turn the light back to 0 so off
    }
    // Set the chip to wait for another message.
    CAN_wait_on_receive(0, CAN_IDT_TRANSOM, CAN_IDT_TRANSOM_L, CAN_IDM_single);
}

int main (void) {

    sei();
    // Initialize CAN
    CAN_init(CAN_ENABLED);

    // Set the array msg to contain 3 bytes
    uint8_t msg[3];
    uint8_t err;

    //Recieve Message
    err = CAN_wait_on_receive(0, CAN_IDT_TRANSOM, CAN_IDT_TRANSOM_L, CAN_IDM_global);

    if (err > 0){
      PORTC |= _BV(PC6);
    }

    DDRC |= _BV(PC0);
    DDRC |= _BV(PC6);

    while(1) {
      // PORTC ^= _BV(PC0); //D1
      // //flashing LED
      // _delay_ms(200);
      if( bit_is_clear(CANEN2, 0))
      {
        CAN_wait_on_receive(0, CAN_IDT_TRANSOM, CAN_IDT_TRANSOM_L, CAN_IDM_global);
      }
    }
}
