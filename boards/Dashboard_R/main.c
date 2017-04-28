/* CAN Tx */
#define F_CPU (4000000L)
#include <avr/io.h>
#include <util/delay.h>
#include "can_api.h"
#include "lcd.h"

int main (void) {
    /* Most basic CAN transmission.
     * Sends the bytes 0x11, 0x66 and
     * 0x0a and then quits. */
    DDRB |= _BV(PB2) | _BV(PB3);
    PORTB |= _BV(PB2);

    // Initialize CAN
    CAN_init(0, 0);

    // Set the array msg to contain 3 bytes
    uint8_t msg[] = { 0x11, 0x66, 0x0a };
    // _delay_ms(100);
    lcd_init(LCD_DISP_ON_BLINK);


    while(1)
    {
      // Toggle LED
      PORTB ^=_BV(PB3);
      // Delay for 200us
      _delay_ms(200);
      // Toggle LED
      PORTB ^= _BV(PB3);

      lcd_puts("HELLO");

      // Transmit message
      CAN_Tx( 0, IDT_GLOBAL, IDT_GLOBAL_L, msg );

      // Wait 2 seconds before sending again
      _delay_ms(2000);
    }
}
