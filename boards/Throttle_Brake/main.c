#define F_CPU (4000000L)
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "can_api.h"



int main (void) {
  sei();

    // Initialize CAN
    CAN_init(0, 0);

    // Set the array msg to contain 3 bytes
    uint8_t msg[] = {0xF1, 0x00, 0x00};


    // Set PE1 to output
    DDRC |= _BV(PC7);
    DDRB |= _BV(PB2);


    while(1)
    {
      CAN_Tx(0, IDT_GLOBAL, IDT_GLOBAL_L, msg );
      //sends CAN Message to the Transom constantly
      //flashes Led to validate the messsage is being sent
      PORTB ^= _BV(PB2); //Dp1
      _delay_ms(500);
    }


    }
