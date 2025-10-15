#ifndef API_INC_API_ACTIONS_H_
#define API_INC_API_ACTIONS_H_

#include "stm32f4xx_hal.h"
#include "API_ht_sensor.h"
#include <stdbool.h>
#include <stdint.h>

void help_action();

bool measurement_action(uint8_t* operation, uint8_t* unit);

bool read_measurement_action(ht_measurement_t* measurement);

bool show_measurement_action(ht_measurement_t* measurement);

bool reset_action();

#endif /* API_INC_API_ACTIONS_H_ */
