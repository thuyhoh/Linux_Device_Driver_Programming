
# Device Tree

## 1. Tổng quan về Device Tree

### 1.1. Device Tree là gì?

**Device Tree (DT)** là một cấu trúc dữ liệu dùng để mô tả phần cứng của hệ thống, đặc biệt phổ biến trong các hệ thống Embedded Linux sử dụng kiến trúc ARM, ARM64 và RISC-V.

Device Tree cung cấp thông tin về các thành phần phần cứng như:

- CPU và bộ nhớ RAM.
- GPIO Controller.
- UART, SPI, I2C Controller.
- Interrupt Controller.
- Clock Controller.
- DMA Controller.
- Các thiết bị ngoại vi kết nối với SoC.
- Bus và địa chỉ ánh xạ bộ nhớ.

Thay vì khai báo trực tiếp thông tin phần cứng trong mã nguồn kernel, Device Tree cho phép mô tả phần cứng độc lập với mã nguồn driver.

**Mục đích chính:**

- Tách biệt thông tin phần cứng khỏi mã nguồn kernel.
- Cho phép một kernel hỗ trợ nhiều board khác nhau.
- Giúp kernel xác định các thiết bị có trong hệ thống.
- Cung cấp tài nguyên phần cứng cho device driver.
- Giảm nhu cầu sửa đổi và biên dịch lại kernel khi thay đổi cấu hình phần cứng.

### 1.2. Các thành phần của Device Tree

Device Tree bao gồm ba thành phần chính:

| Thành phần | Mô tả |
|---|---|
| Node | Đại diện cho một thiết bị hoặc một thành phần phần cứng |
| Property | Mô tả thuộc tính và tài nguyên của node |
| Phandle | Cơ chế tham chiếu giữa các node |
### 1.3. Các định dạng Device Tree

| Định dạng | Tên đầy đủ | Chức năng |
|---|---|---|
| `.dts` | Device Tree Source | File nguồn mô tả phần cứng |
| `.dtsi` | Device Tree Source Include | File chứa cấu hình dùng chung |
| `.dtb` | Device Tree Blob | File nhị phân được bootloader/kernel sử dụng |
| `.dtso` | Device Tree Source Overlay | File nguồn mô tả các thay đổi phần cứng |
| `.dtbo` | Device Tree Blob Overlay | File overlay đã biên dịch |


# 2. Cấu trúc Device Tree

Device Tree được tổ chức dưới dạng cây phân cấp.

Mỗi node có thể chứa:

- Các property.
- Các node con.
- Các tham chiếu đến node khác.

Ví dụ:

```dts
/dts-v1/;

/ {
    model = "Example Embedded Board";
    compatible = "vendor,example-board";

    cpus {
        cpu@0 {
            device_type = "cpu";
            reg = <0>;
        };
    };

    memory@80000000 {
        device_type = "memory";
        reg = <0x80000000 0x10000000>;
    };

    soc {
        compatible = "simple-bus";
        #address-cells = <1>;
        #size-cells = <1>;
        ranges;

        uart0: serial@44e09000 {
            compatible = "vendor,example-uart";
            reg = <0x44e09000 0x1000>;
            status = "okay";
        };
    };
};
```

Đây là ví dụ minh họa cấu trúc, chưa phải Device Tree hoàn chỉnh để boot một board thực tế.

## 2.1. Root node

Root node được biểu diễn bằng:

```dts
/ {
    ...
};
```

Đây là node gốc chứa toàn bộ cây thiết bị.

Các node khác được khai báo bên trong root node hoặc các node con của nó.

## 2.2. Node

Cú pháp:

```dts
node-name {
    property-name = value;
};
```

Node có địa chỉ:

```dts
node-name@unit-address {
    property-name = value;
};
```


**Quy tắc đặt tên:**

- Tên node nên mô tả loại thiết bị.
- `unit-address` thường tương ứng với địa chỉ đầu tiên trong `reg`.
- Các node cùng cấp phải có tên đầy đủ khác nhau.
- Tên node thường sử dụng chữ thường và dấu gạch ngang.
## 2.3. Property

