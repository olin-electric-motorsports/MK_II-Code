/* CAN Tx */
#include <avr/io.h>
#include <util/delay.h>
#include "can_api.h"

int main (void) {
    /* Most basic CAN transmission. 
     * Sends the bytes 0x11, 0x66 and 
     * 0x0a and then quits. */
    
    // Initialize CAN
    CAN_init(CAN_ENABLED);

    // Set the array msg to contain 3 bytes
    uint8_t msg[CAN_IDT_GLOBAL_L] = { 0x11, 0x66, 0x0a };

    // Transmit message
    CAN_transmit( 0, CAN_IDT_GLOBAL, CAN_IDT_GLOBAL_L, msg );
}
