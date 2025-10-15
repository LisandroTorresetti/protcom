#ifndef I2C_INC_I2C_CORE_H_
#define I2C_INC_I2C_CORE_H_

#include <stdbool.h>
#include <stdint.h>
#include "error.h"

#define I2C_ERR_TX   (ERR_BASE_I2C + 1)
#define I2C_ERR_RX   (ERR_BASE_I2C + 2)

app_err_t I2C_master_transmit(uint16_t device_address, uint8_t* message, uint16_t size);

app_err_t I2C_master_receive(uint16_t device_address, uint8_t* buffer, uint16_t size);

#endif /* I2C_INC_I2C_CORE_H_ */