- Property được biểu diễn thông qua một cặp key-value chứa thông tin mô tả thiết bị.
```dts
property-name = value;
```
Ví dụ:
```dts
status = "okay";
clock-frequency = <24000000>;
```

- Ngoài ra property không có giá trị:

```dts
gpio-controller;
```

Property dạng này thường được sử dụng để biểu diễn một thuộc tính Boolean.

## 2.4. Label

Label cho phép đặt tên để tham chiếu đến một node.

```dts
uart0: serial@44e09000 {
    status = "disabled";
};
```

Có thể tham chiếu node thông qua label:

```dts
&uart0 {
    status = "okay";
};
```

## 2.5. Phandle

Phandle là giá trị định danh cho phép một node tham chiếu đến node khác.


```dts
clock0: clock {
    compatible = "fixed-clock";
    #clock-cells = <0>;
    clock-frequency = <24000000>;
};

uart0 {
    clocks = <&clock0>;
};
```

Trong đó:

- `clock0` là label.
- `&clock0` là cú pháp tham chiếu đến node.
- Device Tree Compiler sẽ chuyển tham chiếu thành phandle tương ứng.

**Phân biệt:**

| Thành phần | Ý nghĩa |
|---|---|
| Label | Tên được sử dụng trong mã nguồn DTS |
| `&label` | Tham chiếu đến node qua label |
| Phandle | Giá trị định danh node trong Device Tree đã biên dịch |

---

# 3. Các lệnh và cú pháp Device Tree

## 3.1. Khai báo phiên bản DTS

```dts
/dts-v1/;
```

Thông thường được đặt ở đầu file `.dts`.

## 3.2. Include file

Device Tree hỗ trợ sử dụng lại cấu hình thông qua include.

Trong các file DTS của Linux kernel thường gặp:

```dts
#include "am33xx.dtsi"
```

Đây là cú pháp được xử lý bởi C preprocessor trong quá trình build kernel.

Ngoài ra, cú pháp include của DTC:

```dts
/include/ "example.dtsi"
```


## 3.3. Khai báo node

```dts
node_name {
    propety = value;
};
```

## 3.4. Tham chiếu node

```dts
&uart0 {
    status = "okay";
};
```

Có thể tham chiếu bằng đường dẫn tuyệt đối:

```dts
&{/soc/serial@44e09000} {
    status = "okay";
};
```

Đường dẫn phải tương ứng với cấu trúc thực tế của Device Tree.

## 3.5. Xóa property

```dts
/delete-property/ property-name;
```

Ví dụ:

```dts
&uart0 {
    /delete-property/ some-property;
};
```

## 3.6. Xóa node

```dts
/delete-node/ node-name;
```

Ví dụ:

```dts
/ {
    /delete-node/ old-device;
};
```

Trong một số trường hợp có thể xóa node thông qua tham chiếu label:

```dts
/delete-node/ &old_device;
```

Cú pháp này được sử dụng khi xử lý nguồn DTS, không phải thao tác xóa động một thiết bị đang hoạt động trong kernel.

---

# 4. Biểu diễn dữ liệu trong Device Tree

| Kiểu dữ liệu | Cách biểu diễn trong DTS | Ví dụ |
|---|---|---|
| Chuỗi ký tự | `"..."` | `status = "okay";` |
| Danh sách chuỗi | Các chuỗi phân cách bằng dấu phẩy | `compatible = "vendor,device", "generic-device";` |
| Số nguyên 32 bit | `<...>` | `value = <100>;` |
| Mảng số nguyên 32 bit | `<... ... ...>` | `values = <10 20 30>;` |
| Số nguyên 64 bit | `/bits/ 64 <...>` | `value = /bits/ 64 <0x100000000>;` |
| Boolean | Chỉ khai báo tên property | `gpio-controller;` |
| Dữ liệu nhị phân | `[ ... ]` | `data = [01 02 FF];` |
| Tham chiếu node | `&label` | `clocks = <&clkctrl 1>;` |
| Biểu thức số học | Trong dấu `< >` | `value = <(10 + 20)>;` |


# 5. Các thuộc tính trong Device Tree

