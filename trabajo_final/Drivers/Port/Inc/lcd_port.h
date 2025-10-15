#ifndef PORT_INC_LCD_PORT_H_
#define PORT_INC_LCD_PORT_H_

#include <stdbool.h>
#include <stdint.h>
#include "error.h"

app_err_t lcd_write(uint8_t* data, uint16_t size);

app_err_t lcd_read_data(uint8_t* buffer, uint16_t size);

#endif /* PORT_INC_LCD_PORT_H_ */
