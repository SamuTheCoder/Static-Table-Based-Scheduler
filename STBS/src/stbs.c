#include "../lib/stbs.h"

uint16_t hyper_period;
uint16_t minor_cycle;
uint16_t num_ticks;

uint16_t tick_count = 0;


task_t task_table[MAX_TASKS];
uint32_t num_tasks = 0;
uint8_t tick_scheduler[100][MAX_TASKS];

tick_manager_t tick_manager;

struct k_thread tick_handler_thread;
k_tid_t tick_handler_tid;

struct k_sem tick_handler_sem;

K_THREAD_STACK_DEFINE(tick_handler_stack, 1024);

void tick_handler()
{
    //At each tick, check what tasks are ready to run
    for(int i = 0; i < num_tasks; i++){
        if(tick_scheduler[tick_count][i] == tick_manager.task_ids[0]){
            k_sem_give(tick_manager.semaphores[0]);
        }
        else if(tick_scheduler[tick_count][i] == tick_manager.task_ids[1]){
            k_sem_give(tick_manager.semaphores[1]);
        }
        else if(tick_scheduler[tick_count][i] == tick_manager.task_ids[2]){
            k_sem_give(tick_manager.semaphores[2]);
        }
        else if(tick_scheduler[tick_count][i] == tick_manager.task_ids[3]){
            k_sem_give(tick_manager.semaphores[3]);
        }
        else if(tick_scheduler[tick_count][i] == tick_manager.task_ids[4]){
            k_sem_give(tick_manager.semaphores[4]);
        }
        else if(tick_scheduler[tick_count][i] == tick_manager.task_ids[5]){
            k_sem_give(tick_manager.semaphores[5]);
        }
        else if(tick_scheduler[tick_count][i] == tick_manager.task_ids[6]){
            k_sem_give(tick_manager.semaphores[6]);
        }
        else if(tick_scheduler[tick_count][i] == tick_manager.task_ids[7]){
            k_sem_give(tick_manager.semaphores[7]);
        }
        else if(tick_scheduler[tick_count][i] == tick_manager.task_ids[8]){
            k_sem_give(tick_manager.semaphores[8]);
        }
        else if(tick_scheduler[tick_count][i] == tick_manager.task_ids[9]){
            k_sem_give(tick_manager.semaphores[9]);
        }
    }
    tick_count++;
    if(tick_count == num_ticks){
        tick_count = 0;
    }
}

void tick_thread_code(void *argA , void *argB, void *argC)
{
    /* Timing variables to control task periodicity */
    int64_t fin_time=0, release_time=0;
    
    /* Variables to time execution */
    timing_t start_time, end_time;
    uint64_t total_cycles=0;
    uint64_t total_ns=0;

    printk("Tick Thread init (periodic)\n");

    /* Compute next release instant */
    printk("Minor cycle at tick thread: %d\n", minor_cycle);
    release_time = k_uptime_get() + minor_cycle;

    /* Thread loop */
    while(1) {
        if(k_sem_take(&tick_handler_sem, K_FOREVER) == 0){
            /* Do the workload */  
            start_time = timing_counter_get(); 
            //printk("Tick thread instance released at time: %lld (ms). Last instance exec time was %lld (ns) \n",k_uptime_get(),total_ns);  
            tick_handler();

            end_time = timing_counter_get();

            total_cycles = timing_cycles_get(&start_time, &end_time);
            total_ns = timing_cycles_to_ns(total_cycles);
            //printk("Tick handler took %lld (ns) to execute\n", total_ns);
        
            /* Wait for next release instant */ 
            fin_time = k_uptime_get();
            if( fin_time < release_time) {
                k_msleep(release_time - fin_time); /* There are other variants, k_sleep(), k_usleep(), ... */
                release_time += minor_cycle;

            }
            k_sem_give(&tick_handler_sem);
        }
    }

    /* Stop timing functions */
    timing_stop();
}

void stbs_init(void){
    k_sem_init(&tick_handler_sem, 0, 1);
    
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
    hyper_period = 1;
    for (uint16_t i = 0; i < num_tasks; i++) {
        hyper_period = lcm(hyper_period, task_table[i].period_ticks);
    }

    // Calculate minor cycle
    minor_cycle = task_table[0].period_ticks;
    for (uint32_t i = 1; i < num_tasks; i++) {
        minor_cycle = gcd(minor_cycle, task_table[i].period_ticks);
    }

    printk("Hyper period: %d\n", hyper_period);
    printk("Minor cycle: %d\n", minor_cycle);

    num_ticks = hyper_period / minor_cycle;

    tick_handler_tid = k_thread_create(&tick_handler_thread, tick_handler_stack, 1024, tick_thread_code, NULL, NULL, NULL, TICK_HANDLER_PRIORITY, 0, K_NO_WAIT);
}

void stbs_add_task(uint8_t task_id, uint32_t period_ticks, uint16_t worst_case_execution_time, struct k_sem* semaphore){
    if(num_tasks >= MAX_TASKS){
        printk("Error: Maximum number of tasks reached\n");
        return;
    }
    task_table[num_tasks] = (task_t){.period_ticks = period_ticks, .task_id = task_id, 
        .worst_case_execution_time = worst_case_execution_time};

    tick_manager.task_ids[num_tasks] = task_id;
    tick_manager.semaphores[num_tasks] = semaphore;

    num_tasks++;
}

void stbs_create_tick_scheduler(void){
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
    printk("---------------------------------------------------------\n");
}


void stbs_start(void){
    //Start tick thread

    stbs_create_tick_scheduler();
    timing_init();
    timing_start();
    k_sem_give(&tick_handler_sem);
}

void stbs_stop(void){
    //Stop tick thread
    k_sem_take(&tick_handler_sem, K_FOREVER);
}

uint16_t lcm(uint16_t a, uint16_t b){
    return (a * b) / gcd(a, b);
}

uint16_t gcd(uint16_t a, uint16_t b){
    if(b == 0)
        return a;
    return gcd(b, a % b);
}