## 5.1. Nhóm thuộc tính nhận diện thiết bị

### 5.1.1. compatible

Property `compatible` được sử dụng để nhận diện thiết bị và hỗ trợ quá trình lựa chọn driver phù hợp.

```dts
compatible = "ti,am3352-uart", "ti,omap3-uart";
```

Trong Linux kernel, driver thường khai báo bảng `of_device_id`:

```c
static const struct of_device_id my_driver_of_match[] = {
    { .compatible = "vendor,my-device" },
    { }
};

MODULE_DEVICE_TABLE(of, my_driver_of_match);
```

Khi thiết bị được tạo từ Device Tree và tương ứng với bus/driver framework, kernel có thể sử dụng compatible để tìm driver phù hợp.

**Lưu ý:** Chỉ khai báo `compatible` không đảm bảo driver sẽ được load. Driver còn phải được build, đăng ký đúng bus và thiết bị phải được kernel tạo ra.

### 5.1.2. model

Mô tả tên hoặc model của board.

```dts
model = "TI AM335x BeagleBone Black";
```

Property này thường được khai báo tại root node.

Có thể kiểm tra trên Linux:

```bash
cat /proc/device-tree/model
```

### 5.1.3. status

Xác định trạng thái hoạt động của node.

Ví dụ:

```dts
status = "okay";
```

Các giá trị thường gặp:

| Giá trị | Ý nghĩa |
|---|---|
| `"okay"` | Thiết bị có thể hoạt động |
| `"ok"` | Giá trị hợp lệ tương đương `"okay"` |
| `"disabled"` | Thiết bị bị vô hiệu hóa |
| `"reserved"` | Thiết bị được dành riêng |
| `"fail"` | Thiết bị gặp lỗi |
| `"fail-sss"` | Thiết bị lỗi với mã trạng thái cụ thể |


## 5.2. Nhóm thuộc tính tài nguyên phần cứng

### 5.2.1. reg

Property `reg` mô tả địa chỉ và kích thước vùng tài nguyên của thiết bị.

Ví dụ:

```dts
reg = <0x44e09000 0x1000>;
```

Giả sử node cha khai báo:

```dts
#address-cells = <1>;
#size-cells = <1>;
```

Khi đó:

| Giá trị | Ý nghĩa |
|---|---|
| `0x44e09000` | Địa chỉ bắt đầu |
| `0x1000` | Kích thước vùng địa chỉ |

Thiết bị sử dụng vùng:

```text
Start: 0x44E09000
Size : 0x00001000
End  : 0x44E09FFF
```

Một node có thể chứa nhiều vùng `reg`:

```dts
reg = <0x10000000 0x1000>,
      <0x20000000 0x2000>;
```

Có thể sử dụng `reg-names` để đặt tên cho từng vùng:

```dts
reg-names = "control", "data";
```

### 5.2.2. #address-cells

Xác định số lượng cell 32 bit được sử dụng để biểu diễn địa chỉ trong `reg` của các node con.

Ví dụ:

```dts
#address-cells = <1>;
```

Địa chỉ được biểu diễn bằng một cell 32 bit.

```dts
reg = <0x10000000 0x1000>;
```

Nếu:

```dts
#address-cells = <2>;
```

Địa chỉ được biểu diễn bằng hai cell 32 bit.

Ví dụ:

```dts
reg = <0x00000001 0x00000000 0x1000>;
```

### 5.2.3. #size-cells

Xác định số cell 32 bit dùng để biểu diễn kích thước vùng địa chỉ trong `reg` của các node con.

Ví dụ:

```dts
#size-cells = <1>;
```

Kích thước được biểu diễn bằng một cell.

Nếu:

```dts
#size-cells = <0>;
```

Các node con không có phần kích thước trong `reg`.

Điều này thường gặp với các bus như I2C.

Ví dụ:

```dts
i2c {
    #address-cells = <1>;
    #size-cells = <0>;

    sensor@48 {
        reg = <0x48>;
    };
};
```

**Quan trọng:**

`#address-cells` và `#size-cells` của node cha quyết định cách diễn giải `reg` của node con.

