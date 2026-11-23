#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>

#define BTN_GPIO 60 // GPIO1_28 = 32*1 + 28 = 60

static int btn_irq = -1;
static unsigned long press_count = 0;
static struct tasklet_struct bbb_tasklet;

/* BOTTOM HALF (Tasklet): Chạy sau ngắt, KHÔNG gọi hàm gây sleep */
static void bbb_tasklet_fn(unsigned long data)
{
    (void)data;
    pr_info("BBB_BH: [Bottom Half] Đã xử lý sự kiện nút nhấn lần thứ %lu\n", press_count);
}

/* TOP HALF: Chạy ngay trong ngữ cảnh ngắt, giữ tối giản */
static irqreturn_t bbb_gpio_irq_handler(int irq, void *dev_id)
{
    press_count++;
    pr_info("BBB_BH: [Top Half] Phát hiện ngắt GPIO %d (IRQ %d), lần #%lu\n",
            BTN_GPIO, irq, press_count);

    /* Đẩy phần xử lý sau ngắt xuống Bottom Half */
    tasklet_schedule(&bbb_tasklet);

    return IRQ_HANDLED;
}

static int __init bbb_bh_init(void)
{
    int ret;

    tasklet_init(&bbb_tasklet, bbb_tasklet_fn, 0);

    ret = gpio_request(BTN_GPIO, "bbb_btn_gpio");
    if (ret) {
        pr_err("BBB_BH: Không yêu cầu được GPIO %d (lỗi %d)\n", BTN_GPIO, ret);
        return ret;
    }

    ret = gpio_direction_input(BTN_GPIO);
    if (ret) {
        pr_err("BBB_BH: Không đặt được hướng input cho GPIO %d\n", BTN_GPIO);
        goto err_free_gpio;
    }

    btn_irq = gpio_to_irq(BTN_GPIO);
    if (btn_irq < 0) {
        pr_err("BBB_BH: Không ánh xạ được IRQ cho GPIO %d\n", BTN_GPIO);
        ret = btn_irq;
        goto err_free_gpio;
    }

    ret = request_irq(btn_irq, bbb_gpio_irq_handler,
                      IRQF_TRIGGER_RISING, "bbb_gpio_bh_irq", NULL);
    if (ret) {
        pr_err("BBB_BH: Không đăng ký được IRQ %d (lỗi %d)\n", btn_irq, ret);
        goto err_free_gpio;
    }

    pr_info("BBB_BH: Đã sẵn sàng trên GPIO %d (IRQ %d)\n", BTN_GPIO, btn_irq);
    return 0;

err_free_gpio:
    gpio_free(BTN_GPIO);
    return ret;
}

static void __exit bbb_bh_exit(void)
{
    /* Ngừng nhận ngắt mới, hủy tasklet đang chờ/chạy, rồi giải phóng GPIO */
    if (btn_irq >= 0)
        free_irq(btn_irq, NULL);

    tasklet_kill(&bbb_tasklet);
    gpio_free(BTN_GPIO);
    pr_info("BBB_BH: Đã gỡ driver an toàn.\n");
}

module_init(bbb_bh_init);
module_exit(bbb_bh_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Thuy");
MODULE_DESCRIPTION("Ví dụ Top Half và Bottom Half với GPIO IRQ trên BeagleBone Black");