/* limit of process queue size */
#define SCHEDULE_QUEUE_NUM	3

/* add/stop process to schedule */
void sched_queue_process(int32_t stop_pid, int32_t exec_pid, int remove);

/* schedule next process on call */
void sched(void);

/* find the next open pid, if there is one */
int32_t get_available_pid();

void init_shells(void);
