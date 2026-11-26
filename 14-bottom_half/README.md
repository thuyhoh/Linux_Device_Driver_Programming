# Softirqs và Tasklets trong Linux Kernel (Bottom Half)


## 1. Tổng quan về Bottom Half

Khi một ngắt phần cứng (Hardware Interrupt) xảy ra, trình xử lý ngắt chính (**Top Half / Interrupt Handler**) cần thực thi cực kỳ nhanh chóng vì các ngắt khác có thể bị vô hiệu hóa (disabled) trong lúc nó đang chạy 

Để không làm mất các tín hiệu ngắt tiếp theo từ thiết bị, các công việc nặng và không quá khẩn cấp sẽ được trì hoãn để xử lý sau tại **Bottom Half** . 
Linux cung cấp 3 cơ chế chính cho Bottom Half:
1. **Softirqs** (Software Interrupts)
2. **Tasklets**
3. **Workqueues** *(không đề cập sâu trong bài này)*


## 2. Softirqs (Software Interrupts)

### Đặc điểm chính
- **Định nghĩa tĩnh (Statically defined):** Phải được khai báo ngay tại thời điểm biên dịch nhân (compile-time), không thể tạo động từ module/driver thông thường .
- **Chạy đồng thời (Concurrent):** Cùng một loại `softirq` có thể chạy song song trên nhiều CPU khác nhau tại cùng một thời điểm. Do đó, lập trình viên phải tự quản lý đồng bộ hóa (locking) hoặc sử dụng dữ liệu cục bộ trên từng CPU (`per-CPU data`) để đạt hiệu năng cao nhất .
- **Không thể ngủ (Cannot sleep):** Chạy trong ngữ cảnh ngắt (interrupt context), không được phép gọi các hàm gây block/sleep.
- **Mục đích sử dụng:** Dành cho các tác vụ đòi hỏi tần suất cao và độ trễ cực thấp (như mạng, block device, timer).

### Các loại Softirq được định sẵn trong Linux
Hệ thống chỉ dành sẵn một số lượng giới hạn các `softirq` cố định (ví dụ: `HI_SOFTIRQ`, `TIMER_SOFTIRQ`, `NET_TX_SOFTIRQ`, `NET_RX_SOFTIRQ`, `BLOCK_SOFTIRQ`, `TASKLET_SOFTIRQ`, v.v.).

### Thời điểm thực thi
Hàm cốt lõi `__do_softirq()` sẽ kiểm tra bitmask các `softirq` đang chờ xử lý (pending) trên từng CPU và thực thi chúng vào các thời điểm :
- Ngay khi vừa thoát khỏi một trình xử lý ngắt phần cứng (Hardware Interrupt Handler).
- Bên trong các kernel thread chuyên biệt (`ksoftirqd/n` trên mỗi nhân CPU) khi tải hệ thống quá cao .
- Khi mã nguồn nhân chủ động kích hoạt/kiểm tra (ví dụ: hệ thống mạng).

### Các hàm API cơ bản của Softirq
- `open_softirq()`: Đăng ký một hàm xử lý (action) cho một `softirq` .
- `raise_softirq()` / `raise_softirq_irqoff()`: Kích hoạt (đánh dấu pending) một `softirq` để nhân thực thi.
- `local_bh_disable()` / `local_bh_enable()`: Vô hiệu hóa hoặc bật lại việc thực thi Bottom Half trên CPU hiện tại.


## 3. Tasklets

### Đặc điểm chính
- **Xây dựng trên nền tảng Softirq:** Tasklet thực chất là một lớp trừu tượng được cài đặt dựa trên hai loại softirq có sẵn là `HI_SOFTIRQ` (ưu tiên cao) và `TASKLET_SOFTIRQ` (ưu tiên thường).
- **Khởi tạo động hoặc tĩnh:** Có thể dễ dàng khai báo và sử dụng ngay trong các Linux Device Driver mà không cần sửa đổi mã nguồn lõi của nhân.
- **Tuần tự hóa (Serialized):** Một `tasklet` cụ thể **không bao giờ** chạy đồng thời trên hai CPU khác nhau tại cùng một thời điểm [00:12:21]. Nhờ vậy, lập trình viên không cần lo lắng về việc khóa (locking) dữ liệu nội bộ của chính tasklet đó (tuy nhiên, các tasklet *khác nhau* vẫn có thể chạy song song trên các CPU khác nhau).
- **Không thể ngủ (Cannot sleep):** Giống như `softirq`, tasklet cũng chạy trong ngữ cảnh ngắt nên không được phép gọi các hàm gây sleep.

### Bảng so sánh nhanh: Softirqs vs. Tasklets

| Tiêu chí | Softirqs | Tasklets |
| :--- | :--- | :--- |
| **Cách khởi tạo** | Tĩnh (Compile-time) | Tĩnh hoặc Động (Runtime trong Driver) |
| **Chạy song song cùng loại** | Có (trên nhiều CPU cùng lúc) | Không (1 tasklet chỉ chạy trên 1 CPU tại 1 thời điểm) |
| **Yêu cầu Locking** | Rất khắt khe (cần per-CPU data / spinlock)  | Nới lỏng hơn (không cần khóa bên trong chính nó) |
| **Độ ưu tiên & Tốc độ** | Rất cao, độ trễ thấp nhất | Thấp hơn Softirq một chút |
| **Đối tượng sử dụng** | Các hệ thống con cốt lõi của Kernel | Các Device Driver thông thường |


### Các hàm API làm việc với Tasklet trong Driver

