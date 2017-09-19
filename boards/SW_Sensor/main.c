#include <avr/io.h>
#include <util/delay.h>
#include "can_api.h"

int main (void) {
    // Set PE1 to output
    // Use pin 10 to light up an LED
    DDRC |= _BV(PC0);

    // ADC conf
    ADCSRA |= _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
    // Set ADC reference to internal GND
    ADCSRB |= _BV(AREFEN);
    // Select ADC channel
    ADMUX|= _BV(REFS0) | 6;

    // Init ADC storage var
    uint16_t reading = 0;    

    // Initialize CAN
    CAN_init(CAN_ENABLED);

    // Set the array msg to contain 3 bytes
    uint8_t msg[2] = {0,0};

    while(1) {
        // Toggle PE1 (pin 10)
        // Toggles power to pin 10 to create a "blink"
        // if (reading > 500)
        // {
        //     PORTC |= _BV(PC0);
        // }
        // else
        // {
        //     PORTC &= ~_BV(PC0);
        // }

        // // Start ADC read
        // ADCSRA |= _BV(ADSC);
        // // Block until ADC completes
        // while(bit_is_set(ADCSRA, ADSC));
        // // Store value
        // reading = ADC;

        // //SEND CAN MSG
        // msg = {reading >> 8, reading & ~(0xff << 8)};

        // // Transmit message
        // CAN_transmit( 0, 0x15, 2, msg);
        PORTC ^= _BV(PC0);
        _delay_ms(500);
    }
}

