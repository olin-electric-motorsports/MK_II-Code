/* CAN Rx */
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

    // Set the chip to wait for another message.
    CAN_Rx(0, IDT_GLOBAL, IDT_GLOBAL_L, IDM_single);
}

int main (void) {
    
    // Initialize CAN
    CAN_init(0, 0);

    // Tell the CAN system to wait for a message.
    CAN_Rx(0, IDT_GLOBAL, IDT_GLOBAL_L, IDM_single);

    while(1) {
        // Wait indefinitely for a message to come.
    }
}
