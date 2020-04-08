#include "process.h"
#include "drivers/fs.h"
#include "drivers/rtc.h"
#include "types.h"

// TODO move to fs driver?
int loadProgram(char *command, uint8_t *buf) {
    int fd;
    if ((fd = fs_open(command)) == -1) {
        return -1;
    }

    uint8_t elfHeader[3];
    fs_read(fd, elfHeader, 3);

    /* Check that the header is correct */
    if (elfHeader[0] != 'E' || elfHeader[1] != 'L' || elfHeader[2] != 'F') {
        return -1;
    }

    // todo copy the buffer

    return 0;
}

/* File operations table */
/* Regular file table */
file_op_t fs_operations[] = {
    fs_open, fs_close, fs_read, fs_write
};

/* Directory file table */
file_op_t directory_operations[] = {
    directory_open, directory_close, directory_read, directory_write
};

/* RTC table */
file_op_t rtc_operations[] = {
    rtc_open, rtc_close, rtc_read, rtc_write
};
