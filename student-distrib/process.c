/* process.c - process and executable system calls
 * vim:ts=4 noexpandtab
 */

#include "types.h"
#include "drivers/fs.h"
#include "page.h"
#include "lib.h"
#include "process.h"

//static int32_t command_read(const uint8_t* command, uint8_t* arg);

int32_t process_execute(const uint8_t* command) {
    /* Get filename from command */
    uint8_t filename[32];
    uint8_t args[64];
    int offset;
    
    offset = 0;
    
    /* Get filename, check if there is a file in command argument */
    offset = command_read(command, filename, offset); 
    if (offset == 0)
        return -1;

    /* Getargs for rest of command */
    offset = command_read(command, args, offset);
    if (offset == 0)
        return -1;

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
int32_t command_read(uint8_t* command, uint8_t* arg, int32_t offset) {
    int i;

    /* Strip leading whitespace */
    while (command[offset] == ' ' && command[offset] != '\0') {
        offset++;
    }

    /* Get command arg from command */
    i = 0;
    command = &command[offset];
    while (command[i] != ' ' && command[i] != '\0') {
        arg[i] = command[i];
        offset++;
        i++;
    }

    /* return len of argument parsed*/
    return offset;
}
