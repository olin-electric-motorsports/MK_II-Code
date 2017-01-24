#ifndef CAN_API_H
#define CAN_API_H

#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdlib.h>

/* Node name definitions */
//#define send_demo          ((uint8_t) 0x0)
//#define receive_demo          ((uint8_t) 0x1)

/* Message IDs */
#define IDT_GLOBAL         ((uint8_t) 0x00)
#define IDT_BMS_1          ((uint8_t) 0x01)
#define IDT_BMS_2          ((uint8_t) 0x02)
#define IDT_BMS_3          ((uint8_t) 0x03)
#define IDT_BMS_4          ((uint8_t) 0x04)
#define IDT_PANEL_BOARD    ((uint8_t) 0x05)
#define IDT_AIR_CONTROL    ((uint8_t) 0x06)
#define IDT_MCC            ((uint8_t) 0x07)
#define IDT_DASHBOARD      ((uint8_t) 0x08)
#define IDT_THROTTLE       ((uint8_t) 0x09)
#define IDT_CHARGER        ((uint8_t) 0x10)
#define IDT_DEMO           ((uint8_t) 0x11)

/* Message Lengths */
#define IDT_GLOBAL_L       ((uint8_t) 0x08)
#define IDT_BMS_L          ((uint8_t) 0x08)
#define IDT_PANEL_BOARD_L  ((uint8_t) 0x02)
#define IDT_AIR_CONTROL_L  ((uint8_t) 0x02)
#define IDT_MCC_L          ((uint8_t) 0x02)
#define IDT_DASHBOARD_L    ((uint8_t) 0x02)
#define IDT_THROTTLE_L     ((uint8_t) 0x04)
#define IDT_CHARGER_L      ((uint8_t) 0x02)
#define IDT_DEMO_L         ((uint8_t) 0x02)

/* Masks */
#define IDM_global         ((uint8_t) 0x00)
#define IDM_single         ((uint8_t) 0xff)

/* Function Prototypes */
uint8_t CAN_init( uint8_t interrupt_depth, uint8_t listen );

uint8_t CAN_Tx  ( uint8_t mob, uint8_t ident, uint8_t msg_length, uint8_t msg[]);

uint8_t CAN_Rx  ( uint8_t mob, uint8_t ident, uint8_t msg_length, uint8_t mask);

/* Notes on Usage:
 *      CAN_init MUST be called first. It sets up a variety of settings
 *      for using the CAN functionality.
 *
 *      A CAN_mob must be set as SEND in order to send a message.
 *
 *      To receive messages, you must use the CAN interrupt:
 *              ISR(CAN_INT_vect)
 *      in your code.
 */

/* Contracts for different CAN nodes

Bulkhead:
    Standard/Brake/Thrott1/Thrott2/SpeedL/SpeedR
    0x00/0x00||0x01/0x00~0xff/0x00~0xff/0x00~0xff/0x00~0xff

    PotOutofRange/Pot1/Pot2
    0x01/0x00~0xff/0x00~0xff

    PotsOutofSync/Pot1/Pot2/%diff
    0x02/0x00~0xff/0x00~0xff/0x00~0xff


Dashboard:
    Standard/standby||startup
    0x00/0x00||0xff


MCC:
    Standard/hello!
    0x00/0x00


Panelboard: (Global Watchdog)
    Standard/hello!
    0x00/0x00

    NodeNotResponding/bulkhead||dashboard||mcc||airctrl||bms1||bms2||bms3||bms4
    0x01/0x00~0xff


Air Control Board:
    Standard/Heartbeat


BMS[1-4]:
    Standard/Shunt/temp1/temp2/temp3/tempInternal/Vavg/Vstd
    0x00/0x00~0x3f/0x00~0xff/0x00~0xff/0x00~0xff/0x00~0xff/0x00~0xff/0x00~0xff

    Undervolt/cell0/cell1/cell2/cell3/cell4/cell5/cell6
    0x01/0x00||%undervolted/...

    Overtemp/temp1/temp2/temp3/internaltemp
    0x02/0x00~0xff/...

    TempRange/temp1/temp2/temp3/internaltemp
    0x03/0x00~0xff/...


Charger: (Global Watchdog Spoofer)
    Standard/Charging?
    0x00/0x00||0x01

*/

#endif
