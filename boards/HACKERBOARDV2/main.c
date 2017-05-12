#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include "can_api.h"

#include "state.h"
#include "potentiometer.h"
#include "buttons.h"
#include "display.h"
#include "timer.h"
#include "lcd.h"
#include "can.h"

// Global Globals
volatile uint8_t gFLAGS = 0x00;
volatile uint8_t gSCROLL_POS = 0;
uint8_t gSCROLL_LIMIT = MAIN_SCREEN_LENGTH;
uint8_t gDISPLAY_STATE = MAIN_SCREEN;

uint8_t gBUTTON_STATES = 0x00;
uint8_t gCAN_DATA[8] = { 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8 };
uint8_t gCAN_IDT = 0x00;
uint8_t gCAN_IDT_L = 0x00;
uint8_t gCAN_RATE = 0x00;
uint8_t gEDIT_CAN = 0;
uint8_t gCAN_ERRORS = 0;
uint8_t gCAN_MOB_USED[6] = { 0x00, 0x00, 0x00,
                             0x00, 0x00, 0x00 };
volatile uint8_t gADC_VAL = 0;

int main (void)
{
    sei(); // Allow interrupts

    // Set up IO
    initButtons();
    initADC();
    initLEDs();
    initDisplay();
    initDebounceTimer();
    initADCTimer();

    while (1)
    {
        if (bit_is_set(gFLAGS, NEW_ADC_VAL))
        {
            handle_ADC_update();
            gFLAGS &= ~_BV(NEW_ADC_VAL);
        }

        /*
        if (bit_is_set(gFLAGS, CAN_STATE_CHANGE))
        {
            handle_CAN_state_change();
            gFLAGS &= ~_BV(CAN_STATE_CHANGE);
        }
        */

        if (bit_is_set(gFLAGS, UPDATE_DISPLAY))
        {
            update_display();
            gFLAGS &= ~_BV(UPDATE_DISPLAY);
        }

        if (bit_is_set(gFLAGS, LOGICAL_ERROR))
        {
            cli();
            lcd_clrscr();
            lcd_puts("Logical Error");
            for(;;);
        }
    }
}

