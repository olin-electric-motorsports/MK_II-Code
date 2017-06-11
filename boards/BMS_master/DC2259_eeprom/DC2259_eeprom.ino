/*!
DC2259
ltc6811-1: Battery stack monitor

REVISION HISTORY
$Revision: 1100 $
$Date: 2015-03-11

@verbatim

NOTES
 Setup:
   Set the terminal baud rate to 115200 and select the newline terminator.
   Ensure all jumpers on the demo board are installed in their default positions from the factory.
   Refer to Demo Manual DC2259.

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0)
 binary  : B10000000000
 float   : 1024.0
@endverbatim


Copyright (c) 2013, Linear Technology Corp.(LTC)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of Linear Technology Corp.

The Linear Technology Linduino is not affiliated with the official Arduino team.
However, the Linduino is only possible because of the Arduino team's commitment
to the open-source community.  Please, visit http://www.arduino.cc and
http://store.arduino.cc , and consider a purchase that will help fund their
ongoing work.

Copyright 2015 Linear Technology Corp. (LTC)
 */


/*! @file
    @ingroup ltc68111
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "LTC6811_daisy.h"
#include "eeprom.h"
#include <SPI.h>

#define ENABLED 1
#define DISABLED 0
#define CELL_CHANNELS 12
#define AUX_CHANNELS 6
#define STAT_CHANNELS 4
#define CELL 1
#define AUX 2
#define STAT 3

char get_char();


/**********************************************************
  Setup Variables
  The following variables can be modified to
  configure the software.

***********************************************************/
const uint8_t TOTAL_IC = 2;//!<number of ICs in the daisy chain

//ADC Command Configurations
const uint8_t ADC_OPT = ADC_OPT_DISABLED; // See ltc6811_daisy.h for Options
const uint8_t ADC_CONVERSION_MODE = MD_7KHZ_3KHZ; // See ltc6811_daisy.h for Options
const uint8_t ADC_DCP = DCP_ENABLED; // See ltc6811_daisy.h for Options
const uint8_t CELL_CH_TO_CONVERT = CELL_CH_ALL; // See ltc6811_daisy.h for Options
const uint8_t AUX_CH_TO_CONVERT = AUX_CH_ALL; // See ltc6811_daisy.h for Options
const uint8_t STAT_CH_TO_CONVERT = STAT_CH_ALL; // See ltc6811_daisy.h for Options

const uint16_t MEASUREMENT_LOOP_TIME = 500;//milliseconds(mS)

//Under Voltage and Over Voltage Thresholds
const uint16_t OV_THRESHOLD = 41000; // Over voltage threshold ADC Code. LSB = 0.0001
const uint16_t UV_THRESHOLD = 30000; // Under voltage threshold ADC Code. LSB = 0.0001

//Loop Measurement Setup These Variables are ENABLED or DISABLED Remember ALL CAPS
const uint8_t WRITE_CONFIG = DISABLED; // This is ENABLED or DISABLED
const uint8_t READ_CONFIG = DISABLED; // This is ENABLED or DISABLED
const uint8_t MEASURE_CELL = ENABLED; // This is ENABLED or DISABLED
const uint8_t MEASURE_AUX = ENABLED; // This is ENABLED or DISABLED
const uint8_t MEASURE_STAT = DISABLED; //This is ENABLED or DISABLED

/************************************
  END SETUP
*************************************/

/******************************************************
 *** Global Battery Variables received from 681x commands
 These variables store the results from the ltc6811
 register reads and the array lengths must be based
 on the number of ICs on the stack
 ******************************************************/
uint16_t cell_codes[TOTAL_IC][CELL_CHANNELS];
/*!<
  The cell codes will be stored in the cell_codes[][12] array in the following format:

  |  cell_codes[0][0]| cell_codes[0][1] |  cell_codes[0][2]|    .....     |  cell_codes[0][11]|  cell_codes[1][0] | cell_codes[1][1]|  .....   |
  |------------------|------------------|------------------|--------------|-------------------|-------------------|-----------------|----------|
  |IC1 Cell 1        |IC1 Cell 2        |IC1 Cell 3        |    .....     |  IC1 Cell 12      |IC2 Cell 1         |IC2 Cell 2       | .....    |
****/

uint16_t aux_codes[TOTAL_IC][AUX_CHANNELS];
/*!<
 The GPIO codes will be stored in the aux_codes[][6] array in the following format:

 |  aux_codes[0][0]| aux_codes[0][1] |  aux_codes[0][2]|  aux_codes[0][3]|  aux_codes[0][4]|  aux_codes[0][5]| aux_codes[1][0] |aux_codes[1][1]|  .....    |
 |-----------------|-----------------|-----------------|-----------------|-----------------|-----------------|-----------------|---------------|-----------|
 |IC1 GPIO1        |IC1 GPIO2        |IC1 GPIO3        |IC1 GPIO4        |IC1 GPIO5        |IC1 Vref2        |IC2 GPIO1        |IC2 GPIO2      |  .....    |
*/

