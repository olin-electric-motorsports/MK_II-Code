/* CAN Tx */
#define F_CPU (4000000L)
#include <avr/io.h>
#include <util/delay.h>
#include "can_api.h"
#include "lcd.h"


#define boolean_bit_is_set()

#define SWITCH0 0
#define SWITCH1 1
#define SWITCH2 2
#define MSWITCH0 3
#define MSWITCH1 4
#define MSWITCH2 5
#define MSWITCH3 6

#define BUTTON0 0
#define BUTTON1 1
#define BUTTON2 2

#define MOB_THROTTLE      0
#define MOB_BMS           1
#define MOB_TX_BMS        3
#define MOB_TX_AIRCONTROL 4

#define DEBUG_MODE   0
#define BMS_MODE     1
#define CONFIG_MODE  2
#define DRIVE_MODE   3

/* Keep the state of everything */
uint8_t rSwitch     = 0x00;
uint8_t rButton     = 0x00;
uint8_t rThrottle[] = { 0x02, 0x00, 0x00 };
uint8_t rMode       = 0x00;


void initIO(void){

    //RJ45 LEDs
    DDRB |= _BV(PB2) | _BV(PB3);
    PORTB |= _BV(PB2);

    //Pot input
    DDRC &= ~_BV(PC5); // ADC9

    //Start Button sensing
    DDRB &= ~_BV(PB1); // PCINT21  2
    PORTB &= ~_BV(PB4); // Backlight LED

    // Dashboard lights
    DDRB |= _BV(PB6);  // IMD status (BLED)
    DDRB |= _BV(PB7);  // AMS status (RLED)
    PORTB &= ~_BV(PB6); //BLED
    PORTB &= ~_BV(PB7); //RLED

    // MultiSwitch
    DDRB &= ~_BV(PB1); // PCINT1   0
    DDRE &= ~_BV(PE1); // PCINT25  3
    DDRE &= ~_BV(PE2); // PCINT26  3

    // Switches
    DDRD &= ~_BV(PD3); // PCINT19  2
    DDRC &= ~_BV(PC6); // PCINT14  1
    DDRB &= ~_BV(PB3); // PCINT3   0
}


void initInterrupts(void){
    PCICR |= _BV(PCIE3) | _BV(PCIE2) | _BV(PCIE1) | _BV(PCIE0);

    PCMSK3 |= _BV(PCINT26) | _BV(PCINT25);
    PCMSK2 |= _BV(PCINT22) | _BV(PCINT21) | _BV(PCINT19);
    PCMSK1 |= _BV(PCINT14);
    PCMSK0 |= _BV(PCINT1) | _BV(PCINT3);
}


void initADC( void ){
    // Enable ADC
    ADCSRA |= _BV(ADEN);

    // Setup prescaler ( 32 )
    ADCSRA |= _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);

    //ADCSRB &= ~_BV(AREFEN);
    ADCSRB |= _BV(AREFEN);

    // Reference AVcc
    ADMUX |= _BV(REFS0);

    // Turn on correct channel for Pot input
    ADMUX |= 9;
}


void initTimer( void ){
    // 8-bit timer, CRC mode
    TCCR0A |= _BV(WGM01);

    // clk_IO/1024 prescaler
    // About 10ms per interrupt
    TCCR0B |= _BV(CS02) | _BV(CS00);

    // Interrupt on compare to OCR0A
    TIMSK0 |= _BV(OCIE0A);
    OCR0A = 100;
}


uint8_t convertVoltageToTemperature(uint8_t voltage)
{
    // Convert BMS Temperature reading into human readable temperature
    float x = ((float)(voltage))/256 * 5;
    return (uint8_t)(-3.588*x*x*x + 27.7*x*x - 85.687*x + 121.88);
}


void updateMode(void)
{
    uint8_t all_states = 0x00;
    all_states |= rSwitch >> MSWITCH0;

    switch (all_states)
    {
        case 0:
            rMode = DEBUG_MODE;
            break;
        case 1:
            rMode = BMS_MODE;
            break;
        case 2:
            rMode = CONFIG_MODE;
            break;
        case 3:
            rMode = DRIVE_MODE;
            break;
        default:
            /* I fucked up with logic somewhere... */
            rMode = 0x04;
            break;
    }
}

