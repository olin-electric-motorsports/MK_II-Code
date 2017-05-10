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

void get_identifier(uint8_t *ident, uint8_t *length)
{
    if (gDISPLAY_STATE == SPOOF_GLOBAL_SCREEN ||
            gDISPLAY_STATE == READ_GLOBAL_SCREEN )
    {
        *ident = CAN_IDT_GLOBAL;
        *length = CAN_IDT_GLOBAL_L;
    }
    else if (gDISPLAY_STATE == SPOOF_PANIC_SCREEN ||
            gDISPLAY_STATE == READ_PANIC_SCREEN )
    {
        *ident = CAN_IDT_PANIC;
        *length = CAN_IDT_PANIC_L;
    }
    else if (gDISPLAY_STATE == SPOOF_THROTTLE_SCREEN ||
            gDISPLAY_STATE == READ_THROTTLE_SCREEN )
    {
        *ident = CAN_IDT_THROTTLE;
        *length = CAN_IDT_THROTTLE_L;
    }
    else if (gDISPLAY_STATE == SPOOF_BMS_SCREEN ||
            gDISPLAY_STATE == READ_BMS_SCREEN )
    {
        *ident = CAN_IDT_BMS_MASTER;
        *length = CAN_IDT_BMS_MASTER_L;
    }
    else if (gDISPLAY_STATE == SPOOF_AIR_CTRL_SCREEN ||
            gDISPLAY_STATE == READ_AIR_CTRL_SCREEN )
    {
        *ident = CAN_IDT_AIR_CONTROL;
        *length = CAN_IDT_AIR_CONTROL_L;
    }
    else if (gDISPLAY_STATE == SPOOF_TRANSOM_SCREEN ||
            gDISPLAY_STATE == READ_TRANSOM_SCREEN )
    {
        *ident = CAN_IDT_TRANSOM;
        *length = CAN_IDT_TRANSOM_L;
    }
    else if (gDISPLAY_STATE == SPOOF_LIQ_COOL_SCREEN ||
            gDISPLAY_STATE == READ_LIQ_COOL_SCREEN )
    {
        *ident = CAN_IDT_LIQUID_COOLING;
        *length = CAN_IDT_LIQUID_COOLING_L;
    }
    else if (gDISPLAY_STATE == SPOOF_DASHBOARD_SCREEN ||
            gDISPLAY_STATE == READ_DASHBOARD_SCREEN )
    {
        *ident = CAN_IDT_DASHBOARD;
        *length = CAN_IDT_DASHBOARD_L;
    }
    else if (gDISPLAY_STATE == SPOOF_CHARGING_SCREEN ||
            gDISPLAY_STATE == READ_CHARGING_SCREEN )
    {
        *ident = CAN_IDT_CHARGING;
        *length = CAN_IDT_CHARGING_L;
    }
    else if (gDISPLAY_STATE == SPOOF_MSP_SCREEN ||
            gDISPLAY_STATE == READ_MSP_SCREEN )
    {
        *ident = CAN_IDT_MSP;
        *length = CAN_IDT_MSP_L;
    }
    else
    {
        // TODO: Have a real error for this
        gFLAGS |= _BV(LOGICAL_ERROR);
    }
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
