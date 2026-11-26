# Linux synchronization services
## 1. Các khái niệm chung
- Linux synchronization services bao gồm các cơ chế đồng bộ hóa luồng/tiến trình trong nhân Linux nhằm tránh xung đột dữ liệu (race condition) và bảo vệ tính toàn vẹn của hệ thống khi chạy đa nhiệm.

- Trong lập trình đa luồng (multithreading) và hệ thống nhúng/lập trình hệ thống, khi nhiều luồng hoặc tiến trình cùng chạy song song và tương tác với nhau, việc quản lý quyền truy cập dữ liệu đóng vai trò quyết định tính đúng đắn của chương trình.

- Trong Kernel Space, bài toán Race Condition và bảo vệ Critical Section phức tạp hơn nhiều so với User Space. Một lỗi race condition ở User Space chỉ làm crash ứng dụng, nhưng trong Kernel, nó có thể gây Kernel Panic, làm treo toàn bộ hệ thống hoặc hỏng dữ liệu phần cứng vĩnh viễn.
### 1.1. Race condition — điều kiện tranh chấp
- Bản chất: Là hiện tượng kết quả thực thi của mã nguồn nhân phụ thuộc vào thứ tự lập lịch (scheduling order) ngẫu nhiên giữa các tiến trình, các lõi CPU (SMP) hoặc giữa tiến trình nền và trình phục vụ ngắt (ISR).

- Nguyên nhân: Các luồng/ngắt cùng truy cập một tài nguyên dùng chung mà không có cơ chế tuần tự hóa, dẫn đến hiện tượng ghi đè dữ liệu (Lost Update) hoặc đọc dữ liệu chưa hoàn tất.
### 1.2. Shared resource — tài nguyên dùng chung
- Định nghĩa: Bất kỳ vùng dữ liệu, cấu trúc dữ liệu toàn cục nào trong kernel (struct cdev, hàng đợi vòng kfifo, danh sách thiết bị list_head) hoặc các thanh ghi phần cứng (MMIO) mà nhiều thực thể trong kernel cùng đọc/ghi.

- Đặc thù trong Kernel: Tài nguyên dùng chung có thể được truy cập đồng thời từ nhiều nguồn:

    - Tiến trình người dùng qua lời gọi hệ thống (System Call / Process Context).

    - Ngắt cứng và ngắt mềm (Interrupt Context / SoftIRQ / Tasklet).
### 1.3. Critical section — vùng găng
- Critical section là đoạn mã truy cập dữ liệu dùng chung mà việc xen kẽ thực thi có thể phá vỡ tính đúng đắn. Nó có thể gồm thao tác đọc–sửa–ghi hoặc đọc nhiều trường cần nhất quán với nhau.

- Yêu cầu: Tại một thời điểm, chỉ cho phép tối đa một thực thi độc quyền (exclusive execution) được phép bước vào vùng găng này.

### 1.4. Execution context — ngữ cảnh thực thi

| Ngữ cảnh | Có được gọi API có thể ngủ? | Ví dụ |
| --- | --- | --- |
| Process/task context thông thường | Có, nếu đang ở trạng thái cho phép ngủ | System call, kernel thread |
| Worker của workqueue chạy bằng kernel thread | Có, nếu không giữ khóa hay trạng thái cấm ngủ | Xử lý nền của driver |
| Hard IRQ context | Không | Primary interrupt handler |
| Softirq/tasklet | Không | Xử lý bottom half |
| Threaded IRQ handler | Có, nếu không có ràng buộc cấm ngủ khác | `thread_fn` của `request_threaded_irq()` |

**Process context không đồng nghĩa với luôn được ngủ.** Khi đang giữ spinlock trên kernel thông thường, tắt ngắt hoặc ở trạng thái atomic khác, không được gọi API có thể ngủ.
### 1.5 Deadlock
## 2. spinlockn (struct spinlock_t)
- Nguyên tắc hoạt động: Cơ chế khóa bận (Busy-waiting / Spin-lock). Khi một luồng cố gắng lấy khóa đang bị giữ, nó không chuyển sang trạng thái ngủ mà liên tục quay vòng (loop) kiểm tra trạng thái khóa trên CPU cho đến khi khóa được giải phóng.

- Đặc điểm & Ràng buộc:

    - Không được phép ngủ (Sleep): Tuyệt đối không gọi các hàm có thể gây ngủ (như kmalloc(..., GFP_KERNEL), copy_to_user(), msleep(), hoặc mutex_lock()) bên trong vùng được bảo vệ bởi spinlock.

    - Sử dụng trong mọi ngữ cảnh: Là cơ chế duy nhất an toàn để sử dụng đồng thời trong cả Process Context và Interrupt Context (ISR).

