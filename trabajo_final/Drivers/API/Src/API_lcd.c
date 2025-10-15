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

#define DELAY_1_MS 1
#define DELAY_2_MS 2

#define RS_IR 0
#define RS_DR 1
#define WRITE_OP 0
#define READ_OP 1
#define EN_START 1
#define EN_FINISH 0

static const uint8_t HIGH_NIBBLE_MASK = 0xF0;

static uint8_t INIT_SEQUENCE[7] = {
		FUNCTION_SET_CMD,
		DISPLAY_CONTROL_CMD,
		CLEAR_DISPLAY_CMD,
		ENTRY_MODE_CMD,
		RETURN_HOME_CMD
};

static uint8_t CLEAR_SEQUENCE[2] = {
		CLEAR_DISPLAY_CMD,
		RETURN_HOME_CMD
};


static bool send_commands(uint8_t* cmds, uint8_t size);
static bool lcd_send_cmd(uint8_t cmd);
static bool lcd_send_data(uint8_t* data);
static bool lcd_send_byte(uint8_t data, uint8_t rs);
static bool lcd_send_nibble(uint8_t data, uint8_t rs);
static uint8_t build_lcd_control_byte(uint8_t rs_bit, uint8_t read_op_bit, uint8_t EN_bit);

bool lcd_init() {
	HAL_Delay(100);

	if (!lcd_send_nibble(0x30, RS_IR)) {
		return false;
	}

	HAL_Delay(5);

	if (!lcd_send_nibble(0x30, RS_IR)) {
		return false;
	}

	HAL_Delay(1);

	if (!lcd_send_nibble(0x20, RS_IR)) {
		return false;
	}

	HAL_Delay(1);

	if (!send_commands(INIT_SEQUENCE, 5)) {
		return false;
	}

	if (!lcd_send_data("Medime esta")) {
		return false;
	}

	return true;
}

bool lcd_clear_screen() {
	return send_commands(CLEAR_SEQUENCE, 2);
}

bool lcd_set_cursor(uint8_t row, uint8_t col) {
	uint8_t new_position = SET_DDRAM_ADDRESS_CMD | 0x40;
	return lcd_send_cmd(new_position);
}

bool lcd_print(uint8_t* message) {
	return lcd_send_data(message);
}

bool lcd_println(uint8_t* message) {
	if (!lcd_print(message)) {
		return false;
	}

	return lcd_set_cursor(0,0);
}

bool send_commands(uint8_t* cmds, uint8_t size) {
	if (cmds == NULL) {
		return false;
	}

	for (uint8_t idx = 0; idx < size; idx++) {
		uint8_t cmd = cmds[idx];
		if (!lcd_send_cmd(cmd)) {
			return false;
		}

		(cmd == CLEAR_DISPLAY_CMD || cmd == RETURN_HOME_CMD) ? HAL_Delay(DELAY_2_MS) : HAL_Delay(DELAY_1_MS);
	}


	return true;
}

bool lcd_send_cmd(uint8_t cmd) {
	return lcd_send_byte(cmd, RS_IR);
}

bool lcd_send_data(uint8_t* data) {
	if (data == NULL) {
		return false;
	}

	while (*data) {
		if (!lcd_send_byte(*data++, RS_DR)) {
			return false;
		}
	}


	return true;
}

bool lcd_send_byte(uint8_t data, uint8_t rs) {
	uint8_t high_nibble = data & HIGH_NIBBLE_MASK;
	uint8_t high_nibble_start = high_nibble | build_lcd_control_byte(rs, WRITE_OP, EN_START);
	if (!lcd_write(&high_nibble_start, 1)) {
		return false;
	}

	uint8_t high_nibble_finish = high_nibble | build_lcd_control_byte(rs, WRITE_OP, EN_FINISH);
	if (!lcd_write(&high_nibble_finish, 1)) {
		return false;
	}

	uint8_t low_nibble = (data << 4) & HIGH_NIBBLE_MASK;
	uint8_t low_nibble_start = low_nibble | build_lcd_control_byte(rs, WRITE_OP, EN_START);
	if (!lcd_write(&low_nibble_start, 1)) {
		return false;

	}

	uint8_t low_nibble_finish = low_nibble | build_lcd_control_byte(RS_IR, WRITE_OP, EN_FINISH);
	if (!lcd_write(&low_nibble_finish, 1)) {
		return false;
	}

	return true;
}

bool lcd_send_nibble(uint8_t data, uint8_t rs) {
	uint8_t high_nibble = data & HIGH_NIBBLE_MASK;
	uint8_t high_nibble_start = high_nibble | build_lcd_control_byte(rs, WRITE_OP, EN_START);
	if (!lcd_write(&high_nibble_start, 1)) {
		return false;
	}

	uint8_t high_nibble_finish = high_nibble | build_lcd_control_byte(rs, WRITE_OP, EN_FINISH);
	if (!lcd_write(&high_nibble_finish, 1)) {
		return false;
	}

	return true;
}

uint8_t build_lcd_control_byte(uint8_t RS_bit, uint8_t RW_bit, uint8_t EN_bit) {
	uint8_t ctrl_byte = 0x08;
	return ctrl_byte | (EN_bit << 2) | (RW_bit << 1) | RS_bit;
}

