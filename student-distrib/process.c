/* process.c - process and executable system calls
 * vim:ts=4 noexpandtab
 */

#include "types.h"
#include "page.h"

int32_t process_execute(const uint8_t* command) {
    /* Deconstr command arg */
    /* Check exec magic headers */

    /* Set up pages */
    /* Flush tlb */

    return 0;
}

int32_t process_halt(uint8_t status) {
    return 0;
}