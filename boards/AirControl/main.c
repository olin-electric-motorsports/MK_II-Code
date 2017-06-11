#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "can_api.h"

// Pin Definitions
#define LED1      PC0
#define LED2      PC1
#define PORT_LED1 PORTC
#define PORT_LED2 PORTC

// External LEDs
#define EXT_LED1       PB0
#define EXT_LED2       PB1
#define PORT_EXT_LED1  PORTB
#define PORT_EXT_LED2  PORTB

// Shutdown
#define FINAL_SHUTDOWN  PC7
#define PIN_FINAL_SHUTDOWN  PINC

#define AFAG       PB5
#define ADAE       PB6
#define TAC        PB7
#define RS         PD0
#define PIN_AFAG   PINB
#define PIN_ADAE   PINB
#define PIN_TAC    PINB
#define PIN_RS     PIND

#define CAN_IDX_AHU   4
#define CAN_IDX_AFAG  3
#define CAN_IDX_ADAE  2
#define CAN_IDX_TAC   0
#define CAN_IDX_RS    0

// Precharge
#define PRECHARGE       PB3
#define PORT_PRECHARGE  PORTB

// AIR
#define AIR       PC6
#define PORT_AIR  PORTC

// CAN MObs
#define BROADCAST_MOb  0
#define BMS_READ_MOb   1

// Global Variables
volatile uint8_t gFLAG = 0x01;
#define UPDATE_STATUS  0
#define SET_PRECHARGE  1
#define SET_AIR        2

uint8_t gCAN_MSG[8] = { 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00 };
uint8_t gPRECHARGE_TIMER = 0x00;


// CAN
ISR(CAN_INT_vect) {
}


// 8-bit Timer
ISR(TIMER0_COMPA_vect) {
    // We just set a flag here and let main() handle
    // things
    gFLAG |= _BV(UPDATE_STATUS);
    
    if (bit_is_set(gFLAG, SET_PRECHARGE)) {
        if (gPRECHARGE_TIMER > 200) {
            gPRECHARGE_TIMER = 0x00;
            gFLAG &= ~_BV(SET_PRECHARGE);
            gFLAG |= _BV(SET_AIR);
        } else {
            gPRECHARGE_TIMER++;
        }
    }
}


// 16-bit Timer
ISR(TIMER1_COMPA_vect) {
}


// Set up the 8-bit timer
// Used for Polling timing
static inline void setup_8bit_timer(void) {
    // Set timer mode to CTC
    TCCR0A |= _BV(WGM01);

    // Enable interrupt on Compare Match A
    TIMSK0 |= _BV(OCIE0A);

    // Set match on 0x60/0xFF
    // Fiddle with this to change how often we
    // poll the pins
    OCR0A = 0xFE;

    // Set timer to be clkIO/1024
    TCCR0B |= 0x05;
}


// Set up the 16-bit timer
// Used for precharge timing
static inline void setup_16bit_timer(void) {
}

// Poll every input pin and update
// our CAN values.
static inline void read_all_pins(void) {
    // This is the Shutdown sense that really matters
    if (bit_is_set(PIN_FINAL_SHUTDOWN, FINAL_SHUTDOWN)) {
        gFLAG |= _BV(SET_PRECHARGE);
    } else {
        gFLAG &= ~_BV(SET_PRECHARGE);
        gFLAG &= ~_BV(SET_AIR);
        gPRECHARGE_TIMER = 0x00;
    }

    if (bit_is_set(PIN_AFAG, AFAG)) {
        gCAN_MSG[CAN_IDX_AFAG] = 0xFF;
    } else {
        gCAN_MSG[CAN_IDX_AFAG] = 0x00;
    }

    if (bit_is_set(PIN_ADAE, ADAE)) {
        gCAN_MSG[CAN_IDX_ADAE] = 0xFF;
    } else {
        gCAN_MSG[CAN_IDX_ADAE] = 0x00;
    }

    /* TODO: These are out of date
    if (bit_is_set(PIN_TAC, TAC))
    {
    }
    else
    {
    }

    if (bit_is_set(PIN_RS, RS))
    {
    }
    else
    {
    }
    */
}


int main(void) {
    sei();

    // Initialize CAN
    CAN_init(CAN_ENABLED);

    // Set up LEDs & AIR & Precharge
    DDRB |= _BV(PRECHARGE) | _BV(EXT_LED1) | _BV(EXT_LED2);
    DDRC |= _BV(LED1) | _BV(LED2) | _BV(AIR);

    // Set up our timer (8-bit timer)
    setup_8bit_timer();

    // Start with reading of the ports
    gFLAG |= _BV(UPDATE_STATUS);

    while(1) {
        if (bit_is_set(gFLAG, UPDATE_STATUS)) {
            // Blink our light for timing check
            PORT_LED1 ^= _BV(LED1);
            //PORT_LED2 ^= _BV(LED2);
            PORT_EXT_LED1 ^= _BV(EXT_LED1);

            // Update our pin values
            read_all_pins();

            // Turn on LED2 if all shutdown is high
            if ( gCAN_MSG[CAN_IDX_AHU] == 0xFF  &&
                 gCAN_MSG[CAN_IDX_AFAG] == 0xFF &&
                 gCAN_MSG[CAN_IDX_ADAE] == 0xFF ) {
                PORT_LED2 |= _BV(LED2);
            } else {
                PORT_LED2 &= ~_BV(LED2);
            }

            // Transmit after read
            CAN_transmit(BROADCAST_MOb, CAN_IDT_AIR_CONTROL,
                         CAN_IDT_AIR_CONTROL_L, gCAN_MSG);

            gFLAG &= ~_BV(UPDATE_STATUS);
        }

        if (bit_is_set(gFLAG, SET_AIR)) {
            PORT_AIR |= _BV(AIR);
        } else {
            PORT_AIR &= ~_BV(AIR);
        }

        if (bit_is_clear(CANEN2, 1)) {
            CAN_wait_on_receive(1, CAN_IDT_THROTTLE, CAN_IDT_TRANSOM_L, CAN_IDM_single);
        }

        // TODO: Put everything to sleep
    }
}

