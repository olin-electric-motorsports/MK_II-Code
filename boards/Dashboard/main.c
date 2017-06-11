#include <avr/io.h>
#include <avr/interrupt.h>

#include "can_api.h"
#include "lcd.h"

/* Definitions */
#define LED1  PB6
#define LED2  PB7
#define PORT_LED1  PORTB
#define PORT_LED2  PORTB

#define START_LED  PB4
#define PORT_START_LED  PORTB

#define FLAG_STARTUP_BUTTON 0


/* Global Variables */
volatile uint8_t gFlags = 0x00;
uint8_t gFlags_old = 0x00;
uint8_t gCANMessage[8] = { 0, 0, 0, 0,
                           0, 0, 0, 0 };

/* Interrupts */
ISR(CAN_INT_vect) {

    // Check first MOb
    CANPAGE = (0 << MOBNB0);
    if (bit_is_set(CANSTMOB, RXOK)) {
        // Unrolled read for 5th byte
        volatile uint8_t msg = CANMSG;
        msg = CANMSG;
        msg = CANMSG;
        msg = CANMSG;
        msg = CANMSG;

        if (msg == 0x01) {
            // Ready to drive sound
            PORTD |= _BV(PD6);
        }

        // Reset status
        CANSTMOB = 0x00;
        CAN_wait_on_receive(0,
                            CAN_IDT_THROTTLE,
                            CAN_IDT_THROTTLE_L,
                            CAN_IDM_single);
    }


    // Check second MOb
    CANPAGE = (1 << MOBNB0);
    if (bit_is_set(CANSTMOB, RXOK)) {
        volatile uint8_t msg = CANMSG;
        if (msg == 0xFF) {
            PORT_LED1 |= _BV(LED1);
        } else {
            PORT_LED1 &= ~_BV(LED1);
        }

        msg = CANMSG;
        if (msg == 0xFF) {
            PORT_LED2 |= _BV(LED2);
        } else {
            PORT_LED2 &= ~_BV(LED2);
        }

        // Reset Status
        CANSTMOB = 0x00;
        CAN_wait_on_receive(1, 
                            CAN_IDT_BMS_MASTER, 
                            CAN_IDT_BMS_MASTER_L, 
                            CAN_IDM_single);
    }
}

ISR(PCINT0_vect) {
    if(bit_is_set(PINB, PB5)) {
        gFlags |= _BV(FLAG_STARTUP_BUTTON);
    } else {
        gFlags &= ~_BV(FLAG_STARTUP_BUTTON);
    }
}

ISR(TIMER0_COMPA_vect) {
    uint8_t err = CAN_transmit(2, 
                               CAN_IDT_DASHBOARD, 
                               CAN_IDT_DASHBOARD_L, 
                               gCANMessage);

    if (err) {
        // Reset Status
        // TODO: Actually display error
        CANPAGE = (2 << MOBNB0);
        CANSTMOB = 0x00;
    }
}


/* Functions */

void initTimer(void) {
    // Set up 8-bit timer in CTC mode
    TCCR0A = _BV(WGM01);

    // clkio/1024 prescaler
    TCCR0B = 0x05;

    TIMSK0 |= _BV(OCIE0A);

    OCR0A = 0xFF;
}

void updateStateFromFlags(void) {

    // TODO: Do some clever bit magic so any
    // flag change doesn't set off this branch
    if (bit_is_set(gFlags, FLAG_STARTUP_BUTTON)) {
        gCANMessage[0] = 0xFF;
        lcd_gotoxy(0, 1);
        lcd_puts("Startup On ");
    } else {
        gCANMessage[0] = 0x00;
        lcd_gotoxy(0, 1);
        lcd_puts("Startup Off");
    }
}

int main(void) {
    sei();

    // Sense Start button
    PCICR |= _BV(PCIE0);
    PCMSK0 |= _BV(PCINT5);

    // Set up our CAN timer
    initTimer();

    // Initialize external libraries
    lcd_init(LCD_DISP_ON_CURSOR_BLINK);
    CAN_init(CAN_ENABLED);

    // Wait on CAN
    CAN_wait_on_receive(0, CAN_IDT_THROTTLE, CAN_IDT_THROTTLE_L, CAN_IDM_single);
    CAN_wait_on_receive(1, CAN_IDT_BMS_MASTER, CAN_IDT_BMS_MASTER_L, CAN_IDM_single);

    // Print something
    lcd_puts("I am Dashboard!");
    
    // Our favorite infinite loop!
    while(1) {
        if (gFlags != gFlags_old) {
            updateStateFromFlags();
            gFlags_old = gFlags;
        }
    }
}
