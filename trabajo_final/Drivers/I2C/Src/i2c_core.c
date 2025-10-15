#include "i2c_core.h"
#include "stm32f4xx_hal.h"

static const uint32_t TIMEOUT = 1000;

extern I2C_HandleTypeDef hi2c1;

app_err_t I2C_master_transmit(uint16_t device_address, uint8_t* message, uint16_t size) {
	return (HAL_I2C_Master_Transmit(&hi2c1, device_address << 1, message, size, TIMEOUT) == HAL_OK) ? APP_OK : I2C_ERR_TX;
}

app_err_t I2C_master_receive(uint16_t device_address, uint8_t* buffer, uint16_t size) {
	return (HAL_I2C_Master_Receive(&hi2c1, device_address << 1, buffer, size, TIMEOUT) == HAL_OK) ? APP_OK : I2C_ERR_RX;
}
