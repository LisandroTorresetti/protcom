#include "API_ht_sensor.h"
#include "ht_port.h"
#include "math.h"
#include "stm32f4xx_hal.h"
#include <string.h>

#define MAX_RETRIES 10
#define CELSIUS_STR "C"
#define FARENHEIT_STR "F"
#define KELVIN_STR "K"
#define HT_NO_VALUE NAN

// Commands for AHT20 sensor
static uint8_t STATUS_CMD = 0X71;
static uint8_t TRIGGER_MEASURE_CMD[3] = {0xAC, 0x33, 0x00};
static uint8_t INIT_CMD[3] = {0xBE, 0x08, 0x00};
static uint8_t RESET_CMD = 0xBA;

static const uint16_t STATUS_RESPONSE_SIZE = 1;
static const uint8_t MEASURE_RESPONSE_SIZE = 7;

// Data indexes for humidity and temperature
static const uint8_t HIGH_HUM_BYTE_IDX = 1;
static const uint8_t MEDIUM_HUM_BYTE_IDX = 2;
static const uint8_t LOW_HUM_BYTE_IDX = 3;

static const uint8_t HIGH_TEMP_BYTE_IDX = 3;
static const uint8_t MEDIUM_TEMP_BYTE_IDX = 4;
static const uint8_t LOW_TEMP_BYTE_IDX = 5;

// Valid temperature unit args
static const uint8_t CELSIUS_UNIT_CHAR = 'C';
static const uint8_t KELVIN_UNIT_CHAR = 'K';
static const uint8_t FARENHEIT_UNIT_CHAR = 'F';

// Valid operations
static const uint8_t TEMP_OP_STR[] = "TEMP";
static const uint8_t HUM_OP_STR[] = "HUM";
static const uint8_t TEMP_HUM_OP_STR[] = "TEMP&HUM";

// Masks
static const uint8_t THIRD_BIT_MASK = 0x08;

static ht_query_t query;

// Prototypes
static app_err_t set_operation(ht_query_t* query, uint8_t* operation);
static app_err_t set_temp_unit(ht_query_t* query, uint8_t* unit);
static app_err_t ht_get_temp_and_hum(double* temp, double* hum);
static double convert_temp(double temp);
static uint8_t* unit_to_string();

app_err_t ht_init() {
	HAL_Delay(40);
	bool init_cmd_triggered = false;
	uint8_t retry_counter = 0;

	if (write_command(&STATUS_CMD, 1) != APP_OK) {
		return APP_ERR_INTERNAL;
	}

check_status:
	HAL_Delay(10);
	uint8_t buffer_status = {0};
	if (read_data(&buffer_status, STATUS_RESPONSE_SIZE) != APP_OK) {
		return APP_ERR_INTERNAL;
	}

	if ((buffer_status & THIRD_BIT_MASK) >> 3) {
		// Already initialized
		return APP_OK;
	}

	if (!init_cmd_triggered && write_command(INIT_CMD, sizeof(INIT_CMD)) != APP_OK) {
		return HT_ERR_INIT_SENSOR;
	}

	init_cmd_triggered = true;
	retry_counter++;

	if (retry_counter > MAX_RETRIES) {
		return HT_ERR_INIT_SENSOR;
	}

	goto check_status;
}

app_err_t ht_query_init(ht_query_t* query, uint8_t* operation, uint8_t* unit) {
	app_err_t err = set_operation(query, operation);
	if (err != APP_OK) {
		return err;
	}

	return set_temp_unit(query, unit);
}

app_err_t ht_trigger_measurement(ht_query_t ht_query) {
	if (write_command(TRIGGER_MEASURE_CMD, sizeof(TRIGGER_MEASURE_CMD)) != APP_OK) {
		return HT_ERR_MEASURING;
	}

	query = ht_query;
	return APP_OK;
}

app_err_t ht_read_measurement(ht_measurement_t* measurement) {
	if (measurement == NULL) {
		return APP_ERR_INVALID_ARG;
	}

	double temp, hum;
	app_err_t err = ht_get_temp_and_hum(&temp, &hum);
	if (err != APP_OK) {
		return err;
	}

	temp = convert_temp(temp);

	measurement->temp_data.temp = HT_NO_VALUE;
	measurement->hum = HT_NO_VALUE;

	switch (query.op) {
	case TEMP_OP:
		measurement->temp_data.temp = temp;
		measurement->temp_data.unit = unit_to_string();
		break;

	case HUM_OP:
		measurement->hum = hum;
		break;

	case TEMP_HUM_OP:
		measurement->temp_data.temp = temp;
		measurement->temp_data.unit = unit_to_string();
		measurement->hum = hum;
		break;

	default:
		measurement->temp_data.temp = temp;
		measurement->temp_data.unit = unit_to_string();
	}

	return APP_OK;
}