void updateScreen(void)
{
    char buffer[16];
    lcd_clrscr();
    switch (rMode)
    {
        case DEBUG_MODE:
            lcd_puts("Debug Mode\n");

            memset(buffer, '\0', 16);
            sprintf(buffer, "%3d %3d %3d %x",
                    rThrottle[0], rThrottle[1],
                    rThrottle[2], rSwitch);
            lcd_puts(buffer);

            break;
        case BMS_MODE:
            lcd_puts("BMS Mode\n");
            break;
        case CONFIG_MODE:
            lcd_puts("Settings Mode\n");

            memset(buffer, '\0', 16);
            sprintf(buffer, "%s %s",
                    bit_is_set(rSwitch,SWITCH0)?"S0 ON":"S0 OFF",
                    bit_is_set(rSwitch,SWITCH1)?"S1 ON":"S1 OFF"
                    );
            lcd_puts(buffer);
            break;
        case DRIVE_MODE:
            lcd_puts("Some other Mode\n");
            break;
        default:
            lcd_puts("Invalid mode!\n");

            memset(buffer, '\0', 16);
            sprintf(buffer, "%d %d %d %d %x",
                    bit_is_set(rSwitch,MSWITCH0),
                    bit_is_set(rSwitch,MSWITCH1),
                    bit_is_set(rSwitch,MSWITCH2),
                    bit_is_set(rSwitch,MSWITCH3),
                    (rSwitch >> MSWITCH0)
                    );
            lcd_puts(buffer);
            break;
    }
}

//NEW STUFF
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

//OLD STUFF
ISR(PCINT0_vect)
{
    uint8_t tmp;
    tmp = PINB;

    /* Multiswitch 0 */
    if (bit_is_set(tmp, PB1) && bit_is_clear(rSwitch, MSWITCH0))
    {
        rSwitch |= _BV(MSWITCH0);
        updateMode();
    }
    else if (bit_is_clear(tmp, PB1) && bit_is_set(rSwitch, MSWITCH0))
    {
        rSwitch &= ~_BV(MSWITCH0);
        updateMode();
    }

    /* Switch 2 */
    else if (bit_is_set(tmp, PB3) && bit_is_clear(rSwitch, SWITCH2))
    {
        rSwitch |= _BV(SWITCH2);
    }
    else if (bit_is_clear(tmp, PB3) && bit_is_set(rSwitch, SWITCH2))
    {
        rSwitch &= ~_BV(SWITCH2);
    }
}


ISR(PCINT1_vect)
{
    uint8_t tmp;
    tmp = PINC;

    /* Switch 1 */
    if (bit_is_set(tmp, PC6) && bit_is_clear(rSwitch, SWITCH1))
    {
        rSwitch |= _BV(SWITCH1);
    }
    else if (bit_is_clear(tmp, PC6) && bit_is_set(rSwitch, SWITCH1))
    {
        rSwitch &= ~_BV(SWITCH1);
    }
}


ISR(PCINT2_vect)
{
    uint8_t tmp;
    tmp = PIND;

    /* Switch 0 */
    if (bit_is_set(tmp, PD3) && bit_is_clear(rSwitch, SWITCH0))
    {
        rSwitch |= _BV(SWITCH0);
    }
    else if (bit_is_clear(tmp, PD3) && bit_is_set(rSwitch, SWITCH0))
    {
        rSwitch &= ~_BV(SWITCH0);
    }

    /* Button 0 */
    if (bit_is_set(tmp, PD5) && bit_is_clear(rButton, BUTTON0))
    {
        rButton |= _BV(BUTTON0);

        uint8_t msg[] = {0x00, 0x01};
        CAN_Tx( MOB_TX_BMS, IDT_CHARGER, IDT_CHARGER_L, msg);
    }
    else if (bit_is_clear(tmp, PD5) && bit_is_set(rButton, BUTTON0))
    {
        rButton &= ~_BV(BUTTON0);
    }

    /* Button 1 */
    if (bit_is_set(tmp, PD6))
    {
        rButton |= _BV(BUTTON1);
    }
    else if (bit_is_clear(tmp, PD6) && bit_is_set(rButton, BUTTON1))
    {
        rButton &= ~_BV(BUTTON1);
    }
}


