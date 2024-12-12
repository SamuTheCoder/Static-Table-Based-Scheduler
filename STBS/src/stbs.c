#include "../lib/stbs.h"

K_THREAD_STACK_DEFINE(tick_handler_stack, THREAD_STACK_SIZE);

k_tid_t tick_handler_tid = 1;
k_tid_t btn_handler_tid = 2;
k_tid_t led_handler_tid = 3;
k_tid_t uart_handler_tid = 4;

struct k_thread tick_handler_thread;

struct k_sem btn_handler_sem;
struct k_sem led_handler_sem;
struct k_sem uart_handler_sem;

uint16_t hyper_period;
uint16_t minor_cycle;

task_t task_table[MAX_TASKS];
uint32_t num_tasks;
task_t* tick_scheduler[100][MAX_TASKS];

void stbs_init(void){
    // Tasks: tick_handler, btn_handler, led_handler, uart_handler
    num_tasks = 0;

    // Initialize semaphores
    k_sem_init(&btn_handler_sem, 0, 1);
    k_sem_init(&led_handler_sem, 0, 1);
    k_sem_init(&uart_handler_sem, 0, 1);

    // Add tasks to task table
    stbs_add_task(btn_handler_tid, 100, BTN_HANDLER_PRIORITY, 10, &btn_handler_sem);
    stbs_add_task(led_handler_tid, 200, LED_HANDLER_PRIORITY, 10, &led_handler_sem);
    stbs_add_task(uart_handler_tid, 200, UART_HANDLER_PRIORITY, 10, &uart_handler_sem);
    
    // Initialize tick handler task (periodic, so it needs to be added to the task table)
    //tick_handler_tid = k_thread_create(&tick_handler_thread, tick_handler_stack, K_THREAD_STACK_SIZEOF(tick_handler_stack),
    //                tick_handler, NULL, NULL, NULL, 0, TICK_HANDLER_PRIORITY, K_NO_WAIT);

    struct k_sem tick_handler_sem;

    k_sem_init(&tick_handler_sem, 0, 1);


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
    
    // Sort tasks by period
    for (uint32_t i = 0; i < num_tasks - 1; i++) {
        for (uint32_t j = i + 1; j < num_tasks; j++) {
            if (task_table[i].period_ticks > task_table[j].period_ticks) {
                task_t temp = task_table[i];
                task_table[i] = task_table[j];
                task_table[j] = temp;
            }
        }
    }

    // Calculate hyper-period
    uint16_t hyper_period = 1;
    for (uint16_t i = 0; i < num_tasks; i++) {
        hyper_period = lcm(hyper_period, task_table[i].period_ticks);
    }

    // Calculate minor cycle
    uint32_t minor_cycle = task_table[0].period_ticks;
    for (uint32_t i = 1; i < num_tasks; i++) {
        minor_cycle = gcd(minor_cycle, task_table[i].period_ticks);
    }
    printk("Hyper period: %d\n", hyper_period);
    printk("Minor cycle: %d\n", minor_cycle);

    FIFO *deferred_tasks = malloc(sizeof(FIFO));
    fifo_init(deferred_tasks);

    
    for (uint16_t t = 0; t < hyper_period; t += minor_cycle) {
        printk("Iteration %d\n", t);
        uint16_t used_time = 0;

        // Schedule tasks in priority order
        for (int i = 0; i < num_tasks; i++) {
            if (t % task_table[i].period_ticks == 0) {
                printk("Task %d selected for tick %d\n", i, t);
                if (used_time + task_table[i].worst_case_execution_time <= minor_cycle) {
                    printk("yee\n");
                    tick_scheduler[t / minor_cycle][i] = &task_table[i];
                    used_time += task_table[i].worst_case_execution_time;
                } else {
                    printk("Task %d deferred\n", i);
                    fifo_push(deferred_tasks, *(task_table[i].sem)); // Still need FIFO to manage deferred tasks
                }
            }
        }

        // Print contents of deferred tasks
        printk("Deferred tasks: ");
        printk("Count: %d\n", deferred_tasks->count);
        for (int i = 0; i < deferred_tasks->count; i++) {
            printk("%d ", deferred_tasks->buffer[i]);
        } 

        // Attempt to reschedule deferred tasks
        FIFO *temp_queue = malloc(sizeof(FIFO));
        fifo_init(temp_queue);

        printk("Deferred tasks is empty: %d\n", fifo_is_empty(deferred_tasks));

        while (!fifo_is_empty(deferred_tasks)) {
            printk("There are deferred tasks\n");
            struct k_sem *deferred_task_sem;
            fifo_pop(deferred_tasks, deferred_task_sem);
            int task_idx = -1;

            // Find the task corresponding to the semaphore
            for (int j = 0; j < num_tasks; j++) {
                if (task_table[j].sem == deferred_task_sem) {
                    task_idx = j;
                    break;
                }
            }

            if (task_idx >= 0 && used_time + task_table[task_idx].worst_case_execution_time <= minor_cycle) {
                tick_scheduler[t / minor_cycle][task_idx] = &task_table[task_idx];
                used_time += task_table[task_idx].worst_case_execution_time;
            } else {
                fifo_push(temp_queue, *deferred_task_sem);
            }
        }

        printk("Temp queue is empty: %d\n", fifo_is_empty(temp_queue));

        // Move remaining deferred tasks back
        while (!fifo_is_empty(temp_queue)) {
            struct k_sem *deferred_task_sem;
            fifo_pop(temp_queue, deferred_task_sem);
            fifo_push(deferred_tasks, *deferred_task_sem);
        }
    }

    while(1){
        //Print scheduling table for debugging
        for (int i = 0; i < hyper_period / minor_cycle; i++) {
            for (int j = 0; j < num_tasks; j++) {
                printk("Task %d at tick %d: %d ", j, i, tick_scheduler[i][j]->task_id);
            }
            printk("\n");
        }
        printk("aaagsgfdhfd\n");
        printk("%d\n", hyper_period/minor_cycle); //0
        printk("%d\n", minor_cycle); //100
        printk("%d\n", hyper_period); //0
        k_msleep(1000);
    }
    
   
}

void stbs_add_task(k_tid_t task_id, uint32_t period_ticks, int priority, uint16_t worst_case_execution_time, struct k_sem *sem){
    if(num_tasks >= MAX_TASKS){
        printk("Error: Maximum number of tasks reached\n");
        return;
    }
    task_table[num_tasks] = (task_t){.period_ticks = period_ticks, .task_id = task_id, 
        .worst_case_execution_time = worst_case_execution_time, .priority = priority, .sem = sem};
    num_tasks++;
}

uint16_t lcm(uint16_t a, uint16_t b){
    //printk("a and b: %d %d\n", a, b);
    //printk("a*b: %d\n", a*b);
    //printk("gcd: %d\n", gcd(a, b));
    //printk("Res of lcm: %d\n", (a * b) / gcd(a, b));
    return (a * b) / gcd(a, b);
}

uint16_t gcd(uint16_t a, uint16_t b){
    if(b == 0)
        return a;
    return gcd(b, a % b);
}