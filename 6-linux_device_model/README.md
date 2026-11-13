# Linux Device Driver Model
## I. Giới thiệu chung

- **Linux Device Model** là cơ chế giúp kernel quản lý thống nhất các thiết bị, driver, bus và quan hệ giữa chúng. Thay vì mỗi subsystem tự xây dựng toàn bộ cơ chế quản lý, kernel cung cấp một driver core dùng chung.

- Mô hình này hỗ trợ:

    - Đăng ký và gỡ bỏ thiết bị, driver.
    - Tìm driver phù hợp với thiết bị, gọi là **matching**.
    - Gắn thiết bị với driver, gọi là **binding**.
    - Quản lý quan hệ cha–con và vòng đời đối tượng.
    - Phối hợp quản lý nguồn và xử lý thêm/bớt thiết bị.
    - Biểu diễn một phần thông tin ra userspace thông qua **sysfs**.

- **sysfs** là giao diện quan sát và tương tác với các đối tượng kernel qua thư mục, file thuộc tính và liên kết. sysfs thường được mount tại `/sys`; nó không thay thế cơ chế quản lý đối tượng bên trong kernel.

## II. Các thành phần chính

| Thành phần | Cấu trúc kernel | Vai trò |
|---|---|---|
| Device | `struct device` | Đại diện chung cho một thiết bị trong device model |
| Driver | `struct device_driver` | Mô tả driver và các thao tác liên quan |
| Bus | `struct bus_type` | Tổ chức device, driver và cung cấp quy tắc matching |
| Class | `struct class` | Nhóm thiết bị theo chức năng để userspace dễ tìm |
| Kobject | `struct kobject` | Cung cấp tên, quan hệ cha–con, đếm tham chiếu và nền tảng tích hợp sysfs |
| Kobject type | `struct kobj_type` | Mô tả hành vi của loại đối tượng, gồm `release` và `sysfs_ops` |
| Kset | `struct kset` | Tập hợp các kobject có liên quan |

## V. Sysfs

- Sysfs là filesystem ảo để xuất thông tin, thuộc tính và quan hệ của các đối tượng kernel ra userspace. Kernel cần bật `CONFIG_SYSFS=y`. Trên hệ thống Linux thông thường, sysfs đã được mount tại `/sys`. 

```bash
findmnt -t sysfs
```

- Nếu chưa mount, có thể dùng:

```bash
sudo mount -t sysfs sysfs /sys
```

### 2. Các đường dẫn quan trọng

| Đường dẫn | Ý nghĩa |
|---|---|
| `/sys/devices/` | Phân cấp các đối tượng device |
| `/sys/bus/` | Góc nhìn theo bus, gồm device và driver |
| `/sys/class/` | Góc nhìn theo chức năng thiết bị |
| `/sys/module/` | Thông tin về các module được biểu diễn trong sysfs |
| `/sys/kernel/` | Một số đối tượng và giao diện chung của kernel |



### 3. Thuộc tính trong sysfs

- Một file thuộc tính thể hiện một thông tin hoặc giá trị cấu hình, ví dụ:
  - Đọc `value` dẫn đến callback `show()`.
  - Ghi `value` dẫn đến callback `store()`.
  - Giá trị thực nằm trong trạng thái của driver; không được lưu như một file dữ liệu trên đĩa.

```bash
cat /sys/bus/platform/devices/demo-device/value
echo 123 | sudo tee /sys/bus/platform/devices/demo-device/value
```

- Thuộc tính sysfs thông thường nên có định dạng văn bản đơn giản, ưu tiên một giá trị mỗi file. Sysfs phù hợp để cung cấp trạng thái và cấu hình nhỏ; dữ liệu nhị phân hoặc luồng I/O cần giao diện được thiết kế riêng. 
## VI. Attribute và callback

### 1. `struct attribute`

- Định nghĩa nằm trong `<linux/sysfs.h>`.

```c
struct attribute {
    const char *name;
    umode_t mode;
    // ...
};
/**
  * `name` là tên file
  * `mode` là quyền truy cập. 
*/
```

- Các quyền thường gặp:

| Mode | Quyền truy cập |
|---|---|
| `0444` | Mọi người được đọc |
| `0400` | Chỉ owner được đọc |
| `0200` | Chỉ owner được ghi |
| `0644` | Mọi người được đọc, chỉ owner được ghi |
| `0600` | Chỉ owner được đọc và ghi |

- Note: Owner của thuộc tính sysfs thường là `root`. Trong phiên bản cũ, có các macro `S_IRUGO`(`0444`), `S_IRUSR`(`0600`), và `S_IRUGO | S_IWUSR`(`0644`).

### 2. `struct device_attribute`

- Để tạo thuộc tính cho `struct device`, dùng wrapper được khai báo trong `<linux/device.h>`:

```c
struct device_attribute {
    struct attribute attr;
    ssize_t (*show)(struct device *dev, struct device_attribute *attr, char *buf);
    ssize_t (*store)(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);
    // ...
};
/**
  * `show` : callback khi có lệnh hiển thị dữ liệu từ /sys/bus/platform/devices/<new-device>/<attr> 
  * `store`: callback khi có lệnh ghi dữ liệu vào /sys/bus/platform/devices/<new-device>/<attr> 
*/
```


### 3. Quy ước của `show()`

