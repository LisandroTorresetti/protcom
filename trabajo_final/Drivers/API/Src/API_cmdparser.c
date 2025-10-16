#include "API_cmdparser.h"
#include "API_uart.h"
#include "API_actions.h"
#include <string.h>

// Error definitions
#define  CMDPARSER_ERR_INVALID_CMD (ERR_BASE_CMDPARSER + 2)
#define  CMDPARSER_ERR_UNKNOWN_CMD (ERR_BASE_CMDPARSER + 3)
#define  CMDPARSER_ERR_OVERFLOW (ERR_BASE_CMDPARSER + 4)
#define  CMDPARSER_ERR_ARGS (ERR_BASE_CMDPARSER + 5)
#define  CMDPARSER_ERR_INTERNAL (ERR_BASE_CMDPARSER + 6)
#define  CMDPARSER_ERR_UNKNOWN (ERR_BASE_CMDPARSER + 7)

#define MAX_CMD_LENGTH 25
#define MAX_ARGS 3 // cmd + arg1 + arg2
#define ERR_MSG_MAX_LENGTH 50

// Possible states of the FSM
typedef enum {
  IDLE,
  RECV_CMD,
  PARSE_CMD,
  EXEC_CMD,
  RESET_SENSOR,
  MEASURE,
  READ_DATA,
  SHOW_DATA,
  ERROR_STATE,
} state_t;

// Valid Commands
static uint8_t HELP_CMD[] = "HELP";
static uint8_t GET_CMD[] = "GET";
static uint8_t RESET_CMD[] = "RESET";

static uint8_t *VALID_CMDS[] = {
		HELP_CMD,
		GET_CMD,
		RESET_CMD,
};

static uint8_t PROMPT[] = "\r\n> ";

static state_t system_state;
static app_err_t error_code;

static uint8_t cmd_buffer_idx;
// cmd_buffer: this is where the data coming from UART is stored.
static uint8_t cmd_buffer[MAX_CMD_LENGTH];
// cmd_tokens: the first element is the command and the rest are the arguments
static uint8_t cmd_tokens[MAX_ARGS][MAX_CMD_LENGTH];

static bool idle_check_flag;

static ht_measurement_t measurement;

// Prototypes
static void cmdparser_reset();
static void set_idle_state();
static void set_error_state(app_err_t err);
static void set_state(state_t state);

static void handle_idle_state();
static void handle_recv_state();
static void handle_error_state();
static void handle_parse_state();
static void handle_exec_state();
static void handle_measure_state();
static void handle_read_data_state();
static void handle_show_data_state();
static void handle_reset_state();

static bool is_valid_char(uint8_t character);
static bool command_exists(uint8_t* cmd);
static void clear_buffer(uint8_t* buffer);
static void echo(uint8_t* pstring);

/**
 * @brief inits the cmdparser
 *
 * @return CMDPARSER_ERR_INIT in case of an error, otherwise APP_OK
 *
 */
app_err_t cmdparser_init() {
	if (uartInit() != APP_OK) {
		return CMDPARSER_ERR_INIT;
	}

	set_idle_state();
	cmd_buffer_idx = 0;

	return APP_OK;
}

/**
 * @brief reads the command that the user send and handle the state of the cmdparser
 *
 * @note in case of being in an invalid state, the FSM is reseted
 */
void cmdparser_read_cmd() {
	switch (system_state) {
	case IDLE:
		if (!idle_check_flag) {
			uartSendString(PROMPT);
			idle_check_flag = true;
		}

		handle_idle_state();
		break;
	case RECV_CMD:
		handle_recv_state();
		break;
	case PARSE_CMD:
		handle_parse_state();
		break;
	case EXEC_CMD:
		handle_exec_state();
		break;
	case MEASURE:
		handle_measure_state();
		break;
	case READ_DATA:
		handle_read_data_state();
		break;
	case SHOW_DATA:
		handle_show_data_state();
		break;
	case RESET_SENSOR:
		handle_reset_state();
		break;
	case ERROR_STATE:
		handle_error_state();
		break;
	default:
		cmdparser_reset();
	}
}

/**
 * @brief sets the state of the cmdparser
 *
 */
void set_state(state_t state) {
	system_state = state;
}

/**
 * @brief puts the cmdparser in an IDLE state
 *
 */
