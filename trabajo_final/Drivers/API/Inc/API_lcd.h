#ifndef API_INC_API_LCD_H_
#define API_INC_API_LCD_H_

#include <stdint.h>
#include <stdbool.h>

bool lcd_init();

bool lcd_clear_screen();

bool lcd_set_cursor(uint8_t row, uint8_t col);

bool lcd_print(uint8_t* message);

bool lcd_println(uint8_t* message);

#endif /* API_INC_API_LCD_H_ */
