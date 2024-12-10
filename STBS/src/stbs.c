#include "../lib/stbs.h"

void stbs_init(void){
    // Tasks: tick_handler, btn_handler, led_handler, uart_handler

    // Initialize semaphores
    k_sem_init(&btn_handler_sem, 0, 1);
    k_sem_init(&led_handler_sem, 0, 1);
    k_sem_init(&uart_handler_sem, 0, 1);
    
    // Initialize tick handler task (periodic, so it needs to be added to the task table)
    tick_handler_tid = k_thread_create(&tick_handler_thread, tick_handler_stack, K_THREAD_STACK_SIZEOF(tick_handler_stack),
                    tick_handler, NULL, NULL, NULL, 0, TICK_HANDLER_PRIORITY, K_NO_WAIT);

    struct k_sem tick_handler_sem;

    k_sem_init(&tick_handler_sem, 0, 1);

    task_table[num_tasks] = (task_t){.period_ticks = 0, .task_id = tick_handler_tid, 
        .worst_case_execution_time = 0, .priority = TICK_HANDLER_PRIORITY, .sem = NULL};

    num_tasks++;

    // Algorithm for creating static scheduling table
    /* TIMELINE SCHEDULLING ACCORDING TO BUTTAZZO:

	- Hyperperiod. It is the minimum interval of time after which the schedule repeats
	itself. If H is the length of such an interval, then the schedule in [0, H] is the
	same as that in [kK, (k + 1)K] for any integer k > 0. For a set of periodic
	tasks synchronously activated at time t = 0, the hyperperiod is given by the least
	common multiple of the periods:

	- Calculate LEast common multiplier (Major Cycle)

	- Smallest task's period (minor cycle)

	Task with smallest period needs to be executed every minor cycle, the next tasks every ncycles, BUT TASKS EXECUTING IN A MINOR CYCLE CAN'T BE BIGGER THAN IT */
    
    // Calculate hyperperiod
    hyper_period = 1;
    for(int i = 0; i < num_tasks; i++){
        hyper_period = lcm(hyper_period, task_table[i].period_ticks);
    }

    //Minor cycle is the smallest period
    minor_cycle = 10000;
    for(int i = 0; i < num_tasks; i++){
        if(task_table[i].period_ticks < minor_cycle)
            minor_cycle = task_table[i].period_ticks;
    }

   /* 100ms

    T1 = 25ms
    T2 = 50ms 
    T3 = 100ms

    Tick = 0 --> {T1, T2, T3}
    Tick = 25 --> {T1}
    Tick = 50 --> {T1, T2}
    Tick = 75 --> {T1}
    Tick = 100 --> {T1, T2, T3}

    pendind_tasks = [t1, t2, t3]
    worst_case_idx = 0

    for (tick in ticks){
        for task in task:
            if taks.period % tick == 0
                pending_tasks.add(task)
        if worst_case_idx == 0 and pending_tasks
            pending_tasks.pop(0) # Remove first position of pending_tasks if any
            # add tasks to table
            worst_case_idx += worst_case_period

        worst_case_idx-- # */

    FIFO pending_tasks;
    fifo_init(&pending_tasks);
    uint16_t worst_case_idx = 0;

    uint16_t aux = 0;
    for(uint16_t i = 0; i < hyper_period; i++){
        for(int j = 0; j < num_tasks; j++){
            if(i % task_table[j].period_ticks == 0){
                fifo_push(&pending_tasks, task_table[j].sem);
                aux++;
            }
        }
        if(worst_case_idx == 0 && fifo_is_empty(&pending_tasks)){
            struct k_sem value = *fifo_pop(&pending_tasks);
            tick_scheduler[i] = value;
            worst_case_idx = task_table[0].worst_case_execution_time;
        }
        worst_case_idx--;
    }
   
}

void stbs_add_task(k_tid_t task_id, uint32_t period_ticks, int priority){
    if(num_tasks >= MAX_TASKS){
        printk("Error: Maximum number of tasks reached\n");
        return;
    }
    if(num_tasks == NULL)
        num_tasks = 0;
    task_table[num_tasks] = (task_t){.period_ticks = period_ticks, .task_id = task_id, .worst_case_execution_time = 0, .priority = priority};
    num_tasks++;
}

uint16_t lcm(uint16_t a, uint16_t b){
    return a * b / gcd(a, b);
}

uint16_t gcd(uint16_t a, uint16_t b){
    if(b == 0)
        return a;
    return gcd(b, a % b);
}