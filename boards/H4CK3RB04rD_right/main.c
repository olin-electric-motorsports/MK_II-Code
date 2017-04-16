// RIGHT
#define F_CPU (4000000L)
#include <avr/io.h>
#include <util/delay.h>


int main (void) {
    DDRB |= _BV(PB5) | _BV(PB6);
    DDRD |= _BV(PD0);

    DDRB |= _BV(PB0); // MISO
    SPCR |= _BV(SPE);

    while(1) {
        PORTB ^= _BV(PB5);


        if( bit_is_set(SPSR, SPIF) )
        {
            PORTB |= _BV(PB6);

            /*
            uint8_t msg = SPDR;
            if (msg == 0x3f)
            {
                PORTD |= _BV(PD0);
            }
            */
        }
        else 
        {
            PORTD |= _BV(PD0);
        }

        _delay_ms(500);
        PORTB &= ~_BV(PB6);
        PORTD &= ~_BV(PD0);
        _delay_ms(500);
    }
}
