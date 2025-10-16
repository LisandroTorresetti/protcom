#include "API_uart.h"
#include "stm32f4xx_hal.h"

// Rx/Tx timeout
static const uint32_t TIMEOUT = 1000;

static UART_HandleTypeDef uart_handler;

// Prototypes
static void send_error_msg(uint8_t* error_msg);
static uint16_t get_string_length(const uint8_t* pstring);

/**
 * @brief Initializes the UART peripheral.
 *
 * Configures the UART with the following settings:
 * - Baud rate: 9600
 * - Word length: 9 bits
 * - Stop bits: 1
 * - Parity: Odd
 * - Hardware flow control: None
 * - Mode: TX/RX
 * - Oversampling: 16
 *
 * @return APP_OK if the UART was successfully initialized,
 *         UART_ERR_INIT otherwise.
 */
app_err_t uartInit() {
	UART_InitTypeDef uart_init_config = {
			.BaudRate = 9600,
			.WordLength = UART_WORDLENGTH_9B,
			.StopBits = UART_STOPBITS_1,
			.Parity = UART_PARITY_ODD,
			.HwFlowCtl = UART_HWCONTROL_NONE,
			.Mode = UART_MODE_TX_RX,
			.OverSampling = UART_OVERSAMPLING_16,
	};

	uart_handler.Instance = USART2;
	uart_handler.Init = uart_init_config;

	if (HAL_UART_Init(&uart_handler) != HAL_OK) {
		return UART_ERR_INIT;
	}

	return APP_OK;
}

/**
 * @brief Sends a null-terminated string over UART.
 *
 * The function determines the string length by searching for the
 * null terminator (`'\0'`) before transmitting.
 *
 * @param pstring  Pointer to the null-terminated string to transmit.
 *
 * @return APP_OK if the message is sent correctly, otherwise the corresponding error
 *
 */
app_err_t uartSendString(uint8_t* pstring) {
	if (pstring == NULL) {
		return APP_ERR_INVALID_ARG;
	}

	uint16_t str_length = get_string_length(pstring);
	return uartSendStringSize(pstring, str_length);
}

/**
 * @brief Sends a fixed-size string over UART.
 *
 * Transmits exactly size bytes from the given string buffer.
 *
 * @param pstring  Pointer to the string buffer to transmit.
 * @param size     Number of bytes to transmit.
 *
 * @return APP_OK if the message is sent correctly, otherwise the corresponding error
 *
 */
app_err_t uartSendStringSize(uint8_t* pstring, uint16_t size) {
	if (pstring == NULL || size == 0) {
		return APP_ERR_INVALID_ARG;
	}


	return (HAL_UART_Transmit(&uart_handler, pstring, size, TIMEOUT) != HAL_OK) ? UART_ERR_TX : APP_OK;
}

/**
 * @brief Receives a fixed-size string over UART.
 *
 * Reads exactly @p size bytes from UART into the given buffer.
 *
 * @param pstring  Pointer to the buffer where received data will be stored.
 * @param size     Number of bytes to receive, must be greater than 0.
 *
 * @return APP_OK if the message is received correctly, otherwise the corresponding error
 *
 */
app_err_t uartReceiveStringSize(uint8_t* pstring, uint16_t size) {
	if (pstring == NULL || size == 0) {
		return APP_ERR_INVALID_ARG;
	}

	return (HAL_UART_Receive(&uart_handler, pstring, size, TIMEOUT) != HAL_OK) ? UART_ERR_RX : APP_OK;
}

/**
 * @brief returns the string length
 * *
 * @param pstring: string whose length you want to obtain
 *
 * @return uint16_t the length of the string
 *
 */
uint16_t get_string_length(const uint8_t* pstring) {
	uint16_t counter = 0;
	if (pstring == NULL) {
		return counter;
	}

	while (pstring[counter] != '\0') {
		counter++;
	}

	return counter;
}



