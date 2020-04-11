#ifndef PROCESS_H
#define PROCESS_H

#include "types.h"

/* physical addresses: first program starts at 8MB, others start at 8MB+4MB*pid */
#define PROGRAM_START_ADDR 0x800000
#define MAX_PROGRAM_SIZE 0x400000
/* virtual address used by program (128MB + 0x48000) */
#define PROGRAM_VIRTUAL_START 0x08048000
/* programs stack is at the bottom of the 4MB page */
#define PROGRAM_VIRTUAL_STACK (PROGRAM_VIRTUAL_START + 0x400000-1)

int32_t process_execute(const uint8_t* command);
int32_t process_halt(uint8_t status);

int32_t command_read(int8_t* command, int8_t* arg, uint32_t offset);

#endif
