#define F_CPU (4000000L)
#include <avr/io.h>
#include <util/delay.h>
#include "can_api.h"


int main (void) {

  /* Most basic CAN transmission.
   * Sends the bytes 0x11, 0x66 and
   * 0x0a and then quits. */

  // Initialize CAN
  CAN_init(0, 0);

  // Set the array msg to contain 3 bytes
  uint8_t msg[] = {0xF1};


  // Transmit message


    // Set PE1 to output
    // Use pin 10 to light up an LED
    DDRB |= _BV(PB2);

    while(1) {
        // Toggle PE1 (pin 10)
        // Toggles power to pin 10 to create a "blink"
        CAN_Tx( 0, IDT_GLOBAL, IDT_GLOBAL_L, msg );
        PORTB ^= _BV(PB2);
        _delay_ms(500);

        // Give a delay to the toggle so it doesn't infinitely toggle

    }
}
