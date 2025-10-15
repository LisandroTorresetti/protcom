#ifndef API_INC_API_UART_H_
#define API_INC_API_UART_H_

#include <stdbool.h>
#include <stdint.h>
#include "error.h"

#define UART_ERR_INIT   (ERR_BASE_UART + 1)
#define UART_ERR_TX   (ERR_BASE_UART + 2)
#define UART_ERR_RX   (ERR_BASE_UART + 3)

app_err_t uartInit();

app_err_t uartSendString(uint8_t* pstring);

app_err_t uartSendStringSize(uint8_t* pstring, uint16_t size);

app_err_t uartReceiveStringSize(uint8_t* pstring, uint16_t size);


#endif /* API_INC_API_UART_H_ */
