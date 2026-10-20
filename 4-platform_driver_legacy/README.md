# Platform device driver (legacy) 
## I. Giới thiệu chung
- Platform Driver là trình điều khiển dành cho các Platform Device — những thiết bị phần cứng tích hợp trực tiếp trên bo mạch (SoC - System on Chip hoặc System-on-Module) mà không có cơ chế tự phát hiện (auto-discovery / plug-and-play) như PCI, PCIe hay USB.

Ví dụ trên BeagleBone Black (BBB), các thiết bị sau thường được quản lý bằng platform driver:
- GPIO controller.
- UART.
- I2C controller.
- SPI controller.
- CAN controller (DCAN).
- Một số bộ điều khiển ADC, PWM và watchdog.

### legacy driver 
- Platform Device Driver (Legacy) thường chỉ cách viết driver theo mô hình platform device truyền thống, trong đó lập trình viên tự tạo và đăng ký platform_device bằng code C, thay vì để kernel tự tạo thiết bị từ Device Tree.

## II. Platform device
### 1. Data struct
#### struct platform_device
- struct platform_device dùng để mô tả hardware device cố định trên SOC
``` C
struct platform_device {
    const char      *name;     
    int             id;        	 
    struct device   dev;
    u32             num_resources;
    struct resource *resource;
};
```

#### struct device

``` C
void plf_release(struct device *dev)
{}

.dev = {
    .platform_data = &platform_pdata,   
    .release = plf_release         
}
```

- ví dụ
``` C
struct {
	int data1;
	// ....
}device_data;

void plf_release(struct device *dev)
{
	pr_info("Device release\n");
}

// Khơi tạo mydata
struct device_data my_data = {.data1= //...};

struct platform_device my_device_1 = {
	.name = "device_1",
	.id = 0,
	.dev = {
		.platform_data = &my_data,
		.release = plf_release
	}
};
```

### 2. API
#### platform_device_register
- Cấp phát một platform_device
``` C
int platform_device_register(struct platform_device *pdev);
```

#### platform_add_devices
- Cấp phát nhiều platform_device
``` C
int platform_add_devices(struct platform_device **pdevs, int ndev);
```
#### platform_device_unregister
- Hủy cấp phát  
``` C
int platform_device_unregister(struct platform_device *pdev);
```
## III. Platform driver
### 1. Data struct
- `struct platform_driver`
``` C
struct platform_driver {
    int (*probe)(struct platform_device *);
    int (*remove)(struct platform_device *);

    void (*shutdown)(struct platform_device *);
    int (*suspend)(struct platform_device *, pm_message_t state);
    int (*resume)(struct platform_device *);

    struct device_driver driver;
};
```
- `struct device_driver`
``` C
struct device_driver {
    const char              *name; // tên device_driver
    const struct bus_type   *bus;
    struct module           *owner;

    const struct of_device_id *of_match_table;

    int (*probe)(struct device *dev);
    int (*remove)(struct device *dev);

    struct driver_private *p;
};
```
### 2. API
- <linux/platform_device.h>
#### platform_driver_register
- 
```
int platform_driver_register(struct platform_driver *drv);
```
khi register thành công driver sẽ được add vào `/sys/bus/platform/drivers/"driver.name"`
#### platform_driver_unregister
```	
void platform_driver_unregister(struct platform_driver *drv);
```

## Matching device và driver (legacy)
- Độ ưu tiên matching 
	- Device Tree (of_match_table): So khớp thiết bị với bảng compatible
	- id matching (platform_device_id): So khớp tên thiết bị với bảng ID của driver
	- name matching: So sánh tên device với tên driver
- Đối với legacy driver: cơ chế matching chính là id matching và name matching
- khi device và driver được insmod vào hệ thống. Kernel match device và driver bằng cách so sánh device.name và driver.name
- Khi quá trình matching thành công hàm probe() trong driver sẽ được call 

### Name matching
- device
``` C
static struct platform_device pcdev_device = {
    .name = "my_platform_driver",
    .id = -1,
};
```
- driver
``` C
static struct platform_driver pcdev_driver = {
    .probe = pcdev_probe,
    .remove = pcdev_remove,

    .driver = {
        .name = "my_platform_driver",
    },
};
```
- khi platform_device.name == platform_driver.driver.name quá trình binding sảy ra
``` C
platform_device.name = "pcdev"
platform_driver.driver.name = "pcdetablev"
```
### Id matching
- ID matching là cơ chế dùng để xác định platform_device nào phù hợp với platform_driver thông qua bảng struct platform_device_id
- Linux lấy trường name của platform_device, so sánh với trường .name trong từng phần tử của pcdev_ids[]. Nếu khớp, driver có thể được binding với device thông qua ID matching.
#### Platform driver
``` C
struct platform_device_id my_plfdevs_ids[] = {
    { .name = "my_platform_device_1", .driver_data = 0 },
    { } 
};

struct platform_driver my_platform_driver = {
    .probe    = my_plf_driver_probe,
    .remove   = my_plf_driver_remove,
    .id_table = my_plfdevs_ids, // id table
    .driver   = {
        .name = "my-platform-device"
    },
};
```
```
struct platform_device my_platform_device1 = {
    .name = "my_platform_device_1",
    .id = -1,
    ...
};
```

## API Trao đổi dữ liệu giữa device và driver
### dev_get_platdata
### platform_set_drvdata
### platform_get_device_id

 
