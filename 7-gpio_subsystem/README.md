# GPIO subsystem

## Giới thiệu chung
- GPIO — General Purpose Input/Output là các đường tín hiệu số có thể được phần mềm cấu hình để:
  - Input: đọc trạng thái nút nhấn, cảm biến số hoặc tín hiệu từ thiết bị khác.
  - Output: điều khiển LED, chân enable, reset hoặc mạch đóng cắt.
  - Interrupt: phát hiện thay đổi tín hiệu theo cạnh lên, cạnh xuống hoặc mức, tùy phần cứng hỗ trợ.


## Kernel
- Trong Linux kernel driver, có hai lớp API điều khiển GPIO thường gặp:

  - **Legacy integer-based GPIO API (gpio_*)** trong `<linux/gpio.h>`— làm việc trực tiếp với số chân GPIO dạng unsigned int (ví dụ GPIO số 60 trên BeagleBone Black).

  - **GPIO Descriptor API (gpiod_*)** trong `<linux/gpio/consumer.h>` — làm việc qua con trỏ struct gpio_desc *, chuẩn hiện đại gắn chặt với Device Tree (gpios / *-gpios property) và tự động xử lý cực tính (active-high / active-low).

- `struct gpio_desc` là descriptor đại diện cho một GPIO line trong kernel.
  - Note: Đây là một opaque descriptor đối với consumer: driver lấy descriptor qua API, không tự cấp phát hoặc truy cập trực tiếp các trường bên trong.

``` C
struct gpio_desc *my_gpio;
```
  

###  Legacy integer-based GPIO APIs - gpio_*
#### gpio_to_desc() — Cầu nối từ số GPIO sang Descriptor
```C
struct gpio_desc *gpio_to_desc(unsigned gpio);
```
### GPIO Descriptor APIs - gpiod_*

#### APIs request GPIO
##### gpiod_get()
``` C
struct gpio_desc *gpiod_get(struct device *dev, const char *con_id, enum gpiod_flags flags);
```
- 	Thiết bị consumer sử dụng GPIO
- con_id: Tên chức năng của gpio khai báo trong device tree
  - Ex: ``<con_id>-gpios = <...>`` 
- flags: 	Hướng và giá trị khởi tạo
  - GPIOD_IN	Input
  - GPIOD_OUT_LOW	Output, giá trị logic ban đầu 0
  - GPIOD_OUT_HIGH	Output, giá trị logic ban đầu 1
  - PIOD_ASIS	Không khởi tạo lại hướng và giá trị; dùng khi sẽ cấu hình hướng riêng
##### gpiod_get_index() — request một phần tử trong danh sách
``` C
struct gpio_desc *gpiod_get_index(struct device *dev, const char *con_id, unsigned int idx,enum gpiod_flags flags);
```
- idx: chỉ số thứ tự trong mảng gpio-dtb
- Ex:
``` dtb
led-gpios = <&gpio2 5 GPIO_ACTIVE_HIGH>,
            <&gpio2 7 GPIO_ACTIVE_HIGH>;
```
``` C
struct gpio_desc *second_led;

second_led = gpiod_get_index(dev, "led", 1, GPIOD_OUT_LOW); /* Lấy phần tử thứ hai trong led-gpios */
if (IS_ERR(second_led))
    return PTR_ERR(second_led);
```
##### gpiod_get_array() — request cả nhóm GPIO của một chức năng
``` C
struct gpio_descs *gpiod_get_array(struct device *dev, const char *con_id, enum gpiod_flags flags);
```
- Ex:
``` C
struct gpio_descs *leds;

leds = gpiod_get_array(dev, "led", GPIOD_OUT_LOW);
if (IS_ERR(leds))
    return PTR_ERR(leds);
/**
  * leds->ndescs: Số descriptor trong nhóm
  * leds->desc[i]: Descriptor thứ i
  * leds->info	Thông tin hỗ trợ tối ưu truy cập nhóm
*/

```

#### APIs giải phóng GPIO đã request
``` C
void gpiod_put(struct gpio_desc *desc);
```
``` C
void gpiod_put_array(struct gpio_desc *desc);
```

#### APIs tự quản lý gpio
#### APIs set direction
##### gpiod_direction_input() — chuyển GPIO sang input
``` C
int gpiod_direction_input(struct gpio_desc *desc);
```
##### gpiod_direction_output() — chuyển sang output và đặt giá trị ban đầu
``` C
int gpiod_direction_output(struct gpio_desc *desc, int value);
```
#### APIs get/set values
##### gpiod_get_value() và gpiod_get_value_cansleep() — đọc trạng thái
``` C
int gpiod_get_value(const struct gpio_desc *desc);

int gpiod_get_value_cansleep(const struct gpio_desc *desc);
```
| Kết quả | Ý nghĩa |
|---|---|
| `0` | Inactive |
| `1` | Active |
| `< 0` | Lỗi |

##### gpiod_set_value() và gpiod_set_value_cansleep() — ghi trạng thái

``` C
void gpiod_set_value(struct gpio_desc *desc, int value);

void gpiod_set_value_cansleep(struct gpio_desc *desc, int value);
```


#### APIs get interrupt number
``` C
int gpiod_to_irq(const struct gpio_desc *desc);
```
#### APIs khác
```C
int gpiod_cansleep(const struct gpio_desc *desc);
int gpiod_set_debounce(struct gpio_desc *desc, unsigned int debounce);
```

## User interface

### sysfs
``` sh
echo <gpio-sysfs-no> > /sys/class/gpio/export
echo <0/1> > /sys/class/gpio/<gpio_gpio-sysfs-no>/active_low

echo <in/out/low/high> /sys/class/gpio/<gpio_gpio-sysfs-no>/direction

# input 
cat /sys/class/gpio/<gpio_gpio-sysfs-no>/value
# output
echo <low/high> /sys/class/gpio/<gpio_gpio-sysfs-no>/value
```
### libgpiod
``` sh
gpiodetect --version
gpiodetect
gpioinfo

# Thay N bằng chip đã xác nhận tương ứng bank GPIO1
sudo gpioget --numeric -c /dev/gpiochipN 28

# Chờ cạnh xuống trên P9_12
sudo gpiomon -c /dev/gpiochipN --edges=falling 28
```


