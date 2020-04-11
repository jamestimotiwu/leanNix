#include "types.h"

int32_t process_execute(const uint8_t* command);
int32_t process_halt(uint8_t status);

int32_t command_read(uint8_t* command, uint8_t* arg, int32_t offset);
