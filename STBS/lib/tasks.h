/**
 * @file tasks.h
 * @brief Header file for the use case tasks implemented
 */

#pragma once

#include <zephyr/kernel.h>        
#include <zephyr/device.h>          
#include <zephyr/drivers/gpio.h>           
#include <zephyr/sys/printk.h>      
#include <zephyr/devicetree.h>      
#include "./stbs.h"

// Define the nodes for the buttons and LEDs
#define SW0_NODE DT_ALIAS(sw0)
#define SW1_NODE DT_ALIAS(sw1)
#define SW2_NODE DT_ALIAS(sw2)
#define SW3_NODE DT_ALIAS(sw3)

#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
#define LED3_NODE DT_ALIAS(led3)

#define LED_HANDLER_ID 1
#define BTN_HANDLER_ID 2
#define PRINT_RTDB_ID 3

#define LED_HANDLER_PRIORITY 2
#define BTN_HANDLER_PRIORITY 2
#define PRINT_RTDB_PRIORITY 2


typedef struct {
    bool btn_value_0;
    bool btn_value_1;
    bool btn_value_2;
    bool btn_value_3;
    int n_execs_btn_handler;
    int n_execs_led_handler;
    int n_execs_print_rtdb;
} rtdb_t;

extern rtdb_t rtdb;

static const struct gpio_dt_spec button_1 = GPIO_DT_SPEC_GET(SW0_NODE, gpios);
static const struct gpio_dt_spec button_2 = GPIO_DT_SPEC_GET(SW1_NODE, gpios);
static const struct gpio_dt_spec button_3 = GPIO_DT_SPEC_GET(SW2_NODE, gpios);
static const struct gpio_dt_spec button_4 = GPIO_DT_SPEC_GET(SW3_NODE, gpios);

static const struct gpio_dt_spec led_1 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
static const struct gpio_dt_spec led_2 = GPIO_DT_SPEC_GET(LED1_NODE, gpios);
static const struct gpio_dt_spec led_3 = GPIO_DT_SPEC_GET(LED2_NODE, gpios);
static const struct gpio_dt_spec led_4 = GPIO_DT_SPEC_GET(LED3_NODE, gpios);

extern struct k_sem btn_sem;
extern struct k_sem led_sem;
extern struct k_sem print_rtdb_sem;

/**
 * @brief Handles button events
 */
int btn_handler();

/**
 * @brief Initializes buttons
 */
int initialize_buttons();

/**
 * @brief Handles LED events
 */
int led_handler();

/**
 * @brief Initializes LEDs
 */
int initialize_leds();

/**
 * @brief Prints the RTDB values
 */
void print_rtdb();
