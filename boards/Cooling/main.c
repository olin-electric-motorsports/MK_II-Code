#define F_CPU (4000000L)
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
int main()
{
  //fan is OC1A. Pump is OC1B

  //output PWM Controls Pump!
  TCCR1A |= _BV(COM1B1); 
  TCCR1A |= _BV(COM1B0); //toggle COM1B1 on output compare match
  TCCR1A |= _BV(COM1A1);
  TCCR1A |= _BV(COM1A0); //toggle COM1A1 on output compare match
  TCCR1A |= _BV(WGM10) | _BV(WGM11) | _BV(WGM12); //clear counter on compare match with ICR1 
  TCCR1B |= 1; //CSn2 = 0 CSn1 = 0 CSn0 = 1
  ICR1 = 0xFF; //clear counter at max
  DDRD |= _BV(PD2); // fan
  DDRC |= _BV(PC1); //pump
  OCR1B = 128; // half of a byte in decimal notation
  OCR1A = 1000;
  while(1)
  {
  }
}
