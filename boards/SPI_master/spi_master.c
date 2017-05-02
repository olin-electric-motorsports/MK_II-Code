#define F_CPU (4000000L)
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "lcd.h"

ISR(SPI_STC_vect)
{
    lcd_puts("Spi Interrupt");
}

int main (void)
{
    // Enable Interrupts
    sei();

    // Enable LCD
    lcd_init(LCD_DISP_ON_CURSOR_BLINK);

    // Hello!
    lcd_puts("Hello World!");


    // Setup LED IO pins
    DDRD |= _BV(PD0);

    // Setup SPI IO pins (MOSI and SCK)
    DDRD |= _BV(PD3) | _BV(PD4);

    // Enable alternate SPI (MISO_A, MOSI_A, etc)
    MCUCR |= _BV(SPIPS);

    // Enable SPI
    SPCR |= _BV(SPIE) | _BV(SPE) | _BV(MSTR) | _BV(SPR0);


    while(1)
    {
        _delay_ms(500);
        PORTD |= _BV(PD0);
        SPDR = 0x10;

        _delay_ms(100);
        PORTD &= ~_BV(PD0);
    }
}

