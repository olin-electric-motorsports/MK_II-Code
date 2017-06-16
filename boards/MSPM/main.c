// Lucky Jordan, Olin Electric Motorsports, 5/13/17
// Turns programming LED on if pins are pulled low due to
// sense lines being high

#define F_CPU (4000000L)
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "can_api.h"

// Shutdown
#define EF         PD5
#define GH         PD6
#define IJ         PD7
#define UV         PB2
#define WX         PC4
#define PIN_EF     PIND
#define PIN_GH     PIND
#define PIN_IJ     PIND
#define PIN_UV     PINB
#define PIN_WX     PINC

#define CAN_IDX_EF    4
#define CAN_IDX_GH    3
#define CAN_IDX_IJ    2
#define CAN_IDX_UV    0
#define CAN_IDX_WX    0

// CAN MObs
#define BROADCAST_MOb  0

// Global Variables
volatile uint8_t gFLAG = 0x01;
#define UPDATE_STATUS  0

uint8_t gCAN_MSG[8] = { 0x00, 0x00, 0x00, 0x00,
                        0x00, 0x00, 0x00, 0x00 };

ISR(PCINT0_vect) {
    // Set state of led at pin 29 due to
    // interrupt on pin 16
    // UV sense
    if(PINB & _BV(PB2)) {
      PORTD &= ~_BV(PD0);
    } else {
      PORTD |= _BV(PD0);
    }
}

ISR(PCINT1_vect) {
    // Set state of led at pin 25 due to
    // interrupt on pin 17
    // WX sense
    if(PINC & _BV(PC4)) {
      PORTC &= ~_BV(PC7);
    } else {
      PORTC |= _BV(PC7);
    }
}

ISR(PCINT2_vect) {
    // Set state of LEDs at pins 26, 27, and 28 due to
    // interrupts on pins 13, 14, and 15 respectively
    // EF sense
    if(PIND & _BV(PD5)) {
      PORTB &= ~_BV(PB5);
    } else {
      PORTB |= _BV(PB5);
    }

    // GH sense
    if(PIND & _BV(PD6)) {
      PORTB &= ~_BV(PB6);
    } else {
      PORTB |= _BV(PB6);
    }

    // IJ sense
    if(PIND & _BV(PD7)) {
      PORTB &= ~_BV(PB7);
    } else {
      PORTB |= _BV(PB7);
    }
}

// 8-bit Timer
ISR(TIMER0_COMPA_vect)
{
    // We just set a flag here and let main() handle
    // things
    gFLAG |= _BV(UPDATE_STATUS);
}


// Set up the 8-bit timer
// Used for Polling timing
static inline void setup_8bit_timer(void)
{
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

// Poll every input pin and update
// our CAN values.
static inline void read_all_pins(void)
{
    if (bit_is_set(PIN_EF, EF))
    {
        gCAN_MSG[CAN_IDX_EF] = 0xFF;
    }
    else
    {
        gCAN_MSG[CAN_IDX_EF] = 0x00;
    }

    if (bit_is_set(PIN_GH, GH))
    {
        gCAN_MSG[CAN_IDX_GH] = 0xFF;
    }
    else
    {
        gCAN_MSG[CAN_IDX_GH] = 0x00;
    }

    if (bit_is_set(PIN_IJ, IJ))
    {
        gCAN_MSG[CAN_IDX_IJ] = 0xFF;
    }
    else
    {
        gCAN_MSG[CAN_IDX_IJ] = 0x00;
    }
    if (bit_is_set(PIN_UV, UV))
    {
        gCAN_MSG[CAN_IDX_UV] = 0xFF;
    }
    else
    {
        gCAN_MSG[CAN_IDX_UV] = 0x00;
    }

    if (bit_is_set(PIN_WX, WX))
    {
        gCAN_MSG[CAN_IDX_WX] = 0xFF;
    }
    else
    {
        gCAN_MSG[CAN_IDX_WX] = 0x00;
    }

}

int main (void) {
    sei(); // Enable global interrupts

    // Set PC7, PB5, PB6, PB7, and PD0 to output (pins 25, 26, 27, 28, and 29 respectively)
    DDRC |= _BV(PC7);
    DDRB |= _BV(PB5) | _BV(PB6) | _BV(PB7);
    DDRD |= _BV(PD0);

    // Set PD5, PD6, PD7, PB2, and PC4 to input (pins 13, 14, 15, 16, and 17 respectively)
    DDRD &= ~_BV(PD5) & ~_BV(PD6) & ~_BV(PD7);
    DDRB &= ~_BV(PB2);
    DDRC &= ~_BV(PC4);

    // --------------------------------->
    // Check all pins before setting up interrupts and running main loop

    // UV sense
    if(PINB & _BV(PB2)) {
      PORTD &= ~_BV(PD0);
    } else {
      PORTD |= _BV(PD0);
    }

    // WX sense
    if(PINC & _BV(PC4)) {
      PORTC &= ~_BV(PC7);
    } else {
      PORTC |= _BV(PC7);
    }

    // EF sense
    if(PIND & _BV(PD5)) {
      PORTB &= ~_BV(PB5);
    } else {
      PORTB |= _BV(PB5);
    }

    // GH sense
    if(PIND & _BV(PD6)) {
      PORTB &= ~_BV(PB6);
    } else {
      PORTB |= _BV(PB6);
    }

    // IJ sense
    if(PIND & _BV(PD7)) {
      PORTB &= ~_BV(PB7);
    } else {
      PORTB |= _BV(PB7);
    }

    // Set up interrupts
    PCICR |= _BV(PCIE0) | _BV(PCIE1) | _BV(PCIE2); // enable mask registers for interrupts
    PCMSK0 |= _BV(PCINT2); // PB2
    PCMSK1 |= _BV(PCINT12); // PC4
    PCMSK2 |= _BV(PCINT21) | _BV(PCINT22) | _BV(PCINT23); // PD5, PD6, AND PD7 respectively

    for(;;){
      if (bit_is_set(gFLAG, UPDATE_STATUS))
      {
          // Update our pin values
          read_all_pins();

          // Transmit after read
          CAN_transmit(BROADCAST_MOb, CAN_IDT_AIR_CONTROL,
                       CAN_IDT_AIR_CONTROL_L, gCAN_MSG);

          gFLAG &= ~_BV(UPDATE_STATUS);
      }
    }
}
