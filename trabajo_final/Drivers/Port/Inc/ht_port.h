#ifndef PORT_INC_HT_PORT_H_
#define PORT_INC_HT_PORT_H_

#include <stdbool.h>
#include <stdint.h>


bool write_command(uint8_t* cmd, uint16_t size);

bool read_data(uint8_t* sensor_data, uint16_t size);

#endif /* PORT_INC_HT_PORT_H_ */