ISR(PCINT3_vect)
{
    uint8_t tmp;
    tmp = PINE;

    /* Multiswitch 1 */
    if (bit_is_set(tmp, PE1))
    {
        rSwitch |= _BV(MSWITCH1);
        updateMode();
    }
    else if (bit_is_clear(tmp, PE1) && bit_is_set(rSwitch, MSWITCH1))
    {
        rSwitch &= ~_BV(MSWITCH1);
        updateMode();
    }

    /* Multiswitch 2 */
    if (bit_is_set(tmp, PE2))
    {
        rSwitch |= _BV(MSWITCH2);
        updateMode();
    }
    else if (bit_is_clear(tmp, PE2) && bit_is_set(rSwitch, MSWITCH2))
    {
        rSwitch &= ~_BV(MSWITCH2);
        updateMode();
    }
}


void handleConfigMode(void)
{
    uint8_t msg[] = { 0x00, 0x00 };
    if (bit_is_set(rSwitch, SWITCH1))
    {
        msg[1] = 0x01;
        CAN_Tx(MOB_TX_AIRCONTROL, IDT_DASHBOARD, IDT_DASHBOARD_L, msg);
    }
    else
    {
        msg[0] = 0x00;
        CAN_Tx(MOB_TX_AIRCONTROL, IDT_DASHBOARD, IDT_DASHBOARD_L, msg);
    }


}


void handleMode(void)
{
    switch (rMode)
    {
        case DEBUG_MODE:
            break;
        case BMS_MODE:
            break;
        case CONFIG_MODE:
            handleConfigMode();
            break;
        case DRIVE_MODE:
            break;
        default:
            break;
    }
}


ISR(TIMER0_COMPA_vect)
{
    static int delayer;
    if (delayer==10)
    {
        delayer = 0;
        updateScreen();
        handleMode();
    }
    else
    {
        delayer++;
    }
    //lcd_clrscr();
    /*
    // Que ADC reading
    ADCSRA |= _BV(ADSC);
    // Wait for ADC to finish
    while(bit_is_set(ADCSRA, ADSC));

    uint16_t reading = ADC;
    char output[16];
    sprintf(output, "%d", reading);

    lcd_puts(output);
    */
}


ISR(CAN_INT_vect)
{
    uint8_t msg;

    if (bit_is_set(CANSIT2, MOB_THROTTLE))
    {
        CANPAGE = 0x00;
        CANPAGE = MOB_THROTTLE << MOBNB0;

        msg = CANMSG;
        rThrottle[0] = CANMSG;
        rThrottle[1] = CANMSG;
        rThrottle[2] = CANMSG;

        /* Reload the CAN_Rx */
        CANSTMOB = 0x00;
        loop_until_bit_is_clear(CANEN2, 0);
        CAN_Rx(0, IDT_THROTTLE, IDT_THROTTLE_L, IDM_single);
    }
    /*
    else if (bit_is_set(CANSIT2, MOB_BMS))
    {
        CANPAGE = 0x00;
        CANPAGE = MOB_BMS << MOBNB0;

        msg = CANMSG;
        msg = CANMSG;
        msg = convertVoltageToTemperature(CANMSG);
        msg2= convertVoltageToTemperature(CANMSG);
        msg3= convertVoltageToTemperature(CANMSG);
        sprintf(buffer, "%d %d %d", msg, msg2, msg3);
        lcd_puts(buffer);
        CANSTMOB = 0x00;
        loop_until_bit_is_clear(CANEN2, 0);
        //CAN_Rx(0, IDT_THROTTLE, IDT_THROTTLE_L, IDM_single);
    }
    */
}




int main (void) {

    // Initialize CAN
    CAN_init(0, 0);

    // Set the array msg to contain 3 bytes
    uint8_t msg[] = { 0x11, 0x66, 0x0a };
    // _delay_ms(100);
    lcd_init(LCD_DISP_ON_CURSOR_BLINK);

    lcd_puts("Hello World!\n'Quite Lit!'");


    while(1)
    {
      // Toggle LED
      PORTB ^=_BV(PB3);
      // Delay for 200us
      _delay_ms(200);
      // Toggle LED
      PORTB ^= _BV(PB3);

      // lcd_clrscr()
      // lcd_puts("HELLO");
      // lcd_init(LCD_DISP_ON_BLINK);

      // Transmit message
      CAN_Tx( 0, IDT_GLOBAL, IDT_GLOBAL_L, msg );

      // Wait 2 seconds before sending again
      _delay_ms(2000);
    }
}