void set_idle_state() {
	set_state(IDLE);
	error_code = APP_OK;
	idle_check_flag = false;
}

/**
 * @brief sets an error state for the cmdparser
 *
 */
void set_error_state(app_err_t err) {
	set_state(ERROR_STATE);
	error_code = err;
}

/**
 * @brief resets the cmdparser
 *
 * This function clear buffers, put the cmdparser in an IDLE state and restarts the timeout delay
 *
 */
void cmdparser_reset() {
	set_idle_state();
	clear_buffer(cmd_buffer);
	cmd_buffer_idx = 0;
}

/**
 * @brief handles the IDLE state
 *
 * Waits for a command, and if it receive one, it transitions to RECV state and the command is copied into the buffer.
 * Otherwise, it remains in the same state.
 *
 */
void handle_idle_state() {
	uint8_t raw_cmd_buffer[MAX_CMD_LENGTH];
	clear_buffer(raw_cmd_buffer);
	uartReceiveStringSize(raw_cmd_buffer, MAX_CMD_LENGTH);

	uint8_t *pointer = raw_cmd_buffer;
	if (strlen((char*)pointer)) {
		echo(raw_cmd_buffer);
		while(*pointer) {
			cmd_buffer[cmd_buffer_idx++] = *pointer++;
		}

		set_state(RECV_CMD);
	}
}

/**
 * @brief handles RECV state
 *
 * This function continues receiving and copying the user's command until a line break or carriage return occurs, and if so, it moves to state CMD_PARSE.
 *
 * @note This function can move to the following error states:
 * - CMD_ERR_TIMEOUT: if the newline o carriage returns never occurs within a time interval
 * - CMD_ERR_OVERFLOW: if the command length is greater than the max allowed length
 * - CMD_ERR_SYNTAX: if an invalid character was sent
 *
 */
void handle_recv_state() {
	uint8_t raw_cmd_buffer[MAX_CMD_LENGTH];
	clear_buffer(raw_cmd_buffer);
	uartReceiveStringSize(raw_cmd_buffer, 1);

	uint8_t *pointer = raw_cmd_buffer;
	if (strlen((char*)pointer)) {
		echo(raw_cmd_buffer);

		while(*pointer) {
			if (cmd_buffer_idx >= MAX_CMD_LENGTH) {
				set_error_state(CMDPARSER_ERR_OVERFLOW);
				return;
			}

			if (*pointer == '\n' || *pointer == '\r') {
				cmd_buffer[cmd_buffer_idx] = '\0';
				set_state(PARSE_CMD);
				return;
			}

			cmd_buffer[cmd_buffer_idx++] = *pointer++;
		}
	}
}

/**
 * @brief parses the command entered by the user
 *
 * If the command is valid, it puts the cmdparser in an CMD_OK state, otherwise to error states can be set:
 * - CMD_ERR_ARG: if the command has more args than the allowed amount (4)
 * - CMD_ERR_UNKNOWN: if the command is unknown
 *
 */
void handle_parse_state() {
	for (uint8_t idx = 0; idx < MAX_ARGS; idx++) {
		clear_buffer(cmd_tokens[idx]);
	}

	uint8_t token_idx = 0;
	uint8_t idx = 0;
	uint8_t *cmd_iterator = cmd_buffer;
	while (*cmd_iterator) {
		if (*cmd_iterator == ' ') {
			if (token_idx >= MAX_ARGS) {
				set_error_state(CMDPARSER_ERR_ARGS);
				return;
			}

			cmd_tokens[token_idx][idx] = '\0';

			// Copy next cmd token
			token_idx++;
			idx = 0;

			while (*cmd_iterator == ' ') cmd_iterator++;
			continue;
		}



		uint8_t character = *cmd_iterator++;

		if (!is_valid_char(character)) {
			set_error_state(CMDPARSER_ERR_INVALID_CMD);
			return;
		}

		if (character >= 'a' && character <= 'z') {
			character = character - ('a' - 'A');
		}

		cmd_tokens[token_idx][idx++] = character;
	}

	// Check if command exists
	if (!command_exists(cmd_tokens[0])) {
		set_error_state(CMDPARSER_ERR_UNKNOWN_CMD);
		return;
	}

	set_state(EXEC_CMD);
}

