#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>
#include "lcd.h"

uint8_t FLAG = 0x00;
uint8_t BUTTON_STATES = 0x00;

ISR(PCINT0_vect)
{
    if (bit_is_set(PINB, PB1))
    {
        BUTTON_STATES &= ~_BV(0);
    }
    else
    {
        BUTTON_STATES |= _BV(0);
    }

    if (bit_is_set(PINB, PB0))
    {
        BUTTON_STATES &= ~_BV(1);
    }
    else
    {
        BUTTON_STATES |= _BV(1);
    }
}

void init_spi_master(void)
{
    // Setup SPI IO pins (MOSI and SCK)
    DDRD |= _BV(PD3) | _BV(PD4);

    // Enable SPI
    SPCR |=  _BV(SPE) | _BV(MSTR) | _BV(SPR0);

    // Enable alternate SPI (MISO_A, MOSI_A, etc)
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
    // Global interrupts
    sei();

    // Enable LCD
    lcd_init(LCD_DISP_ON_CURSOR_BLINK);

    // Hello!
    lcd_puts("SPI Example\nPress Button");

    // Setup LED IO pins
    DDRD |= _BV(PD0) | _BV(PD1);

    // Button Interrupts
    PCICR |= _BV(PCIE0);
    PCMSK0 |= _BV(PCINT0) | _BV(PCINT1);

    // SPI init
    init_spi_master();

    uint8_t msg = 0x01;

    // Slave Select
    DDRC |= _BV(PC1);
    PORTC &= ~_BV(PC1);

    while(1)
    {
        if( bit_is_set(BUTTON_STATES, 0))
        {
            uint8_t rec = spi_message(msg);
            msg++;

            lcd_clrscr();
            char buffer[16];
            sprintf(buffer, "val: %x %x", msg, rec);
            lcd_puts(buffer);
            //_delay_ms(1000);

            BUTTON_STATES &= ~_BV(0);
        }

        PORTD ^= _BV(PD0);
        _delay_ms(200);
    }
}

