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

// Global Globals
uint8_t gFLAGS = 0x00;
uint8_t gSCROLL_POS = 0;
uint8_t gSCROLL_LIMIT = MAIN_SCREEN_LENGTH;
uint8_t gDISPLAY_STATE = MAIN_SCREEN;

uint8_t gBUTTON_STATES = 0x00;
uint8_t gCAN_DATA[8] = { 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8 };
uint8_t gCAN_LEN = 0;
uint8_t gCAN_RATE = 0x00;
uint8_t gEDIT_CAN = 0;


int main (void)
{
    sei(); // Allow interrupts

    // Set up IO
    initButtons();
    initADC();
    initLEDs();
    initDisplay();
    initTimer16();

    while (1)
    {
        if (bit_is_set(gFLAGS, UPDATE_DISPLAY))
        {
            update_display();
            gFLAGS &= ~_BV(UPDATE_DISPLAY);
        }
      //uint8_t time = TCNT0;
      //if (time - old_time > 60)
      //{
      //    lcd_clrscr();
      //    char buffer[16];
      //    memset(buffer, '\0', 16);
      //    sprintf(buffer, "%x", time);
      //    lcd_puts(buffer);
      //}
    }
}

