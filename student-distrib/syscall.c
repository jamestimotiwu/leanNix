#include "lib.h"

/* syscall_int()
 *   DESCRIPTION: sys call c code
 *   INPUT/OUTPUT: none
 *   SIDE EFFECTS: prints to screen
 */
void syscall_int() {
    printf("SYSTEM CALL!\n");
}

int32_t syscall_halt(uint8_t status) {
    return 0;
}

int32_t syscall_execute(uint8_t *command) {
    return 0;
}

/* syscall_open
 *   DESCRIPTION: the c code for the open syscall
 *   INPUT: filename -- the file to be opened
 *   OUTPUT: 0 for success, -1 for error
 *   SIDE EFFECTS: creates open file descriptor
 */
int32_t syscall_open(const uint8_t* file_name) {
    printf("syscall_open\n");
    return 0;
}

int32_t syscall_close(uint32_t fd) {
    printf("syscall_close\n");
    return 0;
}

int32_t syscall_read(uint32_t fd, uint8_t *buf, uint32_t count) {
    printf("syscall_read\n");
    return 0;
}

int32_t syscall_write(uint32_t fd, uint8_t *buf, uint32_t count) {
    printf("syscall_write\n");
    return 0;
}