app_err_t ht_reset() {
	app_err_t err = write_command(&RESET_CMD, sizeof(RESET_CMD));
	if (err != APP_OK) {
		return HT_ERR_RESET;
	}

	return ht_init() != APP_OK ? HT_ERR_RESET : APP_OK;
}

app_err_t ht_get_temp_and_hum(double* temp, double* hum) {
	HAL_Delay(80);

	uint8_t read_status = {0};
	do {
		if (read_data(&read_status, STATUS_RESPONSE_SIZE) != APP_OK) {
			return HT_ERR_READ_MEASUREMENT;
		}

	} while (read_status >> 7);


	uint8_t sensor_data_buffer[1] = {0};
	if (read_data(sensor_data_buffer, MEASURE_RESPONSE_SIZE) != APP_OK) {
		return HT_ERR_READ_MEASUREMENT;
	}

	uint32_t raw_hum = ((uint32_t)sensor_data_buffer[HIGH_HUM_BYTE_IDX] << 12) |
	                   ((uint32_t)sensor_data_buffer[MEDIUM_HUM_BYTE_IDX] << 4)  |
	                   ((uint32_t)sensor_data_buffer[LOW_HUM_BYTE_IDX] >> 4);

	uint32_t raw_temp = (((uint32_t)sensor_data_buffer[HIGH_TEMP_BYTE_IDX] & 0x0F) << 16) |
		                ((uint32_t)sensor_data_buffer[MEDIUM_TEMP_BYTE_IDX] << 8)  |
		                ((uint32_t)sensor_data_buffer[LOW_TEMP_BYTE_IDX]);


	// De esos 7 bytes te importan del [1, 5] (contando desde el cero obviamente)
	double divisor = (double) pow(2, 20);
	double result_hum = ((raw_hum / divisor) * 100);
	double result_temp = ((raw_temp / divisor) * 200 - 50);

	*temp = result_temp;
	*hum = result_hum;

	return APP_OK;
}

app_err_t set_operation(ht_query_t* query, uint8_t* operation) {
	if (query == NULL || operation == NULL) {
		return APP_ERR_INVALID_ARG;
	}

	char* op = (char*)operation;
	if (!strcmp(op, (char*)TEMP_OP_STR)) {
		query->op = TEMP_OP;
		return APP_OK;
	}

	if (!strcmp(op, (char*)HUM_OP_STR)) {
		query->op = HUM_OP;
		return APP_OK;
	}

	if (!strcmp(op, (char*)TEMP_HUM_OP_STR)) {
		query->op = TEMP_HUM_OP;
		return APP_OK;
	}


	return HT_ERR_INVALID_OPERATION;
}


app_err_t set_temp_unit(ht_query_t* query, uint8_t* unit) {
	if (query == NULL || unit == NULL) {
		return APP_ERR_INVALID_ARG;
	}

	uint8_t unit_char = *unit;

	if (unit_char == '\0' || unit_char == CELSIUS_UNIT_CHAR) {
		query->unit = CELSIUS;
		return APP_OK;
	}

	if (unit_char == KELVIN_UNIT_CHAR) {
		query->unit = KELVIN;
		return APP_OK;
	}

	if (unit_char == FARENHEIT_UNIT_CHAR) {
		query->unit = FARENHEIT;
		return APP_OK;
	}

	return HT_ERR_INVALID_UNIT;
}

double convert_temp(double temp) {
	switch (query.unit) {
	case FARENHEIT:
		return (temp * 9/5) + 32;
	case KELVIN:
		return temp + 273.15;
	default:
		return temp;
	}
}

uint8_t* unit_to_string() {
	switch (query.unit) {
	case FARENHEIT:
		return (uint8_t*)FARENHEIT_STR;
	case KELVIN:
		return (uint8_t*)KELVIN_STR;
	default:
		return (uint8_t*)CELSIUS_STR;
	}
}
