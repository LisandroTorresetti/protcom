#include "API_actions.h"
#include "API_uart.h"
#include "API_ht_sensor.h"
#include "API_lcd.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

#define MAX_MESSAGE_LENGTH 16

static uint8_t HELP_RESPONSE[] =
		"\r\nCOMMANDS:\r\n"
			"\tHELP: prints the available commands\r\n"
			"\tGET <OPERATION> [UNIT]: performs a measurement using the AHT20 sensor. The property to be measured depends on OPERATION field, which"
			"can have one of the following values:\r\n"
			"\t\t - TEMP\r\n"
			"\t\t - HUM\r\n"
			"\t\t - TEMP&HUM\r\n"
			"\t OBS: It is used to specify in which unit the temperature is, by default is Celsius (C) but other options are: K (Kelvin) or F (Farenheit) \r\n"
			"\tRESET: resets the AHT20 sensor";


static uint8_t TEMP_MSG_TEMPLATE[] = "TEMP: %.2f";
static uint8_t HUM_MSG_TEMPLATE[] = "HUM: %.2f";

// Codes to display % and ° correctly in the LCD
static uint8_t PERCENTAGE_SYMBOL_CODE = 0x25;
static uint8_t DEGREE_SYMBOL_CODE = 0xDF;


/**
 * @brief prints the commands that cmdparser accepts
 *
 */
void help_action() {
	uartSendString(HELP_RESPONSE);
}

/**
 * @brief performs the measurement action
 *
 * @param operation: operation to be performed
 * @param unit: unit of the temperature
 *
 * @return
 *  - APP_OK: if the action is executed correctly
 *  - APP_ERR_INVALID_ARG: if operation or unit are NULL
 *  - APP_ERR_INTERNAL: in case of an error
 */
app_err_t measurement_action(uint8_t* operation, uint8_t* unit) {
	if (operation == NULL || unit == NULL) {
		return APP_ERR_INVALID_ARG;
	}

	ht_query_t query = {0};
	if (ht_query_init(&query, operation, unit) != APP_OK) {
		return APP_ERR_INTERNAL;
	}

	return ht_trigger_measurement(query);
}

/**
 * @brief reads the measurement from the sensor
 *
 * @param measurement: variable in which the result will be stored
 *
 * @return
 *  - APP_OK: if the action is executed correctly
 *  - APP_ERR_INVALID_ARG: if measurement is NULL
 *  - APP_ERR_INTERNAL: in case of an error
 */
app_err_t read_measurement_action(ht_measurement_t* measurement) {
	if (measurement == NULL) {
		return APP_ERR_INVALID_ARG;
	}

	return ht_read_measurement(measurement);
}

/**
 * @brief shows the result of the measurement on the LCD
 *
 * @param measurement: variable that contains the result of the measurement
 *
 * @return
 *  - APP_OK: if the action is executed correctly
 *  - APP_ERR_INVALID_ARG: if measurement is NULL
 *  - APP_ERR_INTERNAL: in case of an error
 */
app_err_t show_measurement_action(ht_measurement_t* measurement) {
	if (measurement == NULL) {
		return APP_ERR_INVALID_ARG;
	}

	if (lcd_clear_screen() != APP_OK) {
		return APP_ERR_INTERNAL;
	}

	if (!isnan(measurement->temp_data.temp)) {
		uint8_t temperature_msg[MAX_MESSAGE_LENGTH] = {0};
		snprintf((char*)temperature_msg, MAX_MESSAGE_LENGTH, (char*)TEMP_MSG_TEMPLATE, measurement->temp_data.temp);

		uint8_t msg_length = strlen((char*)temperature_msg);
		temperature_msg[msg_length] = DEGREE_SYMBOL_CODE;
		temperature_msg[msg_length + 1] = *measurement->temp_data.unit;

		if (lcd_println(temperature_msg) != APP_OK) {
			return APP_ERR_INTERNAL;
		}
	}

	if (!isnan(measurement->hum)) {
		uint8_t humidity_msg[MAX_MESSAGE_LENGTH] = {0};
		snprintf((char*)humidity_msg, MAX_MESSAGE_LENGTH, (char*)HUM_MSG_TEMPLATE, measurement->hum);
		uint8_t msg_length = strlen((char*)humidity_msg);
		humidity_msg[msg_length] = PERCENTAGE_SYMBOL_CODE;
		if (lcd_print(humidity_msg) != APP_OK) {
			return APP_ERR_INTERNAL;
		}
	}

	return APP_OK;
}

/**
 * @brief performs the reset action
 *
 * @return APP_OK if the action is executed correctly, otherwise the corresponding error
 *
 */
app_err_t reset_action() {
	return ht_reset();
}

