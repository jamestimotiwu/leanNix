#include "lib.h"
#include "syscall.h"
#include "drivers/fs.h"
#include "page.h"
#include "interrupt_linkage.h"
#include "process.h"


/* execute
 *   DESCRIPTION: syscall that halts and returns execution to parent process
 *   INPUTS: status -- status of the halt
 *   OUTPUTS: none, because it doesn't return
 *   SIDE EFFECTS: ends execution of a program
 */
int32_t halt (uint8_t status){
    PCB_t *pcb = create_pcb(current_pid);
    /* restore parent data */
    current_pid = pcb->parent_id;

    /* restore parent paging */
    page_map_user(current_pid);

    /* close any relevant FDs */

    /* jump to execute return */

    /* set the stack pointer to parent's stack pointer and jump*/
    asm volatile("movl %0, %%esp \n\
                  jmp execute_return"
            :
            : "g" ((pcb->stack_ptr)) /* inputs */
            : "esp" /* clobbers */);

    return 0; 
}

//static int pid = 0;

/* execute
 *   DESCRIPTION: syscall that executes a command
 *   INPUTS: command - the file to execute
 *   OUTPUTS: 0 if success, else -1
 *   SIDE EFFECTS: starts executing a different program
 */
int32_t execute(const uint8_t* command){
    uint32_t entry;
    PCB_t *pcb;
    int32_t esp;
    int32_t parent_pid = current_pid;
    current_pid++;
    char program[FILENAME_CHAR_LIMIT];

    if (command == NULL)
        return -1;

    /* copy string to kernel file */
    strncpy(program, command, FILENAME_CHAR_LIMIT);


    /* parse args */

    /* check file validity */
    if (!program_valid(program))
        return -1;

    /* set up paging */
    page_map_user(current_pid);

    /* load file into memory and get entry point*/
    entry = program_load(program, current_pid);

    /* create PCB/open FDs */
    pcb = create_pcb(current_pid);
    pcb->process_id = current_pid;

    /* get the stack pointer, put it in esp and then the pcb */
    asm volatile("movl %%esp, %0"
            : "=rm"((esp)) /* outputs */
            :
            : "memory");

    pcb->stack_ptr = esp;
    pcb->parent_id = parent_pid;
    pcb->level = 0;
    //pcb->arguments = {};
    /* initailize stdin and stdout */
    /* other entries (eg. inode) aren't used */
    pcb->fd_arr[STDIN].file_ops = &stdin_file_ops;
    pcb->fd_arr[STDOUT].file_ops = &stdout_file_ops;


    /* prepare for context switch */

    /* push IRET context onto stack and do IRET */
    execute_iret(PROGRAM_VIRTUAL_STACK, entry);

    asm volatile("execute_return:");

    return 0;
}

/* system call read */ 
int32_t read(int32_t fd, void* buf, int32_t nbytes){
    //printf("read wiating\n");
    PCB_t *pcb = create_pcb(current_pid);
    if (fd < 0 || fd >= MAX_NUM_FD)
        return -1;


    return pcb->fd_arr[fd].file_ops->read_ptr(fd, buf, nbytes);
}

/* system call write */ 
int32_t write(int32_t fd, const void* buf, int32_t nbytes){
    PCB_t *pcb = create_pcb(current_pid);
    if (fd < 0 || fd >= MAX_NUM_FD)
        return -1;
    
    return pcb->fd_arr[fd].file_ops->write_ptr(fd, buf, nbytes);
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

