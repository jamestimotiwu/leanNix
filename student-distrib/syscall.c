#include "lib.h"
#include "syscall.h"
#include "drivers/fs.h"
#include "page.h"
#include "interrupt_linkage.h"
#include "process.h"
#include "schedule.h"
#include "x86_desc.h"
#include "drivers/keyboard.h"

int32_t process_arr[PROCESS_NUM] = { 0,0,0,0,0,0 };

/* halt32
 *   DESCRIPTION: same as halt, but argument is 32 bits so
 *                that this can be called during an exception (status = 256)
 *   INPUTS: status -- 32 bit status of halt
 *   OUTPUT: 0
 *   SIDE EFFECTS: halts execution of program
 */
int32_t halt32(uint32_t status) {
    PCB_t *pcb = create_pcb(current_pid);
    PCB_t *parent;

    int32_t i, ebp, esp, old_pid;
    process_arr[current_pid] = 0;
    if (pcb->parent_id == -1) {
        /* restart shell when it tries to halt */
        current_pid = pcb->term_num;
        /* Reload the program */
        pcb->entry = program_load((uint8_t*)"shell", current_pid);
        
        ret_to_user(pcb->entry);
    }

    cli();

    /* close any relevant FDs of process being halted */
    for (i = 0; i < MAX_NUM_FD; i++) {
        if (i != STDIN && i != STDOUT) {
            /* close functions for stdin and stdout return an error, so ignore those */
            close(i);
        } else {
            /* make sure stdin and stdout flags get set to closed */
            set_fd_close(i, pcb);
        }
    }

    old_pid = current_pid;
    /* restore parent data */
    current_pid = pcb->parent_id;
    /* change kernel stack to parent's kernel stack */
    parent = create_pcb(current_pid);
    tss.esp0 = get_kernel_stack(current_pid);

    esp = pcb->stack_ptr;
    ebp = pcb->base_ptr;

    /* remove process with old_pid (process being halted) in queue and replace with parent process id */
    sched_queue_process(old_pid, parent->process_id, 1);

    /* restore parent paging */
    page_map_user(current_pid);

    /* jump to execute return */
    halt_ret(esp, ebp, status);

    return 0;
}

/* halt
 *   DESCRIPTION: syscall that halts and returns execution to parent process
 *   INPUTS: status -- status of the halt
 *   OUTPUTS: none, because it doesn't return
 *   SIDE EFFECTS: ends execution of a program
 */
int32_t halt (uint8_t status){
    return halt32((uint32_t) status);
}


/* execute
 *   DESCRIPTION: syscall that executes a command
 *   INPUTS: command - the file to execute
 *   OUTPUTS: 0 if success, else -1
 *   SIDE EFFECTS: starts executing a different program
 */
