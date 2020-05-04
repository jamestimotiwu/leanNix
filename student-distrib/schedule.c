#include "lib.h"
#include "types.h"
#include "process.h"
#include "x86_desc.h"
#include "page.h"
#include "schedule.h"
#include "interrupt_linkage.h"
#include "drivers/fs.h"
#include "drivers/rtc.h"

/* Program not loaded */
#define NOT_LOADED 0
/* Program is loaded into memory, but not scheduled to execute (ie. its child is scheduled */
#define LOADED 1
/* Program can be scheduled to run */
#define SCHEDULED 2

/* process queue to schedule; by default all processes are NOT_RUNNING (0) */
int32_t process_queue[PROCESS_NUM]; 

/* get_available_pid
 *   DESCRIPTION: finds the next free pid and returns it
 *   INPUTS: none
 *   OUTPUTS: -1 if there are no free pids, else the pid
 *   SIDE EFFECTS: none
 */
int32_t get_available_pid() {
    int i;
    for (i = 0; i < PROCESS_NUM; i++) {
        if (process_queue[i] == NOT_LOADED)
            return i;
    }
    return -1;
}

/* sched
 *   DESCRIPTION: schedules next process to execute round robin
 *   INPUTS: None
 *   OUTPUTS: None
 *   SIDE EFFECTS: starts executing a different program
 */
void sched(void) {
	int32_t p, i;
	int32_t sleep_pid;
	int32_t running_pid;
	int32_t ebp;
    int32_t esp;
    PCB_t* pcb;

    if (current_pid != -1) {

        /* pcb for process to sleep; save esp, ebp */
        sleep_pid = current_pid;
        pcb = create_pcb(sleep_pid);

        /* get the stack pointer and base pointer and save to pcb before sleeping*/
        asm volatile("movl %%esp, %0 \n\
                      movl %%ebp, %1"
            : "=rm"((esp)), "=rm"((ebp)) /* outputs */
            :
            : "memory");

        pcb->sched_bp = ebp;
        pcb->stack_ptr = esp;
	}

    for (i = 1; i < PROCESS_NUM; i++) {
        p = (current_pid + i) % PROCESS_NUM;

        if (process_queue[p] == SCHEDULED) {
            /* p is pid of next process to schedule */
            break;
        }
    }

	/* set next process as running_pid */
	running_pid = p;

	/* restore next scheduled process context */
	pcb = create_pcb((uint32_t) running_pid);

	/* remap paging; flush tlb */
	page_map_user(running_pid);
	/* set next process tss / set next esp */
	tss.esp0 = get_kernel_stack(running_pid);

    current_pid = running_pid;

    if (pcb->rtc_freq != -1)
        rtc_set_freq(pcb->rtc_freq);

    if (pcb->sched_bp == 0) {
        /* First time this program is being entered, so it needs to be handled differently */
        ret_to_user(pcb->entry);
    }

	/* restore next process stack and base pointer */
	asm volatile("movl %0, %%ebp \n\
				  movl %1, %%ebp \n\
                  leave          \n\
                  ret"
		:
		: "r"((pcb->stack_ptr)), "r"((pcb->sched_bp)) /* inputs */
		: "memory");

	return;
}


/* sched_queue_process
 *   DESCRIPTION: add process to process queue
 *   INPUTS: stop_pid - process to stop/remove from queue based on corresponding parent
 *			 exec_pid - process to start/add to queue, indexed to itself if parent
 *			 remove - boolean that decides if stop_pid should be completely removed
 *   OUTPUTS: None
 *   SIDE EFFECTS: adds pid to the schedule queue depending on its pid
 */
void sched_queue_process(int32_t stop_pid, int32_t exec_pid, int remove) {
	/* set process to wait based on parent*/
	if (stop_pid == -1) {
		/* nothing to stop */
		process_queue[exec_pid] = SCHEDULED;
	} else {
		/* parent is not base shell, get parent of next shell */
        if (remove)
            process_queue[stop_pid] = NOT_LOADED; /* remove completely */
        else
            process_queue[stop_pid] = LOADED; /* It's still loaded into memory, just not scheduled */
        process_queue[exec_pid] = SCHEDULED;

	}
	return;
}

/* init_shells
 *   DESCRIPTION: initializes a shell for each open terminal
 *   INPUTS: none
 *   OUTPUTS: None
 *   SIDE EFFECTS: changes scheduling and pcb data structures
 */
void init_shells(void) {
    /* Assumed to be running with cli */

    int i;
    for (i = 0; i < TERM_MAX; i++) {
        /* Initialize a shell for every terminal */
        execute_sh(i);
    }
}

void execute_sh(int32_t term_num) {
    PCB_t* pcb;

    process_arr[term_num] = 1;
    pcb = create_pcb(term_num);

    pcb->term_num = term_num; /* terminal # corresonds with pid for the shells */
    pcb->process_id = term_num;
    pcb->parent_id = -1; /* shells have no parent (top level) */
    pcb->stack_ptr = get_kernel_stack(term_num);
    pcb->argument[0] = '\0'; /* shells are run with no arguments */

    /* Initialize stdin and stdout */
    pcb->fd_arr[STDIN].file_ops = &stdin_file_ops;
    set_fd_open(STDIN, pcb);
    pcb->fd_arr[STDOUT].file_ops = &stdout_file_ops;
    set_fd_open(STDOUT, pcb);

    /* initialize it for scheduling */
    sched_queue_process(pcb->parent_id, pcb->process_id, 0);

    pcb->rtc_freq = -1;
    pcb->sched_bp = 0; /* not yet set */
    page_map_user(term_num);
    pcb->entry = program_load((uint8_t*)"shell", term_num);
    pcb->stack_ptr = get_kernel_stack(current_pid);
}

