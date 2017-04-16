// LEFT
#define F_CPU (4000000L)
#include <avr/io.h>
#include <util/delay.h>

int main (void) {
    DDRD |= _BV(PD7) | _BV(PD6) | _BV(PD5);

    DDRB |= _BV(PB1) | _BV(PB7); // MOSI | SCK
    SPCR |= _BV(SPE) | _BV(MSTR) | _BV(SPR0);

    uint8_t msg = 0x3f;
    while(1) {
        PORTD ^= _BV(PD6);

        SPDR = msg;
        while( !( SPSR & _BV(SPIF) ) )
        {
            PORTD |= _BV(PD7);
        }
        PORTD &= ~_BV(PD7);

        _delay_ms(1000);
    }
}
