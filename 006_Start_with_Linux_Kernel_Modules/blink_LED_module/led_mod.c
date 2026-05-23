#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <stdint.h>
#include <linux/io.h>

#define GPIO0_STARTADDR             (0x44E07000)
#define GPIO0_ENDADDR               (0x44E07FFF)
#define GPIO0_SIZE                  (GPIO0_ENDADDR - GPIO0_STARTADDR)    

#define GPIO_OE_OFFSET              (0x134)
#define GPIO_DATAIN_OFFSET          (0x138)
#define GPIO_DATAOUT_OFFSET         (0x13C)
#define GPIO_CLEARDATAOUT_OFFSET    (0x190)
#define GPIO_SETDATAOUT_OFFSET      (0x194)

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

/* This is decriptive information about the module */
MODULE_LICENSE("GPL"); 
MODULE_AUTHOR("Thuy");
MODULE_INFO(board, "beaglebone black");
MODULE_DESCRIPTION("A kernel module to print some messanges");  
MODULE_VERSION("v1.1");

