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

/**
 * @brief prints the commands that cmdparser accepts
 *
 */
void help_action() {
	uartSendString(HELP_RESPONSE);
}

/**
 * @brief performs an action over the on-board LED
 *
 * @param action: action to be performed, must be ON, OFF or TOGGLE
 *
 * @return TRUE if the action cab be performed correctly, otherwise false
 *
 */
bool measurement_action(uint8_t* operation, uint8_t* unit) {
	if (operation == NULL || unit == NULL) {
		return false;
	}

	ht_query_t query = {0};
	if (!ht_query_init(&query, operation, unit)) {
		return false;
	}

	return ht_trigger_measurement(query);
}


bool read_measurement_action(ht_measurement_t* measurement) {
	if (measurement == NULL) {
		return false;
	}

	return ht_read_measurement(measurement);
}

bool show_measurement_action(ht_measurement_t* measurement) {
	if (measurement == NULL) {
		return false;
	}

	if (!lcd_clear_screen()) {
		return false;
	}

	if (!isnan(measurement->temp_data.temp)) {
		uint8_t temperature_msg[MAX_MESSAGE_LENGTH] = {0};
		snprintf((char*)temperature_msg, MAX_MESSAGE_LENGTH, "TEMP: %.2f", measurement->temp_data.temp);

		uint8_t msg_length = strlen((char*)temperature_msg);
		temperature_msg[msg_length] = 0xDF;
		temperature_msg[msg_length + 1] = *measurement->temp_data.unit;

		if (!lcd_println(temperature_msg)) {
			return false;
		}
	}

	if (!isnan(measurement->hum)) {
		uint8_t humidity_msg[MAX_MESSAGE_LENGTH] = {0};
		snprintf((char*)humidity_msg, MAX_MESSAGE_LENGTH, "HUM: %.2f", measurement->hum);
		uint8_t msg_length = strlen((char*)humidity_msg);
		humidity_msg[msg_length] = 0x25;
		if (!lcd_print(humidity_msg)) {
			return false;
		}
	}

	return true;
}

/**
 * @brief simple function that answers 'PONG'
 *
 */
bool reset_action() {
	return ht_reset();
}

