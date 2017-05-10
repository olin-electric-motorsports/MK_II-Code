#ifndef BUTTONS_H
#define BUTTONS_H

void initButtons ( void );
void led_follow_button ( volatile uint8_t *ledPort, uint8_t ledPin, uint8_t button );

// INTERRUPTS IMPLEMENTED
/*
ISR(PCINT0_vect);
ISR(PCINT2_vect);
*/

#endif
