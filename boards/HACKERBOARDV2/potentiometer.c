#include <avr/io.h>
#include <avr/interrupt.h>
#include "state.h"
#include "potentiometer.h"

ISR(ADC_vect)
{
    // Grab ADC Value
    gADC_VAL = (uint8_t) (ADC >> 2);

    // Set ADC Updated flag
    gFLAGS |= _BV(NEW_ADC_VAL);
}

inline void initADC(void)
{
    // Configure ADC w/ interrupt
    ADCSRA |= _BV(ADIE);
    
    // Should set ADC auto-trigger
    //ADCSRA |= _BV(ADATE);

    // Set Prescaler ( 32 )
    ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);

    // Interrupt on 8-bit timer overflow
    ADCSRB |= 0x03;

    // Set reference
    ADCSRB |= _BV(AREFEN);

    // Default to ADC3
    ADMUX |= 3;
    ADMUX |= _BV(REFS0);
}

void async_read_potentiometer_on(void)
{
    // Enable ADC
    ADCSRA |= _BV(ADEN);
    
    // Start conversion
    //ADCSRA |= _BV(ADSC);
}

void async_read_potentiometer_off(void)
{
    // Enable ADC
    ADCSRA &= ~_BV(ADEN);
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

