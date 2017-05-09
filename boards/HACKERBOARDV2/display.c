#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

#include "display.h"
#include "lcd.h"
#include "state.h"

char *main_menu[] = { "Send", "Receive", "Games", "blank" };

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

void update_scroll(void)
{
    // Should push this into Model
    static uint8_t scroll;
    if (bit_is_set(gBUTTON_STATES, BUTTON3))
    {
        scroll++;
        if (scroll > 3)
        {
            scroll = 3;
        }
    }
    else if (bit_is_set(gBUTTON_STATES, BUTTON1))
    {
        scroll--;
        if (scroll > 3)
        {
            scroll = 0;
        }
    }


    // Update display
    lcd_clrscr();
    char buffer[16];
    memset(buffer, '\0', 16);
    if (scroll % 2 == 0)
    {
        sprintf(buffer, "%s\n%s", main_menu[scroll], main_menu[scroll+1]);
    }
    else
    {
        sprintf(buffer, "%s\n%s", main_menu[scroll-1], main_menu[scroll]);
    }
    lcd_puts(buffer);
    lcd_gotoxy(0, (scroll % 2));
}

inline void update_display(void)
{
}


