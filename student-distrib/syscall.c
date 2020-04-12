#include "lib.h"
#include "syscall.h"
#include "drivers/fs.h"
#include "page.h"
#include "interrupt_linkage.h"
#include "process.h"
#include "x86_desc.h"


/* execute
 *   DESCRIPTION: syscall that halts and returns execution to parent process
 *   INPUTS: status -- status of the halt
 *   OUTPUTS: none, because it doesn't return
 *   SIDE EFFECTS: ends execution of a program
 */
int32_t halt (uint8_t status){
    PCB_t *pcb = create_pcb(current_pid);
    PCB_t *parent;
    int32_t i, ebp;

    /* don't halt the shell (pid 0) */
    if (current_pid == 0)
        return -1;

    /* restore parent data */
    current_pid = pcb->parent_id;
    /* change kernel stack to parent's kernel stack */
    parent = create_pcb(current_pid);
    tss.esp0 = parent->stack_ptr;

    /* restore parent paging */
    page_map_user(current_pid);

    /* close any relevant FDs */
    for (i = 0; i < MAX_NUM_FD; i++) {
        set_fd_close(i, pcb);
    }

    ebp = pcb->base_ptr;

    /* jump to execute return */
    halt_ret(parent->stack_ptr, ebp);
    // TODO remove parent stack ptr

    return 0;
}

/* execute
 *   DESCRIPTION: syscall that executes a command
 *   INPUTS: command - the file to execute
 *   OUTPUTS: 0 if success, else -1
 *   SIDE EFFECTS: starts executing a different program
 */
int32_t execute(const uint8_t* command){
    uint32_t entry;
    PCB_t *pcb;
    int32_t ebp;
    int32_t parent_pid = current_pid;
    uint8_t program[FILENAME_CHAR_LIMIT];

    if (command == NULL)
        return -1;

    /* parse args */
    /* copy string to kernel memory */
    strncpy((int8_t *) program, (int8_t *) command, FILENAME_CHAR_LIMIT);

    /* check file validity */
    if (!program_valid(program))
        return -1;

    /* set up paging */
    current_pid++;
    page_map_user(current_pid);

    /* load file into memory and get entry point*/
    entry = program_load(program, current_pid);

    /* create PCB/open FDs */
    pcb = create_pcb(current_pid);
    pcb->process_id = current_pid;

    /* get the stack pointer and base pointer */
    asm volatile("movl %%ebp, %0"
            : "=rm"((ebp)) /* outputs */
            :
            : "memory");

    pcb->parent_id = parent_pid;

    pcb->base_ptr = ebp;
    pcb->stack_ptr = get_kernel_stack(current_pid);
    tss.esp0 = pcb->stack_ptr; /* set the kernel's stack pointer */
    
    //pcb->arguments = {};

    /* initailize stdin and stdout */
    /* other entries (eg. inode) aren't used */
    pcb->fd_arr[STDIN].file_ops = &stdin_file_ops;
    set_fd_open(STDIN, pcb);
    pcb->fd_arr[STDOUT].file_ops = &stdout_file_ops;
    set_fd_open(STDOUT, pcb);


    /* prepare for context switch */

    /* push IRET context onto stack and do IRET */
    ret_to_user(entry);

    return 0;
}

/* system call read */ 
int32_t read(int32_t fd, void* buf, int32_t nbytes){
    PCB_t *pcb = create_pcb(current_pid);

    if (fd < 0 || fd >= MAX_NUM_FD)
        return -1;

    if (fd_is_open(fd, pcb))
        return pcb->fd_arr[fd].file_ops->read_ptr(fd, buf, nbytes);

    return -1; /* file was not opened */
}

/* system call write */ 
int32_t write(int32_t fd, const void* buf, int32_t nbytes){
    PCB_t *pcb = create_pcb(current_pid);

    if (fd < 0 || fd >= MAX_NUM_FD)
        return -1;
    
    if (fd_is_open(fd, pcb))
        return pcb->fd_arr[fd].file_ops->write_ptr(fd, buf, nbytes);

    return -1; /* file was not opened */
}


/* system call open */ 
int32_t open(const uint8_t* filename){
    int32_t fd;
    /* first find if there is an open file descriptor */
    fd = 0;

    return fd;
}

/* system call close */ 
int32_t close(int32_t fd){
    /* check that fd is valid */
    if (fd < 0 || fd >= MAX_NUM_FD)
        return -1;


    return 0;
}

