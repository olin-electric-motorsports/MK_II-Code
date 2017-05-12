#include <avr/io.h>
#include "can_api.h"
#include "state.h"

ISR(CAN_INT_vect)
{
}

void initCAN(void)
{
    // Start in disabled
    CAN_init(CAN_ENABLED);

    // Enable all interrupts
    CANGIE |= _BV(ENBOFF) | _BV(ENTX) | _BV(ENERR) |
              _BV(ENBX) | _BV(ENERG) | _BV(ENOVRT);
}

void end_all_CAN(void)
{
    CANGCON |= _BV(ABRQ);
}


void start_CAN_transmit(void)
{
    // TODO: Allow usage of multiple MObs
    uint8_t MOb = 0;

    // Select MOb 0
    CANPAGE = MOb << MOBNB0;

    // Is this MOb busy?
    while (bit_is_set(CANEN2, MOb))
    {
        // Abort all
        CANGCON |= _BV(ABRQ);
    }

    // Reset the abort request
    CANGCON &= ~_BV(ABRQ);

    // Start transmit
    uint8_t ident;
    uint8_t length;
    get_identifier(&ident, &length);
    CAN_transmit(MOb, ident, length, gCAN_DATA);
}

void start_CAN_reception(void)
{
    // TODO: Allow usage of multiple MObs
    uint8_t MOb = 0;

    // Select MOb 0
    CANPAGE = MOb << MOBNB0;

    // Is this MOb busy?
    while (bit_is_set(CANEN2, MOb))
    {
        // Abort all
        CANGCON |= _BV(ABRQ);
    }

    // Reset the abort request
    CANGCON &= ~_BV(ABRQ);

    // Start transmit
    uint8_t ident;
    uint8_t length;
    get_identifier(&ident, &length);
    CAN_wait_on_receive(MOb, ident, length, CAN_IDM_single);
}
