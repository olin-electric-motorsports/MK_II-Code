#include <avr/io.h>
#include <avr/pgmspace.h> //TODO: Determine if this is necessary
#include <avr/interrupt.h>

volatile uint8_t FLAGS = 0x00;

int main(void)
{
	sei(); //allow interrupts
	DDRC |= _BV(PC0);
	PORTC |= _BV(PC0);

	init_timer();

	while(1){
		if (FLAGS & 1){
			PORTC ^= _BV(PC0);
			FLAGS &= ~1;
		}
	}

}

ISR(TIMER1_COMPA_vect) 
{
  FLAGS |= 1;
}

void init_timer(void) {
    TCCR1A &= ~(_BV(WGM11) | _BV(WGM10)); //set timer in CTC mode with reset on match with OCR1A
    TCCR1B |= _BV(CS11) | _BV(CS10) | _BV(WGM12); //Set prescaler to 1/64
    TIMSK1 |= _BV(OCIE1A); // Enable overflow interrupts (set TOIE)
    OCR1A |= 50000; //timer compare value
}