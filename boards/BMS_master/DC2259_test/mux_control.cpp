#include <stdint.h>
#include "i2c.h"
#include "LT_SPI.h"
#include "LTC6811_daisy.h"
#include "mux_control.h"
#include <Arduino.h>


void set_mux_channel(uint8_t total_ic, uint8_t i2c_address, uint8_t channel)
{
	uint8_t command = 0x08 | channel;
	write_i2c(total_ic, i2c_address, command, 0, 0); //(total_ic, address, command, data, data_length)
}

void mux_disable(uint8_t total_ic, uint8_t i2c_address)
{
	uint8_t command = 0x00;
	write_i2c(total_ic, i2c_address, command, 0, 0); //(total_ic, address, command, data, data_length)
}

void read_mux_output(uint8_t total_ic)
{
	
}