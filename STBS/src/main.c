#include <zephyr/kernel.h>          /* for kernel functions*/
#include <zephyr/sys/printk.h>      /* for printk()*/
#include <zephyr/timing/timing.h>   /* for timing services */
#include "../lib/tasks.h"

#define STACK_SIZE 1024
#define thread_A_prio 2
#define thread_B_prio 2
#define thread_C_prio 2
#define TICK_HANDLER_PRIORITY 2
#define thread_A_period 3000

K_THREAD_STACK_DEFINE(thread_A_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(thread_B_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(thread_C_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(tick_handler_stack, STACK_SIZE);

struct k_thread thread_A_data;
struct k_thread thread_B_data;
struct k_thread thread_C_data;
struct k_thread tick_handler_thread;

void thread_A_code(void *argA, void *argB, void *argC);
void thread_B_code(void *argA, void *argB, void *argC);
void thread_C_code(void *argA , void *argB, void *argC);
void tick_handler();
void tick_thread_code(void *argA , void *argB, void *argC);


k_tid_t thread_A_tid;
k_tid_t thread_B_tid;
k_tid_t thread_C_tid;
k_tid_t tick_handler_tid;


struct k_sem btn_sem;
struct k_sem led_sem;
struct k_sem print_rtdb_sem;

uint16_t tick_count = 0;

/* Main function */
int main(void) {

    k_msleep(3000);


    stbs_add_task(PRINT_RTDB_ID, 1000, 15);
    stbs_add_task(BTN_HANDLER_ID, 100, 5);
    stbs_add_task(LED_HANDLER_ID, 50, 5);
    stbs_init();

    k_sem_init(&btn_sem, 0, 1);
    k_sem_init(&led_sem, 0, 1);
    k_sem_init(&print_rtdb_sem, 0, 1);

    k_sem_take(&btn_sem, K_NO_WAIT);
    k_sem_take(&led_sem, K_NO_WAIT);
    k_sem_take(&print_rtdb_sem, K_NO_WAIT);

    /* Start threads */
    thread_A_tid = k_thread_create(&thread_A_data, thread_A_stack, STACK_SIZE, thread_A_code, NULL, NULL, NULL, thread_A_prio, 0, K_NO_WAIT);
    thread_B_tid = k_thread_create(&thread_B_data, thread_B_stack, STACK_SIZE, thread_B_code, NULL, NULL, NULL, thread_B_prio, 0, K_NO_WAIT);
    thread_C_tid = k_thread_create(&thread_C_data, thread_C_stack, STACK_SIZE, thread_C_code, NULL, NULL, NULL, thread_C_prio, 0, K_NO_WAIT);
    tick_handler_tid = k_thread_create(&tick_handler_thread, tick_handler_stack, STACK_SIZE, tick_thread_code, NULL, NULL, NULL, TICK_HANDLER_PRIORITY, 0, K_NO_WAIT);


} 

/* Thread code implementation */
void thread_A_code(void *argA , void *argB, void *argC)
{ 
    
    /* Variables to time execution */
    timing_t start_time, end_time;
    uint64_t total_cycles=0;
    uint64_t total_ns=0;
    printk("Thread A init (periodic)\n");
    /* Compute next release instant */
    int ret = initialize_leds();
    if(ret < 0){
        printk("Error initializing leds\n");
    }

    /* Thread loop */
    while(1) {

        if(k_sem_take(&led_sem, K_FOREVER) == 0){
            /* Do the workload */  
            start_time = timing_counter_get(); 
            
            int ret = led_handler();
            if(ret < 0){
                printk("Error handling leds\n");
            }
            end_time = timing_counter_get();
            total_cycles = timing_cycles_get(&start_time, &end_time);
            total_ns = timing_cycles_to_ns(total_cycles);
            printk("Thread A took %lld (ns) to execute\n", total_ns);
            
        }    
    }
    /* Stop timing functions */
    timing_stop();
}

/* Thread code implementation */
void thread_B_code(void *argA , void *argB, void *argC)
{ 
    
    /* Variables to time execution */
    timing_t start_time, end_time;
    uint64_t total_cycles=0;
    uint64_t total_ns=0;
    printk("Thread B init (periodic)\n");
    /* Compute next release instant */
    int ret = initialize_buttons();
    if(ret < 0){
        printk("Error initializing buttons\n");
    }

    /* Thread loop */
    while(1) {

        if(k_sem_take(&btn_sem, K_FOREVER) == 0){
            /* Do the workload */  
            start_time = timing_counter_get(); 
            
            int ret = btn_handler();
            if(ret < 0){
                printk("Error handling buttons\n");
            }
            end_time = timing_counter_get();
            total_cycles = timing_cycles_get(&start_time, &end_time);
            total_ns = timing_cycles_to_ns(total_cycles);
            printk("Thread B took %lld (ns) to execute\n", total_ns);
        }    
    }
    /* Stop timing functions */
    timing_stop();
}

/* Thread code implementation */
void thread_C_code(void *argA , void *argB, void *argC)
{ 
    
    /* Variables to time execution */
    timing_t start_time, end_time;
    uint64_t total_cycles=0;
    uint64_t total_ns=0;
    printk("Thread C init (periodic)\n");

    /* Thread loop */
    while(1) {

        if(k_sem_take(&print_rtdb_sem, K_FOREVER) == 0){
            /* Do the workload */  
            start_time = timing_counter_get(); 
            
            print_rtdb();
            end_time = timing_counter_get();
            total_cycles = timing_cycles_get(&start_time, &end_time);
            total_ns = timing_cycles_to_ns(total_cycles);
            printk("Thread C took %lld (ns) to execute\n", total_ns);
            
        }    
    }
    /* Stop timing functions */
    timing_stop();
}

void tick_handler()
{
    printk("Tick goes on\n");
    //At each tick, check what tasks are ready to run
    for(int i = 0; i < num_tasks; i++){
        switch(tick_scheduler[tick_count][i]){
            case BTN_HANDLER_ID:
                printk("Button handler ready\n");
                k_sem_give(&btn_sem);
                break;
            case LED_HANDLER_ID:
                printk("LED handler ready\n");
                k_sem_give(&led_sem);
                break;
            case PRINT_RTDB_ID:
                printk("Print RTDB ready\n");
                k_sem_give(&print_rtdb_sem);
                break;
            default:
                break;
        }
    }
    tick_count++;
    if(tick_count == num_ticks){
        tick_count = 0;
    }
}

/* Thread code implementation */
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
    release_time = k_uptime_get() + thread_A_period;

    /* Thread loop */
    while(1) {
        
        /* Do the workload */  
        start_time = timing_counter_get(); 
        
        tick_handler();

        end_time = timing_counter_get();

        total_cycles = timing_cycles_get(&start_time, &end_time);
        total_ns = timing_cycles_to_ns(total_cycles);
       
        /* Wait for next release instant */ 
        fin_time = k_uptime_get();
        if( fin_time < release_time) {
            k_msleep(release_time - fin_time); /* There are other variants, k_sleep(), k_usleep(), ... */
            release_time += minor_cycle;

        }
    }

    /* Stop timing functions */
    timing_stop();
}


