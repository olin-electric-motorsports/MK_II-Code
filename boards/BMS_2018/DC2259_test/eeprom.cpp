#include <stdint.h>
#include <Arduino.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "LTC6811_daisy.h"
#include <SPI.h>
#include "eeprom.h"

void write_eeprom(uint8_t memory_address, uint8_t *tx_data, uint8_t data_len)
{
	uint8_t control_code = 0xA0;
	uint8_t control_byte = control_code | 0x00;
	
	write_i2c_eeprom(1 , memory_address, control_byte, tx_data, data_len);
}
void read_eeprom(uint8_t memory_address, uint8_t *rx_data, uint8_t data_len)
{
	uint8_t control_code = 0xA0;
	uint8_t control_byte = control_code | 0x00;
	read_i2c_eeprom(1 , memory_address, control_byte, rx_data,  data_len);
}

void write_i2c_eeprom( uint8_t total_ic , uint8_t address, uint8_t command, uint8_t *data, uint8_t data_len)
{
  uint8_t START = 0x60;
  uint8_t ACK = 0x00;
  uint8_t NACK = 0x08;
  uint8_t BLANK = 0x00;
  uint8_t NO_TRANSMIT = 0x70;
  uint8_t NACK_STOP = 0x09;

  uint8_t loop_count;
  uint8_t transmitted_bytes =0;
  uint8_t remainder=0;
  uint8_t data_counter = 0;
  uint8_t comm[1][6];

  if (((data_len)%3)==0){loop_count = ((data_len)/3);}
  else
  {
    loop_count = ((data_len)/3);
    remainder = data_len - (loop_count*3);
    loop_count++;
  }

  comm[0][0] = START;//NO_TRANSMIT; //
  comm[0][1] = NACK_STOP;//BLANK ; //
  comm[0][2] = START + (command >> 4); //
  comm[0][3] = (command<<4) + NACK ; //
  comm[0][4] = BLANK + (address >>4);
  comm[0][5] = (address<<4) + NACK;

  Serial.print("write setup comm:  ");
  for(int i=0; i<sizeof(comm[0]); i++){
    Serial.print(comm[0][i],BIN);
    Serial.print(", ");
  }
  Serial.println(" "); 
  ltc6811_wrcomm(1,comm);
  ltc6811_stcomm();

  transmitted_bytes = 0;
  for (uint8_t i =0; i<loop_count; i++)
  {
    if ((i == (loop_count-1)) && (remainder != 0))
    {
      for (uint8_t k=0; k<remainder; k++)
      {
        comm[0][transmitted_bytes] = BLANK + (data[data_counter] >> 4); //
        if (k!=(remainder-1))comm[0][transmitted_bytes+1] = (data[data_counter]<<4) | NACK ; //
        else comm[0][transmitted_bytes+1] = (data[data_counter]<<4) | NACK_STOP;
        data_counter++;
        transmitted_bytes = transmitted_bytes +2;
      }
      for (uint8_t k=0; k<(3-remainder); k++)
      {
        comm[0][transmitted_bytes] = NO_TRANSMIT; //
        comm[0][transmitted_bytes+1] =  BLANK ; //
        transmitted_bytes = transmitted_bytes + 2;
      }
      Serial.print("write data comm:  ");
      for(int i=0; i<sizeof(comm[0]); i++){
        Serial.print(comm[0][i],BIN);
        Serial.print(", ");
      }
      Serial.println(" "); 
      ltc6811_wrcomm(1,comm);
      ltc6811_stcomm();
    }
    else
    {
      for (uint8_t k=0; k<3; k++)
      {
        comm[0][k*2] = BLANK + (data[data_counter] >> 4); //
        if (k!=2){comm[0][(k*2)+1] = (data[data_counter]<<4) | NACK ;} //
        else if(remainder!=0){ comm[0][(k*2)+1] = (data[data_counter]<<4) | NACK ;}
		else { comm[0][(k*2)+1] = (data[data_counter]<<4) | NACK_STOP ;}
        data_counter++;
      }
      ltc6811_wrcomm(1,comm);
      ltc6811_stcomm();
    }
  }
  
}