- Ghi kết quả vào `buf` do sysfs cung cấp.
- Dùng `sysfs_emit()` hoặc `sysfs_emit_at()` để định dạng kết quả.
- Thường kết thúc giá trị văn bản bằng `\n`.
- Trả về số byte đã ghi, không tính ký tự NUL kết thúc.
- Có thể trả mã lỗi âm.

#### sysfs_emit
- Ghi một chuỗi định dạng (format string) bắt đầu từ đầu buffer (buf).
- Trả về số lượng ký tự đã được ghi vào buffer (không tính ký tự null kết thúc)

```c
int sysfs_emit(char *buf, const char *fmt, ...);
```
#### sysfs_emit_at
- Ghi một chuỗi định dạng bắt đầu từ một vị trí bù (offset) cụ thể tính từ đầu buffer buf

``` C
int sysfs_emit_at(char *buf, int at, const char *fmt, ...);
```
### 4. Quy ước của `store()`
- Phân tích đầu vào và kiểm tra lỗi chuyển đổi.
- Kiểm tra miền giá trị hợp lệ.
- Đồng bộ khi truy cập trạng thái dùng chung.
- Cập nhật trạng thái hoặc phần cứng.
- Trả về `count` khi xử lý thành công toàn bộ dữ liệu; trả mã lỗi âm nếu thất bại.


## VII. Macro và API tạo thuộc tính
### API
``` C
#include <linux/sysfs.h>
```
``` C
struct kobject *kobject_create_and_add(const char *name, struct kobject *parent);
```
- struct kobject *parent

| Đối tượng | Thư mục đại diện | Nếu tạo đối tượng con tên `"demo"` |
|---|---|---|
| `kernel_kobj` | `/sys/kernel` | `/sys/kernel/demo` |
| `mm_kobj` | `/sys/kernel/mm` | `/sys/kernel/mm/demo` |
| `firmware_kobj` | `/sys/firmware` | `/sys/firmware/demo` |
| `power_kobj` | `/sys/power` | `/sys/power/demo` |
| `hypervisor_kobj` | `/sys/hypervisor` | `/sys/hypervisor/demo` |
| `fs_kobj` | `/sys/fs` | `/sys/fs/demo` |

- Ví dụ mô phỏng
``` C
static struct kobject *kobj;
demo_kobj = kobject_create_and_add("demo", kobj); /*/sys/kernel/<demo>*/

```
#### sysfs_##_file()
- Tạo một file thuộc tính trong sysfs
``` C
int sysfs_create_file(struct kobject *kobj, const struct attribute *attr);
```
- Xóa một file thuộc tính trong sysfs
``` C
void sysfs_remove_file(struct kobject *kobj, const struct attribute *attr);
```
- Ex
``` C 
static struct device_attribute dev_attr_value = {
    .attr = {
        .name = "value",
        .mode = 0444,
    },
    .show = value_show,
    .store = value_store,
};

/* __init */
ret = sysfs_create_file(kobj, dev_attr_value.attr);

/* __exit */
sysfs_remove_file(kobj, dev_attr_value.attr);
```
#### sysfs_##_group()
``` C
int sysfs_create_group(struct kobject *kobj, const struct attribute_group *grp);
```
``` C
void sysfs_remove_group(struct kobject *kobj, const struct attribute_group *grp);
```
-  Ví dụ
``` C
static struct attribute *settings_attrs[] = {
    &value_attr.attr,
    &enable_attr.attr,
    NULL,
};

static const struct attribute_group settings_group = {
    .name = "settings",
    .attrs = settings_attrs,
};

/* __init */
int ret = sysfs_create_group(kobj, &settings_group);
/* __exit */
sysfs_remove_group(kobj, &settings_group)
```
#### sysfs_##_groups()
``` C
int sysfs_create_groups( struct kobject *kobj, const struct attribute_group **groups);
```
``` C
void sysfs_remove_groups( struct kobject *kobj, const struct attribute_group **groups);
```
#### sysfs_chmod_file()
``` C
int sysfs_chmod_file(struct kobject *kobj, const struct attribute *attr, umode_t mode);
```
-  Ví dụ
``` C
ret = sysfs_chmod_file(kobj, &value_attr.attr, 0444);
```
### Macro `DEVICE_ATTR`
``` C
#include <linux/device.h>
```
#### DEVICE_ATTR
``` C
DEVICE_ATTR(attr, mode, attr_show, attr_store)
```
=>  tương đương 
``` C
static struct device_attribute dev_attr_value = {
    .attr = {
        .name = "attr",
        .mode = mode,
    },
    .show = attr_show,
    .store = attr_store,
};
```
#### DEVICE_ATTR_RW
``` C
DEVICE_ATTR_RW(attr)
```
=>  tương đương 
``` C
static struct device_attribute dev_attr_value = {
    .attr = {
        .name = "attr",
        .mode = 0644,
    },
    .show = attr_show,
    .store = attr_store,
};
```
#### DEVICE_ATTR_RO
``` C
DEVICE_ATTR_RW(attr)
```
=>  tương đương 
``` C
static struct device_attribute dev_attr_value = {
    .attr = {
        .name = "attr",
        .mode = 0644,
    },
    .show = attr_show,
    .store = NULL,
};
```
#### DEVICE_ATTR_WO
``` C
DEVICE_ATTR_WO(attr)
```
=>  tương đương 
``` C
static struct device_attribute dev_attr_value = {
    .attr = {
        .name = "value",
        .mode = 0444,
    },
    .show = NULL,
    .store = value_store,
};
```

<!-- kobject_put
kobject_get -->