/**
 * @brief runs the user command
 *
 * If the command is executed correctly, it resets the cmdparser. Otherwise it puts the cdmparse in the corresponding error state
 *
 */
void handle_exec_state() {
	char* char_cmd = (char*) cmd_tokens[0];
	if (!strcmp(char_cmd, (char*)GET_CMD)) {
		set_state(MEASURE);
		return;
	}

	if (!strcmp(char_cmd, (char*)RESET_CMD)) {
		set_state(RESET_SENSOR);
		return;
	}


	if (!strcmp(char_cmd, (char*)HELP_CMD)) {
		help_action();
	} else {
		set_error_state(CMDPARSER_ERR_UNKNOWN);
		return;
	}

	cmdparser_reset();
}

void handle_measure_state() {
	uint8_t amount_of_args = 0;
	for (uint8_t idx = 1; idx < MAX_ARGS; idx++) {
		if (*cmd_tokens[idx] == '\0'){
			break;
		}

		amount_of_args++;
	}

	if (amount_of_args > 2) {
		set_error_state(CMDPARSER_ERR_ARGS);
		return;
	}

	app_err_t err = measurement_action(cmd_tokens[1], cmd_tokens[2]);
	if (err != APP_OK) {
		set_error_state(err);
		return;
	}

	set_state(READ_DATA);
}

void handle_read_data_state() {
	// Clear values from last read
	measurement = (ht_measurement_t){0};
	app_err_t err = read_measurement_action(&measurement);
	if (err != APP_OK) {
		set_error_state(err);
		return;
	}

	set_state(SHOW_DATA);
}

void handle_show_data_state() {
	app_err_t err = show_measurement_action(&measurement);
	if (err != APP_OK) {
		set_error_state(err);
		return;
	}

	cmdparser_reset();
}

void handle_reset_state() {
	app_err_t err = reset_action();
	if (err != APP_OK) {
		set_error_state(err);
		return;
	}

	cmdparser_reset();
}

/**
 * @brief handles all possible errors from cmdparser
 *
 * Sends a message depending on the error that occurred and then resets the cmdparser
 *
 */
void handle_error_state() {
	uint8_t error_msg[ERR_MSG_MAX_LENGTH];

	// TODO: add more error messages, or move this to an error handler

	switch (error_code) {
	case CMDPARSER_ERR_OVERFLOW:
		strcpy((char*)error_msg, "\n\rERROR: line too long");
		break;
	case CMDPARSER_ERR_ARGS:
		strcpy((char*)error_msg, "\n\rERROR: bad args");
		break;
	case CMDPARSER_ERR_UNKNOWN:
		strcpy((char*)error_msg, "\n\rERROR: unknown cmd");
		break;
	default:
		strcpy((char*)error_msg, "\n\rERROR: unknown");
	}

	uartSendString(error_msg);
	cmdparser_reset();
}


/**
 * @brief inits the given buffer with '\0'
 *
 * @param buffer: buffer to fill with '\0'
 */
void clear_buffer(uint8_t* buffer) {
	for (uint32_t idx = 0; idx < MAX_CMD_LENGTH; idx++) {
		buffer[idx] = '\0';
	}
}

/**
 * @brief checks if the given character is valid
 *
 * @note valid characters are: \n, \r, \0, _, ' ', & and letters (uppercase or lowercase)
 *
 * @param character to be check
 */
bool is_valid_char(uint8_t character) {
    if (character == '\n' || character == '\r' || character == '\0' || character == '_' || character == ' ' || character == '&') {
        return true;
    }

    if (character >= 'A' && character <= 'Z') {
        return true;
    }

    if (character >= 'a' && character <= 'z') {
        return true;
    }

    return false;
}

bool command_exists(uint8_t* cmd) {
	uint8_t amount_of_commands = sizeof(VALID_CMDS) / sizeof(VALID_CMDS[0]);

	for (int idx = 0; idx < amount_of_commands; idx++) {
		if (!strcmp((char*)VALID_CMDS[idx], (char*)cmd)) {
			return true;
		}
	}

	return false;
}

/**
 * @brief sends the given character using UART
 *
 * @param pstring: character to be sent
 *
 */
void echo(uint8_t* pstring) {
	uartSendString(pstring);
}

