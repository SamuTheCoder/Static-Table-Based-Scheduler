#pragma once

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <stdint.h>

#define MAX_TASKS 10
#define TICK_SCHEDULER_SIZE 25

typedef struct {
    uint8_t task_id;
    uint16_t period_ticks;
    uint16_t worst_case_execution_time;
} task_t;

extern uint16_t hyper_period;
extern uint16_t minor_cycle;
extern uint16_t num_ticks;

extern task_t task_table[MAX_TASKS];
extern uint32_t num_tasks;

// Table with tasks for each minor cycle
extern uint8_t tick_scheduler[100][MAX_TASKS];

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
void stbs_add_task(uint8_t task_id, uint32_t period_ticks, uint16_t worst_case_execution_time);

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
 * @brief Calculates the least common multiple of two numbers
 */
uint16_t lcm(uint16_t a, uint16_t b);

/**
 * @brief Calculates the greatest common divisor of two numbers
 */
uint16_t gcd(uint16_t a, uint16_t b);

/**
 * @brief Get the current minor cycle
 */
uint16_t get_minor_cycle(void);