### 5.2.4. ranges

Property `ranges` mô tả cách ánh xạ địa chỉ từ bus con sang không gian địa chỉ của bus cha.

Ví dụ:

```dts
bus@0 {
    #address-cells = <1>;
    #size-cells = <1>;

    ranges = <0x00000000 0x40000000 0x10000000>;
};
```

Giả sử bus cha sử dụng một cell địa chỉ.

Ý nghĩa:

```text
Child bus address : 0x00000000
Parent bus address: 0x40000000
Size              : 0x10000000
```

Tức là:

```text
Child 0x00000000
       |
       v
Parent 0x40000000
```

Nếu khai báo:

```dts
ranges;
```

Đây là một property rỗng, thường biểu diễn ánh xạ địa chỉ đồng nhất giữa bus con và bus cha.

Nếu không khai báo `ranges`, không được mặc nhiên kết luận rằng địa chỉ của bus con và bus cha là giống nhau.

### 5.2.5. dma-ranges

Property `dma-ranges` mô tả ánh xạ địa chỉ DMA giữa bus con và bus cha.

Ví dụ:

```dts
dma-ranges = <0x00000000 0x80000000 0x10000000>;
```

Giả sử các số lượng cell tương ứng đều bằng một.

Ý nghĩa:

```text
DMA bus address : 0x00000000
Parent address  : 0x80000000
Size            : 0x10000000
```

Property này giúp hệ thống xác định cách thiết bị DMA truy cập bộ nhớ.

---

## 5.3. Nhóm thuộc tính Interrupt

### 5.3.1. interrupt-parent

Chỉ định Interrupt Controller quản lý interrupt của thiết bị.

```dts
interrupt-parent = <&intc>;
```

### 5.3.2. interrupts

Mô tả interrupt được sử dụng bởi thiết bị.

Ví dụ:

```dts
interrupts = <72>;
```

Cách diễn giải phụ thuộc vào:

```dts
#interrupt-cells
```

của Interrupt Controller tương ứng.

### 5.3.3. #interrupt-cells

Xác định số lượng cell được sử dụng để mô tả một interrupt specifier.

Ví dụ:

```dts
interrupt-controller;
#interrupt-cells = <2>;
```

Khi đó một interrupt specifier có hai cell.

Ý nghĩa cụ thể của từng cell được quy định bởi interrupt binding.

---

## 5.4. Nhóm thuộc tính GPIO

### 5.4.1. gpio-controller

Đánh dấu một node là GPIO Controller.

```dts
gpio-controller;
```

### 5.4.2. #gpio-cells

Xác định số lượng cell của GPIO specifier.

Ví dụ:

```dts
#gpio-cells = <2>;
```

### 5.4.3. gpios

Tham chiếu đến GPIO Controller và chỉ định GPIO được sử dụng.

Ví dụ:

```dts
led-gpios = <&gpio1 21 GPIO_ACTIVE_HIGH>;
```

Trong đó:

- `&gpio1`: GPIO Controller.
- `21`: Chỉ số GPIO trong controller.
- `GPIO_ACTIVE_HIGH`: Mức logic kích hoạt.

Để sử dụng macro:

```dts
#include <dt-bindings/gpio/gpio.h>
```

Các giá trị thường gặp:

```c
GPIO_ACTIVE_HIGH
GPIO_ACTIVE_LOW
```

Lưu ý rằng ý nghĩa của GPIO specifier phụ thuộc vào binding của GPIO Controller.

---

## 5.5. Nhóm thuộc tính Clock

### 5.5.1. clocks

Chỉ định nguồn clock cho thiết bị.

```dts
clocks = <&clkctrl 1>;
```

### 5.5.2. clock-names

Đặt tên cho các clock.

```dts
clock-names = "fck", "ick";
```

### 5.5.3. clock-frequency

Chỉ định tần số clock.

```dts
clock-frequency = <24000000>;
```

Không phải mọi driver đều sử dụng property này. Việc cấu hình clock phải tuân theo binding của thiết bị.

---

## 5.6. Nhóm thuộc tính Pin Control