uint16_t stat_codes[TOTAL_IC][4];
/*!<
 The GPIO codes will be stored in the aux_codes[][6] array in the following format:

 |  aux_codes[0][0]| aux_codes[0][1] |  aux_codes[0][2]|  aux_codes[0][3]|  aux_codes[0][4]|  aux_codes[0][5]| aux_codes[1][0] |aux_codes[1][1]|  .....    |
 |-----------------|-----------------|-----------------|-----------------|-----------------|-----------------|-----------------|---------------|-----------|
 |IC1 GPIO1        |IC1 GPIO2        |IC1 GPIO3        |IC1 GPIO4        |IC1 GPIO5        |IC1 Vref2        |IC2 GPIO1        |IC2 GPIO2      |  .....    |
*/

uint8_t flags_uvov[TOTAL_IC][3];

uint8_t system_thsd[TOTAL_IC][1];

uint8_t system_muxfail[TOTAL_IC][1];

long system_open_wire[TOTAL_IC];

uint8_t tx_cfg[TOTAL_IC][6];
/*!<
  The tx_cfg[][6] stores the ltc6811 configuration data that is going to be written
  to the ltc6811 ICs on the daisy chain. The ltc6811 configuration data that will be
  written should be stored in blocks of 6 bytes. The array should have the following format:

 |  tx_cfg[0][0]| tx_cfg[0][1] |  tx_cfg[0][2]|  tx_cfg[0][3]|  tx_cfg[0][4]|  tx_cfg[0][5]| tx_cfg[1][0] |  tx_cfg[1][1]|  tx_cfg[1][2]|  .....    |
 |--------------|--------------|--------------|--------------|--------------|--------------|--------------|--------------|--------------|-----------|
 |IC1 CFGR0     |IC1 CFGR1     |IC1 CFGR2     |IC1 CFGR3     |IC1 CFGR4     |IC1 CFGR5     |IC2 CFGR0     |IC2 CFGR1     | IC2 CFGR2    |  .....    |

*/

uint8_t rx_cfg[TOTAL_IC][8];
/*!<
  the rx_cfg[][8] array stores the data that is read back from a ltc6811-1 daisy chain.
  The configuration data for each IC  is stored in blocks of 8 bytes. Below is an table illustrating the array organization:

|rx_config[0][0]|rx_config[0][1]|rx_config[0][2]|rx_config[0][3]|rx_config[0][4]|rx_config[0][5]|rx_config[0][6]  |rx_config[0][7] |rx_config[1][0]|rx_config[1][1]|  .....    |
|---------------|---------------|---------------|---------------|---------------|---------------|-----------------|----------------|---------------|---------------|-----------|
|IC1 CFGR0      |IC1 CFGR1      |IC1 CFGR2      |IC1 CFGR3      |IC1 CFGR4      |IC1 CFGR5      |IC1 PEC High     |IC1 PEC Low     |IC2 CFGR0      |IC2 CFGR1      |  .....    |
*/

//EEprom Data structures
uint8_t tx_pdat[4] = {0xAA,0x02,0x03,0x55};
uint8_t rx_pdat[4];
uint8_t pdat_len = 4;



/*!**********************************************************************
 \brief  Inititializes hardware and variables
 ***********************************************************************/
void setup()
{
  Serial.begin(115200);
  quikeval_SPI_connect();
  spi_enable(SPI_CLOCK_DIV16); // This will set the Linduino to have a 1MHz Clock
  init_cfg();  //initialize the 681x configuration array to be written
 
  print_menu();
  pinMode(4,OUTPUT);
  digitalWrite(4,HIGH);
}

/*!*********************************************************************
  \brief main loop

***********************************************************************/
void loop()
{

  if (Serial.available())           // Check for user input
  {
    uint32_t user_command;
    user_command = read_int();      // Read the user command
    Serial.println(user_command);
    run_command(user_command);
  }
}


