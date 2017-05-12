#define F_CPU (4000000L)
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "can_api.h"

uint8_t FLAGS = 0x00;
uint8_t total_ic = 1; //number of BMS slaves on the daisy chain

void init_spi_master(void)
{
    // Setup SPI IO pins (MOSI and SCK)
    DDRB |= _BV(PB1) | _BV(PB7);

    // Enable SPI
    SPCR |=  _BV(SPE) | _BV(MSTR) | _BV(SPR0);

    // Enable alternate SPI (MISO_A, MOSI_A, etc)
    // Take this out if you are using standard pins!
    //MCUCR |= _BV(SPIPS);
}

uint8_t spi_message(uint8_t msg)
{
    // Set message
    SPDR = msg;

    // Wait for transmission to finish
    while(!(SPSR & (1<<SPIF))); 

    return SPDR;
}

int main (void) {
    sei(); //alllow interrupts
    // Set PB5,PB6,PC0 to output
    DDRB |= _BV(PB5) | _BV(PB6) | _BV(PB7) | _BV(PB4) | _BV(PB2);
    PORTB ^= _BV(PB5); //have program LEDs alternate
    DDRB &= ~_BV(PB3); //BSPD Current Sense
    DDRC |= _BV(PC0); //program LED
    PORTC &= ~_BV(PC0); //have the LED startup off
    PORTB |= _BV(PB2); //close relay 

    //CAN_init(0,0); //turn on CAN
    //CAN_Rx(0, IDT_GLOBAL, IDT_GLOBAL_L, IDM_single);

    PCICR |= _BV(PCIE0); //enable 0th mask register for interrupts
    PCMSK0 |= _BV(PCINT3); //enable interrupts for INT3

    // SPI init
    init_spi_master();
    
    uint8_t msg = 0x01;

    // Slave Select
    DDRD |= _BV(PD3);
    PORTD |= _BV(PD3); //set master one high
    PORTB |= _BV(PB4); //set slave one low

    uint8_t rx_cfg[total_ic][8];


    while(1) {
        // Toggle PB5,PB6,PC0
        if(FLAGS & 1){
            PORTC |= _BV(PC0); //show status of BSPD current sense
            wakeup_idle(total_ic);
            ltc6811_rdcfg(total_ic, rx_cfg);
        }
        else {
            PORTC &= ~_BV(PC0);
        }

        PORTB ^= _BV(PB5) | _BV(PB6);
        
        // PORTB &= ~_BV(PB4); //set CS low
        // uint8_t rec = spi_message(msg);
        // PORTB |= _BV(PB4); //set CS high
        // msg++;    

        // Give a delay to the toggle so it is visible
        _delay_ms(200);
    }

}

// ISR(CAN_INT_vect){
//     CANPAGE = 0x00; //reset the CAN page
//     uint8_t msg = CANMSG; //grab the first byte of the CAN message
//     PORTB ^= _BV(PB6);
//     CAN_Rx(0, IDT_GLOBAL, IDT_GLOBAL_L, IDM_single); //setup to receive again
// }

ISR(PCINT0_vect){
    if (bit_is_set(PINB, PB3)){
        FLAGS |= 1;
    }
    else {
        FLAGS &= ~1;
    }
    
}









/*
 Generic wakeup command to wake isoSPI up out of idle
*/
void wakeup_idle(uint8_t total_ic)
{
  uint8_t i;
  for (i=0; i<total_ic+1; i++)
  {
    PORTB &= ~_BV(PB4); //set CS low
    _delay_us(2); //Guarantees the isoSPI will be in ready mode
    PORTB |= _BV(PB4); //set CS high
  }
}


/*
 Writes and read a set number of bytes using the SPI port.

*/
void spi_write_read(uint8_t tx_Data[],//array of data to be written on SPI port
                    uint8_t tx_len, //length of the tx data arry
                    uint8_t *rx_data,//Input: array that will store the data read by the SPI port
                    uint8_t rx_len //Option: number of bytes to be read from the SPI port
                   )
{
  uint8_t i;
  for (i = 0; i < tx_len; i++)
  {
    spi_message(tx_Data[i]);
  }

  for (i = 0; i < rx_len; i++)
  {
    rx_data[i] = (uint8_t)spi_message(0x00);
  }

}


/*
Reads configuration registers of a ltc6811 daisy chain
*/
void ltc6811_rdcfg(uint8_t total_ic, //Number of ICs in the system
                     uint8_t r_config[][8] //A two dimensional array that the function stores the read configuration data.
                    )
{
  const uint8_t BYTES_IN_REG = 8;

  uint8_t cmd[4];
  uint8_t *rx_data;
  //int8_t pec_error = 0;
  //uint16_t data_pec;
  //uint16_t received_pec;

  rx_data = (uint8_t *) malloc((8*total_ic)*sizeof(uint8_t));

  cmd[0] = 0x00;
  cmd[1] = 0x02;
  cmd[2] = 0x2b;
  cmd[3] = 0x0A;

  wakeup_idle(total_ic); //This will guarantee that the ltc6811 isoSPI port is awake. 

  PORTB &= ~_BV(PB4); //set CS low
  spi_write_read(cmd, 4, rx_data, (BYTES_IN_REG*total_ic));         //Read the configuration data of all ICs on the daisy chain into
  PORTB |= _BV(PB4); //set CS high                         //rx_data[] array

  // for (uint8_t current_ic = 0; current_ic < total_ic; current_ic++)       //executes for each ltc6811 in the daisy chain and packs the data
  // {
  //   //into the r_config array as well as check the received Config data
  //   //for any bit errors

  //   for (uint8_t current_byte = 0; current_byte < BYTES_IN_REG; current_byte++)
  //   {
  //     r_config[current_ic][current_byte] = rx_data[current_byte + (current_ic*BYTES_IN_REG)];
  //   }

  //   received_pec = (r_config[current_ic][6]<<8) + r_config[current_ic][7];
  //   data_pec = pec15_calc(6, &r_config[current_ic][0]);
  //   if (received_pec != data_pec)
  //   {
  //     pec_error = -1;
  //   }
  // }

  // free(rx_data);
  // return(pec_error);
}
