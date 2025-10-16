#ifndef API_INC_API_LCD_H_
#define API_INC_API_LCD_H_

#include <stdint.h>
#include <stdbool.h>
#include "error.h"

#define LCD_ERR_INIT (ERR_BASE_LCD + 1)
#define LCD_ERR_SENDING_CMD (ERR_BASE_LCD + 2)
#define LCD_ERR_SENDING_DATA (ERR_BASE_LCD + 3)
#define LCD_ERR_INVALID_ROW_IDX (ERR_BASE_LCD + 4)
#define LCD_ERR_INVALID_COL_IDX (ERR_BASE_LCD + 5)

app_err_t lcd_init();

app_err_t lcd_clear_screen();

app_err_t lcd_set_cursor(uint8_t row, uint8_t col);

app_err_t lcd_print(uint8_t* message);

app_err_t lcd_println(uint8_t* message);

#endif /* API_INC_API_LCD_H_ */
