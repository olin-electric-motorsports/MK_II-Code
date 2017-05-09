#include <avr/io.h>
#include "buttons.h"
#include "state.h"


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

