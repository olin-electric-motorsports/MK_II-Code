#define F_CPU (4000000L)
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

ISR(SPI_STC_vect)
{
    uint8_t msg = SPDR;
    if( msg == 0x10 )
    {
        PORTD |= _BV(PD2);
    }
}

int main (void)
{
    // Enable Interrupts
    sei();

    // Setup SPI IO pins (MISO)
    DDRD |= _BV(PD2);

    // Enable Alternate SPI
    MCUCR |= _BV(SPIPS);

    // Enable SPI
    SPCR |= _BV(SPIE) | _BV(SPE);
    
    while(1)
    {
    }
}
