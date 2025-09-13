#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>

#define DRIVE_AUTHOR    "trongthuynl2004@gmail.com"
#define DRIVE_VERS      "1.1"
#define DRIVE_LICENSE   "GPL"
#define DRIVE_DESC      "LED control"

#define GPIO0_STARTADDR             (0x44E07000)
#define GPIO0_ENDADDR               (0x44E07FFF)
#define GPIO0_SIZE                  (GPIO0_ENDADDR - GPIO0_STARTADDR)    

#define GPIO_OE_OFFSET              (0x134)
#define GPIO_DATAIN_OFFSET          (0x138)
#define GPIO_DATAOUT_OFFSET         (0x13C)
#define GPIO_CLEARDATAOUT_OFFSET    (0x190)
#define GPIO_SETDATAOUT_OFFSET      (0x194)

#define LED                         (~(1<<4))
#define BUTTON                      (1<<5)

static uint32_t __iomem *gpio0_reg;
static struct timer_list exp_timer;


static void check_button(unsigned long data);
static void check_button(unsigned long data)
{
    unsigned int delay = 0.1 * HZ;
    uint32_t temp;
    if(button) // kiểm tra nút nhấn
    {
        printk("LED ON");
    }else{
        printk("LED OFF");
    }
    exp_timer.expires = jiffies + delay;
    exp_timer.function = (void *)check_button;
    add_timer(&exp_timer);
}

static int  __init check_event_timer_init(void)
{
    printk("Led module init \n");
    gpio0_reg = ioremap(GPIO0_STARTADDR, GPIO0_SIZE);
    if(!gpio0_reg)
    {
        printk("ioremap is fail\n");
        goto address_gpio_fail;
    }
    // LED
    uint32_t temp;
    temp = temp & LED | BUTTON; 

    iowrite32(temp, (gpio0_reg + GPIO_OE_OFFSET/4));

    init_timer_on_stack(&exp_timer);
	exp_timer.expires = jiffies + 1 * HZ;
	exp_timer.function = (void *)check_button;
	exp_timer.data = NULL;

	add_timer(&exp_timer);

    return 0;
address_gpio_fail:
    iounmap(gpio0_reg);
    return -1;
}

static void  __exit check_event_timer_exit(void)
{  
    *(gpio0_reg + GPIO_CLEARDATAOUT_OFFSET/4) |= (1<<4);
    *(gpio0_reg + GPIO_OE_OFFSET/4) |= (1<<4);
    iounmap(gpio0_reg);
    printk("Led module exit \n");
}


module_init(check_event_timer_init);
module_exit(check_event_timer_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(AUTHOR);

