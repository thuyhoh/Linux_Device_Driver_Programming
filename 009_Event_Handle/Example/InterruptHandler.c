#include <linux/module.h>
#include <linux/interrutp.h>
#include <linux/io.h>

#define AUTHOR "NGUYEN TRONG THUY"
#define BUTTON

#define GPIO0_STARTADDR             (0x44E07000)
#define GPIO0_ENDADDR               (0x44E07FFF)
#define GPIO0_SIZE                  (GPIO0_ENDADDR - GPIO0_STARTADDR)    

#define GPIO_OE_OFFSET              (0x134)
#define GPIO_DATAIN_OFFSET          (0x138)
#define GPIO_DATAOUT_OFFSET         (0x13C)
#define GPIO_CLEARDATAOUT_OFFSET    (0x190)
#define GPIO_SETDATAOUT_OFFSET      (0x194)
#define GPIO_IRQSTATUS_SET_0_OFFSET (0x34)
#define GPIO_LEVELDETECT0_OFFSET    (0x140)

static uint32_t __iomem *gpio0_reg;
unsigned int irq_eint0;

irqreturn_t interrupt_handler(int irq, void *dev_id, struct pt_regs *regs)
{   
    uint32_t reg;
    if(ioread32(gpio0_reg + GPIO_DATAIN_OFFSET/4)) // đọc giá trị từ gpiio0_0
        if(reg = ioread32(gpio0_reg + GPIO_DATAOUT_OFFSET/4) & 1<<4) // Kiểm tra trạng thái của led gpio0_4
        {
            printk("LED OFF");
            int temp =  reg | 1<<4;
            iowrite32(temp, (gpio0_reg + GPIO_DATAOUT_OFFSET/4));
        }else
        {
            printk("LED ON");
            int temp = reg & (~(1<<4))
            iowrite32(temp, (gpio0_reg + GPIO_DATAOUT_OFFSET/4));
        }
    return IRQ_HANDLED;
}

static int __init interrupt_init(void)
{
    int ret;
    printk("Start!\n");
    gpio0_reg = ioremap(GPIO0_STARTADDR, GPIO0_SIZE);
    // Config LED and BUTTON
    uint32_t temp;
    temp = (temp & (~(1<<4))) | 1;// BUTTON : gpio0_0, LED : gpio0_4

    iowrite32(temp, (gpio0_reg + GPIO_OE_OFFSET/4));
    // Config interrupt
    temp = 1;
    iowrite32(temp, (gpio0_reg + GPIO_LEVELDETECT0_OFFSET/4));
    iowrite32(temp, (gpio0_reg + GPIO_IRQSTATUS_SET_0_OFFSET/4))
    irq_eint0 = gpio_to_irq(BUTTON);

    ret = request_irq(irq_eint0, (irq_handler_t)interrupt_handler, IRQF_SHARED, "EINT0", (void *)interrupt_handler);
    if(ret)
        printk("Init EINT0 success\n");
    return 0;
}

static void __exit interrupt_exit(void)
{
    printk("END!\n");
    iounmap(gpio0_reg);
    free_irq(irq_eint0, (void *)interrupt_handler);
}

module_init(interrupt_init);
module_exit(interrupt_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR(AUTHOR);
