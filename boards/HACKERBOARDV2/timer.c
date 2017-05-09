#include <avr/io.h>
#include "timer.h"

void initTimer8(void)
{
    // Normal mode of operation
    TCCR0A = 0x00;
    
    // Prescale to ClkIO/1024
    TCCR0B |= 0x05;
}

void initTimer16(void)
{
    // Normal mode of operation
    TCCR1A = 0x00;
    
    // Prescale to ClkIO/1024
    TCCR1B |= 0x05;
}