#### Khai báo & Khởi tạo
- **Khai báo tĩnh:**
```c
DECLARE_TASKLET(name, func, data);
DECLARE_TASKLET_DISABLED(name, func, data);
```
- Khởi tạo động:
``` C
struct tasklet_struct *my_tasklet;
tasklet_init(my_tasklet, func, data);
```

#### Lên lịch thực thi (Scheduling)
tasklet_schedule(&my_tasklet): Đưa tasklet vào hàng đợi TASKLET_SOFTIRQ để thực thi với độ ưu tiên bình thường.

tasklet_hi_schedule(&my_tasklet): Đưa tasklet vào hàng đợi HI_SOFTIRQ để thực thi với độ ưu tiên cao.
#### Điều khiển & Hủy bỏ
tasklet_disable(&my_tasklet): Vô hiệu hóa tasklet. Nếu tasklet hiện đang chạy trên một CPU khác, hàm sẽ đợi cho tới khi nó thực thi xong mới trả về.

tasklet_disable_nosync(&my_tasklet): Vô hiệu hóa tasklet ngay lập tức mà không đợi phiên đang chạy kết thúc.

tasklet_enable(&my_tasklet): Kích hoạt lại một tasklet trước đó đã bị vô hiệu hóa (cần gọi tương ứng với số lần disable).

tasklet_kill(&my_tasklet): Gỡ tasklet khỏi hàng đợi nếu đang chờ chạy và đảm bảo nó không được lên lịch lại. Thường được gọi trong hàm exit (hoặc remove) khi gỡ bỏ driver khỏi kernel để tránh rò rỉ tài nguyên hoặc lỗi truy cập vùng nhớ đã giải phóng.

## 4. Workqueue

### Khi nào nên dùng Workqueue?
Sử dụng Workqueue bất cứ khi nào hàm xử lý Bottom Half **cần phải sleep (ngủ) hoặc block**, ví dụ:
* Cấp phát bộ nhớ động có khả năng chờ (ví dụ: cờ `GFP_KERNEL`).
* Thực hiện các thao tác I/O đồng bộ/chờ đợi (blocking I/O).
* Sử dụng các cơ chế đồng bộ hóa có khả năng gây ngủ như `mutex` hoặc `semaphore`.
* Gọi các hàm trì hoãn như `msleep()`.

## 2. Kiến trúc và Cấu trúc dữ liệu nội bộ

Hệ thống Workqueue trong Linux kernel được tổ chức xoay quanh các cấu trúc dữ liệu chính sau:

1. **`struct work_struct` (và `struct delayed_work`):**
   * Đại diện cho một đơn vị công việc (work item) cần thực thi.
   * Chứa dữ liệu trạng thái (`data`), con trỏ danh sách liên kết (`entry`) và con trỏ hàm callback (`work_func_t func`).
2. **`struct workqueue_struct`:**
   * Đại diện cho một hàng đợi công việc (workqueue), lưu giữ các cờ cấu hình (`flags`), giới hạn số tác vụ hoạt động đồng thời (`max_active`) và liên kết tới các `pool_workqueue`.
3. **`struct pool_workqueue`:**
   * Cầu nối trung gian liên kết `workqueue_struct` với `worker_pool`, quản lý số lượng work đang kích hoạt và cơ chế đồng bộ hóa.
4. **`struct worker_pool`:**
   * Quản lý danh sách các `work_struct` đang chờ xử lý (`worklist`) và tập hợp các luồng thực thi (`workers`).
   * Mặc định, mỗi CPU có các worker pool riêng (cho mức ưu tiên thường và mức ưu tiên cao), ngoài ra còn có các pool *unbound* (không gắn cố định vào CPU nào).

### Luồng thực thi của Worker Thread

## 3. Các Macro và API quan trọng
### 3.1. Khai báo và Khởi tạo Work
- Khai báo tĩnh (Compile-time):
``` C
DECLARE_WORK(name, void (*func)(struct work_struct *work));
DECLARE_DELAYED_WORK(name, void (*func)(struct work_struct *work));
```
- Khởi tạo động (Run-time):
``` C
INIT_WORK(struct work_struct *work, void (*func)(struct work_struct *work));
INIT_DELAYED_WORK(struct delayed_work *dwork, void (*func)(struct work_struct *work));
```
### 3.2. Lập lịch thực thi trên System Shared Workqueue (events)
``` C
// Đưa work vào hàng đợi để chạy ngay khi worker thread sẵn sàng
bool schedule_work(struct work_struct *work);

// Đưa work vào hàng đợi sau một khoảng trễ (tính theo jiffies)
bool schedule_delayed_work(struct delayed_work *dwork, unsigned long delay);

// Chỉ định CPU cụ thể để thực thi work
bool schedule_work_on(int cpu, struct work_struct *work);
bool schedule_delayed_work_on(int cpu, struct delayed_work *dwork, unsigned long delay);
```
3.3. Tạo và Quản lý Custom Workqueue
``` C
// Cấp phát một workqueue mới
struct workqueue_struct *alloc_workqueue(const char *fmt, unsigned int flags, int max_active, ...);

// Đẩy work vào custom workqueue
bool queue_work(struct workqueue_struct *wq, struct work_struct *work);
bool queue_delayed_work(struct workqueue_struct *wq, struct delayed_work *dwork, unsigned long delay);

// Đợi tất cả work hoàn thành và hủy workqueue khi gỡ module
void flush_workqueue(struct workqueue_struct *wq);
void destroy_workqueue(struct workqueue_struct *wq);
```


