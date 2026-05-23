#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <stdint.h>
#include <linux/io.h>

#define DRIVE_AUTHOR    "trongthuynl2004@gmail.com"
#define DRIVE_VERS      "1.1"
#define DRIVE_LICENSE   "GPL"
#define DRIVE_DESC      "Control GPIO0 pin 4"


uint32_t __iomem *gpio0_reg;

static int  __init led_mod_init(void)
{
    printk("Led module init \n");
    gpio0_reg = ioremap(GPIO0_STARTADDR, GPIO0_SIZE);
    if(!gpio0_reg)
    {
        printk("ioremap is fail\n");
    }
    
    *(gpio0_reg + GPIO_OE_OFFSET/4) &= ~(1<<4);
    // turn on led
    *(gpio0_reg + GPIO_SETDATAOUT_OFFSET/4) |= (1<<4);
    return 0;
}

static void  __exit led_mod_exit(void)
{  
    *(gpio0_reg + GPIO_CLEARDATAOUT_OFFSET/4) |= (1<<4);
    *(gpio0_reg + GPIO_OE_OFFSET/4) |= (1<<4);
    iounmap(gpio0_reg);
    printk("Led module exit \n");
}

module_init(led_mod_init);
module_exit(led_mod_exit);

MODULE_AUTHOR(DRIVE_AUTHOR);
MODULE_LICENSE("GPL"); 