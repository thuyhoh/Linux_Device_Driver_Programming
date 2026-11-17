# Linux synchronization services
## Cacs khai niem chung
- Linux synchronization services bao gồm các cơ chế đồng bộ hóa luồng/tiến trình trong nhân Linux nhằm tránh xung đột dữ liệu (race condition) và bảo vệ tính toàn vẹn của hệ thống khi chạy đa nhiệm.

- Trong lập trình đa luồng (multithreading) và hệ thống nhúng/lập trình hệ thống, khi nhiều luồng hoặc tiến trình cùng chạy song song và tương tác với nhau, việc quản lý quyền truy cập dữ liệu đóng vai trò quyết định tính đúng đắn của chương trình.

- Trong Kernel Space, bài toán Race Condition và bảo vệ Critical Section phức tạp hơn nhiều so với User Space. Một lỗi race condition ở User Space chỉ làm crash ứng dụng, nhưng trong Kernel, nó có thể gây Kernel Panic, làm treo toàn bộ hệ thống hoặc hỏng dữ liệu phần cứng vĩnh viễn.
### Race Condition
- Bản chất: Là hiện tượng kết quả thực thi của mã nguồn nhân phụ thuộc vào thứ tự lập lịch (scheduling order) ngẫu nhiên giữa các tiến trình, các lõi CPU (SMP) hoặc giữa tiến trình nền và trình phục vụ ngắt (ISR).

- Nguyên nhân: Các luồng/ngắt cùng truy cập một tài nguyên dùng chung mà không có cơ chế tuần tự hóa, dẫn đến hiện tượng ghi đè dữ liệu (Lost Update) hoặc đọc dữ liệu chưa hoàn tất.
### share resource
- Định nghĩa: Bất kỳ vùng dữ liệu, cấu trúc dữ liệu toàn cục nào trong kernel (struct cdev, hàng đợi vòng kfifo, danh sách thiết bị list_head) hoặc các thanh ghi phần cứng (MMIO) mà nhiều thực thể trong kernel cùng đọc/ghi.

- Đặc thù trong Kernel: Tài nguyên dùng chung có thể được truy cập đồng thời từ nhiều nguồn:

    - Tiến trình người dùng qua lời gọi hệ thống (System Call / Process Context).

    - Ngắt cứng và ngắt mềm (Interrupt Context / SoftIRQ / Tasklet).
### critical section
- Định nghĩa: Đoạn mã nguồn thực hiện các thao tác đọc-sửa-ghi (Read-Modify-Write) trên Shared Resource.

- Yêu cầu: Tại một thời điểm, chỉ cho phép tối đa một thực thi độc quyền (exclusive execution) được phép bước vào vùng găng này.
<!-- ### Tinh huong -->

## Linux locking services
### spinlockn (spinlock_t)
- Nguyên tắc hoạt động: Cơ chế khóa bận (Busy-waiting / Spin-lock). Khi một luồng cố gắng lấy khóa đang bị giữ, nó không chuyển sang trạng thái ngủ mà liên tục quay vòng (loop) kiểm tra trạng thái khóa trên CPU cho đến khi khóa được giải phóng.

- Đặc điểm & Ràng buộc:

    - Không được phép ngủ (Sleep): Tuyệt đối không gọi các hàm có thể gây ngủ (như kmalloc(..., GFP_KERNEL), copy_to_user(), msleep(), hoặc mutex_lock()) bên trong vùng được bảo vệ bởi spinlock.

    - Sử dụng trong mọi ngữ cảnh: Là cơ chế duy nhất an toàn để sử dụng đồng thời trong cả Process Context và Interrupt Context (ISR).
- Các biến thể phổ biến:

    - spin_lock(&lock) / spin_unlock(&lock): Dùng khi chắc chắn không bị ngắt chen ngang trên cùng CPU.

    - spin_lock_irqsave(&lock, flags) / spin_unlock_irqrestore(&lock, flags): Vừa lấy khóa vừa tắt ngắt cục bộ trên CPU hiện tại, bắt buộc phải dùng khi chia sẻ dữ liệu giữa tiến trình và ngắt cứng để tránh deadlock.
### Mutex
- Nguyên tắc hoạt động: Cơ chế khóa độc quyền có hỗ trợ ngủ (Sleep-able Lock). Khi một tiến trình không thể lấy được mutex, nó sẽ được đưa vào hàng đợi chờ (sleep queue) và bộ lập lịch sẽ chuyển CPU cho tiến trình khác.
Đặc điểm & Ràng buộc:

    - Chỉ dùng trong Process Context: Nghiêm cấm sử dụng trong Interrupt Context (ISR, SoftIRQ, Tasklet) vì các ngữ cảnh ngắt không được phép ngủ.

    - Cho phép Sleep bên trong: Lập trình viên có thể thoải mái gọi các hàm cấp phát bộ nhớ (kmalloc với GFP_KERNEL), copy dữ liệu với user-space, hoặc gọi các hàm I/O gây block bên trong vùng găng của mutex.
### Semaphore (struct semaphore)\
- Nguyên tắc hoạt động: Cơ chế đồng bộ hóa dựa trên bộ đếm số nguyên (Counter-based Synchronization). Giống như Mutex, semaphore đưa tiến trình vào trạng thái ngủ khi tài nguyên chưa sẵn sàng, nhưng cho phép một số lượng luồng giới hạn ($N$ luồng) cùng lúc truy cập vào vùng găng (Counting Semaphore).
- Đặc điểm & Phân biệt với Mutex
    - Ownership (Quyền sở hữu): Mutex có khái niệm chủ sở hữu (chỉ tiến trình giữ khóa mới được phép mở khóa). Semaphore không có khái niệm chủ sở hữu chặt chẽ như vậy (bất kỳ tiến trình nào cũng có thể gọi up()).

    - Ứng dụng: Trong Linux Kernel hiện đại, Mutex được ưu tiên sử dụng cho bài toán khóa độc quyền thông thường (down_interruptible() / up() hoặc mutex_lock() / mutex_unlock()), trong khi Semaphore thường dùng khi cần quản lý pool tài nguyên có số lượng cố định hoặc truyền tín hiệu đồng bộ giữa các tiến trình.