#ifndef INC_ERROR_H_
#define INC_ERROR_H_

#include <stdint.h>

typedef int32_t app_err_t;

#define APP_OK           0
#define APP_FAIL        -1
#define APP_ERR_INTERNAL -2
#define APP_ERR_INVALID_ARG -3
#define APP_ERR_UNKNOWN -4


#define ERR_BASE_HTSENSOR   0x1000
#define ERR_BASE_LCD        0x2000
#define ERR_BASE_UART       0x3000
#define ERR_BASE_CMDPARSER  0x4000
#define ERR_BASE_I2C  		0x5000

uint8_t* app_err_to_name(app_err_t err);

#endif /* INC_ERROR_H_ */