/*!*****************************************
  \brief executes the user command

 Menu Entry 1: Write Configuration \n

 Menu Entry 2: Read Configuration \n

 Menu Entry 3: Start Cell voltage conversion \n

 Menu Entry 4: Read cell voltages

 Menu Entry 5: Start Auxiliary voltage conversion

 Menu Entry 6: Read Auxiliary voltages

 Menu Entry 7: Start Status voltage conversion

 Menu Entry 8: Read Status Register voltages

 Menu Entry 9: Loop IC Measurements

 Menu 10: Run Open Wire Test

 Menu 11: Change Written Configuration Data

 Menu 12: Run ADC Digital Filter and Memory Self Test

 Menu 13: Enable Discharge Transistor

 Menu 14: Clear Discharge Transistors

 Menu 15: Clear All Measurement Registers

 Menu 16: Run Mux Decoder Self Test

 Menu 17: Run ADC Overlap Self Test

*******************************************/
void run_command(uint32_t cmd)
{
  int8_t error = 0;
  uint32_t conv_time = 0;
  uint32_t user_command;
  int8_t readIC=0;
  char input = 0;
  switch (cmd)
  {

    case 1: // Write Configuration Register
      wakeup_sleep(TOTAL_IC);
      ltc6811_wrcfg(TOTAL_IC,tx_cfg);
      print_config();
      break;

    case 2: // Read Configuration Register
      wakeup_sleep(TOTAL_IC);
      error = ltc6811_rdcfg(TOTAL_IC,rx_cfg);
      check_error(error);
      print_rxconfig();
      break;

    case 3: // Start Cell ADC Measurement
      wakeup_sleep(TOTAL_IC);
      ltc6811_adcv(ADC_CONVERSION_MODE,ADC_DCP,CELL_CH_TO_CONVERT);
      conv_time = ltc6811_pollAdc();
      Serial.print(F("cell conversion completed in:"));
      Serial.print(((float)conv_time/1000), 1);
      Serial.println(F("mS"));
      Serial.println();
      break;

    case 4: // Read Cell Voltage Registers
      wakeup_sleep(TOTAL_IC);
      error = ltc6811_rdcv(0, TOTAL_IC,cell_codes); // Set to read back all cell voltage registers
      check_error(error);
      print_cells();
      break;

    case 5: // Start GPIO ADC Measurement
      wakeup_sleep(TOTAL_IC);
      ltc6811_adax(MD_7KHZ_3KHZ , AUX_CH_ALL);
      ltc6811_pollAdc();
      Serial.println(F("aux conversion completed"));
      Serial.println();
      break;

    case 6: // Read AUX Voltage Registers
      wakeup_sleep(TOTAL_IC);
      error = ltc6811_rdaux(0,TOTAL_IC,aux_codes); // Set to read back all aux registers
      check_error(error);
      print_aux();
      break;

    case 7: // Start Status ADC Measurement
      wakeup_sleep(TOTAL_IC);
      ltc6811_adstat(ADC_CONVERSION_MODE, STAT_CH_TO_CONVERT);
      ltc6811_pollAdc();
      Serial.println(F("stat conversion completed"));
      Serial.println();
      break;

    case 8: // Read Status registers
      wakeup_sleep(TOTAL_IC);
      error = ltc6811_rdstat(0,TOTAL_IC,stat_codes,flags_uvov,system_muxfail,system_thsd); // Set to read back all aux registers
      check_error(error);
      print_stat();
      break;

    case 9: // Loop Measurements
      Serial.println(F("transmit 'm' to quit"));
      wakeup_sleep(TOTAL_IC);
      ltc6811_wrcfg(TOTAL_IC,tx_cfg);
      while (input != 'm')
      {
        if (Serial.available() > 0)
        {
          input = read_char();
        }
        if (WRITE_CONFIG == ENABLED)
        {
          wakeup_sleep(TOTAL_IC);
          ltc6811_wrcfg(TOTAL_IC,tx_cfg);
          print_config();
        }

        if (READ_CONFIG == ENABLED)
        {
          wakeup_sleep(TOTAL_IC);
          error = ltc6811_rdcfg(TOTAL_IC,rx_cfg);
          check_error(error);
          print_rxconfig();
        }

        if (MEASURE_CELL == ENABLED)
        {
          wakeup_idle(TOTAL_IC);
          ltc6811_adcv(ADC_CONVERSION_MODE,ADC_DCP,CELL_CH_TO_CONVERT);
          ltc6811_pollAdc();
          wakeup_idle(TOTAL_IC);
          error = ltc6811_rdcv(0, TOTAL_IC,cell_codes);
          check_error(error);
          print_cells();

        }

        if (MEASURE_AUX == ENABLED)
        {
          wakeup_idle(TOTAL_IC);
          ltc6811_adax(ADC_CONVERSION_MODE , AUX_CH_ALL);
          ltc6811_pollAdc();
          wakeup_idle(TOTAL_IC);
          error = ltc6811_rdaux(0,TOTAL_IC,aux_codes); // Set to read back all aux registers
          check_error(error);
          print_aux();
        }

        if (MEASURE_STAT == ENABLED)
        {
          wakeup_idle(TOTAL_IC);
          ltc6811_adstat(ADC_CONVERSION_MODE, STAT_CH_ALL);
          ltc6811_pollAdc();
          wakeup_idle(TOTAL_IC);
          error = ltc6811_rdstat(0,TOTAL_IC,stat_codes,flags_uvov,system_muxfail,system_thsd); // Set to read back all aux registers
          check_error(error);
          print_stat();
        }

        delay(MEASUREMENT_LOOP_TIME);
      }
      print_menu();
      break;

    case 10: // Run open wire self test
      run_openwire(system_open_wire);
      print_open();
      break;

    case 11: // Read in raw configuration data
      if (TOTAL_IC>1)
      {
        Serial.println(F("Please Enter the position of the IC "));
        input = read_int();
      }
      else input = 1;
      if ((input>0)&&(input<TOTAL_IC))read_config_data(tx_cfg,input);
      else Serial.println(F("invalid IC number"));
      print_menu();
      break;

    case 12:  // Run the ADC/Memory Self Test
      wakeup_sleep(TOTAL_IC);
      error = run_cell_adc_st(ADC_CONVERSION_MODE,CELL);
      Serial.print(error, DEC);
      Serial.println(F(" : errors detected in Digital Filter and CELL Memory \n"));

      wakeup_sleep(TOTAL_IC);
      error = run_cell_adc_st(ADC_CONVERSION_MODE, AUX);
      Serial.print(error, DEC);
      Serial.println(F(" : errors detected in Digital Filter and AUX Memory \n"));

      wakeup_sleep(TOTAL_IC);
      error = run_cell_adc_st(ADC_CONVERSION_MODE, STAT);
      Serial.print(error, DEC);
      Serial.println(F(" : errors detected in Digital Filter and STAT Memory \n"));
      print_menu();
      break;

    case 13: // Enable a discharge transistor
      Serial.println(F("Please enter the Spin number"));
      readIC = (int8_t)read_int();
      set_discharge(readIC);
      wakeup_sleep(TOTAL_IC);
      ltc6811_wrcfg(TOTAL_IC,tx_cfg);
      print_config();
      break;

    case 14: // Clear all discharge transistors
      clear_discharge();
      wakeup_sleep(TOTAL_IC);
      ltc6811_wrcfg(TOTAL_IC,tx_cfg);
      print_config();
      break;

    case 15: // Clear all ADC measurement registers
      wakeup_sleep(TOTAL_IC);
      ltc6811_clrcell();
      ltc6811_clraux();
      ltc6811_clrstat();
      Serial.println(F("All Registers Cleared"));
      break;

    case 16: // Run the Mux Decoder Self Test
      wakeup_sleep(TOTAL_IC);
      ltc6811_diagn();
      delay(5);
      error = ltc6811_rdstat(0,TOTAL_IC,stat_codes,flags_uvov,system_muxfail,system_thsd); // Set to read back all aux registers
      check_error(error);
      error = 0;
      for (int ic = 0; ic<TOTAL_IC; ic++)
      {
        if (system_muxfail[ic][0] != 0) error++;
      }
      if (error==0) Serial.println(F("Mux Test: PASS "));
      else Serial.println(F("Mux Test: FAIL "));

      break;

    case 17: // Run ADC Overlap self test
      wakeup_sleep(TOTAL_IC);
      error = (int8_t)run_adc_overlap();
      if (error==0) Serial.println(F("Overlap Test: PASS "));
      else Serial.println(F("Overlap Test: FAIL"));
      break;

    case 18: // Run ADC Redundancy self test
      wakeup_sleep(TOTAL_IC);
      error = run_adc_redundancy_st(ADC_CONVERSION_MODE, AUX);
      Serial.print(error, DEC);
      Serial.println(F(" : errors detected in AUX Measurement \n"));

      wakeup_sleep(TOTAL_IC);
      error = run_adc_redundancy_st(ADC_CONVERSION_MODE, STAT);
      Serial.print(error, DEC);
      Serial.println(F(" : errors detected in STAT Measurement \n"));
      break;

    case 19: //write control
       tx_pdat[0] = 0x11;
       tx_pdat[1] = 0x12;
       tx_pdat[2] = 0x13;
       tx_pdat[3] = 0x14;
       pdat_len = 4;
       wakeup_sleep(10);
       write_eeprom(0, tx_pdat,pdat_len); //(address, data, length)
       delay(1000);
       read_eeprom(0,  rx_pdat,pdat_len);

       Serial.println("Transmitted Data");
       for(uint8_t i = 0; i<pdat_len;i++)
       {
          Serial.print(tx_pdat[i],HEX);
          Serial.print(", ");
       }
       Serial.println();
       Serial.println("Received Data");
       for(uint8_t i = 0; i<pdat_len;i++)
       {
          Serial.print(rx_pdat[i],HEX);
          Serial.print(", ");
       }
       Serial.println();
    break;

    case 20: //write I2C
      //write_i2c( uint8_t total_ic , uint8_t address, uint8_t command, uint8_t *data, uint8_t data_len)
      //write_i2c(total_ic, 0x48, 
    
    case 'm':
      print_menu();
      break;

    default:
      Serial.println(F("Incorrect Option"));
      break;
  }
}

