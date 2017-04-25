/* CAN Tx */
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
    uint8_t msg[] = { 0x11, 0x66, 0x0a };

    // Transmit message
    CAN_Tx( 0, IDT_GLOBAL, IDT_GLOBAL_L, msg );
}
