#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include "can_api.h"

#include "state.h"
#include "potentiometer.h"
#include "buttons.h"
#include "display.h"
#include "timer.h"
#include "lcd.h"

uint8_t gDISPLAY_FLAGS = 0x00;
uint8_t gSCROLL_STATE = 0x00;
uint8_t gBUTTON_STATES = 0x00;
uint8_t glDEBOUNCE_THRESH = 2;

ISR(PCINT0_vect)
{
    static uint8_t button2_time_last;
    static uint8_t button3_time_last;

    uint8_t current_time = TCNT1;

    // Button 2
    if (current_time - button2_time_last > glDEBOUNCE_THRESH)
    {
        if (bit_is_set(PINB, PB1))
        {
            gBUTTON_STATES &= ~_BV(BUTTON2);
        }
        else
        {
            gBUTTON_STATES |= _BV(BUTTON2);
        }

        led_follow_button( &PORT_LED2, LED2, BUTTON2 );
        gDISPLAY_FLAGS |= _BV(UPDATE_DISPLAY);
    }

    // Button 3
    if (current_time - button3_time_last > glDEBOUNCE_THRESH)
    {
        if (bit_is_set(PINB, PB0))
        {
            gBUTTON_STATES &= ~_BV(BUTTON3);
        }
        else
        {
            gBUTTON_STATES |= _BV(BUTTON3);
        }

        led_follow_button( &PORT_LED3, LED3, BUTTON3 );
        gDISPLAY_FLAGS |= _BV(UPDATE_DISPLAY);
    }

}


ISR(PCINT2_vect)
{
    static uint8_t button1_time_last;

    uint8_t current_time = TCNT1;

    // Button 1
    if (current_time - button1_time_last > glDEBOUNCE_THRESH)
    {
        if (bit_is_set(PIND, PD7))
        {
            gBUTTON_STATES &= ~_BV(BUTTON1);
        }
        else
        {
            gBUTTON_STATES |= _BV(BUTTON1);
        }

        led_follow_button( &PORT_LED1, LED1, BUTTON1 );
        gDISPLAY_FLAGS |= _BV(UPDATE_DISPLAY);
    }
}


int main (void)
{
    sei(); // Allow interrupts

    // Set up IO
    initButtons();
    initADC();
    initLEDs();
    initDisplay();
    initTimer16();

    while (1)
    {
        if (bit_is_set(gDISPLAY_FLAGS, UPDATE_DISPLAY))
        {
            update_scroll();
            gDISPLAY_FLAGS &= ~_BV(UPDATE_DISPLAY);
        }
      //uint8_t time = TCNT0;
      //if (time - old_time > 60)
      //{
      //    lcd_clrscr();
      //    char buffer[16];
      //    memset(buffer, '\0', 16);
      //    sprintf(buffer, "%x", time);
      //    lcd_puts(buffer);
      //}
    }
}

