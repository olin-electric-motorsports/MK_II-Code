#ifndef DISPLAY_H
#define DISPLAY_H

// LEDs
#define LED1 PD0
#define LED2 PC0
#define LED3 PD1

#define PORT_LED1 PORTD
#define PORT_LED2 PORTC
#define PORT_LED3 PORTD

void initLEDs ( void );
void initDisplay ( void );
void update_display ( void );
void update_scroll ( void );

#endif
