#pragma once

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <stdint.h>
#include "../lib/fifo.h"

#define MAX_TASKS 10

#define TICK_HANDLER_PRIORITY 1
#define BTN_HANDLER_PRIORITY 2
#define LED_HANDLER_PRIORITY 2
#define UART_HANDLER_PRIORITY 2

#define THREAD_STACK_SIZE 1024
#define TICK_SCHEDULER_SIZE 25

extern k_tid_t tick_handler_tid;
extern k_tid_t btn_handler_tid;
extern k_tid_t led_handler_tid;
extern k_tid_t uart_handler_tid;

extern uint16_t hyper_period;
extern uint16_t minor_cycle;

// Semaphores for each task
extern struct k_sem btn_handler_sem;
extern struct k_sem led_handler_sem;
extern struct k_sem uart_handler_sem;


extern task_t task_table[MAX_TASKS];
extern uint32_t num_tasks;

extern struct k_thread tick_handler_thread;

// Table with tasks for each minor cycle
extern task_t* tick_scheduler[100][MAX_TASKS];

/**
 * @brief Initializes the STBS system, including creating eventual system tasks, initializing variables, etc.
 * 
 */
void stbs_init(void);


/**
 * @brief Starts the STBS system
 * 
 */
void stbs_start(void);

/**
 * @brief Stops the STBS system
 * 
 */
void stbs_stop(void);

/**
 * @brief Adds a task to the STBS scheduler, with a period of period_ticks. task_id is a suitable identifier 
 * 
 * @param task_id Identifier of the task
 * @param period_ticks Period of the task in ticks
 * @param priority Priority of the task
 */
void stbs_add_task(k_tid_t task_id, uint32_t period_ticks, int priority, uint16_t worst_case_execution_time, struct k_sem *sem);

/**
 * @brief Removes a task identified by task_id from the table
 * 
 * @param task_id Identifier of the task
 */
void stbs_remove_task(int task_id);

/**
 * @brief Used inside a task body, terminates a task’s job and makes the task wait for its next activation, triggered by the STBS scheduler
 * 
 */
void stbs_wait_for_next_period(void);

/**
 * @brief Handler for the system tick
 * 
 */
void tick_handler(void* arg1, void* arg2, void* arg3);

/**
 * @brief Calculates the least common multiple of two numbers
 */
uint16_t lcm(uint16_t a, uint16_t b);

/**
 * @brief Calculates the greatest common divisor of two numbers
 */
uint16_t gcd(uint16_t a, uint16_t b);