/*!***********************************
 \brief Initializes the configuration array
 **************************************/
void init_cfg()
{
  uint16_t uv_val = (UV_THRESHOLD/16)-1;
  uint16_t ov_val = (OV_THRESHOLD/16);
  for (int i = 0; i<TOTAL_IC; i++)
  {
    tx_cfg[i][0] = 0xFC | ADC_OPT;
    tx_cfg[i][1] = (uint8_t)(uv_val&0xFF) ;
    tx_cfg[i][2] = (uint8_t)((ov_val&0x00F)|((uv_val&0xF00)>>8)) ;
    tx_cfg[i][3] = (uint8_t)((ov_val&0xFF0)>>4);
    tx_cfg[i][4] = 0x00 ;
    tx_cfg[i][5] = 0x00 ;

  }

}

/*!*********************************
  \brief Prints the main menu
***********************************/
void print_menu()
{
  Serial.println(F("Please enter ltc6811 Command"));
  Serial.println(F("Write Configuration: 1            | Run Open Wire Test: 10"));
  Serial.println(F("Read Configuration: 2             | Change Written Configuration: 11"));
  Serial.println(F("Start Cell Voltage Conversion: 3  | Run ADC Self Test: 12"));
  Serial.println(F("Read Cell Voltages: 4             | Set Discharge: 13"));
  Serial.println(F("Start Aux Voltage Conversion: 5   | Clear Discharge: 14"));
  Serial.println(F("Read Aux Voltages: 6              | Clear Registers: 15"));
  Serial.println(F("Start Stat Voltage Conversion: 7  | Run Mux Self Test: 16"));
  Serial.println(F("Read Stat Voltages: 8             | Run ADC overlap Test: 17"));
  Serial.println(F("loop cell voltages: 9             | Run digital Redundancy Test: 18"));
  Serial.println(F("                                  | Write/Read EEprom: 19"));
  Serial.println(F("                                  | Write I2C message: 20"));
  Serial.println();
  Serial.println(F("Please enter command: "));
  Serial.println();
}

