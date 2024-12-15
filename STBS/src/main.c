#include <zephyr/kernel.h>          /* for kernel functions*/
#include <zephyr/sys/printk.h>      /* for printk()*/
#include <zephyr/timing/timing.h>   /* for timing services */
#include "../lib/tasks.h"

#define STACK_SIZE 1024
#define thread_A_prio 1
#define thread_B_prio 1
#define thread_C_prio 1
#define thread_A_period 3000

K_THREAD_STACK_DEFINE(led_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(btn_stack, STACK_SIZE);
K_THREAD_STACK_DEFINE(rtdb_stack, STACK_SIZE);

struct k_thread led_data;
struct k_thread btn_data;
struct k_thread rtdb_data;

void led_thread_code(void *argA, void *argB, void *argC);
void btn_thread_code(void *argA, void *argB, void *argC);
void rtdb_thread_code(void *argA , void *argB, void *argC);
void tick_handler();
void tick_thread_code(void *argA , void *argB, void *argC);


k_tid_t led_tid;
k_tid_t btn_tid;
k_tid_t rtdb_tid;


struct k_sem btn_sem;
struct k_sem led_sem;
struct k_sem print_rtdb_sem;

/* Main function */
int main(void) {

    k_msleep(3000);

    k_sem_init(&btn_sem, 0, 1);
    k_sem_init(&led_sem, 0, 1);
    k_sem_init(&print_rtdb_sem, 0, 1);

    k_sem_take(&btn_sem, K_NO_WAIT);
    k_sem_take(&led_sem, K_NO_WAIT);
    k_sem_take(&print_rtdb_sem, K_NO_WAIT);

    stbs_add_task(PRINT_RTDB_ID, 1000, 15, &print_rtdb_sem);
    stbs_add_task(BTN_HANDLER_ID, 200, 5, &btn_sem);
    stbs_add_task(LED_HANDLER_ID, 20, 5, &led_sem);
    stbs_init();

    stbs_start();

    timing_init();
    timing_start();

    /* Start threads */
    led_tid = k_thread_create(&led_data, led_stack, STACK_SIZE, led_thread_code, NULL, NULL, NULL, thread_A_prio, 0, K_NO_WAIT);
    btn_tid = k_thread_create(&btn_data, btn_stack, STACK_SIZE, btn_thread_code, NULL, NULL, NULL, thread_B_prio, 0, K_NO_WAIT);
    rtdb_tid = k_thread_create(&rtdb_data, rtdb_stack, STACK_SIZE, rtdb_thread_code, NULL, NULL, NULL, thread_C_prio, 0, K_NO_WAIT);

    return;
} 

/* Thread code implementation */
void led_thread_code(void *argA , void *argB, void *argC)
{ 
    
    /* Variables to time execution */
    timing_t start_time, end_time;
    uint64_t total_cycles=0;
    uint64_t total_ns=0;
    printk("Led Thread init (periodic)\n");
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
            //printk("Led Thread took %lld (ns) to execute\n", total_ns);
            
        }    
    }
    /* Stop timing functions */
    timing_stop();
}

/* Thread code implementation */
void btn_thread_code(void *argA , void *argB, void *argC)
{ 
    
    /* Variables to time execution */
    timing_t start_time, end_time;
    uint64_t total_cycles=0;
    uint64_t total_ns=0;
    printk("Button thread init (periodic)\n");
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
            //printk("Button thread took %lld (ns) to execute\n", total_ns);
        }    
    }
    /* Stop timing functions */
    timing_stop();
}

/* Thread code implementation */
void rtdb_thread_code(void *argA , void *argB, void *argC)
{ 
    
    /* Variables to time execution */
    timing_t start_time, end_time;
    uint64_t total_cycles=0;
    uint64_t total_ns=0;
    printk("Print RTDB thread init (periodic)\n");

    /* Thread loop */
    while(1) {

        if(k_sem_take(&print_rtdb_sem, K_FOREVER) == 0){
            /* Do the workload */  
            start_time = timing_counter_get(); 
            
            print_rtdb();
            end_time = timing_counter_get();
            total_cycles = timing_cycles_get(&start_time, &end_time);
            total_ns = timing_cycles_to_ns(total_cycles);
            //printk("Print RTDB thread took %lld (ns) to execute\n", total_ns);
            
        }    
    }
    /* Stop timing functions */
    timing_stop();
}


