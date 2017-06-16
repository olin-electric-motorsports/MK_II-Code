#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdio.h>

#include "can_api.h"
#include "lcd.h"

/* Definitions */
#define LED1       PB6
#define LED2       PB7
#define PORT_LED1  PORTB
#define PORT_LED2  PORTB

#define START_LED       PB4
#define PORT_START_LED  PORTB

#define R2D       PD6
#define PORT_R2D  PORTD

#define FLAG_STARTUP_BUTTON  0
#define FLAG_BRAKE_PEDAL     1
#define FLAG_AIR_CLOSED      2


/* Global Variables */
volatile uint8_t gFlags = 0x00;
uint8_t gFlags_old = 0xFF;
uint8_t gCANMessage[8] = { 0, 0, 0, 0,
                           0, 0, 0, 0 };
volatile uint8_t gR2DTimeout = 0x00;


/* Interrupts */
ISR(CAN_INT_vect) {

    // Check first MOb (Throttle)
    CANPAGE = (0 << MOBNB0);
    if (bit_is_set(CANSTMOB, RXOK)) {
        // Unrolled read for 5th byte
        volatile uint8_t msg = CANMSG;
        msg = CANMSG;
        msg = CANMSG;

        // Brake status
        if (msg == 0x01) {
            gFlags |= _BV(FLAG_BRAKE_PEDAL);
        } else {
            gFlags &= ~_BV(FLAG_BRAKE_PEDAL);
        }

        // Reset status
        CANSTMOB = 0x00;
        CAN_wait_on_receive(0,
                            CAN_IDT_THROTTLE,
                            CAN_IDT_THROTTLE_L,
                            CAN_IDM_single);
    }

    // Check second MOb (BMS)
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

    CANPAGE = (2 << MOBNB0);
    if (bit_is_set(CANSTMOB, RXOK)) {
        volatile uint8_t msg = CANMSG;
        msg = CANMSG;
        
        if (msg == 0xFF) {
            gFlags |= _BV(FLAG_AIR_CLOSED);
        } else {
            gFlags &= ~_BV(FLAG_AIR_CLOSED);
        }

        // Reset Status
        CANSTMOB = 0x00;
        CAN_wait_on_receive(2,
                            CAN_IDT_AIR_CONTROL,
                            CAN_IDT_AIR_CONTROL_L,
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
    // CAN stuff
    uint8_t err = CAN_transmit(3, 
                               CAN_IDT_DASHBOARD, 
                               CAN_IDT_DASHBOARD_L, 
                               gCANMessage);

    if (err) {
        // Reset Status
        // TODO: Actually display error
        CANPAGE = (2 << MOBNB0);
        CANSTMOB = 0x00;
    }

    // R2D timeout
    if (gR2DTimeout == 0) {
        PORT_R2D &= ~_BV(R2D);
    } else {
        gR2DTimeout--;
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

void checkShutdownState(void) {
    if (bit_is_set(PINC, PC0)) {
        gCANMessage[5] = 0xFF;
    } else {
        gCANMessage[5] = 0x00;
    }

    if (bit_is_set(PINC, PC1)) {
        gCANMessage[6] = 0xFF;
    } else {
        gCANMessage[6] = 0x00;
    }

    if (bit_is_set(PIND, PD7)) {
        gCANMessage[7] = 0xFF;
    } else {
        gCANMessage[7] = 0x00;
    }
}

void updateStateFromFlags(void) {
    uint8_t diff = gFlags^gFlags_old;

    if (bit_is_set(diff, FLAG_STARTUP_BUTTON)) {
        if (bit_is_set(gFlags, FLAG_STARTUP_BUTTON)) {
            PORT_START_LED |= _BV(START_LED);
        } else {
            PORT_START_LED &= ~_BV(START_LED);
        }
    }

    // Update our CAN message
    if (bit_is_set(gFlags, FLAG_STARTUP_BUTTON)
            && bit_is_set(gFlags, FLAG_BRAKE_PEDAL)) {
        gCANMessage[0] = 0xFF;
    } else {
        gCANMessage[0] = 0x00;
    }

    if (bit_is_set(gFlags, FLAG_AIR_CLOSED)) {
        // Enable R2D sound
        PORT_R2D |= _BV(R2D);
        gR2DTimeout = 30;

        // Print to screen
        lcd_gotoxy(0, 0);
        lcd_puts("R2D On");
    }
}

int main(void) {
    sei();
    // Setup IO
    DDRD |= _BV(R2D);
    DDRB |= _BV(PB4) | _BV(PB6) | _BV(PB7);

    // Pin change interrupts
    PCICR |= _BV(PCIE0);
    PCMSK0 |= _BV(PCINT5); // PB5

    // Set up our CAN timer
    initTimer();

    // Initialize external libraries
    lcd_init(LCD_DISP_ON_CURSOR_BLINK);
    CAN_init(CAN_ENABLED);

    // Wait on CAN
    CAN_wait_on_receive(0,
                        CAN_IDT_THROTTLE,
                        CAN_IDT_THROTTLE_L,
                        CAN_IDM_single);
    CAN_wait_on_receive(1,
                        CAN_IDT_BMS_MASTER,
                        CAN_IDT_BMS_MASTER_L,
                        CAN_IDM_single);
    CAN_wait_on_receive(2,
                        CAN_IDT_AIR_CONTROL,
                        CAN_IDT_AIR_CONTROL_L,
                        CAN_IDM_single);

    // Print something
    lcd_puts("I am Dashboard!");
    
    // Our favorite infinite loop!
    while(1) {
        checkShutdownState();

        if (gFlags != gFlags_old) {
            updateStateFromFlags();
            gFlags_old = gFlags;
        }
    }
}
