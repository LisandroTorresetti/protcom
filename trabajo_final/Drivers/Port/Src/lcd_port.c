#include "lcd_port.h"
#include "i2c_core.h"

static const uint16_t LCD_ADDRESS = 0x27;

bool lcd_write(uint8_t* data, uint16_t size) {
	return I2C_master_transmit(LCD_ADDRESS, data, size);
}

bool lcd_read_data(uint8_t* buffer, uint16_t size) {
	return I2C_master_receive(LCD_ADDRESS, buffer, size);
}


