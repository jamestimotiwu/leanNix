#include <types.h>
#include <process.h>

int32_t running_pid = -1;

/* sched
 *   DESCRIPTION: schedules next process to execute round robin
 *   INPUTS: None 
 *   OUTPUTS: None
 *   SIDE EFFECTS: starts executing a different program
 */
void sched (void) {
	int32_t p;

	/* check if processes exist and nothing is running */
    if (running_pid == -1 && process_arr[0] == 1) {
		running_pid = 0;
	}

	p = running_pid;
	/* increment running pid to next process to schedule */
	while (p < PROCESS_NUM) {
		p++;
		running_pid = p;

		if (running_pid == PROCESS_NUM)
			running_pid = 0;

		if (process_arr[running_pid] = 1) {
			switch_process(running_pid);
			break;
		}
	}
	return;
}


