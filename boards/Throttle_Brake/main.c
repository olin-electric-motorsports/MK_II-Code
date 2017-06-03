#define F_CPU (4000000L)
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "can_api.h"

// Throttle 1Out is pin 17, which is ADC8 which is MUX 01000 = 0x08
// Throttle 1Out is pin 18, which is ADC9 which is MUX 01001 = 0x09

// BSPD BSO is pin 28, which is ADC4 which is MUX 00100 = 0x04

uint8_t FLAG = 0x00;

uint8_t throttleMin;  // our min throttle value, found by experimentation
uint8_t throttleMax;  // our max throttle value, found by experimentation
uint8_t legalDeviation;  // 10% of the throttle range

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

uint8_t[] brakePlausibility(uint16_t rThrottle[], uint16_t rBrake, uint8_t currentMSG[])
{
  if(rBrake != 0)
  {
    // Set the torque command for the motor controller to 0 torque
  }
}

uint8_t[] throttleComparison(uint16_t rThrottle[], uint8_t currentMSG[])
{
  //These two if statements check if the two throttle values are within the 10% legal deviation of each other
  if((rThrottle[0] > rThrottle[1] + legalDeviation) || (rThrottle[1] > rThrottle[0] + legalDeviation) || rThrottle[0] < throttleMin || rThrottle[0] > throttleMax || rThrottle[1] < throttleMin || rThrottle[1] > throttleMax)
  {
    inverterMSG[5] = 0x00;
  }
  // If two throttle values were in the legal devaition the original msg is returned
  return inverterMSG;

}

uint8_t[] setInverterMSG(uint16_t rThrottle[], uint8_t currentMSG[])
{
  return currentMSG;
}

int main (void) {
  uint8_t state = 0;

  uint8_t startUpConditions = 0x00;
  uint8_t channels[] = {8,9};

  /* Brake and Throttle states */
  uint16_t rBrake = 0x00;
  uint16_t rThrottle[] = {0x00,0x00};

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
  uint8_t msg[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  /**** initialize the array inverterMSG ****/
  //inialized to be disabled, direction be clockwise("foward"), torque and speed to be 0, discharge disabled,torque limits set to default EEPROM parameters
  uint8_t inverterMSG[] = {0x00, 0x00, 0x00, 0x00, 0b00000000, 0x00, 0x00, 0x00};


  DDRC |= _BV(PC7);
  DDRB |= _BV(PB2);

  // brake light
  PCICR |= _BV(PCIE2);//in 2nd mask register
  PCMSK2 |= _BV(PCINT21); //enable interups for pin 21 the brake mosfet

  while(1)
  {
    /**** Pre-Startup State ****/
    if(state==0)
    {

    }
    /**** Startup State ****/
    else if(state==1)
    {

      /**** Starts Up The Inverter ****/
      CAN_transmit(CAN_MOB_1, 0xC0, 8, inverterMSG);  // sets inverter to be disabled(required to be enabled)
      inverterMSG[5] = 0b00000001; // sets inverterMSG to enable inverter
      CAN_transmit(CAN_MOB_1, 0xC0, 8, inverterMSG);  // sets inverter to be enabled
    }
    /**** Post-Startup State ****/
    else if(state==2)
    {

    }
    //OCR0B = (uint8_t) (reading >> 2); //shifts the 10 bit two to the right to make 6

    /* Reads and stores data from ADC for both throttle potentiometers*/
    uint8_t i;
    for (i = 0; i <2; i++)
    {
      //Switches ADC input channel
      ADMUX &= ~(0x1F);
      ADMUX |= channels[i];

      //Read from ADC
      ADCSRA |= _BV(ADSC);
      //wait for ADC reading
      while(bit_is_set(ADCSRA,ADSC));
      rThrottle[i] = ADC;
    }

    /* Reads and stores data from ADC for brake */
    ADMUX &= ~(0x1F); //Switches ADC input channel
    ADMUX |= 4;

    //Read from ADC
    ADCSRA |= _BV(ADSC);
    //wait for ADC reading
    while(bit_is_set(ADCSRA,ADSC));
    rBrake = ADC;

    /********* CAN Communication Code **************/
    /*
    // statements to change the CAN Messsage of the brake
    if (FLAG & 0x01){ // if the brake is pressed flag and set 1set message segment to 0xFF
    msg[0] = 0xFF;
    PORTB |= _BV(PB2); //turns on the led

  }
  else{ // if the brake is not pressed the message stays all zeros
  msg[0] = 0x00;
  PORTB &= ~_BV(PB2); //turns off the led
}
*/

    msg = throttleComparison(rThrottle, msg);

    msg[0] = rThrottle[0] & 11110000;
    msg[1] = rThrottle[0] & 00001111;

    msg[2] = rThrottle[1] & 11110000;
    msg[3] = rThrottle[1] & 00001111;

    CAN_transmit(CAN_MOB_0, CAN_IDT_THROTTLE, CAN_IDT_THROTTLE_L, msg);
    CAN_transmit(CAN_MOB_1, 0xC0, 8, inverterMSG)
    _delay_ms(1); //delay so the car doesn't flip shit

  } // end of while loop


}
