#include <avr/io.h>
#include "potentiometer.h"

inline void initADC(void)
{
    // Enable ADC
    ADCSRA |= _BV(ADEN);

    // Set Prescaler ( 32 )
    ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
    
    // Default to ADC3
    ADCSRB |= _BV(AREFEN);
    ADMUX |= 3;
    ADMUX |= _BV(REFS0);
}

inline uint16_t sync_read_potentiometer(void)
{
    // Force ADMUX to 3 
    ADMUX &= ~( 0x1F );
    ADMUX |= 3;

    // Start conversion
    ADCSRA |= _BV(ADSC);

    // Wait to finish
    while (bit_is_set(ADCSRA, ADSC));

    // Read value
    return ADC;
}

