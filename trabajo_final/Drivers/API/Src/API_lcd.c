#include "API_lcd.h"
#include "lcd_port.h"
#include "stm32f4xx_hal.h"

// LCD commands
#define CLEAR_DISPLAY_CMD 0x01
#define RETURN_HOME_CMD 0x02
#define ENTRY_MODE_CMD 0x06
#define DISPLAY_CONTROL_CMD 0x0C
#define FUNCTION_SET_CMD 0x28
#define SET_DDRAM_ADDRESS_CMD 0x80

#define FIRST_ROW_ADDRESS 0x00
#define SECOND_ROW_ADDRESS 0x40

#define DELAY_1_MS 1
#define DELAY_2_MS 2

// Values of the control nibble that must be send with each command
#define RS_IR 0
#define RS_DR 1
#define WRITE_OP 0
#define READ_OP 1
#define EN_START 1
#define EN_FINISH 0

static const uint8_t HIGH_NIBBLE_MASK = 0xF0;

// Sequence of commands to initialize the LCD
static uint8_t INIT_SEQUENCE[7] = {
		FUNCTION_SET_CMD,
		DISPLAY_CONTROL_CMD,
		CLEAR_DISPLAY_CMD,
		ENTRY_MODE_CMD,
		RETURN_HOME_CMD
};

// Sequence of commands to clear the screen
static uint8_t CLEAR_SEQUENCE[2] = {
		CLEAR_DISPLAY_CMD,
		RETURN_HOME_CMD
};

// Init message to be displayed if it's all good
static uint8_t init_msg[] = "Welcome :)";

static uint8_t current_row = FIRST_ROW_ADDRESS;

// Prototypes
static app_err_t send_commands(uint8_t* cmds, uint8_t size);
static app_err_t lcd_send_cmd(uint8_t cmd);
static app_err_t lcd_send_data(uint8_t* data);
static app_err_t lcd_send_byte(uint8_t data, uint8_t rs);
static app_err_t lcd_send_nibble(uint8_t data, uint8_t rs);
static uint8_t build_lcd_control_byte(uint8_t rs_bit, uint8_t read_op_bit, uint8_t EN_bit);

/*
 * @brief inits the LCD
 *
 *  Sends the initialization sequence to the LCD and then shows the init message
 *
 * @return
 *  - APP_OK if the LCD is initialized correctly
 *  - LCD_ERR_INIT: in case of an error
 *
 */
app_err_t lcd_init() {
	HAL_Delay(100);

	if (lcd_send_nibble(0x30, RS_IR) != APP_OK) {
		return LCD_ERR_INIT;
	}

	HAL_Delay(5);

	if (lcd_send_nibble(0x30, RS_IR) != APP_OK) {
		return LCD_ERR_INIT;
	}

	HAL_Delay(1);

	if (lcd_send_nibble(0x20, RS_IR) != APP_OK) {
		return LCD_ERR_INIT;
	}

	HAL_Delay(1);

	uint8_t amount_of_cmds = sizeof(INIT_SEQUENCE) / sizeof(INIT_SEQUENCE[0]);
	if (send_commands(INIT_SEQUENCE, amount_of_cmds) != APP_OK) {
		return LCD_ERR_INIT;
	}

	if (lcd_print(init_msg) != APP_OK) {
		return LCD_ERR_INIT;
	}

	return APP_OK;
}

/*
 * @brief clears the LCD screen
 *
 *  Sends the clear screen sequence to the LCD
 *
 * @return APP_OK if it's all good, otherwise the corresponding error
 *
 */
app_err_t lcd_clear_screen() {
	current_row = FIRST_ROW_ADDRESS;
	return send_commands(CLEAR_SEQUENCE, 2);
}

/*
 * @brief sets the cursor in a specific position of the LCD
 *
 * @param row: new row position
 * @param col: new column position
 *
 * @return
 * - APP_OK if the cursor is set in the new position correctly
 * - LCD_ERR_INVALID_ROW_IDX or LCD_ERR_INVALID_COL_IDX: in case of an invalid row or column value, respectively
 * - Otherwise the corresponding error
 *
 */
app_err_t lcd_set_cursor(uint8_t row, uint8_t col) {
	if (row > 2) {
		return LCD_ERR_INVALID_ROW_IDX;
	}

	if (col > 15) {
		return LCD_ERR_INVALID_COL_IDX;
	}

	uint8_t new_row = (row == 0) ? FIRST_ROW_ADDRESS : SECOND_ROW_ADDRESS;
	uint8_t new_address = new_row + col;
	app_err_t err = lcd_send_cmd(SET_DDRAM_ADDRESS_CMD | new_address);
	if (err != APP_OK) {
		return err;
	}

	current_row = new_row;
	return APP_OK;
}

/*
 * @brief prints the given message on the LCD screen
 *
 * @param message: message to be displayed on the LCD
 *
 * @return APP_OK if it's all good, otherwise the corresponding error
 *
 */
app_err_t lcd_print(uint8_t* message) {
	if (message == NULL) {
		return APP_ERR_INVALID_ARG;
	}

	return lcd_send_data(message);
}

/*
 * @brief prints the given message on the LCD screen and change the cursor position
 *
 * @param message: message to be displayed on the LCD
 *
 * @return APP_OK if it's all good, otherwise the corresponding error
 *
 */
