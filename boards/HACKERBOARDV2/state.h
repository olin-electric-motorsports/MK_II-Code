// Define Global variables that keep track of the state

extern volatile uint8_t gFLAGS;
#define UPDATE_DISPLAY    0
#define NEW_ADC_VAL       1
#define CAN_STATE_CHANGE  2
#define LOGICAL_ERROR     3 // TODO: Have an error handling method

extern volatile uint8_t gSCROLL_POS;

extern uint8_t gSCROLL_LIMIT;
#define MAIN_SCREEN_LENGTH 4
#define SEND_SCREEN_LENGTH 12
#define REC_SCREEN_LENGTH 12

#define SPOOF_THROTTLE_SCREEN_LENGTH 10
#define SPOOF_THROTTLE_CAN_LENGTH 7

extern uint8_t gDISPLAY_STATE;
// Menus are 0-3
#define MAIN_SCREEN  0
#define SEND_SCREEN  1
#define REC_SCREEN   2
#define GAME_SCREEN  3

// Spoof is 4 - 13
#define SPOOF_GLOBAL_SCREEN     4
#define SPOOF_PANIC_SCREEN      5
#define SPOOF_THROTTLE_SCREEN   6
#define SPOOF_BMS_SCREEN        7
#define SPOOF_AIR_CTRL_SCREEN   8
#define SPOOF_TRANSOM_SCREEN    9
#define SPOOF_LIQ_COOL_SCREEN   10
#define SPOOF_DASHBOARD_SCREEN  11
#define SPOOF_CHARGING_SCREEN   12
#define SPOOF_MSP_SCREEN        13

// Read is 14 - 23
#define READ_GLOBAL_SCREEN     14
#define READ_PANIC_SCREEN      15
#define READ_THROTTLE_SCREEN   16
#define READ_BMS_SCREEN        17
#define READ_AIR_CTRL_SCREEN   18
#define READ_TRANSOM_SCREEN    19
#define READ_LIQ_COOL_SCREEN   20
#define READ_DASHBOARD_SCREEN  21
#define READ_CHARGING_SCREEN   22
#define READ_MSP_SCREEN        23


extern uint8_t gBUTTON_STATES;
#define BUTTON1 0 //PCINT23
#define BUTTON2 1 //PCINT0
#define BUTTON3 2 //PCINT1

extern uint8_t gCAN_DATA[8];
extern uint8_t gCAN_LEN;
extern uint8_t gCAN_RATE;
extern uint8_t gEDIT_CAN;
extern volatile uint8_t gADC_VAL;

extern uint8_t gCAN_ERRORS;
#define ERR_CAN_BUSY 0;
#define ERR_CAN_BOFF 1; // Bus off
#define ERR_CAN_BERR 2; // Bit Error
#define ERR_CAN_SERR 3; // Stuff Error
#define ERR_CAN_CERR 4; // CRC Error
#define ERR_CAN_FERR 5; // Form Error
#define ERR_CAN_AERR 6; // Ack Error
#define ERR_CAN_DLC  7; // DLC Warning Error

// TODO: Extend to use all MObs at once if needed

// Functions
void handle_select(void);
void handle_ADC_update(void);

