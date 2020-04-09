/* process.c - process and executable system calls
 * vim:ts=4 noexpandtab
 */

#include "types.h"
#include "drivers/fs.h"
#include "page.h"
#include "lib.h"

int32_t process_execute(const uint8_t* command) {
    /* Get filename from command */
    uint8_t* filename;
    int i;
    int command_len;

    /* Check if command has a filename */
    command_len = strlen(command);
    if (command_len == 0) {
        return -1;
    }

    /* Get filename from command */
    i = 0;
    while (command[i] != ' ' || i < command_len) {
        filename[i] = command[i];
        i++;
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

//int8_t command_read(const uint8_t* command) {
//
//}
