#define F_CPU (4000000L)
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void init_spi_slave(void)
{
    // Setup SPI IO pins (MISO)
    DDRD |= _BV(PD2);

    // Enable SPI
    SPCR |= _BV(SPE);

    // Enable Alternate SPI (MISO_A, MOSI_A, etc)
    // Take this out if you are using standard pins!
    MCUCR |= _BV(SPIPS);
}

uint8_t spi_message(uint8_t msg)
{
    // Set message
    SPDR = msg;

    // Wait for transmission to finish
    while(!(SPSR & (1<<SPIF))); 

    return SPDR;
}

int main (void)
{
    init_spi_slave();
    DDRD |= _BV(PD0) | _BV(PD1);

    uint8_t msg = 0x3F;
    while(1)
    {
        spi_message(msg);
        PORTD ^= _BV(PD0) | _BV(PD1);
    }
}
