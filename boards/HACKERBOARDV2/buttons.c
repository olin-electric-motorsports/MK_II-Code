#include <avr/io.h>
#include <avr/interrupt.h>
#include "buttons.h"
#include "state.h"
#include "display.h" // LED Definitions

// Local Global
uint8_t glDEBOUNCE_THRESH = 10;

ISR(PCINT0_vect)
{
    // Set up software debounce
    static uint16_t button2_time_last;
    static uint16_t button3_time_last;
    uint16_t current_time = TCNT1;

    /* Button 2 */
    if (current_time - button2_time_last > glDEBOUNCE_THRESH)
    {
        button2_time_last = current_time; 
        if (bit_is_set(PINB, PB1))
        {
            gBUTTON_STATES &= ~_BV(BUTTON2);
        }
        else
        {
            gBUTTON_STATES |= _BV(BUTTON2);
            handle_select();
        }

        led_follow_button( &PORT_LED2, LED2, BUTTON2 );
    }

    // Button 3
    if (current_time - button3_time_last > glDEBOUNCE_THRESH)
    {
        button3_time_last = current_time;

        if (bit_is_set(PINB, PB0))
        {
            gBUTTON_STATES &= ~_BV(BUTTON3);
        }
        else
        {
            gBUTTON_STATES |= _BV(BUTTON3);

            if (!gEDIT_CAN)
            {
                gSCROLL_POS++;
                if (gSCROLL_POS > (gSCROLL_LIMIT-1))
                {
                    gSCROLL_POS = (gSCROLL_LIMIT-1);
                }
                else
                {
                    gFLAGS |= _BV(UPDATE_DISPLAY);
                }
            }
        }

        led_follow_button( &PORT_LED3, LED3, BUTTON3 );
    }

}


ISR(PCINT2_vect)
{
    // Set up software debounce
    static uint16_t button1_time_last;
    uint16_t current_time = TCNT1;

    /* Button 1 */
    // Debounce
    if (current_time - button1_time_last > glDEBOUNCE_THRESH)
    {
        button1_time_last = current_time;

        // Update state
        if (bit_is_set(PIND, PD7))
        {
            gBUTTON_STATES &= ~_BV(BUTTON1);
        }
        else
        {
            gBUTTON_STATES |= _BV(BUTTON1);

            // Don't scroll when editing
            if (!gEDIT_CAN)
            {
                // Change scroll only on Positive press
                gSCROLL_POS--;
                if (gSCROLL_POS > (gSCROLL_LIMIT-1)) // Unsigned
                {
                    gSCROLL_POS = 0;
                }
                else
                {
                    // Trigger update of display
                    gFLAGS |= _BV(UPDATE_DISPLAY);
                }
            }
        }

        // Blink dem LEDs
        led_follow_button( &PORT_LED1, LED1, BUTTON1 );
    }
}


void initButtons(void)
{
    // Set up BUTTON IO
    // Default to inputs
    DDRD &= ~_BV(PD7);
    DDRB &= ~( _BV(PB0) | _BV(PB1) );

    // Set up interrupts
    PCICR |= _BV(PCIE2) | _BV(PCIE0);
    PCMSK2 |= _BV(PCINT23);
    PCMSK0 |= _BV(PCINT0) | _BV(PCINT1);
}


inline void led_follow_button(volatile uint8_t *ledPort, uint8_t ledPin, uint8_t button)
{
    if( bit_is_set( gBUTTON_STATES, button ) )
    {
        *ledPort |= _BV(ledPin);
    }
    else
    {
        *ledPort &= ~_BV(ledPin);
    }
}

