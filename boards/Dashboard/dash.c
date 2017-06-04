#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "can_api.h"
#include "lcd.h"

uint8_t RTDsound_msg[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t RTDSound = 0; // whether or not the RTDSound is playing currently

ISR(CAN_INT_vect){

    // CAN ready to recieve the first message
    CAN_read_received(0, CAN_IDT_THROTTLE_L, RTDsound_msg);

    if (RTDsound_msg[4] == 0x01){ // if starting up
        PORTD |= _BV(PD6); //turn on the ready to drive sound
        RTDSound = 1;  // Indicates RTDSound is playing
    }
    lcd_clrscr();
    lcd_puts((char)RTDsound_msg[1]);

    // Set the chip to wait for another message.
    CAN_wait_on_receive(0, CAN_IDT_THROTTLE, CAN_IDT_THROTTLE_L, CAN_IDM_single);
}

int main (void) {
    /**** Start Button is pin 26 on Atmega which is ADC6 ****/
    uint8_t msg[] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

    uint16_t startButton = 0;

    uint16_t RTDSoundCounter = 0;

    lcd_init(LCD_DISP_ON_CURSOR_BLINK);

    sei(); // enables global interupts

    //Enable ADC, set prescalar to 128 (slow down ADC clock)
    //begin conversion
    ADCSRA |= _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
    //Enable internal reference voltage
    ADCSRB &= _BV(AREFEN);
    //Set internal reference voltage as AVcc
    ADMUX |= _BV(REFS0); //sets the reference voltage
    //Reads by default from ADC0 (pin 11); this line
    //  is redundant. the timer
    ADMUX |= _BV( 0x00 );
    //No prescaling on PWM clock
    TCCR0B |= _BV(CS00);
    //Set up phase-correct PWM on OC0B
    TCCR0A |= _BV(COM0B1) | _BV(WGM00);
    //Reset the other PWM pin
    TCCR0A &= ~_BV(COM0B0);

    // Initialize CAN
    CAN_init(CAN_ENABLED);

    //Recieve Message
    CAN_wait_on_receive(0, CAN_IDT_THROTTLE, CAN_IDT_THROTTLE_L, CAN_IDM_global);

    DDRD |= _BV(PD6);

    while(1) {
      /**** Reads Start Button Using ADC on pin 26 ****/
      ADMUX &= ~(0x1F); //Switches ADC input channel
      ADMUX |= 6; // ADC channel 6

      //Read from ADC
      ADCSRA |= _BV(ADSC);
      //wait for ADC reading
      while(bit_is_set(ADCSRA,ADSC));
      startButton = ADC;

      /**** RTD Sound ****/
      if(RTDSound == 1)
      {
        RTDSoundCounter++;
        if(RTDSoundCounter==2000)
        {
          RTDSound = 0;
          PORTD &= ~(_BV(PD6)); //turn the sound off because it is anoying as all hell
        }
      }

      /**** Checks if Start Button Is Pressed ****/
      if(startButton != 0)
      {
        msg[0] = 1;
      }

      /**** Sends CAN Message ****/
      CAN_transmit(0, CAN_IDT_DASHBOARD, CAN_IDT_DASHBOARD_L, msg);

      _delay_ms(1);
      if( bit_is_clear(CANEN2, 0))
      {
        CAN_wait_on_receive(0, CAN_IDT_THROTTLE, CAN_IDT_THROTTLE_L, CAN_IDM_global);
      }
    }
}
