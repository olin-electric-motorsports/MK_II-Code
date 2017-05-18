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
  // set up for throttle

  //Enable ADC, set prescalar to 128 (slow down ADC clock)
  //begin conversion
  ADCSRA |= _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
  //Enable internal reference voltage
  ADCSRB &= _BV(AREFEN);
  //Set internal reference voltage as AVcc
  ADMUX |= _BV(REFS0); //sets the reference voltage
  //Reads by default from ADC0 (pin 11); this line
  //  is redundant. the timer
  ADMUX |= _BV( 0x00 );
  //No prescaling on PWM clock
  TCCR0B |= _BV(CS00);
  //Set up phase-correct PWM on OC0B
  TCCR0A |= _BV(COM0B1) | _BV(WGM00);
  //Reset the other PWM pin
  TCCR0A &= ~_BV(COM0B0);

  //set output this one is set to the torque encoder 1
  DDRC |= _BV(PC7);



  // Initialize CAN for sending brake message
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
    // PWM throttle

    //Read from ADC
    ADCSRA |=  _BV(ADSC);
    //Wait for ADC reading
    while(bit_is_set(ADCSRA, ADSC));
    uint16_t reading = ADC;

    OCR0B = (uint8_t) (reading >> 2); //shifts the 10 bit two to the right to make 6

    // here is where you will in a for loop read in the value of the ADC, store the
    //previous one and compare the values based on the rules you need to change the ADCMUX to
    //be each of your specific bits and then you need to swap between each potenciometer values
    //based on which potentiometer bec we are cul and have 2.

    //swap AMMUX between the two diff throttle bits. Save the ADC and compare the values

    // statements to change the CAN Messsage of the brake
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