int32_t execute(const uint8_t* command){
    /* Maximum 3 shells */
    int p;
    uint32_t entry;
    int32_t ebp;
    int32_t esp;
    int32_t parent_pid = current_pid;
    uint8_t program[KB_BUF_SIZE+1];
    int i = 0, j = 0;

    /* Find an open pid */
    if ((p = get_available_pid()) == -1)
        return -1;

    PCB_t *pcb = create_pcb(p);

    /* Child process executes in the same terminal as the parent */
    if (parent_pid == -1) {
        /* Top-level shell: the terminal # is same as pid */
        pcb->term_num = p;
    } else {
        /* Terminal number inherited from parent */
        pcb->term_num = create_pcb(parent_pid)->term_num;
    }
        

    if (command == NULL)
        return -1;

    /* parse args */

    /* skip the leading whitespace */
    while (command[i] == ' ')
        i++;

    /* Copy program name from the command into kernel memory */
    while (command[i] != ' ' && command[i] != '\0') {
        program[j] = command[i];
        j++;
        i++;
    }
    program[j] = '\0';

    /* skip the whitespace */
    while (command[i] == ' ')
        i++;

    /* Copy the remaining characters in command into the program's argument */
    j = 0;
    while (command[i] != '\0') {
        pcb->argument[j] = command[i];
        j++;
        i++;
    }
    pcb->argument[j] = '\0';

    /* check file validity */
    if (!program_valid(program))
        return -1;

    /* execute didn't fail, so update pid info */
    process_arr[p] = 1;
    pcb->process_id = p;

    /* set up paging */
    page_map_user(p);

    /* load file into memory and get entry point*/
    entry = program_load(program, p);


    pcb->parent_id = parent_pid;


    /* initailize stdin and stdout */
    /* other entries (eg. inode) aren't used */
    pcb->fd_arr[STDIN].file_ops = &stdin_file_ops;
    set_fd_open(STDIN, pcb);
    pcb->fd_arr[STDOUT].file_ops = &stdout_file_ops;
    set_fd_open(STDOUT, pcb);

    pcb->entry = entry;
    /* No sched base pointer yet - scheduler will handle this case differently */
    pcb->sched_bp = 0;
    /* add new process to schedule */
    sched_queue_process(pcb->parent_id, pcb->process_id, 0);
    /* set no rtc_freq */
    pcb->rtc_freq = -1;

    /* get the stack pointer and base pointer */
    asm volatile("movl %%esp, %0 \n\
                  movl %%ebp, %1"
                  
            : "=r"((esp)), "=r"((ebp)) /* outputs */
            :
            : "memory");

    pcb->base_ptr = ebp;
    pcb->stack_ptr = esp;

    tss.esp0 = get_kernel_stack(current_pid); /* set the kernel's stack pointer */

    /* Force scheduler to run */
    sched();

    return 0;
}

/* read
 *   DESCRIPTION: read bytes of a file
 *   INPUTS: fd -- the file descriptor
 *           buf -- the destination buffer
 *           nbytes -- number of bytes to read
 *   OUTPUTS: number of bytes copied
 *   SIDE EFFECTS: changes buf
 */
int32_t read(int32_t fd, void* buf, int32_t nbytes){
    PCB_t *pcb = create_pcb(current_pid);

    if (buf == NULL || fd < 0 || fd >= MAX_NUM_FD)
        return -1;

    if (fd_is_open(fd, pcb))
        return pcb->fd_arr[fd].file_ops->read_ptr(fd, buf, nbytes);

    return -1; /* file was not opened */
}

/* write
 *   DESCRIPTION: write bytes into file
 *   INPUTS: fd -- the file descriptor
 *           buf -- the source buffer
 *           nbytes -- number of bytes to write
 *   OUTPUTS: number of bytes copied
 *   SIDE EFFECTS: writes to a file
 */
int32_t write(int32_t fd, const void* buf, int32_t nbytes){
    PCB_t *pcb = create_pcb(current_pid);

    if (buf == NULL || fd < 0 || fd >= MAX_NUM_FD)
        return -1;
    if (fd_is_open(fd, pcb))
        return pcb->fd_arr[fd].file_ops->write_ptr(fd, buf, nbytes);

    return -1; /* file was not opened */
}

/* open
 *   DESCRIPTION: open a file descriptor
 *   INPUTS: filename -- the name of the file to open
 *   OUTPUTS: the file descriptor, or -1 if failure
 *   SIDE EFFECTS: changes pcb open files
 */