/*!************************************************************
  \brief Prints cell voltage codes to the serial port
 *************************************************************/
void print_cells()
{


  for (int current_ic = 0 ; current_ic < TOTAL_IC; current_ic++)
  {
    Serial.print(" IC ");
    Serial.print(current_ic+1,DEC);
    for (int i=0; i<CELL_CHANNELS; i++)
    {
      Serial.print(" C");
      Serial.print(i+1,DEC);
      Serial.print(":");
      Serial.print(cell_codes[current_ic][i]*0.0001,4);
      Serial.print(",");
    }
    Serial.println();
  }
  Serial.println();
}

/*!****************************************************************************
  \brief Prints Open wire test results to the serial port
 *****************************************************************************/
void print_open()
{
  for (int current_ic =0 ; current_ic < TOTAL_IC; current_ic++)
  {
    if (system_open_wire[current_ic] == 0)
    {
      Serial.print("No Opens Detected on IC: ");
      Serial.print(current_ic+1, DEC);
      Serial.println();
    }
    for (int cell=0; cell<CELL_CHANNELS+1; cell++)
    {
      if ((system_open_wire[current_ic] &(1<<cell))>0)
      {
        Serial.print(F("There is an open wire on IC: "));
        Serial.print(current_ic + 1,DEC);
        Serial.print(F(" Channel: "));
        Serial.println(cell,DEC);
      }
    }
  }
}

/*!****************************************************************************
  \brief Prints GPIO voltage codes and Vref2 voltage code onto the serial port
 *****************************************************************************/
void print_aux()
{

  for (int current_ic =0 ; current_ic < TOTAL_IC; current_ic++)
  {
    Serial.print(" IC ");
    Serial.print(current_ic+1,DEC);
    for (int i=0; i < 5; i++)
    {
      Serial.print(F(" GPIO-"));
      Serial.print(i+1,DEC);
      Serial.print(":");
      Serial.print(aux_codes[current_ic][i]*0.0001,4);
      Serial.print(",");
    }
    Serial.print(F(" Vref2"));
    Serial.print(":");
    Serial.print(aux_codes[current_ic][5]*0.0001,4);
    Serial.println();
  }
  Serial.println();
}

/*!****************************************************************************
  \brief Prints Status voltage codes and Vref2 voltage code onto the serial port
 *****************************************************************************/
