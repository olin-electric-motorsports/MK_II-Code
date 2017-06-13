#define F_CPU (4000000L)
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "can_api.h"

// Throttle1 min value is 645, max is 832, full range is 187
// Throttle2 min value is 436, max is 0x02 0x40 576, full range is 140

// Throttle 1Out is pin 17, which is ADC8 which is MUX 01000 = 0x08
// Throttle 1Out is pin 18, which is ADC9 which is MUX 01001 = 0x09

// BSPD BSO is pin 28, which is ADC4 which is MUX 00100 = 0x04

uint8_t FLAG = 0x00;
uint8_t startUpConditions = 0b00000000; // bit 0 is shudown circuit, bit 1 is button, bit 2 is brake

uint8_t throttleMin;  // our min throttle value, found by experimentation
uint8_t throttleMax;  // our max throttle value, found by experimentation
uint8_t legalDeviation;  // 10% of the throttle range



ISR(PCINT2_vect){

  uint8_t recievedMSG[8];
  CAN_read_received(0, 8, recievedMSG);

  /*** If the message is from Dashboard ***/
  if((CANIDT1 == (uint8_t)CAN_IDT_DASHBOARD>>3) && (CANIDT2 == (uint8_t)CAN_IDT_DASHBOARD<<5)){
    /**** Checks If Button Is Pressed ****/
    if(recievedMSG[0]==0b00000001)
    {
      startUpConditions |= 0b00000010;  // changes bit 1 to 1
    }
    else if(recievedMSG[0]=0b00000000)
    {
      startUpConditions &= 0b11111101;  // changes bit 1 to 0
    }
  }//end Dashboard if

  /*** If the message is from AIRControl ***/
  if((CANIDT1 == (uint8_t)CAN_IDT_AIR_CONTROL>>3) && (CANIDT2 == (uint8_t)CAN_IDT_AIR_CONTROL<<5))
  {
    /**** Checks if Shutdown Circuit is Closed ****/
    if(recievedMSG[1]==0b00000001)
    {
      startUpConditions |= 0b00000001;  // changes bit 0 to 1
    }
  }
}

uint8_t brakePlausibility(uint16_t rThrottle[], uint16_t rBrake)
{
  if(rBrake != 0)
  {
    // Set the torque command for the motor controller to 0 torque
  }
  return 0;
}


uint8_t throttleComparison(uint16_t rThrottle[])
{
  //These two if statements check if the two throttle values are within the 10% legal deviation of each other
  if((rThrottle[0] > rThrottle[1] + legalDeviation) || (rThrottle[1] > rThrottle[0] + legalDeviation) || rThrottle[0] < throttleMin || rThrottle[0] > throttleMax || rThrottle[1] < throttleMin || rThrottle[1] > throttleMax)
  {
    return 1;  // if not in legal deveiation
  }
  // If two throttle values were in the legal deviation the original msg is returned
  return 0;

}

uint8_t setinverterMSG(uint16_t rThrottle[])
{
  return 0;
}

int main (void) {
  uint16_t canCounter = 0;
  uint8_t ledCounter = 0;
  uint8_t state = 0;

  uint8_t channels[] = {8,9};

  /* Brake and Throttle states */
  uint16_t rBrake = 0x00;
  uint16_t rThrottle[] = {0x00,0x00};

  sei();
  // set up for throttle

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

  //set output this one is set to the torque encoder 1
  DDRC |= _BV(PC7);



  /**** Initialize CAN for sending brake message ****/
  CAN_init(CAN_ENABLED);


  // Set the array msg to be zero when brake nor pressed
  uint8_t msg[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  /**** initialize the array inverterMSG ****/
  //inialized to be disabled, direction be counter-clockwise("foward"), torque and speed to be 0, discharge disabled,torque limits set to default EEPROM parameters
  uint8_t inverterMSG[] = {0x00, 0x00, 0x00, 0x00, 0b00000001, 0x00, 0x00, 0x00};

  DDRC |= _BV(PC7);
  DDRB |= _BV(PB2);

  DDRB |= _BV(PB6);
  PORTB |= _BV(PB6);

  // brake light
  PCICR |= _BV(PCIE2);//in 2nd mask register
  PCMSK2 |= _BV(PCINT21); //enable interups for pin 21 the brake mosfet

  while(1)
  {
    /***** Reads and stores data from ADC for both throttle potentiometers *****/
    uint8_t i;
    for (i = 0; i <2; i++)
    {
      //Switches ADC input channel
      ADMUX &= ~(0x1F);
      ADMUX |= channels[i];

      //Read from ADC
      ADCSRA |= _BV(ADSC);
      //wait for ADC reading
      while(bit_is_set(ADCSRA,ADSC));
      rThrottle[i] = ADC;
    }

    /***** Reads and stores data from ADC for brake *****/
    ADMUX &= ~(0x1F); //Switches ADC input channel
    ADMUX |= 4;

    //Read from ADC
    ADCSRA |= _BV(ADSC);
    //wait for ADC reading
    while(bit_is_set(ADCSRA,ADSC));
    rBrake = ADC;


    /*==================State Machine=====================*/

    /***************** Pre-Startup State *******************/
    if(state==0)
    {
      /**** Checks Brake ****/
      if(rBrake != 0)
      {
        startUpConditions |= 0b00000100;
      }
      else
      {
        startUpConditions &= 0b11111011;
      }

      /**** Checks if all startup conditions are met ****/
      if(startUpConditions == 0b00000111)
      {
        state = 1;  // switches states
        msg[4] = 1;  // Says startup sequence is starting
      }
      state = 1; // CHANGE LATER DO NOT KEEP THIS LINE
    }//end of state 0

    /********************* Startup State *************************/
    else if(state==1)
    {
      msg[4] = 0;
      msg[5] = 0x01;
      /**** Starts Up The Inverter ****/
      CAN_transmit(CAN_MOB_1, 0xC0, 8, inverterMSG);
      inverterMSG[5] = 0b00000001; // sets inverterMSG to enable inverter
      CAN_transmit(CAN_MOB_1, 0xC0, 8, inverterMSG);

      state = 2;
    }//end of state 1

    /******************** Post-Startup State *********************/
    else if(state==2)
    {
        if(throttleComparison(rThrottle)==1)
        {
          //inverterMSG[5] &= 0b11111110;
        }
        else
        {
          // send torque command to inverter
        }
    }//end of state 2

    /**** Programming LED Blink Code ****/
    if(ledCounter == 50)
    {
    PORTB ^= _BV(PB6);
      ledCounter = 0;
    }
    ledCounter++;

    /********* CAN Communication Code **************/


    msg[0] = (uint8_t)(rThrottle[0] >> 8);
    msg[1] = (uint8_t) rThrottle[0];

    msg[2] = (uint8_t)(rThrottle[1] >> 8);
    msg[3] = (uint8_t) rThrottle[1];

    inverterMSG[0] = 0x05;
    inverterMSG[1] = 0xBC;

    if(canCounter == 100)
    {
      CAN_transmit(CAN_MOB_0, 0x0B, CAN_IDT_THROTTLE_L, msg);
      CAN_transmit(CAN_MOB_1, 0xC0, 8, inverterMSG);

      canCounter = 0;
    }
    canCounter ++;


    _delay_ms(1); //delay so the car doesn't flip shit

  } // end of while loop


}
