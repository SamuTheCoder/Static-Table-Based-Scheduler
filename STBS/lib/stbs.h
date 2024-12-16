#pragma once

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/timing/timing.h>
#include <stdint.h>

#define MAX_TASKS 10
#define TICK_SCHEDULER_SIZE 25
#define TICK_HANDLER_PRIORITY 2

typedef struct {
    uint8_t task_id;
    uint16_t period_ticks;
    uint16_t worst_case_execution_time;
} task_t;

typedef struct {
    struct k_sem* semaphores[MAX_TASKS];
    uint8_t task_ids[MAX_TASKS];
} tick_manager_t;

extern uint16_t hyper_period;
extern uint16_t minor_cycle;
extern uint16_t num_ticks;

extern task_t task_table[MAX_TASKS];
extern uint32_t num_tasks;

// Table with tasks for each minor cycle
extern uint8_t tick_scheduler[100][MAX_TASKS];

extern tick_manager_t tick_manager;

extern struct k_thread tick_handler_thread;

extern k_tid_t tick_handler_tid;

extern struct k_sem tick_handler_sem;

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
 * @param semaphore Semaphore to be given when the task is ready to run
 */
void stbs_add_task(uint8_t task_id, uint32_t period_ticks, uint16_t worst_case_execution_time, struct k_sem* semaphore);

/**
 * @brief Function to create the tick scheduler
 */
void stbs_create_tick_scheduler(void);


/**
 * @brief Task code for the tick handler
 */
void tick_handler(void);

/**
 * @brief Task code for the tick handler
 */
void tick_handler_code(void *argA, void *argB, void *argC);


/**
 * @brief Calculates the least common multiple of two numbers
 */
uint16_t lcm(uint16_t a, uint16_t b);

/**
 * @brief Calculates the greatest common divisor of two numbers
 */
uint16_t gcd(uint16_t a, uint16_t b);






