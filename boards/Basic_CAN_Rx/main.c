/* CAN Rx */
#include <avr/io.h>
#include <util/delay.h>
#include "can_api.h"

ISR(CAN_INT_vect)
{
    // Read message into memory
    uint8_t msg[CAN_IDT_GLOBAL_L]; 
    CAN_read_received(0, CAN_IDT_GLOBAL_L, msg);

    // Set the chip to wait for another message.
    CAN_wait_on_receive(0, CAN_IDT_GLOBAL, CAN_IDT_GLOBAL_L, CAN_IDM_single);
}


int main (void)
{
    // Initialize CAN
    CAN_init(CAN_ENABLED);

    // Tell the CAN system to wait for a message.
    CAN_wait_on_receive(0, CAN_IDT_GLOBAL, CAN_IDT_GLOBAL_L, CAN_IDM_single);

    while(1) {
        // Wait indefinitely for a message to come.
    }
}