### 2.1 APIs
``` C
#include <linux/spinlock.h>
```
#### APIs Khởi tạo
##### Khởi tạo tĩnh
``` C
static DEFINE_SPINLOCK(lock);
```
##### Khởi tạo động
``` C
void spin_lock_init(spinlock_t *lock);
```
#### APIs Khóa và mở khóa cơ bản
``` C
void spin_lock(spinlock_t *lock);
void spin_unlock(spinlock_t *lock);
int spin_trylock(spinlock_t *lock);
```
#### Biến thể an toàn với ngắt cục bộ (IRQ-safe)
``` C
spin_lock_irqsave(spinlock_t *lock, unsigned long flags);
void spin_unlock_irqrestore(spinlock_t *lock, unsigned long flags);

void spin_lock_irq(spinlock_t *lock);
void spin_unlock_irq(spinlock_t *lock);
```

## 3. Mutex (struct mutex_t)
- Nguyên tắc hoạt động: Cơ chế khóa độc quyền có hỗ trợ ngủ (Sleep-able Lock). Khi một tiến trình không thể lấy được mutex, nó sẽ được đưa vào hàng đợi chờ (sleep queue) và bộ lập lịch sẽ chuyển CPU cho tiến trình khác.
Đặc điểm & Ràng buộc:

    - Chỉ dùng trong Process Context: Nghiêm cấm sử dụng trong Interrupt Context (ISR, SoftIRQ, Tasklet) vì các ngữ cảnh ngắt không được phép ngủ.

    - Cho phép Sleep bên trong: Lập trình viên có thể thoải mái gọi các hàm cấp phát bộ nhớ (kmalloc với GFP_KERNEL), copy dữ liệu với user-space, hoặc gọi các hàm I/O gây block bên trong vùng găng của mutex.
### APIs
#### APIs Khởi tạo
``` C
#include <linux/mutex.h>
```
##### Khởi tạo tĩnh
``` C
static DEFINE_MUTEX(my_mutex);
```
##### Khởi tạo động
``` C
void mutex_init(struct mutex *lock);
```
#### APIs Khóa và mở khóa
``` C
void mutex_lock(struct mutex *lock);
int mutex_lock_interruptible(struct mutex *lock); /* Trả về 0 nếu thành công, -EINTR nếu bị ngắt bởi signal */
int mutex_trylock(struct mutex *lock);            /* Trả về 1 nếu chiếm được khóa, 0 nếu đang bận */
void mutex_unlock(struct mutex *lock);
```
## 4. Semaphore (struct semaphore)
- Nguyên tắc hoạt động: Cơ chế đồng bộ hóa dựa trên bộ đếm số nguyên (Counter-based Synchronization). Giống như Mutex, semaphore đưa tiến trình vào trạng thái ngủ khi tài nguyên chưa sẵn sàng, nhưng cho phép một số lượng luồng giới hạn ($N$ luồng) cùng lúc truy cập vào vùng găng (Counting Semaphore).
- Đặc điểm & Phân biệt với Mutex
    - Ownership (Quyền sở hữu): Mutex có khái niệm chủ sở hữu (chỉ tiến trình giữ khóa mới được phép mở khóa). Semaphore không có khái niệm chủ sở hữu chặt chẽ như vậy (bất kỳ tiến trình nào cũng có thể gọi up()).

    - Ứng dụng: Trong Linux Kernel hiện đại, Mutex được ưu tiên sử dụng cho bài toán khóa độc quyền thông thường (down_interruptible() / up() hoặc mutex_lock() / mutex_unlock()), trong khi Semaphore thường dùng khi cần quản lý pool tài nguyên có số lượng cố định hoặc truyền tín hiệu đồng bộ giữa các tiến trình.

### APIs
``` C
#include <linux/semaphore.h>
```
#### APIs Khởi tạo
##### Khởi tạo tĩnh
``` C
static DEFINE_SEMAPHORE(my_sem, n); /* n là giá trị bộ đếm ban đầu */
```
##### Khởi tạo động
``` C
void sema_init(struct semaphore *sem, int val);
```
#### APIs Chiếm (down) và giải phóng (up)
``` C
void down(struct semaphore *sem);
int down_interruptible(struct semaphore *sem); /* Trả về 0 nếu thành công, -EINTR nếu bị signal đánh thức */
int down_trylock(struct semaphore *sem);       /* Trả về 0 nếu thành công, khác 0 nếu không lấy được */
void up(struct semaphore *sem);
```


