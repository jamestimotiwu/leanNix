#include "lib.h"
#include "syscall.h"
#include "drivers/fs.h"
#include "page.h"
#include "interrupt_linkage.h"
#include "process.h"

#define FDA_SIZE 8

/* execute
 *   DESCRIPTION: syscall that halts and returns execution to parent process
 *   INPUTS: status -- status of the halt
 *   OUTPUTS: none, because it doesn't return
 *   SIDE EFFECTS: ends execution of a program
 */
int32_t halt (uint8_t status){
    PCB_t *pcb = create_pcb(current_pid);
    PCB_t *parent;
    /* restore parent data */
    current_pid = pcb->parent_id;
    parent = create_pcb(current_pid);

    /* restore parent paging */
    page_map_user(current_pid);

    /* close any relevant FDs */
    int i;
    for (i=0; i < FDA_SIZE; i++){
      pcb->fd_arr[i].file_ops = NULL;
      pcb->fd_arr[i].inode = -1;
      pcb->fd_arr[i].file_pos = 0;
      pcb->fd_arr[i].flags = 0;
    }
    /* jump to execute return */

    /* set the stack pointer to parent's stack pointer and jump*/
    asm volatile("movl %0, %%esp \n\
                  jmp execute_iret_return"
            :
            : "g" ((parent->stack_ptr)) /* inputs */
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
    char program[FILENAME_CHAR_LIMIT];

    if (command == NULL)
        return -1;

    /* parse args */
    /* copy string to kernel file TODO */
    strncpy(program, command, FILENAME_CHAR_LIMIT);

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

    /* get the stack pointer, put it in esp and then the pcb */
    asm volatile("movl %%esp, %0"
            : "=rm"((esp)) /* outputs */
            :
            : "memory");

    pcb->stack_ptr = esp+46;
    pcb->parent_id = parent_pid;
    pcb->level = 0;
    //pcb->arguments = {};
    /* initailize stdin and stdout */
    /* other entries (eg. inode) aren't used */
    pcb->fd_arr[STDIN].file_ops = &stdin_file_ops;
    pcb->fd_arr[STDOUT].file_ops = &stdout_file_ops; // TODO: set open


    /* prepare for context switch */

    /* push IRET context onto stack and do IRET */
    execute_iret(PROGRAM_VIRTUAL_STACK, entry);

    return 0;
}

/* system call read */
int32_t read(int32_t fd, void* buf, int32_t nbytes){
    //printf("read wiating\n");
    PCB_t *pcb = create_pcb(current_pid);
    // TODO: also check that the fd is open
    if (fd < 0 || fd >= MAX_NUM_FD)
        return -1;


    return pcb->fd_arr[fd].file_ops->read_ptr(fd, buf, nbytes);
}

/* system call write */
int32_t write(int32_t fd, const void* buf, int32_t nbytes){
    PCB_t *pcb = create_pcb(current_pid);
    // TODO: also check that the fd is open
    if (fd < 0 || fd >= MAX_NUM_FD)
        return -1;

    return pcb->fd_arr[fd].file_ops->write_ptr(fd, buf, nbytes);
}


/* system call open */
int32_t open(const uint8_t* filename){
    //int32_t fd;
    /* first find if there is an open file descriptor */
    //fd = 0;
    PCB_t* pcb = create_pcb(current_pid);
    int i;
    for(i = 0; i < FDA_SIZE; i++){
      if(pcb->fd_arr[i].flags == 0)
        break;
    }
    if(i == FDA_SIZE)
      return -1;
    dir_entry_t dentry;
    read_dentry_by_name(filename, &dentry);
    //check type to assign file_ops
    if(dentry.type == 0)
      pcb->fd_arr[i].file_ops = &rtc_file_ops;
    else if(dentry.type == 1)
      pcb->fd_arr[i].file_ops = &dir_file_ops;
    else if(dentry.type == 2)
      pcb->fd_arr[i].file_ops = &fs_file_ops;

    pcb->fd_arr[i].inode = dentry.inode_num;
    pcb->fd_arr[i].flags = 1;
    pcb->fd_arr[i].file_pos = 0;
    return pcb->fd_arr[i].file_ops->open_ptr(filename);
    //return fd;
}

/* system call close */
int32_t close(int32_t fd){
    /* check that fd is valid */
    if (fd < 0 || fd >= MAX_NUM_FD)
        return -1;
    PCB_t* pcb = create_pcb(current_pid);
    pcb->fd_arr[fd].file_ops = &dir_file_ops;
    pcb->fd_arr[fd].flags = 1;

    return pcb->fd_arr[fd].file_ops->close_ptr(fd);
    //return 0;
}
