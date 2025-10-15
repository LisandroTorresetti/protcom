#ifndef API_INC_API_CMDPARSER_H_
#define API_INC_API_CMDPARSER_H_

#include <stdbool.h>
#include "error.h"

#define CMDPARSER_ERR_INIT (ERR_BASE_CMDPARSER + 1)

app_err_t cmdparser_init();

void cmdparser_read_cmd();

#endif /* API_INC_API_CMDPARSER_H_ */
