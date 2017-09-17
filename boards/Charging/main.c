/* CAN Rx */
#define F_CPU (4000000L)
#include <avr/io.h>
#include <util/delay.h>
#include "can_api.h"
#include "lcd.h"

uint8_t led = 1;

ISR(CAN_INT_vect)
{
    // Reset CANPAGE (A necessary step; don't worry about it
    CANPAGE = 0x00;

    // msg now contains the first byte of the CAN message.
    // Repeat this call to receive the other bytes.
    uint8_t msg = CANMSG;

    //Throw LED trigger
    led = 0;

    // Set the chip to wait for another message.
    CAN_Rx(0, IDT_GLOBAL, IDT_GLOBAL_L, IDM_single);
}

int main (void) {

    // Initialize CAN
    //CAN_init(0, 0);

    // Tell the CAN system to wait for a message.
    //CAN_Rx(0, IDT_GLOBAL, IDT_GLOBAL_L, IDM_single);
    _delay_ms(100);
    lcd_init(LCD_DISP_ON_BLINK);


    while(1) {
        // Wait for trigger from led var
        if(led==1){
          // Toggle LED
          PORTB ^=_BV(PB2);
          // Delay for 200us
          _delay_ms(200);
          // Toggle LED
          PORTB ^= _BV(PB2);
          // Reset led Toggle
          led = 0;
        }

    }
}
