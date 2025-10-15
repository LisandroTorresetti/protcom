#ifndef API_INC_API_ACTIONS_H_
#define API_INC_API_ACTIONS_H_

#include "stm32f4xx_hal.h"
#include "API_ht_sensor.h"
#include <stdbool.h>
#include <stdint.h>
#include "error.h"

void help_action();

app_err_t measurement_action(uint8_t* operation, uint8_t* unit);

app_err_t read_measurement_action(ht_measurement_t* measurement);

app_err_t show_measurement_action(ht_measurement_t* measurement);

app_err_t reset_action();

#endif /* API_INC_API_ACTIONS_H_ */
