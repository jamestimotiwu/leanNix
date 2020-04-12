/* process.c - process and executable system calls
 * vim:ts=4 noexpandtab
 */

#include "types.h"
#include "drivers/fs.h"
#include "page.h"
#include "lib.h"
#include "process.h"
#include "drivers/terminal.h"
#include "drivers/rtc.h"
//static int32_t command_read(const uint8_t* command, uint8_t* arg);

/* start at -1 since shell will start with pid of 0 */
int32_t current_pid = -1;

/* get the PCB */
PCB_t* get_PCB(){
  PCB_t* position;
  /* AND ESP with mask*/
  asm volatile(
    "andl %%esp, %%eax"
    :"=a"(position) /* output */
    :"a"(PCB_MASK)  /* input */
  );
  return position;
}

/* 8KB */
#define MAX_PCB_SIZE (8<<10)
/* 8MB */
#define PCB_OFFSET (8<<20)


PCB_t *create_pcb(int32_t pid) {
	/* PCB address: 8MB - 8KB*(pid+1) */
	PCB_t *pcb = (PCB_t *) (PCB_OFFSET - MAX_PCB_SIZE*(pid+1));
	return pcb;
}

int32_t bad_call() {
	return -1;
}

file_ops_ptr_t stdin_file_ops = { (read_op)terminal_read, (write_op)bad_call, (open_op)bad_call, (close_op)bad_call };
file_ops_ptr_t stdout_file_ops = { (read_op)bad_call, (write_op)terminal_write, (open_op)bad_call, (close_op)bad_call };
file_ops_ptr_t rtc_file_ops = {(read_op)rtc_read, (write_op)rtc_write, (open_op)rtc_open, (close_op)rtc_close };
file_ops_ptr_t dir_file_ops = {(read_op)directory_read, (write_op)directory_write, (open_op)directory_open, (close_op)directory_close };
file_ops_ptr_t fs_file_ops = {(read_op)fs_read, (write_op)fs_write, (open_op)fs_open, (close_op)fs_close };

int32_t process_execute(const uint8_t* command) {
    /* Get filename from command */
    int8_t filename[32];
    int8_t args[128];
    int offset;

    offset = 0;

    /* Get filename, check if there is a file in command argument */
    offset = command_read((int8_t*)command, filename, offset);
    if (offset == 0)
        return -1;

    /* Getargs for rest of command */
    offset = command_read((int8_t*)command, args, offset);
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
int32_t command_read(int8_t* command, int8_t* arg, uint32_t offset) {
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