app_err_t lcd_println(uint8_t* message) {
	app_err_t err = lcd_print(message);
	if (err != APP_OK) {
		return err;
	}

	uint8_t new_row = (current_row == FIRST_ROW_ADDRESS) ? 1 : 0;
	return lcd_set_cursor(new_row, 0);
}

app_err_t send_commands(uint8_t* cmds, uint8_t size) {
	if (cmds == NULL) {
		return APP_ERR_INVALID_ARG;
	}

	for (uint8_t idx = 0; idx < size; idx++) {
		uint8_t cmd = cmds[idx];
		if (lcd_send_cmd(cmd) != APP_OK) {
			return LCD_ERR_SENDING_CMD;
		}

		(cmd == CLEAR_DISPLAY_CMD || cmd == RETURN_HOME_CMD) ? HAL_Delay(DELAY_2_MS) : HAL_Delay(DELAY_1_MS);
	}


	return APP_OK;
}

/*
 * @brief sends a command to the LCD
 *
 * @param cmd to send
 *
 * @return APP_OK if the command is sent correctly, otherwise the corresponding error
 *
 */
app_err_t lcd_send_cmd(uint8_t cmd) {
	return lcd_send_byte(cmd, RS_IR);
}

/*
 * @brief sends the given data to the LCD
 *
 * @param data: to send
 *
 * @return
 *  -APP_OK if the data is sent correctly
 *  -LCD_ERR_SENDING_DATA: in case of an error
 *  - APP_ERR_INVALID_ARG: if data is NULL
 *
 */
app_err_t lcd_send_data(uint8_t* data) {
	if (data == NULL) {
		return APP_ERR_INVALID_ARG;
	}

	while (*data) {
		if (lcd_send_byte(*data++, RS_DR) != APP_OK) {
			return LCD_ERR_SENDING_DATA;
		}
	}


	return APP_OK;
}

/*
 * @brief sends a byte to the LCD
 *
 * @param data: byte to send
 * @param rs: 0: if its a command, 1: if its data
 *
 * @return APP_OK if the byte is sent correctly, otherwise APP_ERR_INTERNAL
 *
 */
app_err_t lcd_send_byte(uint8_t data, uint8_t rs) {
	uint8_t high_nibble = data & HIGH_NIBBLE_MASK;
	uint8_t high_nibble_start = high_nibble | build_lcd_control_byte(rs, WRITE_OP, EN_START);
	if (lcd_write(&high_nibble_start, 1) != APP_OK) {
		return APP_ERR_INTERNAL;
	}

	uint8_t high_nibble_finish = high_nibble | build_lcd_control_byte(rs, WRITE_OP, EN_FINISH);
	if (lcd_write(&high_nibble_finish, 1) != APP_OK) {
		return APP_ERR_INTERNAL;
	}

	uint8_t low_nibble = (data << 4) & HIGH_NIBBLE_MASK;
	uint8_t low_nibble_start = low_nibble | build_lcd_control_byte(rs, WRITE_OP, EN_START);
	if (lcd_write(&low_nibble_start, 1) != APP_OK) {
		return APP_ERR_INTERNAL;

	}

	uint8_t low_nibble_finish = low_nibble | build_lcd_control_byte(RS_IR, WRITE_OP, EN_FINISH);
	if (lcd_write(&low_nibble_finish, 1) != APP_OK) {
		return APP_ERR_INTERNAL;
	}

	return APP_OK;
}

/*
 * @brief sends the high nibble of the given byte
 *
 *
 * @param data: byte that contains the nibble to send
 * @param rs: 0: if its a command, 1: if its data
 *
 * @return APP_OK if the nibble is sent correctly, otherwise APP_ERR_INTERNAL
 *
 */
app_err_t lcd_send_nibble(uint8_t data, uint8_t rs) {
	uint8_t high_nibble = data & HIGH_NIBBLE_MASK;
	uint8_t high_nibble_start = high_nibble | build_lcd_control_byte(rs, WRITE_OP, EN_START);
	if (lcd_write(&high_nibble_start, 1) != APP_OK) {
		return APP_ERR_INTERNAL;
	}

	uint8_t high_nibble_finish = high_nibble | build_lcd_control_byte(rs, WRITE_OP, EN_FINISH);
	if (lcd_write(&high_nibble_finish, 1) != APP_OK) {
		return APP_ERR_INTERNAL;
	}

	return APP_OK;
}


/*
 * @brief build the LCD control byte
 *
 * This byte has 0s in the high nibble, will the second nibble has the data for Register Selector, if its a read or write operation
 * and the value for the start data read/write (signal E)
 *
 * @param RS_bit: 0 for Instruction Register, 1 for data register
 * @param RW_bit: 0: Write, 1: Read
 * @param EN_bit: 0: end data read/write, 1: start data read/write
 *
 * @return a uint8_t byte that has the given params set. By default backlight is ON
 *
 */
uint8_t build_lcd_control_byte(uint8_t RS_bit, uint8_t RW_bit, uint8_t EN_bit) {
	uint8_t ctrl_byte = 0x08;
	return ctrl_byte | (EN_bit << 2) | (RW_bit << 1) | RS_bit;
}

