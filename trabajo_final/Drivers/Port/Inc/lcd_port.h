#ifndef PORT_INC_LCD_PORT_H_
#define PORT_INC_LCD_PORT_H_

#include <stdbool.h>
#include <stdint.h>

bool lcd_write(uint8_t* data, uint16_t size);

bool lcd_read_data(uint8_t* buffer, uint16_t size);

#endif /* PORT_INC_LCD_PORT_H_ */
