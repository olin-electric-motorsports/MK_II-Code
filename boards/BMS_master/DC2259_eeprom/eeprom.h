/************************************
REVISION HISTORY
$Revision: 1000 $
$Date: 2016-1-4
*/

#define IC_ADDRESS 0xC8 // 1100100 0


void write_eeprom(uint8_t memory_address, uint8_t *tx_data, uint8_t data_len);
void read_eeprom(uint8_t memory_address, uint8_t *rx_data, uint8_t data_len);
void write_i2c(uint8_t total_ic, uint8_t address, uint8_t command, uint8_t *data, uint8_t data_len);
uint8_t read_i2c(uint8_t total_ic, uint8_t address, uint8_t command, uint8_t *rx_data, uint8_t data_len);

