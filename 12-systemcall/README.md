# Systemcall in Linux


## I. Tổng quan về System Call
* **Định nghĩa:** System Call là giao diện lập trình duy nhất cho phép các tiến trình ở không gian người dùng (**User Space**) giao tiếp và yêu cầu tài nguyên phần cứng hoặc dịch vụ từ nhân hệ điều hành (**Kernel Space**).
* **Mục đích:**
  * Đóng vai trò là lớp trừu tượng hóa phần cứng (Hardware Abstraction Layer).
  * Đảm bảo tính ổn định và bảo mật cho hệ thống bằng cách ngăn chặn các ứng dụng truy cập trực tiếp vào phần cứng hoặc vùng nhớ của nhân.

## II. Cơ chế hoạt động & Thực thi

### Quy trình gọi System Call
1. **Ứng dụng gọi API thư viện C:** Thay vì gọi trực tiếp system call, lập trình viên thường gọi các hàm chuẩn từ thư viện C (`glibc`), ví dụ: `printf()`.
2. **Thư viện C kích hoạt System Call:** Bên dưới, `printf()` sẽ gọi hàm bọc (wrapper) tương ứng là system call `write()`.
3. **Định danh bằng số hiệu (System Call Number):** Mỗi system call có một số hiệu duy nhất được đăng ký trong bảng `sys_call_table`.
4. **Truyền tham số:** Số hiệu system call và các đối số được nạp vào các thanh ghi (registers) của CPU trước khi chuyển quyền điều khiển vào nhân.

### System Call Handler
* Khi có yêu cầu, một ngắt phần mềm (ví dụ: `int 0x80` trên kiến trúc x86 cũ hoặc lệnh `syscall`) được kích hoạt để chuyển CPU từ chế độ **User Mode** sang **Kernel Mode**.
* **System Call Handler** tiếp nhận số hiệu từ thanh ghi, đối chiếu với `sys_call_table` và gọi hàm xử lý tương ứng trong nhân.
* Sau khi hoàn tất, kết quả (hoặc mã lỗi qua biến toàn cục `errno`) được trả về cho tiến trình ở User Space.


##  III. Trao đổi dữ liệu giữa User Space và Kernel Space
Do hai vùng nhớ được tách biệt hoàn toàn để đảm bảo an toàn, nhân Linux không đọc/ghi trực tiếp con trỏ từ người dùng mà sử dụng hai hàm chuyên biệt:
* `copy_from_user()`: Sao chép dữ liệu từ không gian người dùng vào không gian nhân.
* `copy_to_user()`: Sao chép dữ liệu từ không gian nhân trả về cho người dùng.

> **Lưu ý:** Các hàm này luôn kiểm tra tính hợp lệ của địa chỉ bộ nhớ và quyền hạn thực thi (thông qua các cơ chế kiểm tra quyền như `capable()`) trước khi thao tác.


##  IV. Các bước cơ bản để triển khai System Call mới
1. **Đăng ký số hiệu:** Thêm một mục mới vào bảng hệ thống (`sys_call_table`).
2. **Khai báo nguyên mẫu:** Định nghĩa hàm trong tệp header (`syscall.h`).
3. **Viết mã xử lý:** Cài đặt logic cho system call mới trong mã nguồn nhân Linux (sử dụng các macro như `SYSCALL_DEFINE...`).
4. **Biên dịch và sử dụng:** Biên dịch lại nhân Linux và gọi system call từ User Space thông qua hàm `syscall(<số_hiệu>, <tham_số>...)`.


##  V. Khi nào nên TRÁNH tạo System Call mới?
Việc thêm system call mới thường không được khuyến khích trừ khi thực sự cần thiết vì:
* **Khó bảo trì và thay đổi:** Giao diện system call một khi đã phát hành phải giữ tính tương thích ngược lâu dài, rất khó sửa đổi về sau.
* **Giảm tính di động (Portability):** Mã nguồn ứng dụng phụ thuộc vào system call tự định nghĩa sẽ khó chạy trên các phiên bản hoặc bản phân phối Linux khác.

**Giải pháp thay thế được khuyên dùng:**
* Tạo **Device Node** và giao tiếp thông qua các system call chuẩn có sẵn (`open`, `read`, `write`, `ioctl`).
* Sử dụng hệ thống tệp ảo như `/proc` hoặc `/sys` (sysfs) để trao đổi dữ liệu giữa ứng dụng và nhân.

## VI. systemcall implementation
#### Thêm mã nguồn syscall vào \<KERNEL>/
- \<KERNEL>/my_syscall/my_systemcall.c
``` C
#include <linux/kernel.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE1(my_systemcall, int, val)
{
	pr_info("my new system call\n");
	pr_info("my_systemcall called from userspace with val = %d\n", val);
	return val;
}
```
- \<KERNEL>/my_syscall/Makefile
``` make
obj-y := my_sys_call.o
```
- \<KERNEL>/Makefile Thêm vào core-y
``` C
core-y += < kernel/ ... > my_syscall/
```

#### Khai báo prototype và đăng ký số hiệu syscall trong \<KERNEL>/
- Khai báo prototype trong \<KERNEL>/include/linux/syscalls.h
``` C
asmlinkage long sys_my_sys_call(void);
```
- đăng ký số hiệu syscall trong \<KERNEL>/arch/arm/tools/syscall.tbl
``` syscall.tbl
451  common  my_systemcall  sys_my_systemcall
# 451 : systemcall number (note: không được trùng lặp) 
# common : Phạm vi ABI áp dụng.
# my_sys_call : Name Kernel dùng tên này để sinh macro số hiệu tương ứng
# sys_my_sys_call : Tên hàm entry point trong kernel
```

#### Rebuild kernel + reboot devices
### Macro SYSCALL_DEFINEx
| Macro | Số tham số | Cú pháp | 
|---|:---:|---|
| `SYSCALL_DEFINE0` | **0** | `SYSCALL_DEFINE0(name)` |
| `SYSCALL_DEFINE1` | **1** | `SYSCALL_DEFINE1(name, type, name)` | 
| `SYSCALL_DEFINE2` | **2** | `SYSCALL_DEFINE2(name, t1, n1, t2, n2)` |
| `SYSCALL_DEFINE3` | **3** | `SYSCALL_DEFINE3(name, t1, n1, t2, n2, t3, n3)` | 
| `SYSCALL_DEFINE4` | **4** | 4 cặp `type, name` | 
| `SYSCALL_DEFINE5` | **5** | 5 cặp `type, name` | 
| `SYSCALL_DEFINE6` | **6** | 6 cặp `type, name` | 

## VII. User interface
- Giao tiếp giữa user và systemcall thông qua syscall()
``` C
long syscall(long number, ...);
```
#### Example
``` C
#define _GNU_SOURCE
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/syscall.h>

#define __NR_my_systemcall 451

int main(void)
{
  long ret = syscall(__NR_my_systemcall, 10);
  if (ret < 0) {
    perror("syscall failed"); /* tự đọc errno */
    return 1;
  }
  printf("ret = %ld\n", ret);
  return 0;
}
```