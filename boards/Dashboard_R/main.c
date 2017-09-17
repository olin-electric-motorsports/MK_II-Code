/* CAN Tx */
#define F_CPU (4000000L)
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
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

#define SBUTTON0 0  //Start Button
#define SBUTTON1 1  //Start Button

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
      DDRB |= _BV(PB2) | _BV(PB3);  // PCINT2 & PCINT3
      PORTB |= _BV(PB2);

    //Start Button sensing
      DDRB &= ~_BV(PB5); // PCINT5 Button
      PORTB &= ~_BV(PB4); // Backlight LED  PCINT4

    // Dashboard lights
      DDRB |= _BV(PB6);  // IMD status (BLED) PCINT6
      DDRB |= _BV(PB7);  // AMS status (RLED) PCINT7
      PORTB &= ~_BV(PB6); //IMD status (BLED)
      PORTB &= ~_BV(PB7); //AMS status (RLED)

    // Blue Buttons
      DDRB &= ~_BV(PB0); // PCINT0  Button1
      DDRB &= ~_BV(PB1); // PCINT1  Button2

    //RTD Speaker Control
      DDRC |= _BV(PC1); //Enable PCINT9

    //PWM init
      //Output compare pin is OC1B, so we need OCR1B as our counter
      TCCR0B |= _BV(CS00); //Clock prescale set to max speed
      TCCR0A |= _BV(COM1B1) | _BV(WGM00); //Enable the right pwm compare and mode
      TCCR0A &= ~_BV(COM0B1); //Make sure other PWM is off
      DDRC |= _BV(PC1); //Enable

      OCR1B = (uint8_t) 0x0F; //Set counter to a 50% duty cycle
}


void initInterrupts(void){
    PCICR |= _BV(PCIE3) | _BV(PCIE2) | _BV(PCIE1) | _BV(PCIE0); // Pin Change Interrupt Flag Register

    //Pin Change Mask Register 2
    // QR Sense and MN Sense
    PCMSK2 |= _BV(PCINT22) | _BV(PCINT23);

    //Pin Change Mask Register 1
    // OP Sense, RTD Control, CAN_Tx, CAN_Rx
    PCMSK1 |= _BV(PCINT8) | _BV(PCINT9) | _BV(PCINT10) | _BV(PCINT11);

    //Pin Change Mask Register 0
    // Blue Button 1, Blue Button 2, Start, BLED, and RLED
    PCMSK0 |= _BV(PCINT0) | _BV(PCINT1) | _BV(PCINT5) | _BV(PCINT6) | _BV(PCINT7);
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


void updateScreen(void)
{
    char buffer[16];
    lcd_clrscr();

}

//NEW STUFF
ISR(PCINT5)
{
    uint8_t tmp;
    tmp = PINB;

    //Start Button Signaling Drive Mode Enabled
    if (bit_is_set(tmp, PB5))
    {
        PORTB ^=_BV(PB4);

        uint8_t msg[] = {0x00, 0x01};
        CAN_Tx( MOB_TX_BMS, IDT_CHARGER, IDT_CHARGER_L, msg);
    }
}

ISR(PCINT0)
{
    //Blue Button2
    if (bit_is_set( , ))
    {
      /*
        TODO
      */
    }
}

ISR(PCINT1)
{
    //Blue Button1
    if (bit_is_set( , ))
    {
      /*
        TODO
      */
    }
}

ISR(PCINT5)
{
    //Start Button
    if (bit_is_set( , ))
    {
      /*
        TODO
      */
    }
}

ISR(PCINT6)
{
    //Blue LED
    if (bit_is_set( , ))
    {
      /*
        TODO
      */
    }
}

ISR(PCINT7)
{
    //Red LED
    if (bit_is_set( , ))
    {
      /*
        TODO
      */
    }
}

//SENSE LINES

ISR(PCINT8)
{
    //OP Sense
    if (bit_is_set( , ))
    {
      /*
        TODO
      */
    }
}

ISR(PCINT22)
{
    //QR Sense
    if (bit_is_set( , ))
    {
      /*
        TODO
      */
    }
}

ISR(PCINT23)
{
    //MN Sense
    if (bit_is_set( , ))
    {
      /*
        TODO
      */
    }
}

//END SENSE LINES

ISR(PCINT9)
{
    //RTD Control
    if (bit_is_set( , ))
    {
      /*
        TODO
      */
    }
}


ISR(CAN_INT_vect)
{
    // Reset CANPAGE (A necessary step; don't worry about it
    CANPAGE = 0x00;

    // msg now contains the first byte of the CAN message.
    // Repeat this call to receive the other bytes.
    uint8_t msg = CANMSG;


    if (bit_is_set( , ))
    {
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

  sei();

  initIO();
  initInterrupts();
  initTimer();

  CAN_init(0, 0);
  CAN_Rx(MOB_THROTTLE, IDT_THROTTLE, IDT_THROTTLE_L, IDM_single); //TODO
  CAN_Rx(MOB_BMS, IDT_BMS_2, IDT_BMS_L, IDM_single);  //TODO

  lcd_init(LCD_DISP_ON_CURSOR_BLINK);
  lcd_clrscr();
  lcd_puts("Ready to go!\n");

  for(;;){
  }
}
