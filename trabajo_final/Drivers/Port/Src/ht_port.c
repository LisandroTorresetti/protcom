#include "ht_port.h"
#include "stm32f4xx_hal.h"
#include "i2c_core.h"

static const uint16_t HT_SENSOR_ADDRESS = 0x38;

bool write_command(uint8_t* cmd, uint16_t size) {
	return I2C_master_transmit(HT_SENSOR_ADDRESS, cmd, size);
}

bool read_data(uint8_t* sensor_data, uint16_t size) {
	return I2C_master_receive(HT_SENSOR_ADDRESS, sensor_data, size);
}