void print_stat()
{

  for (int current_ic =0 ; current_ic < TOTAL_IC; current_ic++)
  {
    Serial.print(F(" IC "));
    Serial.print(current_ic+1,DEC);
    Serial.print(F(" SOC:"));
    Serial.print(stat_codes[current_ic][0]*0.0001*20,4);
    Serial.print(F(","));
    Serial.print(F(" Itemp:"));
    Serial.print(stat_codes[current_ic][1]*0.0001,4);
    Serial.print(F(","));
    Serial.print(F(" VregA:"));
    Serial.print(stat_codes[current_ic][2]*0.0001,4);
    Serial.print(F(","));
    Serial.print(F(" VregD:"));
    Serial.print(stat_codes[current_ic][3]*0.0001,4);
    Serial.println();
  }

  Serial.println();
}

/*!******************************************************************************
 \brief Prints the configuration data that is going to be written to the ltc6811
 to the serial port.
 ********************************************************************************/
void print_config()
{
  int cfg_pec;

  Serial.println(F("Written Configuration: "));
  for (int current_ic = 0; current_ic<TOTAL_IC; current_ic++)
  {
    Serial.print(F(" IC "));
    Serial.print(current_ic+1,DEC);
    Serial.print(F(": "));
    Serial.print(F("0x"));
    serial_print_hex(tx_cfg[current_ic][0]);
    Serial.print(F(", 0x"));
    serial_print_hex(tx_cfg[current_ic][1]);
    Serial.print(F(", 0x"));
    serial_print_hex(tx_cfg[current_ic][2]);
    Serial.print(F(", 0x"));
    serial_print_hex(tx_cfg[current_ic][3]);
    Serial.print(F(", 0x"));
    serial_print_hex(tx_cfg[current_ic][4]);
    Serial.print(F(", 0x"));
    serial_print_hex(tx_cfg[current_ic][5]);
    Serial.print(F(", Calculated PEC: 0x"));
    cfg_pec = pec15_calc(6,&tx_cfg[current_ic][0]);
    serial_print_hex((uint8_t)(cfg_pec>>8));
    Serial.print(F(", 0x"));
    serial_print_hex((uint8_t)(cfg_pec));
    Serial.println();
  }
  Serial.println();
}

/*!*****************************************************************
 \brief Prints the configuration data that was read back from the
 ltc6811 to the serial port.
 *******************************************************************/
void print_rxconfig()
{
  Serial.println(F("Received Configuration "));
  for (int current_ic=0; current_ic<TOTAL_IC; current_ic++)
  {
    Serial.print(F(" IC "));
    Serial.print(current_ic+1,DEC);
    Serial.print(F(": 0x"));
    serial_print_hex(rx_cfg[current_ic][0]);
    Serial.print(F(", 0x"));
    serial_print_hex(rx_cfg[current_ic][1]);
    Serial.print(F(", 0x"));
    serial_print_hex(rx_cfg[current_ic][2]);
    Serial.print(F(", 0x"));
    serial_print_hex(rx_cfg[current_ic][3]);
    Serial.print(F(", 0x"));
    serial_print_hex(rx_cfg[current_ic][4]);
    Serial.print(F(", 0x"));
    serial_print_hex(rx_cfg[current_ic][5]);
    Serial.print(F(", Received PEC: 0x"));
    serial_print_hex(rx_cfg[current_ic][6]);
    Serial.print(F(", 0x"));
    serial_print_hex(rx_cfg[current_ic][7]);
    Serial.println();
  }
  Serial.println();
}



void serial_print_hex(uint8_t data)
{
  if (data< 16)
  {
    Serial.print("0");
    Serial.print((byte)data,HEX);
  }
  else
    Serial.print((byte)data,HEX);
}

//Function to check error flag and print PEC error message
void check_error(int error)
{
  if (error == -1)
  {
    Serial.println(F("A PEC error was detected in the received data"));
  }
}

//This sets a discharge bit in the configuration register
void set_discharge(int Cell)
{
  for (int i=0; i<TOTAL_IC; i++)
  {
    if (Cell<9)
    {
      tx_cfg[i][4] = tx_cfg[i][4] + (1<<(Cell-1));
    }
    else if (Cell < 13)
    {
      tx_cfg[i][5] = tx_cfg[i][5] + (1<<(Cell-9));
    }
  }
}

//Clears the DCC bits in the configuration register of all of the ICs in a stack
void clear_discharge()
{
  for (int i=0; i<TOTAL_IC; i++)
  {
    tx_cfg[i][4] = 0;
    tx_cfg[i][5] = 0;
  }
}

