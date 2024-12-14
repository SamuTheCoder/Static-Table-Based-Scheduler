#include "../lib/stbs.h"

uint16_t hyper_period;
uint16_t minor_cycle;
uint16_t num_ticks;

task_t task_table[MAX_TASKS];
uint32_t num_tasks = 0;
uint8_t tick_scheduler[100][MAX_TASKS];

void stbs_init(void){
    // Tasks: tick_handler, btn_handler, led_handler, uart_handler


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

    num_ticks = hyper_period / minor_cycle;

    task_t postponed_tasks[MAX_TASKS];
    uint8_t num_postponed_tasks = 0;

    
    for (uint16_t t = 0; t < hyper_period; t += minor_cycle) {
        printk("Iteration %d\n", t);
        uint16_t used_time = 0;

        // Schedule tasks in priority order
        for (int i = 0; i < num_tasks; i++) {
            if (t % task_table[i].period_ticks == 0) {
                printk("Task %d selected for tick %d\n", i, t);
                if (used_time + task_table[i].worst_case_execution_time <= minor_cycle) {
                    printk("Task %d scheduled\n", i);
                    tick_scheduler[t / minor_cycle][i] = task_table[i].task_id;
                    used_time += task_table[i].worst_case_execution_time;
                } else {
                    printk("Task %d deferred\n", i);
                    tick_scheduler[t / minor_cycle][i] = 255;
                    postponed_tasks[num_postponed_tasks] = task_table[i];
                    num_postponed_tasks++;
                }
            }
            else {          
                tick_scheduler[t / minor_cycle][i] =  255;
            }
        }

        // Print contents of deferred tasks
        printk("Postponed tasks: \n");
        for (int i = 0; i < num_postponed_tasks; i++) {
            printk("%d \n", postponed_tasks[i].task_id);
        }

        // Attempt to reschedule deferred tasks
        task_t temp_task_table[MAX_TASKS];
        uint8_t temp_num_tasks = 0;

        
        for(int i = 0; i < num_postponed_tasks; i++){
            printk("There are deferred tasks\n");
            int task_idx = -1;

            for (int j = 0; j < num_tasks; j++) {
                if (task_table[j].task_id == postponed_tasks[i].task_id) {
                    printk("Task %d found with id %d\n", j, task_table[j].task_id);
                    task_idx = j;
                    break;
                }
            }

            if (task_idx >= 0 && used_time + task_table[task_idx].worst_case_execution_time <= minor_cycle) {
                tick_scheduler[t / minor_cycle][task_idx] = task_table[task_idx].task_id;
                used_time += task_table[task_idx].worst_case_execution_time;
            } else {
                printk("Task %d deferred again\n", task_idx);
                temp_task_table[temp_num_tasks] = task_table[task_idx];
                temp_num_tasks++;
            }
        }

        //printk("Temp queue is empty: %d\n", fifo_is_empty(temp_queue));

        // Move remaining deferred tasks back
        if(temp_num_tasks > 0){
            for (int i = 0; i < temp_num_tasks; i++) {
                postponed_tasks[i] = temp_task_table[i];
            }
            num_postponed_tasks = temp_num_tasks;
        }
        else{
            num_postponed_tasks = 0;
        }
        temp_num_tasks = 0;
    }
    printk("---------------------------------------------------------\n");
    /*
    while(1){
        //Print scheduling table for debugging
        printk("%d\n", hyper_period/minor_cycle); //0
        printk("%d\n", minor_cycle); //100
        printk("%d\n", hyper_period); //0
        for (int i = 0; i < hyper_period / minor_cycle; i++) {
            for (int j = 0; j < num_tasks; j++) {
                if(tick_scheduler[i][j] < 255){
                    printk("Task %d at tick %d\n", tick_scheduler[i][j], i);
                }          
            }  
            k_msleep(1000);        
        }
    }
    */
   
}

void stbs_add_task(uint8_t task_id, uint32_t period_ticks, uint16_t worst_case_execution_time){
    if(num_tasks >= MAX_TASKS){
        printk("Error: Maximum number of tasks reached\n");
        return;
    }
    task_table[num_tasks] = (task_t){.period_ticks = period_ticks, .task_id = task_id, 
        .worst_case_execution_time = worst_case_execution_time};
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