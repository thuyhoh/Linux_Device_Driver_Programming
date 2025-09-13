# Xử lý sự kiện trong hệ thống 

## Tổng quan về sự kiện
- Xử lý theo cơ chế pooling
    - triển khai đơn giản
    - Không hiệu quả, tốn CPU
- Xử lý bằng interrupt và exception
    - Xử lý phức tạp hơn pooling
    - Hiệu quả hơn 
## Xử lý sự kiện bằng cơ chế pooling
- Trong source code có một vòng while liên tục kiểm tra sự kiện đã xảy ra hay chưa
- Tạo một timer thread chạy định kì kiểm tra thông qua:
    - struct timer_list
    - init_timer_on_stack
    - add_timer
``` C
#include <linux/timer.h>
#include <linux/jiffies.h>

struct timer_list exp_timer; // Cấu trúc dữ liệu biểu diễn một timer.
exp_timer.expires = jiffies + delay * HZ; // Thời điểm timer hết hạn (tính theo jiffies)
// jiffies là một biến toàn cục trong Linux kernel, dùng để đếm số tick của bộ đếm hệ thống (system timer interrupt) kể từ khi kernel bắt đầu chạy.
// Mỗi lần ngắt hệ thống xảy ra (do timer hardware tạo ra), jiffies được tăng lên 1 đơn vị.
// HZ là số tick trong 1 giây.
exp_timer.function = function; // hàm callback được gọi khi timer hết hạn
exp_timer.data = 0; // Dữ liệu truyền vào function 

add_timer(&exp_timer);
```
## Xử lý sự kiện bằng cơ chế interrupt và exception
### 1. Interrupt
####  1. Xử lý ngắt chung
- Ngắt(Interrupt)
- có thể chia các loại ngắt thành: Ngắt cứng, ngắt mềm, ngắt có thể bỏ qua, ngắt không thể bỏ qua 

- Những quy định trong xử lý ngắt:
    - Không được sleep trong interrupt handler
    - Phải thực thi nhanh nhất có thể
    - thường chia sẻ data với các irq handler khác
``` C
/**
 * @brief: 
 * @param[irq]: Số hiệu IRQ (Interrupt Request Line)
 * @param[handler]: Hàm callback (ISR) sẽ được gọi khi xảy ra ngắ
 * @param[flags]: Cờ cho biết kiểu và cách xử lý ngắt.
 *      - IRQF_SHARED: Cho phép chia sẻ cùng một IRQ cho nhiều thiết bị.
 *      - IRQF_TRIGGER_RISING, IRQF_TRIGGER_FALLING, IRQF_TRIGGER_HIGH, IRQF_TRIGGER_LOW: Cấu hình cạnh hoặc mức kích hoạt ngắt.
 * @param[name]: Tên của thiết bị/ngắt (dùng để debug, xuất ra /proc/interrupts).
 * @param[dev]:
 *      - NULL 
 *      - Nếu dùng IRQF_SHARED, mỗi handler phải có dev riêng để kernel phân biệt
 * @retval: 
 *      - 0 nếu đăng ký thành công.
 *      - < 0 nếu thất bại 
*/

int request_irq(unsigned int irq, irq_handler_t handler, unsigned long flags, const char *name, void *dev);

/* callback function Prototype */
irqreturn_t handler(int irq, void *dev_id)
{
    /* ... */
}
```

- hiển thị các ngắt đang thực thi trên hệ thống thông qua: ``cat /proc/interrupt`` 

- Kiểm tra ngữ cảnh ngắt thông qua các hàm:
    - Irqs_disabled()
    - In_interrupt()
    - In_irq()

#### 2. Top half và bottom half
##### 1. Top half
- sử lý công việc critical. không thể delay được 
- Những lưu ý khi trong top half:
    - Kmalloc với cờ GFP_ATOMIC
    - sử dụng Delay() thay thể cho msleep()
    - Printk() 
##### 2. Bottom half
- Xử lý các việc vần nhiều thời gian hoặc có thể delay được 
- `tip`: Trong interrupt handler tạo hàm add_timer để tạo 1 timer để xử lý sau

### Exception
