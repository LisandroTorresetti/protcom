#ifndef I2C_INC_I2C_CORE_H_
#define I2C_INC_I2C_CORE_H_

#include <stdbool.h>
#include <stdint.h>

bool I2C_master_transmit(uint16_t device_address, uint8_t* message, uint16_t size);

bool I2C_master_receive(uint16_t device_address, uint8_t* buffer, uint16_t size);

#endif /* I2C_INC_I2C_CORE_H_ */