int16_t run_cell_adc_st(uint8_t adc_mode, uint8_t adc_reg)
{
  int16_t error = 0;
  uint16_t expected_result = 0;
  for (int self_test = 1; self_test<3; self_test++)
  {

    expected_result = ltc6811_st_lookup(adc_mode,self_test);
    wakeup_idle(TOTAL_IC);
    switch (adc_reg)
    {
      case CELL:
        ltc6811_clrcell();
        ltc6811_cvst(adc_mode,self_test);
        ltc6811_pollAdc();
        wakeup_idle(TOTAL_IC);
        error = ltc6811_rdcv(0, TOTAL_IC,cell_codes);
        for (int ic = 0; ic < TOTAL_IC; ic++)
        {
          for (int channel=0; channel< CELL_CHANNELS; channel++)
          {
            if (cell_codes[ic][channel] != expected_result)
            {
              error = error+1;
            }
          }
        }
        break;
      case AUX:
        ltc6811_clraux();
        ltc6811_axst(adc_mode,self_test);
        ltc6811_pollAdc();
        wakeup_idle(TOTAL_IC);
        error = ltc6811_rdaux(0, TOTAL_IC,aux_codes);
        for (int ic = 0; ic < TOTAL_IC; ic++)
        {
          for (int channel=0; channel< AUX_CHANNELS; channel++)
          {
            if (aux_codes[ic][channel] != expected_result)
            {
              error = error+1;
            }
          }
        }
        break;
      case STAT:
        ltc6811_clrstat();
        ltc6811_statst(adc_mode,self_test);
        ltc6811_pollAdc();
        wakeup_idle(TOTAL_IC);
        error = ltc6811_rdstat(0,TOTAL_IC,stat_codes,flags_uvov,system_muxfail,system_thsd);
        for (int ic = 0; ic < TOTAL_IC; ic++)
        {
          for (int channel=0; channel< STAT_CHANNELS; channel++)
          {
            if (stat_codes[ic][channel] != expected_result)
            {
              error = error+1;
            }
          }
        }
        break;

      default:
        error = -1;
        break;
    }
  }
  return(error);
}

int16_t run_adc_redundancy_st(uint8_t adc_mode, uint8_t adc_reg)
{
  int16_t error = 0;
  for (int self_test = 1; self_test<3; self_test++)
  {
    wakeup_idle(TOTAL_IC);
    switch (adc_reg)
    {

      case AUX:
        ltc6811_clraux();
        ltc6811_adaxd(adc_mode,AUX_CH_ALL);
        ltc6811_pollAdc();
        wakeup_idle(TOTAL_IC);
        error = ltc6811_rdaux(0, TOTAL_IC,aux_codes);
        for (int ic = 0; ic < TOTAL_IC; ic++)
        {
          for (int channel=0; channel< AUX_CHANNELS; channel++)
          {
            if (aux_codes[ic][channel] >= 65280)
            {
              error = error+1;
            }
          }
        }
        break;
      case STAT:
        ltc6811_clrstat();
        ltc6811_adstatd(adc_mode,STAT_CH_ALL);
        ltc6811_pollAdc();
        wakeup_idle(TOTAL_IC);
        error = ltc6811_rdstat(0,TOTAL_IC,stat_codes,flags_uvov,system_muxfail,system_thsd);
        for (int ic = 0; ic < TOTAL_IC; ic++)
        {
          for (int channel=0; channel< STAT_CHANNELS; channel++)
          {
            if (stat_codes[ic][channel] >= 65280)
            {
              error = error+1;
            }
          }
        }
        break;

      default:
        error = -1;
        break;
    }
  }
  return(error);
}

