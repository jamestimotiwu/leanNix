/* limit of process queue size */
#define SCHEDULE_QUEUE_NUM	3

/* add/stop process to schedule */
void sched_queue_process(int32_t stop_pid, int32_t exec_pid);

/* schedule next process on call */
void sched(void);