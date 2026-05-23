# Character Driver
## I. khái niệm
- Character Device File là một loại tệp đặc biệt trong hệ điều hành Unix/Linux dùng để giao tiếp với thiết bị phần cứng theo kiểu truyền dữ liệu từng ký tự (byte) một cách tuần tự.
- Các thiết bị được đại diện bởi các tệp đặc biệt trong thư mục
### Chuẩn cấu trúc Character Device
- cấp phát device number
- khởi tạo cdev struct
- thêm cdev vào system
- khởi tạo /sys/class/...
- khởi tạo /dev/...

## II. Device number
- Device number: là các giá trị số nguyên được sử dụng để nhận diện và quản lý các thiết bị (devices) trong hệ thống, đặc biệt là trong giao tiếp với kernel thông qua các tệp đặc biệt trong thư mục.
- Nó bao gồm major number và minor number:
  - Major number: Xác định loại Kernel driver nào phụ trách tương tác thiết bị
  - Minor Number: phân biệt giữa các thiết bị cùng sử dụng chung 1 driver
### 1. Device number representation
- Macro xác định major_number(12 bit) và minor_number(20 bit) thông qua device_number(dev_t => uint32_t) nằm trong thư viện`include/linux/kdev_t.h`
``` C
dev_t device_number;
int minor_no = MINOR(device_number);
int major_no = MAJOR(device_number);
```
- Ghép major_number và minor_number thành device_number 
```C
MKDEV(int major, int minor);
```
### 2. Dynamically register a range of char device numbers
- Cấp phát device number có thể sử dụng một trong hai cách:
- Cấp phát device number. Phương thức cấp phát cho device number nằm trong thư viện: `linux/fs.h`

``` C
/**
 * @brief: một hàm trong Linux Kernel dùng để đăng ký ngả (range) số thiết bị ký tự một cách động, thay vì phải chỉ định cố định major number trong mã nguồn
 * dev_t *dev	Con trỏ tới biến dev_t để nhận kết quả: chứa cả major và minor đầu tiên.
 * baseminor	Số minor đầu tiên trong dải bạn muốn cấp (thường là 0).
 * count	Số lượng thiết bị ký tự (số minor number) bạn cần cấp.
 * name	Tên chuỗi để hiển thị trong /proc/devices hoặc các công cụ chẩn đoán.
 **/
int alloc_chrdev_region(dev_t *dev, unsigned baseminor, unsigned count, const char *name);
```

![P1](./imgs/P1.png)

### 3. Unallocate device number 
``` C
/**
 * @brief: hàm kernel dùng để hủy đăng ký (giải phóng) vùng số thiết bị (device number region) mà bạn đã cấp phát trướ
 * @param[in]: from đại diện cho major & minor bắt đầu (thường được trả về bởi alloc_chrdev_region() hoặc tạo bằng MKDEV(major, minor))
 * @param[in]: count: Số lượng minor number bạn muốn hủy đăng ký (giống như lúc bạn đã đăng ký)
**/
void unregister_chrdev_region(dev_t from, unsigned count);
```

## III. Cdev structure
- `include/linux/cdev.h`
![P6](./imgs/P6.png)
### 1. THIS_MODULE macro
- THIS_MODULE là một macro trong Linux kernel, dùng bên trong mã kernel module để chỉ chính module hiện tại.
- `include/linux/export.h`
``` C
#ifdef MODULE
extern struct module __this_module;
#define THIS_MODULE (&__this_module)
#else
#define THIS_MODULE ((struct module *)0)
#endif
```

