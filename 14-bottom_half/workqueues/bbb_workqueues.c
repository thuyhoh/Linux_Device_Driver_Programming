#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>

#define IRQ_NO 1 // Ví dụ sử dụng line ngắt bàn phím để kiểm thử

/* 1. Định nghĩa hàm xử lý Bottom Half (chạy trong Process Context) */
void my_workqueue_fn(struct work_struct *work)
{
    pr_info("Executing Workqueue Function (Process Context - Co the sleep)\n");
}

/* 2. Khai báo tĩnh cấu trúc work_struct */
DECLARE_WORK(my_workqueue, my_workqueue_fn);

/* 3. Top Half: Interrupt Handler */
static irqreturn_t irq_handler(int irq, void *dev_id)
{
    pr_info("Interrupt Occurred (Top Half)\n");
    
    /* Đẩy công việc xuống Bottom Half */
    schedule_work(&my_workqueue);
    
    return IRQ_HANDLED;
}

static int __init my_driver_init(void)
{
    /* Đăng ký ngắt với cờ IRQF_SHARED nếu dùng chung IRQ line */
    if (request_irq(IRQ_NO, irq_handler, IRQF_SHARED, "my_wq_device", (void *)(irq_handler))) {
        pr_err("Cannot register IRQ %d\n", IRQ_NO);
        return -EIO;
    }
    pr_info("Driver loaded successfully\n");
    return 0;
}

static void __exit my_driver_exit(void)
{
    /* Đảm bảo work đang chờ được xử lý xong trước khi giải phóng tài nguyên */
    flush_work(&my_workqueue);
    free_irq(IRQ_NO, (void *)(irq_handler));
    pr_info("Driver unloaded\n");
}

module_init(my_driver_init);
module_exit(my_driver_exit);
MODULE_LICENSE("GPL");