void run_openwire(long *open_wire_result)
{
  uint16_t OPENWIRE_THRESHOLD = 4000;
  const uint8_t  N_CHANNELS = CELL_CHANNELS;

  uint16_t pullUp_cell_codes[TOTAL_IC][N_CHANNELS];
  uint16_t pullDwn_cell_codes[TOTAL_IC][N_CHANNELS];
  uint16_t openWire_delta[TOTAL_IC][N_CHANNELS];
  int8_t error;

  wakeup_sleep(TOTAL_IC);
  ltc6811_adow(MD_7KHZ_3KHZ,PULL_UP_CURRENT);
  ltc6811_pollAdc();
  wakeup_idle(TOTAL_IC);
  ltc6811_adow(MD_7KHZ_3KHZ,PULL_UP_CURRENT);
  ltc6811_pollAdc();
  wakeup_idle(TOTAL_IC);
  error = ltc6811_rdcv(0, TOTAL_IC,pullUp_cell_codes); // Set to read back all cell voltage registers
  check_error(error);

  wakeup_idle(TOTAL_IC);
  ltc6811_adow(MD_7KHZ_3KHZ,PULL_DOWN_CURRENT);
  ltc6811_pollAdc();
  wakeup_idle(TOTAL_IC);
  ltc6811_adow(MD_7KHZ_3KHZ,PULL_DOWN_CURRENT);
  ltc6811_pollAdc();
  wakeup_idle(TOTAL_IC);
  error = ltc6811_rdcv(0, TOTAL_IC,pullDwn_cell_codes); // Set to read back all cell voltage registers
  check_error(error);


  for (int ic=0; ic<TOTAL_IC; ic++)
  {
    open_wire_result[ic] =0;
    for (int cell=0; cell<N_CHANNELS; cell++)
    {
      if (pullDwn_cell_codes[ic][cell]>pullUp_cell_codes[ic][cell])
      {
        openWire_delta[ic][cell] = pullDwn_cell_codes[ic][cell] - pullUp_cell_codes[ic][cell]  ;
      }
      else
      {
        openWire_delta[ic][cell] = 0;
      }
      //Serial.println(openWire_delta[ic][cell],DEC);
    }
  }
  for (int ic=0; ic<TOTAL_IC; ic++)
  {
    for (int cell=1; cell<N_CHANNELS; cell++)
    {

      if (openWire_delta[ic][cell]>OPENWIRE_THRESHOLD)
      {
        open_wire_result[ic] += (1<<cell);

      }
      /*
      Serial.println();
      Serial.println(pullUp_cell_codes[0][cell],DEC);
      Serial.println(pullDwn_cell_codes[0][cell],DEC);
      */
    }
    if (pullUp_cell_codes[ic][0] == 0)
    {
      open_wire_result[ic] += 1;
    }
    if (pullUp_cell_codes[ic][N_CHANNELS-1] == 0)
    {
      open_wire_result[ic] += (1<<(N_CHANNELS));
    }
  }
}

uint16_t run_adc_overlap()
{
  uint16_t error = 0;
  int32_t measure_delta =0;
  int16_t failure_pos_limit = 20;
  int16_t failure_neg_limit = -20;
  wakeup_idle(TOTAL_IC);
  ltc6811_adol(MD_7KHZ_3KHZ,DCP_DISABLED);
  ltc6811_pollAdc();
  wakeup_idle(TOTAL_IC);
  error = ltc6811_rdcv(0, TOTAL_IC,cell_codes);
  check_error(error);
  for (int ic = 0; ic<TOTAL_IC; ic++)
  {
    measure_delta = (int32_t)cell_codes[ic][6]-(int32_t)cell_codes[ic][7];
    if ((measure_delta>failure_pos_limit) || (measure_delta<failure_neg_limit))
    {
      error = error | (1<<(ic-1));
    }
  }
  return(error);
}

// hex conversion constants
char hex_digits[16]=
{
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

// global variables

char hex_to_byte_buffer[5]=
{
  '0', 'x', '0', '0', '\0'
};               // buffer for ASCII hex to byte conversion
char byte_to_hex_buffer[3]=
{
  '\0','\0','\0'
};

char read_hex()
// read 2 hex characters from the serial buffer and convert
// them to a byte
{
  byte data;
  hex_to_byte_buffer[2]=get_char();
  hex_to_byte_buffer[3]=get_char();
  get_char();
  get_char();
  data = strtol(hex_to_byte_buffer, NULL, 0);
  return(data);
}

void read_config_data(uint8_t cfg_data[][6], uint8_t nIC)
{
  //uint8_t cfg_data[6];
  Serial.println("Please enter two HEX characters for each configuration byte ");
  Serial.print("Enter Hex for CFGR0: ");
  cfg_data[nIC][0] = read_hex();
  Serial.print(cfg_data[nIC][0],HEX);
  Serial.println();
  Serial.print("Enter Hex for CFGR1: ");
  cfg_data[nIC][1] = read_hex();
  Serial.print(cfg_data[nIC][1],HEX);
  Serial.println();
  Serial.print("Enter Hex for CFGR2: ");
  cfg_data[nIC][2] = read_hex();
  Serial.print(cfg_data[nIC][2],HEX);
  Serial.println();
  Serial.print("Enter Hex for CFGR3: ");
  cfg_data[nIC][3] = read_hex();
  Serial.print(cfg_data[nIC][3],HEX);
  Serial.println();
  Serial.print("Enter Hex for CFGR4: ");
  cfg_data[nIC][4] = read_hex();
  Serial.print(cfg_data[nIC][4],HEX);
  Serial.println();
  Serial.print("Enter Hex for CFGR5: ");
  cfg_data[nIC][5] = read_hex();
  Serial.print(cfg_data[nIC][5],HEX);
  Serial.println();
  Serial.println( "Configuration Received");

  //return(cfg_data);
}



char get_char()
{

  // read a command from the serial port
  while (Serial.available() <= 0);
  return(Serial.read());

}



