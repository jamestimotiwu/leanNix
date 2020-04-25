#include "lib.h"
#include "types.h"
#include "process.h"
#include "x86_desc.h"
#include "page.h"
#include "schedule.h"

/* idx of current scheduled process */
int32_t running_idx = 0;

/* process queue to schedule */
int32_t process_queue[SCHEDULE_QUEUE_NUM] = { -1, -1, -1 };

/* sched
 *   DESCRIPTION: schedules next process to execute round robin
 *   INPUTS: None
 *   OUTPUTS: None
 *   SIDE EFFECTS: starts executing a different program
 */
void sched(void) {
	int32_t p;
	int32_t sleep_pid;
	int32_t running_pid;
	int32_t ebp;
	int32_t esp;

	/* check if processes exist and nothing is running */
	if (process_queue[running_idx] != -1) {
		return;
	}

	/* pcb for process to sleep; save esp, ebp */
	sleep_pid = process_queue[running_idx];
	PCB_t* pcb = create_pcb(sleep_pid);

	/* get the stack pointer and base pointer and save to pcb before sleeping*/
	asm volatile("movl %%esp, %0 \n\
				  movl %%ebp, %1"
		: "=rm"((esp)), "=rm"((ebp)) /* outputs */
		:
		: "memory");

	pcb->base_ptr = ebp;
	pcb->stack_ptr = esp;

	/* get next process to schedule by incrementing queue pointer */
	p = running_idx;
	while (p < SCHEDULE_QUEUE_NUM) {
		p++;
		running_idx = p;

		if (running_idx == SCHEDULE_QUEUE_NUM)
			running_idx = 0;

		if (process_queue[running_idx] != -1) {
			break;
		}
	}
	/* set next process as running_pid */
	running_pid = process_queue[running_idx];

	/* restore next scheduled process context */
	pcb = create_pcb(running_pid);

	/* remap paging; flush tlb */
	page_map_user(running_pid);
	/* set next process tss / set next esp */
	tss.esp0 = get_kernel_stack(running_pid);
	/* restore next process stack and base pointer */
	asm volatile("movl %0, %%esp \n\
				  movl %1, %%ebp"
		:
		: "r"((pcb->stack_ptr)), "r"((pcb->base_ptr)) /* outputs */
		: "memory");

	return;
}


/* sched_queue_process
 *   DESCRIPTION: add process to process queue
 *   INPUTS: stop_pid - process to stop/remove from queue based on corresponding parent
 *			 exec_pid - process to start/add to queue, indexed to itself if parent
 *   OUTPUTS: None
 *   SIDE EFFECTS: adds pid to the schedule queue depending on its pid
 */
void sched_queue_process(int32_t stop_pid, int32_t exec_pid) {
	int p;
	/* set process to wait based on parent*/
	if (stop_pid == -1) {
		/* if exec_pid < 3, it is a parent/shell */
		process_queue[exec_pid] = exec_pid;
	}
	else if (stop_pid < 3) {
		/* set exec_pid using stop_pid index */
		process_queue[stop_pid] = exec_pid;
	}
	else {
		/* parent is not base shell, get parent of next shell */
		for (p = 0; p < SCHEDULE_QUEUE_NUM; p++) {
			if (process_queue[p] == stop_pid) {
				process_queue[p] = exec_pid;
				break;
			}
		}

	}
	return;
}

