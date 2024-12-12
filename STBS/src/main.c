#include <zephyr/kernel.h>          /* for kernel functions*/
#include <zephyr/sys/printk.h>      /* for printk()*/
#include <zephyr/timing/timing.h>   /* for timing services */
#include "../lib/stbs.h"
#include "../lib/tasks.h"



/* Main function */
int main(void) {

    k_msleep(3000);
    printk("tjetje\n");
    stbs_init();
    

} 


