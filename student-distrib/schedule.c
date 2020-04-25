#include "types.h"
#include "process.h"
#include "x86_desc.h"
#include "syscall.h"
#include "page.h"

int32_t running_pid = -1;

/* sched
 *   DESCRIPTION: schedules next process to execute round robin
 *   INPUTS: None 
 *   OUTPUTS: None
 *   SIDE EFFECTS: starts executing a different program
 */
void sched (void) {
	int32_t p;
	int32_t sleep_pid;
	int32_t ebp;
	int32_t esp;
	cli();
	/* check if processes exist and nothing is running */
    if (running_pid == -1 && process_arr[0] == 1) {
		running_pid = 0;
	}
	/* old process; save esp, ebp */
	sleep_pid = running_pid;
	PCB_t *pcb = create_pcb(sleep_pid);

    /* get the stack pointer and base pointer */
    asm volatile("movl %%esp, %0 \n\
				  movl %%ebp, %1"
            : "=rm"((esp)), "=rm"((ebp)) /* outputs */
            :
            : "memory");
	
	pcb->base_ptr = ebp;
	pcb->stack_ptr = esp;

	/* increment running pid to next process to schedule */
	p = running_pid;
	while (p < PROCESS_NUM) {
		p++;
		running_pid = p;

		if (running_pid == PROCESS_NUM)
			running_pid = 0;

		if (process_arr[running_pid] = 1) {
			break;
		}
	}

	/* remap paging; flush tlb */
	page_map_user(running_pid);
	/* restore next scheduled process context */
	PCB_t *pcb = create_pcb(running_pid);

	/* set next process tss / set next esp */
	tss.esp0 = get_kernel_stack(running_pid);

	/* restore next process stack and base pointer */
	asm volatile("movl %0, %%esp \n\
				  movl %1, %%ebp"
		: "r"((pcb->esp)), "r"((pcb->ebp)) /* outputs */
		:
		: "memory");

	send_eoi(0);
	sti();
	return;
}
