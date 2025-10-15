#ifndef API_INC_API_HT_SENSOR_H_
#define API_INC_API_HT_SENSOR_H_

#include <stdbool.h>
#include <stdint.h>
#include "error.h"

#define HT_ERR_INIT_SENSOR   (ERR_BASE_HTSENSOR + 1)
#define HT_ERR_INVALID_UNIT   (ERR_BASE_HTSENSOR + 2)
#define HT_ERR_INVALID_OPERATION   (ERR_BASE_HTSENSOR + 3)
#define HT_ERR_MEASURING (ERR_BASE_HTSENSOR + 4)
#define HT_ERR_RESET (ERR_BASE_HTSENSOR + 5)
#define HT_ERR_READ_MEASUREMENT (ERR_BASE_HTSENSOR + 6)

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

app_err_t ht_init();

app_err_t ht_query_init(ht_query_t* query, uint8_t* operation, uint8_t* unit);

app_err_t ht_trigger_measurement(ht_query_t query);

app_err_t ht_read_measurement(ht_measurement_t* measurement);

app_err_t ht_reset();

#endif /* API_INC_API_HT_SENSOR_H_ */
