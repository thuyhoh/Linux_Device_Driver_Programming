# Device Tree
## 1. Introduction to the device tree
- Device Tree (DT) là một cấu trúc dữ liệu được sử dụng trong Embedded Linux, đặc biệt là trên các hệ thống kiến trúc ARM và RISC-V, để mô tả phần cứng cho nhân Linux (kernel) một cách độc lập với mã nguồn kernel. 
- Nguồn: ``Documentation/devicetree/usage-model.txt``
- Linux sử dụng Device Tree cho các trường hợp sau:
    - Xác định nền tảng
    - Số lượng thiết bị. DT sẽ xác định thiết bị và tạo ra cấu trúc dữ liệu phần mềm cần thiết sẽ được sử dụng bởi kernel code.
    - Lý tưởng nhất là cây thiết bị không phụ thuộc vào bất kỳ hệ điều hành nào: khi bạn thay đổi hệ điều hành. Bạn vẫn có thể sử dụng cùng một tệp DT để mô tả phần cứng cho hệ điều hành mới.
    - Nghĩa là DT làm cho việc "thêm thông tin thiết bị" không phụ thuộc vào hệ điều hành.
## 2. Device Tree structure
- The DT suports a hierachical way of writing HW description at the
    - SOC level: On-chip processor and on-chip peripherals
        - file path: ``bb-kernel\KERNEL\arch\arm\boot\dts\am33xx.dtsi``
    - Common board level(am335x-bone-common.dtsi): Peripherals onboard like sensors, LED, button, external memory.
        - file path: ``bb-kernel\KERNEL\arch\arm\boot\dts\am335x-bone-common.dtsi``
    - Board-spercific level(am335xx-boneblack.dts)
        - file path: ``bb-kernel\KERNEL\arch\arm\boot\dts\am335xx-boneblack.dts``

### 2.1 Property
- name: [@address]
- alias, label, phandle
- address cell
- size cell
### 2.2 API
- Register access
``` c
struct resource *res;
void __iomem *base;
res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
base = devm_ioremap_resource(&pdev->dev, res);
```

## 3. The description-base GPIO interface
- GPIO được đặc trưng bởi cấu trúc struct gpio_desc
```c
#include <linux/gpio/consumer.h>

struct gpio_desc{
  struct gpio_chip *chip;
  unsigner long flags;
  const char *label;
};
```
- Với giao diện mô tả, trước khi phân bổ và nắm quyền sở hữu GPIO, các GPIO đó phải được ánh xạ ở đâu đó. Thực tế, có ba loại ánh xạ trong kernel.
    - Ánh xạ dữ liệu nền tảng: ánh xạ được thực hiện trong board file.
    - Ánh xạ giao diện Cấu hình và Nguồn nâng cao (ACPI): Ánh xạ được thực hiện theo kiểu ACPI. Thường được sử dụng trên các hệ thống dựa trên x68
    - Cây thiết bị: Ánh xạ được thực hiện theo kiểu DT

### 3.1 GPIO descriptor mapping - the device tree
- property: -gpios hoặc -gpio
- Mọi hàm giao diện dựa trên mô tả đều dựa trên biến gpio_suffixed[], được định nghĩa trong drivers/gpio/gpiolib.h và được hiển thị như sau
``` c
/* GPIO suffixes used for ACPI and device tree lookup */
static const char *const gpio_suffixes[] = {"gpio", "gpios"};
```

### 3.2 API
- Allocate and using gpio
``` C
struct gpio_desc *gpiod_get_index(struct device *dev, const char *con_id, unsigned int idx, enum gpiod_flags flags);
struct gpio_desc *gpiod_det(struct device *dev, const char *con_id, enum gpiod_flags flags);
int gpiod_direction_input(struct gpio_desc *desc);
int gpiod_direction_output(struct gpio_desc *desc, int value);
int gpiod_get_value(onst struct gpio_desc *desc);
void gpiod_set_value(const struct gpio_desc *desc);
int gpiod_put(struct gpio_desc *desc);
int gpiod_set_debounce(struct gpio_desc *desc, unsigned debounce);
int gpiod_to_irq(const struct gpiod_desc *desc);
int gpiod_cansleep(const struct gpiod_desc *desc);
```


