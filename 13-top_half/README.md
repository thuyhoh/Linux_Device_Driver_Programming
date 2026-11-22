# Interrupt and Interrupt handlers in Linux(Top Half)
## 1. Cơ chế Ngắt và Bộ điều khiển Ngắt

### 1.1 Khái niệm Ngắt (Interrupt)
* **Polling:** CPU liên tục kiểm tra trạng thái thiết bị ngoại vi $\Rightarrow$ Lãng phí tài nguyên xử lý.
* **Interrupt:** Thiết bị chủ động gửi tín hiệu phần cứng báo cho CPU khi có sự kiện cần xử lý $\Rightarrow$ Tối ưu hóa hiệu năng CPU.

### 1.2 Phân biệt Exception và Interrupt
* **Exception (Ngoại lệ):** Phát sinh đồng bộ (synchronous) từ chính dòng lệnh đang thực thi của CPU (ví dụ: lỗi chia cho 0, lỗi truy cập trang nhớ - Page Fault).
* **Interrupt (Ngắt phần cứng):** Phát sinh bất đồng bộ (asynchronous) từ các thiết bị ngoại vi hoặc bộ định thời (Timer) bên ngoài CPU.

### 1.3 Bộ điều khiển Ngắt (Interrupt Controller)
* Tín hiệu ngắt từ thiết bị không đi trực tiếp vào nhân CPU mà đi qua bộ điều khiển ngắt (ví dụ: PIC - Programmable Interrupt Controller trên hệ thống đơn giản hoặc IO APIC trên hệ thống đa nhân modern).
* Bộ điều khiển ngắt chuyển đổi tín hiệu vật lý thành số hiệu đường ngắt (IRQ Number) và thông báo cho CPU.


## 2. Bảng mô tả ngắt (IDT) và Luồng thực thi

### 2.1 Bảng IDT (Interrupt Descriptor Table)
* CPU tra cứu vị trí bảng IDT trong bộ nhớ thông qua thanh ghi đặc biệt `IDTR`.
* Bảng IDT chứa các con trỏ trỏ đến các địa chỉ của trình xử lý tương ứng cho từng vectơ ngắt:
  * Các vectơ ngắt đầu tiên ($0 - 31$): Dành cho các Trap/Exception của CPU.
  * Các vectơ tiếp theo: Dành cho ngắt phần cứng thiết bị.
  * Vectơ ngắt hệ thống (ví dụ: $0x80$ / 128 trên x86): Dùng cho lệnh gọi hệ thống (System Call).

### 2.2 Luồng xử lý khi ngắt xảy ra
1. Thiết bị phát tín hiệu IRQ.
2. Bộ điều khiển ngắt xác định IRQ number và gửi tín hiệu tới CPU.
3. CPU tạm dừng dòng lệnh hiện tại, lưu ngữ cảnh (context) và mức ưu tiên.
4. Tra cứu IDT để nhảy tới hàm xử lý ngắt tương ứng.
5. Thực thi trình xử lý ngắt, gửi xác nhận (ACK) cho bộ điều khiển ngắt và khôi phục ngữ cảnh CPU.

## 3. Ngữ cảnh Ngắt (Interrupt Context) và Quy tắc

Mã nguồn chạy trong **Interrupt Context** khác hoàn toàn so với **Process Context**:
* **Không gắn với tiến trình:** Ngắt có thể xảy ra ở bất kỳ thời điểm nào, không liên quan đến tiến trình đang chạy hiện tại.
* **Quy tắc tuyệt đối - Không được ngủ (No Sleep):**
  * Không sử dụng các hàm gây `sleep`, `schedule()`, hoặc chờ Semaphore/Mutex.
  * Không thực hiện các thao tác I/O blocking hoặc cấp phát bộ nhớ blocking (`GFP_KERNEL`).
* **Thời gian thực thi tối đa ngắn:** ISR phải kết thúc càng nhanh càng tốt để tránh làm nghẽn hệ thống.

## 4. Linux Kernel Interrupt APIs 
- #include <linux/interrupt.h>
### request_irq
``` C
int request_irq(unsigned int irq, irq_handler_t handler, unsigned long flags, const char *name, void *dev);
```
* `irq`: Số hiệu đường ngắt phần cứng (IRQ number).
* `handler`: Con trỏ trỏ tới hàm xử lý ngắt (ISR).
* `name`: Tên thiết bị hiển thị trong `/proc/interrupts`.
* `dev`: Con trỏ cookie nhận dạng (bắt buộc khi dùng ngắt chia sẻ).
* `flags`: Các cờ quy định thuộc tính ngắt 

|flags|nội dung|
|:----|:-------|
|IRQF_DISABLED||
|IRQF_SAMPLE_RANDOM|Cho biết thiết bị sinh ngắt có thể đóng góp entropy cho bộ sinh số ngẫu nhiên của kernel|
|IRQF_TIMER|Đánh dấu ngắt thuộc loại ngắt bộ định thời (timer interrupt)|
|IRQF_SHARED| Cho phép nhiều trình điều khiển thiết bị cùng chia sẻ chung một đường IRQ(dev bắt buộc phải là địa chỉ duy nhất khác NULL)|
### irq_handler
``` C
static irq_handler_t handler(int irq, void *dev);
```
- return value
    - IRQ_HANDLED: Báo cho kernel biết ngắt xuất phát từ thiết bị do handler này quản lý và đã được xử lý thành công.

    - IRQ_NONE: Báo cho kernel biết ngắt không đến từ thiết bị của handler này
### free_irq
``` C
void free_irq(unsigned int irq, void *dev);
```

### 4.2 Điều khiển Bật / Tắt Ngắt
Kernel cung cấp các hàm điều khiển việc vô hiệu hóa hoặc cho phép ngắt:

* **Tắt/Bật ngắt cục bộ trên CPU hiện tại:**
  * `local_irq_disable()` / `local_irq_enable()`
  * `local_irq_save(flags)` / `local_irq_restore(flags)` (Lưu và khôi phục trạng thái ngắt).
* **Vô hiệu hóa một đường ngắt cụ thể:**
  * `disable_irq(irq)` (Vô hiệu hóa đường ngắt và chờ trình xử lý hiện tại hoàn tất).
  * `disable_irq_nosync(irq)` (Vô hiệu hóa đường ngắt ngay lập tức mà không chờ).
  * `enable_irq(irq)` (Kích hoạt lại đường ngắt).
* **Kiểm tra ngữ cảnh:**
  * `in_interrupt()`: Trả về `true` nếu mã nguồn đang chạy trong ngữ cảnh ngắt.

