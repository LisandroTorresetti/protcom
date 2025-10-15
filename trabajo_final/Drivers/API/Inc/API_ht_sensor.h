#ifndef API_INC_API_HT_SENSOR_H_
#define API_INC_API_HT_SENSOR_H_

#include <stdbool.h>
#include <stdint.h>

typedef enum {
	TEMP_OP,
	HUM_OP,
	TEMP_HUM_OP,
} ht_operation_t;

typedef enum {
	CELSIUS,
	KELVIN,
	FARENHEIT,
} temp_unit_t;

typedef struct {
	ht_operation_t op;
	temp_unit_t unit;
} ht_query_t;

typedef struct {
	double temp;
	uint8_t* unit;
} temp_t;

typedef struct {
	temp_t temp_data;
	double hum;
} ht_measurement_t;

bool ht_init();

bool ht_query_init(ht_query_t* query, uint8_t* operation, uint8_t* unit);

bool ht_trigger_measurement(ht_query_t query);

bool ht_read_measurement(ht_measurement_t* measurement);

bool ht_reset();

#endif /* API_INC_API_HT_SENSOR_H_ */
