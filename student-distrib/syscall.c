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
    /* restore parent data */
    /* restore parent paging */
    /* close any relevant FDs */
    /* jump to execute return */

    return 0; 
}

static int pid = 0;

/* execute
 *   DESCRIPTION: syscall that executes a command
 *   INPUTS: command - the file to execute
 *   OUTPUTS: 0 if success, else -1
 *   SIDE EFFECTS: starts executing a different program
 */
int32_t execute(const uint8_t* command){
    uint32_t entry;

    if (command == NULL)
        return -1;

    /* parse args */

    /* check file validity */
    if (!program_valid(command))
        return -1;

    /* set up paging */
    page_map_user(pid);
    // todo: flush to tlb?

    /* load file into memory */
    entry = program_load(command, pid); /* also get the entry point */

    /* create PCB/open FDs */

    /* prepare for context switch */

    /* push IRET context onto stack and do IRET */
    /* set up iret stack and execute (esp=entry, ebp=) */
    execute_iret(PROGRAM_VIRTUAL_STACK, entry);

    /* return */

    return 0;
}

/* system call read */ 
int32_t read(int32_t fd, void* buf, int32_t nbytes){


 printf("syscall read invoked\n");
 return 0; 



}

/* system call write */ 
int32_t write(int32_t fd, const void* buf, int32_t nbytes){

 printf("syscall write invoked\n");
 return 0;

}


/* system call open */ 
int32_t open(const uint8_t* filename){

printf("syscall open invoked\n");
return 0; 


}

/* system call close */ 
int32_t close(int32_t fd){

printf("syscall close invoked\n");
return 0;

}
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