#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

#include "display.h"
#include "lcd.h"
#include "state.h"


void initLEDs(void)
{
    // Set up LED IO
    DDRD |= _BV(PD0) | _BV(PD1);
    DDRC |= _BV(PC0);

    // Startup Blink
    PORT_LED1 |= _BV(LED1);
    _delay_ms(200);
    PORT_LED1 &= ~_BV(LED1);
    PORT_LED2 |= _BV(LED2);
    _delay_ms(200);
    PORT_LED2 &= ~_BV(LED2);
    PORT_LED3 |= _BV(LED3);
    _delay_ms(200);
    PORT_LED3 &= ~_BV(LED3);

    PORT_LED1 |= _BV(LED1);
    PORT_LED2 |= _BV(LED2);
    PORT_LED3 |= _BV(LED3);
    _delay_ms(200);
    PORT_LED1 &= ~_BV(LED1);
    PORT_LED2 &= ~_BV(LED2);
    PORT_LED3 &= ~_BV(LED3);
    _delay_ms(200);
    PORT_LED1 |= _BV(LED1);
    PORT_LED2 |= _BV(LED2);
    PORT_LED3 |= _BV(LED3);
    _delay_ms(200);
    PORT_LED1 &= ~_BV(LED1);
    PORT_LED2 &= ~_BV(LED2);
    PORT_LED3 &= ~_BV(LED3);
}

inline void initDisplay(void)
{
    lcd_init(LCD_DISP_ON_CURSOR_BLINK);
    lcd_puts("HackerBoard V2\nSoftware 0.1");
}

static inline void update_spoof_screen(void)
{
    char *spoof_throttle[] = { "Rate    ",
                               "Torque1 ", 
                               "Torque2 ", 
                               "Brake   ", 
                               "BSPD    ",
                               "ShD 0x05", 
                               "ShD 0x06", 
                               "ShD 0x07", 
                               "CAN ERR ",
                               "Back    " };

    char **list;
    switch (gDISPLAY_STATE)
    {
        case SPOOF_THROTTLE_SCREEN:
            list = spoof_throttle;
            break;
        default:
            break;
    }

    // Update display
    lcd_clrscr();
    char buffer[33];
    char val1[9];
    char val2[9];

    memset(buffer, '\0', 33);
    memset(val1, '\0', 9);
    memset(val2, '\0', 9);

    if (gSCROLL_POS % 2 == 0)
    {
        if ( (uint8_t) (gSCROLL_POS - 1) < gCAN_IDT_L)
        {
            sprintf(val1, "    0x%02X", gCAN_DATA[gSCROLL_POS-1]);
        }
        else if (gSCROLL_POS == 0)
        {
            sprintf(val1, "    0x%02X", gCAN_RATE);
        }

        if ( gSCROLL_POS < gCAN_IDT_L )
        {
            sprintf(val2, "    0x%02X", gCAN_DATA[gSCROLL_POS]);
        }

        sprintf(buffer, "%s%s\n%s%s", list[gSCROLL_POS], val1,
                list[gSCROLL_POS+1], val2);
    }
    else
    {
        if ( (uint8_t) (gSCROLL_POS - 2) < gCAN_IDT_L)
        {
            sprintf(val1, "    0x%02X", gCAN_DATA[gSCROLL_POS-2]);
        }
        else if ((gSCROLL_POS - 1) == 0)
        {
            sprintf(val1, "    0x%02X", gCAN_RATE);
        }

        if ( (uint8_t) (gSCROLL_POS - 1) < gCAN_IDT_L)
        {
            sprintf(val2, "    0x%02X", gCAN_DATA[gSCROLL_POS-1]);
        }

        sprintf(buffer, "%s%s\n%s%s", list[gSCROLL_POS-1], val1,
                list[gSCROLL_POS], val2);
    }

    lcd_puts(buffer);
    if (gEDIT_CAN)
    {
        lcd_gotoxy(9, (gSCROLL_POS % 2));
        lcd_putc('E');
        lcd_putc(':');
    }
    else
    {
        lcd_gotoxy(0, (gSCROLL_POS % 2));
    }
}

inline void update_main_screen(void)
{
    // Main Menu list of strings
    char *main_menu[] = { "Send", "Receive", "Games", "Hacker" };

    // Board list of strings
    char *board_list[] = { "Global", "Panic", "Throttle",
                           "BMS", "Air Control", "Transom", 
                           "Liquid Cooling", "Dashboard", 
                           "Charging", "MSP", "Back", "   "};


    char **list;
    switch (gDISPLAY_STATE)
    {
        case MAIN_SCREEN:
            list = main_menu;
            break;
        case SEND_SCREEN:
            list = board_list;
            break;
        case REC_SCREEN:
            list = board_list;
            break;
        default:
            list = main_menu;
    }

    // Update display
    lcd_clrscr();
    char buffer[33];
    memset(buffer, '\0', 33);
    if (gSCROLL_POS % 2 == 0)
    {
        sprintf(buffer, "%s\n%s", list[gSCROLL_POS],
                list[gSCROLL_POS+1]);
    }
    else
    {
        sprintf(buffer, "%s\n%s", list[gSCROLL_POS-1],
                list[gSCROLL_POS]);
    }
    lcd_puts(buffer);
    lcd_gotoxy(0, (gSCROLL_POS % 2));
}

void update_display(void)
{
    if (gDISPLAY_STATE < 4)
    {
        update_main_screen();
    }
    else if (gDISPLAY_STATE < 14)
    {
        update_spoof_screen();
    }
}
