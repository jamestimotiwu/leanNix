#ifndef PROCESS_H
#define PROCESS_H

#include "types.h"

int32_t process_execute(const uint8_t* command);
int32_t process_halt(uint8_t status);

int32_t command_read(int8_t* command, int8_t* arg, uint32_t offset);

#endif
