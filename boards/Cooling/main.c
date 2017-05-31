#define F_CPU (4000000L)
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "ds18b20.h"
int main()
{
  //fan is OC1A. Pump is OC1B

  //output PWM Controls Pump!
  TCCR1A |= _BV(COM1B1); //
  TCCR1A |= _BV(COM1B0);
  TCCR1A |= _BV(COM1A1);
  TCCR1A |= _BV(COM1A0);
  TCCR1A |= _BV(WGM10);
  TCCR1B |= _BV(WGM12);
  TCCR1A &= ~ _BV(WGM11);
  DDRD |= _BV(PD2); // fan
  DDRC |= _BV(PC1); //pump
  OCR1B = 128; // half of a byte in decimal notation
  OCR1A = 128;
  TCCR1B |= 5; //CSn2 = 1 CSn1 = 0 CSn0 = 1
  while(1)
  {
  }
}
