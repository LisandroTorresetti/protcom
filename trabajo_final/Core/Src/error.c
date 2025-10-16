#include "error.h"
#include "API_uart.h"
#include "API_lcd.h"
#include "API_ht_sensor.h"
#include "i2c_core.h"
#include "API_cmdparser.h"

/**
 * @brief returns the error code as an array of characters
 *
 * @param err: err from which the name is going to be returned
 *
 * @return uint8_t* with the error as an array of characters
 */
uint8_t* app_err_to_name(app_err_t err) {
    switch (err) {
        case APP_OK:               return (uint8_t*)"APP_OK";
        case APP_FAIL:             return (uint8_t*)"APP_FAIL";
        case APP_ERR_INTERNAL:     return (uint8_t*)"APP_ERR_INTERNAL";
        case APP_ERR_INVALID_ARG:  return (uint8_t*)"APP_ERR_INVALID_ARG";
        case APP_ERR_UNKNOWN:      return (uint8_t*)"APP_ERR_UNKNOWN";

        // --- HT sensor ---
        case HT_ERR_INIT_SENSOR:    	return (uint8_t*)"HT_ERR_INIT_SENSOR";
        case HT_ERR_INVALID_UNIT:   	return (uint8_t*)"HT_ERR_INVALID_UNIT";
        case HT_ERR_INVALID_OPERATION:  return (uint8_t*)"HT_ERR_INVALID_OPERATION";
        case HT_ERR_MEASURING:    		return (uint8_t*)"HT_ERR_MEASURING";
        case HT_ERR_RESET:    			return (uint8_t*)"HT_ERR_RESET";
        case HT_ERR_READ_MEASUREMENT:   return (uint8_t*)"HT_ERR_READ_MEASUREMENT";

        // --- LCD ---
        case LCD_ERR_INIT:    			return (uint8_t*)"LCD_ERR_INIT";
        case LCD_ERR_SENDING_CMD:    	return (uint8_t*)"LCD_ERR_SENDING_CMD";
        case LCD_ERR_SENDING_DATA:    	return (uint8_t*)"LCD_ERR_SENDING_DATA";
        case LCD_ERR_INVALID_ROW_IDX:   return (uint8_t*)"LCD_ERR_INVALID_ROW_IDX";
        case LCD_ERR_INVALID_COL_IDX:   return (uint8_t*)"LCD_ERR_INVALID_COL_IDX";

        // --- UART ---
        case UART_ERR_INIT:    	return (uint8_t*)"UART_ERR_INIT";
        case UART_ERR_TX:    	return (uint8_t*)"UART_ERR_TX";
        case UART_ERR_RX:    	return (uint8_t*)"UART_ERR_RX";

        // --- I2C ---
        case I2C_ERR_TX:    	return (uint8_t*)"I2C_ERR_TX";
        case I2C_ERR_RX:    	return (uint8_t*)"I2C_ERR_RX";

        // --- CMDParser ---
        case CMDPARSER_ERR_INIT:    		return (uint8_t*)"CMDPARSER_ERR_INIT";
        case CMDPARSER_ERR_INVALID_CMD:    	return (uint8_t*)"CMDPARSER_ERR_INVALID_CMD";
        case CMDPARSER_ERR_UNKNOWN_CMD:    	return (uint8_t*)"CMDPARSER_ERR_UNKNOWN_CMD";
        case CMDPARSER_ERR_OVERFLOW:    	return (uint8_t*)"CMDPARSER_ERR_OVERFLOW";
        case CMDPARSER_ERR_ARGS:    		return (uint8_t*)"CMDPARSER_ERR_ARGS";
        case CMDPARSER_ERR_INTERNAL:    	return (uint8_t*)"CMDPARSER_ERR_INTERNAL";
        case CMDPARSER_ERR_UNKNOWN:    		return (uint8_t*)"CMDPARSER_ERR_UNKNOWN";

        default:
        	return (uint8_t*)"UNKNOWN_ERROR";
    }
}

