#pragma once

#include <zephyr/kernel.h>         /* for kernel services */
#include <zephyr/device.h>          /* for device drivers */
#include <zephyr/drivers/gpio.h>            /* for GPIO access */
#include <zephyr/sys/printk.h>      /* for printk()*/
#include <zephyr/devicetree.h>      /* for devicetree access */

#define SW0_NODE DT_ALIAS(sw0)
#define SW1_NODE DT_ALIAS(sw1)
#define SW2_NODE DT_ALIAS(sw2)
#define SW3_NODE DT_ALIAS(sw3)

#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
#define LED3_NODE DT_ALIAS(led3)

typedef struct {
    bool btn_value_0;
    bool btn_value_1;
    bool btn_value_2;
    bool btn_value_3;
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

int btn_handler();

int initialize_buttons();

int led_handler();

int initialize_leds();