#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"

ISR(TIMER0_OVF_vect)
{
    ADCSRA |= _BV(ADSC);
}


void initADCTimer(void)
{
    // TODO: Should probably use Auto-triggering ADC
    // instead of a dedicated timer...
    
    /* Use the 8-bit timer for ADC reading
     */

    // Normal mode of operation
    TCCR0A |= 0x00; 

    TIMSK0 |= _BV(TOIE0);

    // Prescale to ClkIO/1024
    TCCR0B |= 0x05;
}

void initDebounceTimer(void)
{
    /* Use the 16-bit timer for Software Debouncing
     * and general timing 
     */

    // Normal mode of operation
    TCCR1A = 0x00;
    
    // Prescale to ClkIO/256
    TCCR1B |= 0x04;
}

