/* process.c - process and executable system calls
 * vim:ts=4 noexpandtab
 */

#include "types.h"
#include "drivers/fs.h"
#include "page.h"
#include "lib.h"

static int32_t command_read(const uint8_t* command, const uint8_t* arg);

int32_t process_execute(const uint8_t* command) {
    /* Get filename from command */
    uint8_t filename[32];
    uint8_t args[128];

    /* Get filename, check if there is a file in command argument */
    if (command_read(command, filename) == 0) {
        return -1;
    }

    /* Getargs for rest of command */
    if (command_read(command, args) == 0) {
        return -1;
    }

    /* Check if file exists with fs_open */

    /* Check exec magic headers */

    uint32_t proc_num = 0;
    /* Set up pages and flush tlb */
    page_map_user(proc_num);

    return 0;
}

int32_t process_halt(uint8_t status) {
    return 0;
}

/* command_read
 * Inputs: command -- number to convert
 *         arg -- buffer of argument to get
 * Return Value: length of argument parsed
 * Function: Reads first argument and remove leading whitespaces */
int32_t command_read(const uint8_t* command, const uint8_t* arg) {
    //uint8_t* arg[128];
    int command_len;
    int i;
    
    /* Check if command has anything remaining or valid */
    command_len = strlen((const int8_t*) command);
    if (command_len == 0) {
        return 0;
    }

    /* Strip leading whitespace */
    i = 0;
    while (command[i] == ' ' || i < command_len) {
        i++;
    }

    /* Get command arg from command */
    command = &command[i];
    i = 0;
    while (command[i] != ' ' || i < command_len) {
        arg[i] = command[i];
    }

    /* Set new starting address for rest of command */
    command = &command[i];

    /* return len of argument parsed*/
    return i;
}