int32_t open(const uint8_t* filename){
    PCB_t* pcb = create_pcb(current_pid);
    int i;

    /* check user pointer for null */
    if (filename == NULL)
        return -1;

    /* find if there is an open file descriptor */
    for(i = 0; i < MAX_NUM_FD; i++){
        if(!fd_is_open(i, pcb))
            /* An fd that can be opened has been found */
            break;
    }

    if(i == MAX_NUM_FD)
        return -1;

    dir_entry_t dentry;
    if (-1 == read_dentry_by_name(filename, &dentry))
        return -1;

    //check type to assign file_ops
    if(dentry.type == FTYPE_RTC)
      pcb->fd_arr[i].file_ops = &rtc_file_ops;
    else if(dentry.type == FTYPE_DIR)
      pcb->fd_arr[i].file_ops = &dir_file_ops;
    else if(dentry.type == FTYPE_FILE)
      pcb->fd_arr[i].file_ops = &fs_file_ops;
    else
        return -1;

    pcb->fd_arr[i].inode = dentry.inode_num;
    pcb->fd_arr[i].file_pos = 0;
    pcb->fd_arr[i].file_ops->open_ptr(filename); // return this?
    set_fd_open(i, pcb);

    return i;
}

/* close
 *   DESCRIPTION: close a file descriptor
 *   INPUTS: filename -- the name of the file to close
 *   OUTPUTS: the file descriptor
 *   SIDE EFFECTS: changes pcb fd_arr
 */
int32_t close(int32_t fd){
    int retval;

    /* check that fd is valid (can't close stdout or stdin) */
    if (fd < 0 || fd >= MAX_NUM_FD || fd == STDOUT || fd == STDIN)
        return -1;

    PCB_t* pcb = create_pcb(current_pid);
    if (!fd_is_open(fd, pcb)) {
        /* trying to close an already closed fd */
        return -1;
    }

    /* set flags to 0 (closed) */
    set_fd_close(fd, pcb);

    retval = pcb->fd_arr[fd].file_ops->close_ptr(fd);
    
    return retval;
}

/* getargs
 *   DESCRIPTION: get the arguments of the command
 *   INPUTS: buf -- location to copy the argument into
 *           nbytes -- number of bytes to copy
 *   OUTPUTS: 0 on success, else -1 (if buf is null or argument is empty)
 *   SIDE EFFECTS: changes user-level buf
 */
int32_t getargs(uint8_t *buf, int32_t nbytes) {
    PCB_t* pcb = create_pcb(current_pid);
    int i;

    /* fail if pointer is invalid or argument is empty */
    if (buf == NULL || pcb->argument[0] == '\0')
        return -1;

    for (i = 0; i < nbytes && pcb->argument[i] != '\0'; i++) {
        buf[i] = pcb->argument[i];
    }

    if (i == nbytes)
        /* There are still more bytes to copy */
        return -1;

    buf[i] = '\0';

    return 0;

}

/* vidmap
 *   DESCRIPTION: Sets screen address pointer to user video memory page
 *   INPUTS: screen_start - screen address pointer
 *   OUTPUTS: screen_address to user video memory page
 *   SIDE EFFECTS: modify given screen_start pointer
 */
int32_t vidmap(uint8_t** screen_start) {
    PCB_t* pcb = create_pcb(current_pid);
    uint32_t term_num = pcb->term_num;

    if (screen_start == NULL) {
        return -1;
    }
    /* Validate screen_start double ptr */
    if ((uint32_t)screen_start < USER_PAGE_VMIN || (uint32_t)screen_start >= USER_PAGE_VMAX) {
        return -1;
    }

    *screen_start = (uint8_t*)(USER_VMEM_VADDR + (term_vid_addr[term_num] << 12));
    return 0;
}

/* set_handler
 *   DESCRIPTION: changes the default action taken when a signal is received
 *   INPUTS: signum -- the number of the signal
 *           handler_address - address of function to be set as the handler
 *   OUTPUTS: 0 if success, -1 if failure
 *   SIDE EFFECTS: changes signal behavior
 */
int32_t set_handler(int32_t signum, void *handler_address) {
    // not yet implemented
    return -1;
}

/* signum
 *   DESCRIPTION: used by signal handler to return execution to program
 *   INPUTS: none
 *   OUTPUTS: 0 if success, -1 if failure
 *   SIDE EFFECTS: returns exeuction to user program
 */
int32_t sigreturn(void) {
    // not yet implemented
    return -1;
}

