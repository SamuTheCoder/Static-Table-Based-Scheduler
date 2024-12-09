#include "../lib/tasks.h"

rtdb_t rtdb = {1, 1, 1, 1};

int btn_handler()
{
    //Button 1
    rtdb.btn_value_0 = gpio_pin_get_dt(&button_1);

    //Button 2
    rtdb.btn_value_1 = gpio_pin_get_dt(&button_2);

    //Button 3
    rtdb.btn_value_2 = gpio_pin_get_dt(&button_3);

    //Button 4
    rtdb.btn_value_3 = gpio_pin_get_dt(&button_4);
    
    

    return 0;
}

int initialize_buttons()
{
    int ret;

    //Button 1
    if(!device_is_ready(button_1.port)) {
        printk("Device %s is not ready\n", button_1.port->name);
        return -1;
    }

    ret = gpio_pin_configure_dt(&button_1, GPIO_INPUT);
    if (ret < 0) {
        return -1;
    }

    //Button 2
    if(!device_is_ready(button_2.port)) {
        printk("Device %s is not ready\n", button_2.port->name);
        return -1;
    }

    ret = gpio_pin_configure_dt(&button_2, GPIO_INPUT);
    if (ret < 0) {
        return -1;
    }

    //Button 3
    if(!device_is_ready(button_3.port)) {
        printk("Device %s is not ready\n", button_3.port->name);
        return -1;
    }

    ret = gpio_pin_configure_dt(&button_3, GPIO_INPUT);
    if (ret < 0) {
        return -1;
    }

    //Button 4
    if(!device_is_ready(button_4.port)) {
        printk("Device %s is not ready\n", button_4.port->name);
        return -1;
    }

    ret = gpio_pin_configure_dt(&button_4, GPIO_INPUT);
    if (ret < 0) {
        return -1;
    }

    return 0;
}

int led_handler()
{
    //LED 1
    gpio_pin_set_dt(&led_1, rtdb.btn_value_0);

    k_msleep(100);

    //LED 2
    gpio_pin_set_dt(&led_2, rtdb.btn_value_1);

    k_msleep(20);

    //LED 3
    gpio_pin_set_dt(&led_3, rtdb.btn_value_2);

    k_msleep(20);

    //LED 4
    gpio_pin_set_dt(&led_4, rtdb.btn_value_3);

    k_msleep(20);

    return 0;
}

int initialize_leds(){
    int ret;

    //LED 1
    if(!device_is_ready(led_1.port)) {
        printk("Device %s is not ready\n", led_1.port->name);
        return -1;
    }

    ret = gpio_pin_configure_dt(&led_1, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        return -1;
    }

    //LED 2
    if(!device_is_ready(led_2.port)) {
        printk("Device %s is not ready\n", led_2.port->name);
        return -1;
    }

    ret = gpio_pin_configure_dt(&led_2, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        return -1;
    }

    //LED 3
    if(!device_is_ready(led_3.port)) {
        printk("Device %s is not ready\n", led_3.port->name);
        return -1;
    }

    ret = gpio_pin_configure_dt(&led_3, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        return -1;
    }

    //LED 4
    if(!device_is_ready(led_4.port)) {
        printk("Device %s is not ready\n", led_4.port->name);
        return -1;
    }

    ret = gpio_pin_configure_dt(&led_4, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        return -1;
    }

    return 0;

}