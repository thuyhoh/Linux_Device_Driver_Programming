# Start with hello_kernel module
## I. tải phiên bản linux-kernel-header

``` shell
sudo apt install linux-headers-`uname -r`
```
## II. thông tin cơ bản file kernel modules 
- ví dụ về hello kernel modules cơ bản
``` C
#include <linux/module.h>  /* Thu vien nay dinh nghia cac macro nhu module_init va module_exit */
#include <linux/fs.h>      /* Thu vien nay dinh nghia cac ham allocate major & minor number */

#define DRIVER_AUTHOR "xxxxxxxx@gmail.com" // thông tin về tác giả
#define DRIVER_DESC   "Hello world kernel module" // mô tả về Driver
#define DRIVER_VERS   "1.0" // phiên bản của driver
#define DRIVER_LICENSE "GPL" // giấy phép của driver

/* Constructor */
static int  __init hello_world_init(void)
{
    printk(KERN_INFO "Hello world kernel module\n");
    return 0;
}

/* Destructor */
static void  __exit hello_world_exit(void)
{
    printk(KERN_INFO "Goodbye\n");
}

module_init(hello_world_init);
module_exit(hello_world_exit);

MODULE_LICENSE(DRIVER_LICENSE); 
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);  
MODULE_VERSION(DRIVER_VERS);
```
### 1. Các thành phần trong kernel modules file cơ bản
#### i. Phần đầu (Header Includes)
``` C
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
```
#### ii. Thông tin mô tả Module (Module Metadata)
``` C
MODULE_LICENSE(DRIVER_LICENSE); 
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);  
MODULE_VERSION(DRIVER_VERS);
```
  - MODULE_LICENSE(): Quyền sử dụng (bắt buộc, nếu không sẽ cảnh báo khi load).
  - MODULE_AUTHOR(): Thông tin tác giả.
  - MODULE_DESCRIPTION(): Mô tả ngắn.
  - MODULE_VERSION(): Thông tin phiên bản
#### iii. Hàm khởi tạo module (Module Init Function)
- Hàm này được gọi khi module được load vào kernel
``` C
static int  __init hello_world_init(void)
{
    printk(KERN_INFO "Hello world kernel module\n");
    return 0;
}
```
- Gắn với macro:
``` C
module_init(hello_world_init);
```
#### iv. Hàm cleanup / exit (Module Exit Function)
- Hàm được gọi khi module bị gỡ bỏ (rmmod).
``` C
static void  __exit hello_world_exit(void)
{
    printk(KERN_INFO "Goodbye\n");
}
```
- gắn với macro:
``` C
module_exit(hello_world_exit);
```
#### v. Các hàm xử lý logic chính (Tùy chọn)
- Có thể thêm các chức năng chính: quản lý thiết bị, xử lý syscall, hooks vào netfilter, hoặc bất kỳ xử lý nào liên quan đến kernel.
#### vi. Makefile
``` Makefile
# Hello world kernel module

EXTRA_CFLAGS = -Wall
obj-m = hello.o

KDIR = /lib/modules/`uname -r`/build

all:
	make -C $(KDIR) M=`pwd` modules

clean:
	make -C $(KDIR) M=`pwd` clean
```
#### load kernel module vào kernel
``` shell
insmod driver.ko
```
#### remove kernel module khỏi kernel
``` shell
rmmod driver.ko
```



