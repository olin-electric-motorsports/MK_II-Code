#define F_CPU (4000000L)
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "can_api.h"

uint8_t FLAG = 0x00;
ISR(PCINT2_vect){
  FLAG ^= 0x01;
}

int main (void) {
  sei();
  // Initialize CAN
  CAN_init(CAN_ENABLED);

  // Set the array msg to contain 3 bytes
  uint8_t msg[] = {0x00, 0x00, 0x00};


  // Set PE1 to output
  DDRC |= _BV(PC7);
  DDRB |= _BV(PB2);

  PCICR |= _BV(PCIE2);//in 2nd mask register
  PCMSK2 |= _BV(PCINT21); //enable interups for pin 21

  while(1)
  {
    if (FLAG & 0x01){
      msg[0] = 0xFF;
      PORTB |= _BV(PB2); //turns on the led

    }
    else{
      msg[0] = 0x00;
      PORTB &= ~_BV(PB2); //turns off the led
    }
      CAN_transmit(0, CAN_IDT_THROTTLE, CAN_IDT_THROTTLE_L, msg );
      _delay_ms(1);

  }


}
