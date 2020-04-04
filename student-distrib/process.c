#include "process.h"
#include "drivers/fs.h"
#include "drivers/rtc.h"

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
