
/*  
 *  hello-1.c - The simplest kernel module.
 */
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/gpio/consumer.h>	

struct gpio_desc *led, *button;

#define IO_LED     21
#define IO_BUTTON  22

static int __init my_init(void)
{
	int status;
	printk(KERN_INFO "gpioctrl - initializing\n");
	led = gpio_to_desc(IO_LED);
	
	if (!led) {
		printk(KERN_ERR "gpioctrl - failed to getting pin 21\n");
		return -ENODEV;
	}

	button = gpio_to_desc(IO_BUTTON);
	if (!button) {
		printk(KERN_ERR "gpioctrl - failed to getting pin 22\n");
		return -ENODEV;
	}
	status = gpiod_direction_output(led, 0);
	if (status) {
		printk(KERN_ERR "gpioctrl - failed to set LED GPIO direction\n");
		return status;
	}
	status = gpiod_direction_input(button);
	if (status) {
		printk(KERN_ERR "gpioctrl - failed to set BUTTON GPIO direction\n");
		return status;
	}

	gpiod_set_value(led, 1);
	printk(KERN_INFO "gpioctrl - Button is %spressed\n", gpiod_get_value(button) ? "" : "not ");
	return 0;
}

static void __exit my_exit(void)
{
	goiod_set_value(led, 0);
	printk(KERN_INFO "gpioctrl - exiting\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Thuy");
MODULE_DESCRIPTION("An example for GPIOs without using device tree");