uint8_t read_i2c_eeprom( uint8_t total_ic , uint8_t address, uint8_t command, uint8_t *data, uint8_t data_len)
{
  uint8_t START = 0x60;
  uint8_t ACK = 0x00;
  uint8_t NACK = 0x08;
  uint8_t BLANK = 0x00;
  uint8_t NO_TRANSMIT = 0x70;
  uint8_t NACK_STOP = 0x09;

  uint8_t loop_count;
  uint8_t transmitted_bytes =0;
  uint8_t remainder=0;
  uint8_t data_counter = 0;
  uint8_t rx_data = 0;

  uint8_t comm[1][6];
  uint8_t rx_comm[1][8];
  if (((data_len)%3)==0)
  {
    loop_count = ((data_len)/3);
  }
  else
  {
    loop_count = ((data_len)/3);
    remainder = data_len - (loop_count*3);
    loop_count++;
  }

  comm[0][0] = START + (command >> 4); //
  comm[0][1] = ((command)<<4) + NACK ; //
  comm[0][2] = BLANK + (address >>4);
  comm[0][3] = (address<<4) + NACK;
  comm[0][4] = START + (command >> 4); //
  comm[0][5] = (command<<4) + 0x10 + ACK ; //

   Serial.print("read setup comm:  ");
  for(int i=0; i<sizeof(comm[0]); i++){
    Serial.print(comm[0][i],BIN);
    Serial.print(", ");
  }
  Serial.println(" "); 
  Serial.println(" "); 
  ltc6811_wrcomm(total_ic,comm);
  ltc6811_rdcomm(total_ic,rx_comm);
  ltc6811_stcomm();

  for (int i =0; i<loop_count; i++)
  {
    if ((i == (loop_count-1)) && (remainder != 0))
    {
      for (int k=0; k<remainder; k++)
      {
        comm[0][transmitted_bytes] = BLANK + 0x0F; //
        if (k!=(remainder-1))comm[0][transmitted_bytes+1] = 0xF0 + ACK ; //
        else comm[0][transmitted_bytes+1] = 0xF0 + NACK_STOP;
        data_counter++;
        transmitted_bytes = transmitted_bytes +2;
      }
      for (int k=0; k<(3-remainder); k++)
      {
        comm[0][transmitted_bytes] = NO_TRANSMIT; //
        comm[0][transmitted_bytes+1] =  ACK ; //
        transmitted_bytes = transmitted_bytes + 2;
      }

      Serial.print("read data comm:  ");
      for(int i=0; i<sizeof(comm[0]); i++){
        Serial.print(comm[0][i],BIN);
        Serial.print(", ");
      }
      Serial.println(" "); 
      Serial.println(" "); 
      ltc6811_wrcomm(1,comm);
      ltc6811_stcomm();
      ltc6811_rdcomm(1,rx_comm);
      data_counter=0;
      for (int k=0; k<remainder; k++)
      {
        data[rx_data] = ((rx_comm[0][data_counter]&0x0F)<<4)|((rx_comm[0][data_counter+1]&0xF0)>>4); //
        Serial.println(data[rx_data],HEX);
        rx_data++;
        data_counter = data_counter+2;
      }
    }
    else
    {
      for (uint8_t k=0; k<3; k++)
      {
        comm[0][(k*2)] = BLANK + 0x0F; //
        if (k!=2)comm[0][(k*2)+1] = 0xF0 + ACK ; //
        else if(remainder!=0){ comm[0][(k*2)+1] = 0xF0 | ACK ;}
		else { comm[0][(k*2)+1] = 0xF0 | NACK_STOP ;}
		data_counter++;
      }
      ltc6811_wrcomm(1,comm);
      ltc6811_stcomm();
      ltc6811_rdcomm(1,rx_comm);
      data_counter =0;
      for (int k=0; k<3; k++)
      {
        data[rx_data] = ((rx_comm[0][data_counter]&0x0F)<<4) | ((rx_comm[0][data_counter+1]&0xF0)>>4); //
        rx_data++;
        data_counter = data_counter+2;
      }
    }
  }

}
