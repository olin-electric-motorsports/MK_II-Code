#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include "lcd.h"
#include "pinDefinitions.h"

uint8_t FLAGS = 0x00;
uint8_t BUTTON_STATES = 0x00;
uint16_t ADC_VALUE = 0x00;


void grab_new_adc(void)
{
    ADCSRA |= _BV(ADSC);
    while (bit_is_set(ADCSRA, ADSC));

    ADC_VALUE = ADC;
}


void initIO(void)
{
    // Set up LED IO
    DDRD |= _BV(PD0) | _BV(PD1);
    DDRC |= _BV(PC0);

    // Set up BUTTON IO
    // Default to inputs
    DDRD &= ~_BV(PD7);
    DDRB &= ~( _BV(PB0) | _BV(PB1) );

    // Set up interrupts
    PCICR |= _BV(PCIE2) | _BV(PCIE0);
    PCMSK2 |= _BV(PCINT23);
    PCMSK0 |= _BV(PCINT0) | _BV(PCINT1);

    // Set up POT
    // Enable ADC
    ADCSRA |= _BV(ADEN);

    // Set Prescaler ( 32 )
    ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
    
    ADCSRB |= _BV(AREFEN);
    ADMUX |= 3;
    ADMUX |= _BV(REFS0);
}


void grab_new_adc(void)
{
    ADCSRA |= _BV(ADSC);
    while (bit_is_set(ADCSRA, ADSC));

    ADC_VALUE = ADC;
}


ISR(PCINT0_vect)
{
    // Button 2
    if (bit_is_set(PINB, PB1))
    {
        BUTTON_STATES &= ~_BV(BUTTON2);
    }
    else
    {
        BUTTON_STATES |= _BV(BUTTON2);
    }

    // Button 3
    if (bit_is_set(PINB, PB0))
    {
        BUTTON_STATES &= ~_BV(BUTTON3);
    }
    else
    {
        BUTTON_STATES |= _BV(BUTTON3);
    }

    // Update Display
    FLAGS |= _BV(UPDATE_DISPLAY);
}


ISR(PCINT2_vect)
{
    // Button 1
    if (bit_is_set(PIND, PD7))
    {
        BUTTON_STATES &= ~_BV(BUTTON1);
        grab_new_adc();
    }
    else
    {
        BUTTON_STATES |= _BV(BUTTON1);
    }

    // Update Display
    FLAGS |= _BV(UPDATE_DISPLAY);
}


void update_display(void)
{
    lcd_clrscr();
    char buffer[16];
    memset(buffer, '\0', 16);
    sprintf(buffer, "%x\n%x", BUTTON_STATES, ADC_VALUE);
    lcd_puts(buffer);
}


void led_follow_button(volatile uint8_t *ledPort, uint8_t ledPin, uint8_t button)
{
    if( bit_is_set( BUTTON_STATES, button ) )
    {
        *ledPort |= _BV(ledPin);
    }
    else
    {
        *ledPort &= ~_BV(ledPin);
    }
}


int main (void)
{
    sei(); // Allow interrupts

    // Set up IO
    initIO();
    lcd_init(LCD_DISP_ON_CURSOR_BLINK);

    // CODE
    PORT_LED1 |= _BV(LED1);
    PORT_LED2 |= _BV(LED2);
    PORT_LED3 |= _BV(LED3);

    lcd_puts("Hello World!");

    // SPI Initialization
    for( int i =0; i < 3; i++){
        PORT_LED1 ^= _BV(LED1);
    }

    while (1)
    {
        if( bit_is_set( FLAGS, UPDATE_DISPLAY ) )
        {
            update_display();
            FLAGS &= ~_BV(UPDATE_DISPLAY);
        }
        
        led_follow_button( &PORT_LED1, LED1, BUTTON1 );
        led_follow_button( &PORT_LED2, LED2, BUTTON2 );
        led_follow_button( &PORT_LED3, LED3, BUTTON3 );
    }
}

