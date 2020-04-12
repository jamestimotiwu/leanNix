/* process.c - process and executable system calls
 * vim:ts=4 noexpandtab
 */

#include "types.h"
#include "drivers/fs.h"
#include "page.h"
#include "lib.h"
#include "process.h"
#include "drivers/terminal.h"

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
#define PROCESS_DATA_SIZE (8<<10)
/* 8MB */
#define PCB_OFFSET (8<<20)

/* get_kernel_stack
 *   DESCRIPTION: gets the top of the kernel stack for a given pid
 *   INPUTS: pid -- the pid of the process
 *   OUTPUTS: address of the stack
 *   SIDE EFFECTS: none
 */
uint32_t get_kernel_stack(int32_t pid) {
	/* get the bottom of the kernel stack */
	return PCB_OFFSET - PROCESS_DATA_SIZE*(pid);
}

PCB_t *create_pcb(int32_t pid) {
	/* PCB address: 8MB - 8KB*(pid+1) */
	PCB_t *pcb = (PCB_t *) (PCB_OFFSET - PROCESS_DATA_SIZE*(pid+1));
	return pcb;
}

/* bad_call
 *   DESCRIPTION: used instead of terminal open/close
 *   INPUTS: none
 *   OUTPUTS: -1 (failure)
 */
int32_t bad_call() {
	return -1;
}

file_ops_ptr_t stdin_file_ops = { (read_op)terminal_read, (write_op)bad_call,
	                              (open_op)bad_call, (close_op)bad_call };

file_ops_ptr_t stdout_file_ops = { (read_op)bad_call, (write_op)terminal_write,
	                               (open_op)bad_call, (close_op)bad_call };

/* set_fd_open
 *   DESCRIPTION: sets the flag of a file descriptor to open
 *   INPUTS: fd -- the file descriptor
 *           pcb -- program control block
 *   OUTPUTS: none
 *   SIDE EFFECTS: changes fd_arr in pcb
 */
void set_fd_open(int32_t fd, PCB_t *pcb) {
	/* set the fd's open flag to 1 */
	pcb->fd_arr[fd].flags |= FDFLAG_OPEN;
}

/* set_fd_close
 *   DESCRIPTION: sets the flag of a file descriptor to closed
 *   INPUTS: fd -- the file descriptor
 *           pcb -- program control block
 *   OUTPUTS: none
 *   SIDE EFFECTS: changes fd_arr in pcb
 */
void set_fd_close(int32_t fd, PCB_t *pcb) {
	/* set the fd's open flag to 0 */
	pcb->fd_arr[fd].flags &= ~FDFLAG_OPEN;
}

int32_t fd_is_open(int32_t fd, PCB_t *pcb) {
	return pcb->fd_arr[fd].flags & FDFLAG_OPEN;
}

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



