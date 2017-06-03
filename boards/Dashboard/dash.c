#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "can_api.h"

uint8_t RTDsound_msg[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// ISR to
ISR(CAN_INT_vect){

    // CAN ready to recieve the first message
    CAN_read_received(0, CAN_IDT_THROTTLE_L, RTDsound_msg);

    if (RTDsound_msg[0] == 0x01){ // if pushed
        PORTD |= _BV(PD6); //turn on the ready to drive sound
    }
    if (RTDsound_msg[0] == 0x00){ //if not pushed
        PORTD &= ~(_BV(PD6)); //turn the sound off because it is anoying as all hell
    }
    // Set the chip to wait for another message.
    CAN_wait_on_receive(0, CAN_IDT_THROTTLE, CAN_IDT_THROTTLE_L, CAN_IDM_single);
}

int main (void) {


    sei();
    // Initialize CAN
    CAN_init(CAN_ENABLED);

    //Recieve Message
    CAN_wait_on_receive(0, CAN_IDT_THROTTLE, CAN_IDT_THROTTLE_L, CAN_IDM_global);

    DDRD |= _BV(PD6);

    while(1) {

      _delay_ms(1000);
      if( bit_is_clear(CANEN2, 0))
      {
        CAN_wait_on_receive(0, CAN_IDT_THROTTLE, CAN_IDT_THROTTLE_L, CAN_IDM_global);
      }
    }
}