### 2. Initialize a cdev structure
``` C
/**
 * @brief: khởi tạo một cấu trúc cdev
 * @param[in]cdev: Con trỏ tới struct cdev (biến đại diện cho thiết bị ký tự).
 * @param[in]fops: Con trỏ tới cấu trúc file_operations chứa các hàm xử lý file operations như open, read, write...
*/
void cdev_init(struct cdev *cdev, const struct file_operations *fops);
```
### 3. Add a char device to the Kernel VFS
``` C
/**
 * @brief: đăng ký character device vào hệ thống thiết bị (device model) của Linux.
 * @param[in] p: Con trỏ tới struct cdev đã được khởi tạo (thường bằng cdev_init()).
 * @param[in] dev: Số thiết bị (device number), thường là kết quả của MKDEV(major, minor), hoặc alloc_chrdev_region() cấp
 * @param[in] count: Số lượng minor devices mà driver này quản lý bắt đầu từ dev. Thường là 1 cho driver đơn giản. 
 * @retval: < 0 false
*/
int cdev_add(struct cdev *p, dev_t dev, unsigned count);
```
### 4. Delete cdev struct
``` C
/**
 * @param[in] cdev:
*/
cdev_del(struct cdev_t *cdev);
```


## IV. Device file
- `include/linux/device.h`
### Create and register a class with sysfs
``` C
/**
 * @brief: tạo một "class" thiết bị trong /sys/class, nhằm hỗ trợ việc tạo device node tự động trong /dev
 * @param[in] owner: Thường là THIS_MODULE, chỉ ra module nào sở hữu class này. 
 * @param[in] name: Tên class, sẽ hiển thị trong /sys/class/<name>/.
 * @retval: == NULL -> false
*/
struct class *class_create(struct module *owner, const char *name);
```
### Destroys a struct class structure
``` C
/**
  *
*/
void class_destroy(struct class *cls);
```


### /dev/...
``` C
/**
 * @param[in] class:	Con trỏ tới class mà bạn đã tạo từ class_create().
 * @param[in] parent:	Thiết bị cha, thường là NULL nếu không có.
 * @param[in] devt:	Device number (dev_t), chứa major và minor number
 * @param[in] drvdata:	Con trỏ dữ liệu riêng của driver, có thể là NULL hoặc struct của bạn.
 * @param[in] fmt: , ...	Format string để đặt tên thiết bị (ví dụ "mydevice%d", ...)
*/
struct device *device_create(struct class *class, struct device *parent,
                             dev_t devt, void *drvdata, const char *fmt, ...);
```
### Remove a device
``` C
void device_destroy(struct class *class, dev_t devt);
```

## V. File operation modthods
- character driver sẽ hỗ trợ xử lý cho các lệnh gọi hệ thống người dùng: open, close, read, write, llseek.
- `include/linux/fs.h`

### 1. file operation structure
![P5](./imgs/P5.png)
### 2. Data copying between Kernel space and user space
- dữ liệu không thể tự trao đổi giữa kernel và user một cách bình thường mà phải thông qua các hàm chuyên dụng như copy_to_user, copy_from_user.
- Vai trò của hai hàm này:
  - Sao chép dữ liệu giữa không gian người dùng và không gian hạt nhân
  - Kiểm tra xem con trỏ không gian người dùng có hợp lệ hay không
  - Nếu con trỏ không hợp lệ, không thực hiện sao chép.
  - Nếu gặp địa chỉ không hợp lệ trong quá trình sao chép, chỉ một phần dữ liệu được sao chép. Trong cả hai trường hợp, giá trị trả về là lượng bộ nhớ vẫn cần được sao chép
#### 2.1 __user macro
- Đây là một macro được sử dụng với các con trỏ cấp người dùng, báo cho nhà phát triển biết rằng không nên tin tưởng hoặc coi đó là một con trỏ hợp lệ để tránh các lỗi trong nhân hệ điều hành.

- GCC không quan tâm bạn có sử dụng macro __user với con trỏ cấp người dùng hay không. Điều này được kiểm tra bởi sparse, một công cụ kiểm tra ngữ nghĩa của nhân Linux, để tìm các lỗi lập trình có thể xảy ra.
#### 2.2 copy_to_user
``` C
unsigned long copy_to_user(void __user *to, const void *from, unsigned long n);
```
#### 2.3 Copy_from_user
``` C
unsigned long copy_from_user(void *to, const void __user *from, unsigned long n);
```
![P4](./imgs/P4.png)

- `include/linux/uaccess.h`

#### open method
![P3](./imgs/P3.png)
