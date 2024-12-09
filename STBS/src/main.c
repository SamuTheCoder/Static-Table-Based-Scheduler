#include <zephyr/kernel.h>          /* for kernel functions*/
#include <zephyr/sys/printk.h>      /* for printk()*/
#include <zephyr/timing/timing.h>   /* for timing services */
#include "../lib/stbs.h"
#include "../lib/tasks.h"

/* Size of stack area used by each thread (can be thread specific, if necessary)*/
#define STACK_SIZE 1024

/* Thread scheduling priority */
#define thread_A_prio 1
#define thread_B_prio 1
#define thread_C_prio 1

/* Thread periodicity (in ms)*/
#define thread_A_period 1000
#define thread_B_period 2000
#define thread_C_period 4000

/* Create thread stack space */
K_THREAD_STACK_DEFINE(thread_A_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(thread_B_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(thread_C_stack, STACK_SIZE);
  

/* Create variables for thread data */
struct k_thread thread_A_data;
struct k_thread thread_B_data;
struct k_thread thread_C_data;

/* Create task IDs */
k_tid_t thread_A_tid;
k_tid_t thread_B_tid;
k_tid_t thread_C_tid;

/* Thread code prototypes */
void thread_A_code(void *argA, void *argB, void *argC);
void thread_B_code(void *argA, void *argB, void *argC);
void thread_C_code(void *argA, void *argB, void *argC);


/* Main function */
int main(void) {


    /* Start timing functions */
    timing_init();
    timing_start();

    thread_A_tid = k_thread_create(&thread_A_data, thread_A_stack,
        K_THREAD_STACK_SIZEOF(thread_A_stack), thread_A_code,
        NULL, NULL, NULL, thread_A_prio, 0, K_NO_WAIT);

    thread_B_tid = k_thread_create(&thread_B_data, thread_B_stack,
        K_THREAD_STACK_SIZEOF(thread_B_stack), thread_B_code,
        NULL, NULL, NULL, thread_B_prio, 0, K_NO_WAIT);

    thread_B_tid = k_thread_create(&thread_C_data, thread_C_stack,
        K_THREAD_STACK_SIZEOF(thread_C_stack), thread_C_code,
        NULL, NULL, NULL, thread_C_prio, 0, K_NO_WAIT);

    return -1;

} 

/* Thread code implementation */
void thread_A_code(void *argA , void *argB, void *argC)
{ 
    /* Timing variables to control task periodicity */
    int64_t fin_time=0, release_time=0;
    
    /* Variables to time execution */
    timing_t start_time, end_time;
    uint64_t total_cycles=0;
    uint64_t total_ns=0;

    printk("Thread A init (periodic)\n");

    /* Compute next release instant */
    release_time = k_uptime_get() + thread_A_period;

    int ret = initialize_buttons();

    /* Thread loop */
    while(1) {
        
        /* Do the workload */  
        start_time = timing_counter_get(); 
        
        int ret = btn_handler();

        end_time = timing_counter_get();

        total_cycles = timing_cycles_get(&start_time, &end_time);
        total_ns = timing_cycles_to_ns(total_cycles);
        printk("Thread A took %lld (ns) to execute\n", total_ns);
       
        /* Wait for next release instant */ 
        fin_time = k_uptime_get();
        if( fin_time < release_time) {
            k_msleep(release_time - fin_time); /* There are other variants, k_sleep(), k_usleep(), ... */
            release_time += thread_A_period;

        }
    }

    /* Stop timing functions */
    timing_stop();
}

void thread_B_code(void *argA , void *argB, void *argC)
{
  
    /* Timing variables to control task periodicity */
    int64_t fin_time=0, release_time=0;
    
    /* Variables to time execution */
    timing_t start_time, end_time;
    uint64_t total_cycles=0;
    uint64_t total_ns=0;

    printk("Thread B init (periodic)\n");

    /* Compute next release instant */
    release_time = k_uptime_get() + thread_A_period;

    int ret = initialize_leds();
    if(ret < 0) {
        printk("Error initializing LEDs\n");
        return;
    }

    /* Thread loop */
    while(1) {
        
        /* Do the workload */  
        start_time = timing_counter_get(); 
        
        int ret = led_handler();
        if(ret < 0) {
            printk("Error handling LEDs\n");
            return;
        }

        end_time = timing_counter_get();

        total_cycles = timing_cycles_get(&start_time, &end_time);
        total_ns = timing_cycles_to_ns(total_cycles);
        printk("Thread B took %lld (ns) to execute\n", total_ns);
       
        /* Wait for next release instant */ 
        fin_time = k_uptime_get();
        if( fin_time < release_time) {
            k_msleep(release_time - fin_time); /* There are other variants, k_sleep(), k_usleep(), ... */
            release_time += thread_A_period;

        }
    }

    /* Stop timing functions */
    timing_stop();
}

void thread_C_code(void *argA , void *argB, void *argC)
{
  
    /* Timing variables to control task periodicity */
    int64_t fin_time=0, release_time=0;
    
    /* Variables to time execution */
    timing_t start_time, end_time;
    uint64_t total_cycles=0;
    uint64_t total_ns=0;

    printk("Thread C init (periodic)\n");

    /* Compute next release instant */
    release_time = k_uptime_get() + thread_A_period;

    /* Thread loop */
    while(1) {
        
        /* Do the workload */  
        start_time = timing_counter_get(); 
        {
            int i=0, j=1;
            for (i=0;i<10000;i++)
                j+=((float)i/3);
        }
        end_time = timing_counter_get();

        total_cycles = timing_cycles_get(&start_time, &end_time);
        total_ns = timing_cycles_to_ns(total_cycles);
        printk("Thread C took %lld (ns) to execute\n", total_ns);

       
        /* Wait for next release instant */ 
        fin_time = k_uptime_get();
        if( fin_time < release_time) {
            k_msleep(release_time - fin_time); /* There are other variants, k_sleep(), k_usleep(), ... */
            release_time += thread_A_period;

        }
    }

    /* Stop timing functions */
    timing_stop();
}

