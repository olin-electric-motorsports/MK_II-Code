#define F_CPU (4000000L)
#include <avr/io.h>
#include <util/delay.h>
#include "can_api.h"

ISR(CAN_INT_vect)
{
    // Reset CANPAGE (A necessary step; don't worry about it
    CANPAGE = 0x00;

    // msg now contains the first byte of the CAN message.
    // Repeat this call to receive the other bytes.
    uint8_t msg = CANMSG;
    if (msg == 0xF1){
      PORTC ^= _BV(PC6);
      //PORTC ^= _BV(PC0);
    }

    // Set the chip to wait for another message.
    CAN_Rx(0, IDT_GLOBAL, IDT_GLOBAL_L, IDM_single);
    PORTC ^= _BV(PC0);
}

int main (void) {

  /* Most basic CAN transmission.
   * Sends the bytes 0x11, 0x66 and
   * 0x0a and then quits. */

  // Initialize CAN
  CAN_init(0, 0);

  // Set the array msg to contain 3 bytes
  uint8_t msg[] = {0xF1};

  // Transmit message
  //CAN_Tx( 0, IDT_GLOBAL, IDT_GLOBAL_L, msg );
  CAN_Rx(0, IDT_GLOBAL, IDT_GLOBAL_L, IDM_single);



    // Set PE1 to output
    // Use pin 10 to light up an LED
    DDRC |= _BV(PC6);
    DDRC |= _BV(PC0);

    while(1) {
        // Toggle PE1 (pin 10)
        // Toggles power to pin 10 to create a "blink"
        // Give a delay to the toggle so it doesn't infinitely toggle

    }
}