Pin Control được sử dụng để cấu hình chức năng của các chân vật lý.

Một chân trên SoC có thể hỗ trợ nhiều chức năng:

```text
Physical Pin
     |
     +-- GPIO
     |
     +-- UART
     |
     +-- SPI
     |
     +-- I2C
```

### pinctrl-names

```dts
pinctrl-names = "default";
```

### pinctrl-0

```dts
pinctrl-0 = <&uart1_pins>;
```

Ví dụ:

```dts
&uart1 {
    pinctrl-names = "default";
    pinctrl-0 = <&uart1_pins>;
    status = "okay";
};
```

Kernel sử dụng Pin Control subsystem để áp dụng cấu hình chân tương ứng.

---

## 5.7. Nhóm thuộc tính DMA

### dmas

```dts
dmas = <&dma_controller 5>;
```

### dma-names

```dts
dma-names = "rx", "tx";
```

Các thuộc tính này mô tả kênh DMA mà thiết bị sử dụng.

Số cell và ý nghĩa tham số phụ thuộc vào DMA Controller binding.

---

## 5.8. Nhóm thuộc tính khác

| Property | Chức năng |
|---|---|
| `device_type` | Mô tả loại thiết bị trong một số node đặc biệt |
| `reg-names` | Đặt tên cho các vùng `reg` |
| `interrupt-names` | Đặt tên cho interrupt |
| `reset-gpios` | GPIO dùng để reset thiết bị |
| `resets` | Tham chiếu Reset Controller |
| `reset-names` | Đặt tên các reset |
| `power-domains` | Xác định power domain |
| `vdd-supply` | Tham chiếu nguồn điện |
| `wakeup-source` | Cho biết khả năng sử dụng thiết bị làm nguồn đánh thức |
| `aliases` | Định nghĩa tên bí danh cho các node |
| `chosen` | Chứa các tham số được bootloader cung cấp |

---

# 6. Các node đặc biệt

## 6.1. aliases

Node `aliases` cung cấp tên bí danh cho các node khác.

Ví dụ:

```dts
aliases {
    serial0 = &uart0;
    serial1 = &uart1;
};
```

Giúp các thành phần phần mềm tham chiếu đến thiết bị thông qua tên logic.

Lưu ý: Alias không tự đảm bảo thiết bị sẽ được đặt tên `/dev/ttyS0` hoặc `/dev/ttyS1`. Điều đó còn phụ thuộc driver và cơ chế đánh số của kernel.

## 6.2. chosen

Node `chosen` chứa các thông tin liên quan đến quá trình boot.

Ví dụ:

```dts
chosen {
    bootargs = "console=ttyS0,115200 root=/dev/mmcblk0p2 rw";
    stdout-path = &uart0;
};
```

Trong đó:

- `bootargs`: Tham số command line cho kernel.
- `stdout-path`: Thiết bị được chọn làm console đầu ra theo quy ước Device Tree.

Bootloader có thể cập nhật các thuộc tính trong node này trước khi khởi động kernel.

## 6.3. memory

Node `memory` mô tả vùng RAM của hệ thống.

Ví dụ:

```dts
memory@80000000 {
    device_type = "memory";
    reg = <0x80000000 0x20000000>;
};
```

Giả sử root node sử dụng một address cell và một size cell.

Trong đó:

```text
RAM Start: 0x80000000
RAM Size : 0x20000000
         = 512 MiB
```

## 6.4. reserved-memory

Node `reserved-memory` khai báo các vùng bộ nhớ dành riêng.

Ví dụ:

```dts
reserved-memory {
    #address-cells = <1>;
    #size-cells = <1>;
    ranges;

    reserved_region: buffer@9f000000 {
        reg = <0x9f000000 0x01000000>;
        no-map;
    };
};
```

Trong đó:

- `reg`: Địa chỉ và kích thước vùng reserved memory.
- `no-map`: Yêu cầu hệ điều hành không tạo ánh xạ tuyến tính thông thường cho vùng này.

Các vùng này có thể được sử dụng bởi firmware, remote processor hoặc những thành phần phần cứng đặc biệt.
