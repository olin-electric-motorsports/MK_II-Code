// Define Global variables that keep track of the state

extern uint8_t gFLAGS;
#define UPDATE_DISPLAY 0

extern uint8_t gSCROLL_POS;

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


extern uint8_t gBUTTON_STATES;
#define BUTTON1 0 //PCINT23
#define BUTTON2 1 //PCINT0
#define BUTTON3 2 //PCINT1

extern uint8_t gCAN_DATA[8];
extern uint8_t gCAN_LEN;
extern uint8_t gCAN_RATE;
extern uint8_t gEDIT_CAN;


// Functions
void handle_select(void);
