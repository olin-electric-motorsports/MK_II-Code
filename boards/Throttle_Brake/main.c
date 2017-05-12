#define F_CPU (4000000L)
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "can_api.h"

uint8_t FLAG = 0x00;

// ISR to flag when the brake is pressed
ISR(PCINT2_vect){
  if (bit_is_set(PINC,PC6))
  {
    FLAG |= _BV(0x01);
  }
  else
  {
    FLAG &= ~_BV(0x01);
  }
}

int main (void) {
  sei();
  // Initialize CAN
  CAN_init(CAN_ENABLED);

  // Set the array msg to be zero when brake nor pressed
  uint8_t msg[] = {0x00, 0x00, 0x00};


  DDRC |= _BV(PC7);
  DDRB |= _BV(PB2);

  // brake light
  PCICR |= _BV(PCIE2);//in 2nd mask register
  PCMSK2 |= _BV(PCINT21); //enable interups for pin 21 the brake mosfet

  while(1)
  {
    if (FLAG & 0x01){ // if the brake is pressed flag and set 1set message segment to 0xFF
      msg[0] = 0xFF;
      PORTB |= _BV(PB2); //turns on the led

    }
    else{ // if the brake is not pressed the message stays all zeros
      msg[0] = 0x00;
      PORTB &= ~_BV(PB2); //turns off the led
    }
      CAN_transmit(0, CAN_IDT_THROTTLE, CAN_IDT_THROTTLE_L, msg );
      _delay_ms(1); //delay so the car doesn't flip shit

  }


}
