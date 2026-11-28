# SPI subsystem

# Phát triển driver thiết bị SPI trên Linux


## I. Tổng quan giao tiếp SPI


## II. Kiến trúc SPI subsystem trong Linux
### 1. Vai trò của SPI core
- SPI core (drivers/spi/spi.c) là lớp trung gian nằm giữa SPI controller driver và SPI protocol driver
- Nhờ có SPI core, hai lớp này tách biệt hoàn toàn: controller không cần biết giao thức của ngoại vi, còn protocol driver không cần biết chi tiết thanh ghi phần cứng của từng SoC.
```
[ Protocol Driver / spidev ]   (tạo spi_message / spi_transfer, gọi spi_sync / spi_async)
               │
               ▼
  ┌──────────────────────────┐
  │         SPI Core         │   (tạo spi_device từ DT, match driver, xếp hàng & điều phối
  │   (drivers/spi/spi.c)    │    giao dịch, gọi callback complete / đánh thức spi_sync)
  └──────────────────────────┘
               │
               ▼
  [ SPI Controller Driver ]    (điều khiển McSPI / SCLK / MOSI / MISO / CS / IRQ / DMA)
```
### 2. SPI controller driver và SPI protocol driver

#### 2.1. SPI controller driver
- Đối tượng: Khối phần cứng SPI trên SoC (ví dụ: McSPI trên TI AM335x của BeagleBone Black, SPI trên STM32/i.MX/Raspberry Pi) hoặc bộ chuyển đổi SPI/bit-banging.
- Nhiệm vụ:
  - Điều khiển trực tiếp thanh ghi phần cứng của controller.
  - Sinh xung clock SCLK, điều khiển chân chọn chip CS (HW CS hoặc qua cs-gpios), truyền/nhận dữ liệu qua MOSI/MISO.
  - Áp dụng thông số truyền thông (mode CPOL/CPHA, bits_per_word, speed_hz).
  - Quản lý ngắt (IRQ) và DMA cho các phiên truyền nhận (spi_message / spi_transfer).
- Thực tế sử dụng: Thường có sẵn trong kernel / BSP của vendor SoC; khi phát triển driver cho thiết bị ngoại vi, bạn hiếm khi phải tự viết lớp này.
#### 2.2. SPI protocol driver
- Đối tượng: Driver dành riêng cho từng chip ngoại vi gắn trên bus SPI (cảm biến IMU/ADC, màn hình LCD như ILI9341, bộ nhớ Flash/EEPROM, hoặc spidev giao tiếp với user space).
- Nhiệm vụ:
  - Khớp (match) với thiết bị khai báo trong Device Tree (of_match_table) hoặc bảng ID (spi_device_id), xử lý vòng đời qua probe() và remove().
  - Định nghĩa giao thức riêng của chip: chuỗi lệnh khởi tạo, địa chỉ thanh ghi, độ trễ, giải mã dữ liệu đọc về.
  - Đóng gói dữ liệu vào struct spi_transfer và struct spi_message, sau đó gọi spi_sync(), spi_async(), spi_write(), spi_read() hoặc spi_write_then_read() thông qua SPI core.
- Thực tế sử dụng: Đây là lớp driver bạn viết hoặc tùy biến khi đưa một linh kiện SPI mới lên board.
### 3. struct spi_*
#### 3.1. struct spi_controller
- Đây là đối tượng kernel đại diện cho bộ điều khiển SPI.
##### Các trường quan trọng
| Trường | Ý nghĩa |
| --- | --- |
| `bus_num` | Số bus SPI được Linux sử dụng |
| `num_chipselect` | Số chỉ số chip select mà controller hỗ trợ |
| `mode_bits` | Những cờ cấu hình SPI controller hỗ trợ |
| `bits_per_word_mask` | Những kích thước SPI word được hỗ trợ |
| `min_speed_hz`, `max_speed_hz` | Giới hạn tốc độ của controller |
| `transfer_one` | Callback phía controller để thực hiện một transfer |
#### 3.2. struct spi_device
- struct spi_device đại diện cho thiết bị do SPI core khởi tạo từ DT/board info
- Với node:
```dts
ili9341@0 {
    compatible = "thuy,ili9341";
    reg = <0>;
    spi-max-frequency = <8000000>;

    dc-gpios = <&gpio1 16 GPIO_ACTIVE_HIGH>;
    reset-gpios = <&gpio1 28 GPIO_ACTIVE_LOW>;
};
```
- Khi node được xử lý trên controller đã đăng ký, SPI core có thể tạo một `spi_device` tương ứng. Driver nhận con trỏ tới đối tượng đó qua:

```c
static int ili9341_probe(struct spi_device *spi)
{
    /* spi mô tả thiết bị đang được probe */
    return 0;
}
```
- `struct spi_device` được dùng để cấu hình lại thiết bị trước khi truyền


##### Các trường quan trọng

| Trường | Ý nghĩa trong driver LCD |
| --- | --- |
| `dev` | Đối tượng `struct device` chung của thiết bị |
| `controller` | Controller phục vụ màn hình này |
| `chip_select` | Chỉ số CS; với node trên thường là 0 |
| `max_speed_hz` | Giới hạn tốc độ thiết bị; ở đây là 8 MHz |
| `mode` | Cờ CPOL, CPHA và các tùy chọn khác |
| `bits_per_word` | Số bit của một SPI word |
| `irq` | IRQ của ngoại vi nếu có khai báo và sử dụng |
| `modalias` | Thông tin hỗ trợ nhận diện và nạp driver |



#### 3.3.  struct spi_transfer
##### Các trường quan trọng
| Trường | Ý nghĩa |
| --- | --- |
| `tx_buf` | Con trỏ tới dữ liệu cần gửi |
| `rx_buf` | Con trỏ tới vùng lưu dữ liệu nhận |
| `len` | Độ dài tính bằng **byte** |
| `speed_hz` | Tốc độ yêu cầu riêng; 0 dùng cấu hình mặc định |
| `bits_per_word` | Kích thước word riêng; 0 dùng mặc định |
| `cs_change` | Yêu cầu hành vi CS đặc biệt sau transfer |
| `delay_usecs` | Khoảng chờ sau transfer trong API Linux 5.4 |
#### struct spi_message
##### Các trường quan trọng
| Trường | Vai trò |
| --- | --- |
| `transfers` | Danh sách liên kết chứa các transfer |
| `spi` | Thiết bị đích khi message được gửi |
| `status` | Trạng thái hoàn tất: 0 hoặc mã lỗi âm |
| `actual_length` | Tổng số byte đã truyền qua các đoạn thành công |
| `complete` | Callback hoàn tất, thường dùng với async |
| `context` | Con trỏ ngữ cảnh truyền cho callback |

## III. Khai báo thiết bị bằng Device Tree
### 3.1. Cấu hình SPI controller 
#### Cấu hình spi pin control
| Pin | Cấu hình |
|:----|:---------|
| SCLK | OUTPUT |
| MOSI | OUTPUT |
| MISO | INPUT  |
| CS   | OUTPUT |

```
&am33xx_pinmux {
    spi0_pins_default: spi0-pins-default {
        pinctrl-single,pins = <
            AM33XX_PADCONF(AM335X_PIN_SPI0_SCLK, PIN_OUTPUT_PULLUP, MUX_MODE0)
            AM33XX_PADCONF(AM335X_PIN_SPI0_D0, PIN_OUTPUT_PULLUP, MUX_MODE0)
            AM33XX_PADCONF(AM335X_PIN_SPI0_D1, PIN_INPUT_PULLUP, MUX_MODE0)
            AM33XX_PADCONF(AM335X_PIN_SPI0_CS0, PIN_OUTPUT_PULLUP, MUX_MODE7)
        >;
    };
};
```
#### Cấu hình SPI controller
``` dts
&spi0 {
    status = "okay";
    pinctrl-names = "default";
    pinctrl-0 = <&spi0_pins_default>;
    #address-cells = <1>;
    #size-cells = <0>;
    cs-gpios = <&gpio0 5 GPIO_ACTIVE_LOW>;
	
    /*...*/
};
```
### 3.2. Khai báo node SPI device
- SPI device node được khai báo trong spi controller
- Mỗi device node trên cùng một spi bus khác nhau chủ yếu ở reg , compatible. Ngoài ra còn có các tham số cấu hình và tài nguyên ngoại vi   
  - reg (Chip Select index): Phân biệt các thiết bị nằm trên cùng một bus SPI. Mỗi thiết bị trên bus phải có một số reg riêng ứng với đường CS (0, 1, 2...) mà nó dùng.
  - compatible: Quyết định driver nào trong kernel sẽ khớp
``` dts
&spi0 {
    /*...*/
    spidev0:spi-dev0@0{
        compatible = "cpmpany,spi-dev-0";
        reg = <0>;
        spi-max-frequency = <4000000>;
        /*  interrupt-parent = <&gpio1>;
            interrupts = <18 IRQ_TYPE_EDGE_FALLING>; // GPIO1_18 */
    };
    spidev1:spi-dev1@1{
        reg = <1>;
        /*...*/
    };
    /*...*/
};
```
### 3.3. SPI device tree property
#### cs-gpios
- Khai báo chân GPIO đóng vai trò làm tín hiệu Chip Select (CS) thay vì dùng (hoặc kết hợp với) chân CS cứng của controller SPI.
```
cs-gpios =  <&gpio1 16 GPIO_ACTIVE_LOW>,
            <&gpio1 17 GPIO_ACTIVE_LOW>;
```
#### spi-cpol & spi-cpha
- Hai thuộc tính boolean (chỉ cần có mặt là bật) điều khiển cực tính xung clock (CPOL) và pha lấy mẫu dữ liệu (CPHA), quyết định SPI Mode 0–3:

|MODE| Nội dung |
|:---|:---------|
|Mode 0 (CPOL=0, CPHA=0)| không cần khai báo cả hai cờ|
|Mode 1 (CPOL=0, CPHA=1)| chỉ thêm spi-cpha|
|Mode 2 (CPOL=1, CPHA=0)| chỉ thêm spi-cpol|
|Mode 3 (CPOL=1, CPHA=1)| thêm cả spi-cpol và spi-cpha|
#### spi-cs-high
- Thuộc tính boolean: đảo mức tích cực của tín hiệu CS thành active-high (kéo lên mức 1 để chọn thiết bị, mặc định là active-low 0).
- Dùng khi thiết bị ngoại vi hoặc mạch đệm/level-shifter yêu cầu CS ở mức cao khi giao tiếp.
#### spi-lsb-first
- Thuộc tính boolean: truyền/nhận bit theo thứ tự LSB trước (Least Significant Bit first) thay vì mặc định MSB trước (Most Significant Bit first).
#### spi-3wire
- Thuộc tính boolean: cấu hình chế độ SPI 3 dây (3-wire SPI / half-duplex trên một chân dữ liệu chung), trong đó đường dữ liệu dùng chung cho cả truyền và nhận thay vì tách riêng MOSI và MISO.
#### spi-max-frequency
- Thuộc tính bắt buộc trên đa số node thiết bị SPI: khai báo tần số xung clock tối đa (đơn vị Hz) mà thiết bị hỗ trợ trên bus này.

``` dts
spi-max-frequency = <4000000>; /* Tối đa 4 MHz */
```
#### Interrupt
- Trên BeagleBone Black (BBB) dùng SoC TI AM335x khái niệm ngắt trên SPI gồm hai lớp tách biệt nhau: 
  - `ngắt của bộ điều khiển SPI (McSPI controller IRQ)`: RX FIFO có dữ liệu nhận được, TX FIFO trống, Kết thúc phiên truyền/nhận
  - `ngắt từ thiết bị ngoại vi gắn trên bus SPI (spi->irq)`: Thông qua một đường ngắt ngoài 
- Device tree chỉ hỗ trợ ngắt từ ngoại vi gắn trên bus SPI thông qua kế thừa ngắt từ ngoại vi GPIO 
```
interrupt-parent = <&gpio1>;
interrupts = <18 IRQ_TYPE_EDGE_FALLING>;
```


## IV. SPI protocol driver
### 1. APIs
- Header <linux/spi/spi.h> cung cấp các hàm API chuẩn của SPI core trong Linux kernel để đăng ký protocol driver, quản lý dữ liệu riêng của thiết bị, cấu hình tham số truyền thông và thực hiện giao dịch đồng bộ/bất đồng bộ với `spi_device`
```
#include <linux/spi/spi.h>
```
#### 1.1. APIs Đăng ký và quản lý vòng đời SPI driver
##### `spi_register_driver()`
- Đăng ký một `struct spi_driver` với SPI bus.
- Trả về `0` nếu đăng ký thành công; trả về mã lỗi âm nếu đăng ký thất bại.
``` C
int spi_register_driver(struct spi_driver *sdrv)
```
##### `spi_unregister_driver()`
- Hủy đăng ký driver.
- Tháo driver khỏi các thiết bị mà nó đang điều khiển và gọi callback `remove()` tương ứng.
- Có thể sleep; thường gọi trong đường thoát module.
``` C
void spi_unregister_driver(struct spi_driver *sdrv)
```
##### Macro `module_spi_driver()`
- Tự tạo phần đăng ký và hủy đăng ký driver cho module. Dùng thay cho cặp init/exit thủ công
``` C
module_spi_driver(struct spi_driver *sdrv)
```

#### 1.2. APIs liên kết dữ liệu riêng với thiết bị
##### `spi_set_drvdata()`
- Lưu con trỏ `data` vào vùng driver data của đối tượng thiết bị.
- Không cấp phát, sao chép hay tự giải phóng cấu trúc mà `data` trỏ tới.
``` C
static inline void spi_set_drvdata(struct spi_device *spi, void *data);
```
##### `spi_get_drvdata()`
- Lấy lại con trỏ đã lưu bằng `spi_set_drvdata()`.
- Trả về `NULL` nếu driver data chưa được gán trên thiết bị mới.
- Không tự khóa dữ liệu hoặc tăng tham chiếu tới cấu trúc riêng.
``` C
static inline void *spi_get_drvdata(const struct spi_device *spi);
```
#### 1.3. API cấu hình thông số giao tiếp
- Áp dụng và kiểm tra cấu hình giao tiếp lưu trong `struct spi_device`
- Chỉ gọi trong ngữ cảnh được phép sleep, khi không có yêu cầu truyền đang chờ hoặc đang chạy tới thiết bị.
``` C
int spi_setup(struct spi_device *spi);
```
#### 1.4. API truyền nhận dữ liệu
##### `spi_write()`: ghi đồng bộ
``` C
static inline int spi_write(struct spi_device *spi, const void *buf, size_t len);
```
##### `spi_read()`: đọc đồng bộ
``` C
static inline int spi_read(struct spi_device *spi, void *buf, size_t len);
```
##### `spi_write_then_read()`: ghi trước, đọc sau
``` C
int spi_write_then_read(struct spi_device *spi, const void *txbuf, unsigned n_tx, void *rxbuf, unsigned n_rx);
```
##### `spi_sync()`: truyền một message đồng bộ
``` C
int spi_sync(struct spi_device *spi, struct spi_message *message);
```
##### `spi_async()`: truyền một message bất đồng bộ
``` C
int spi_async(struct spi_device *spi, struct spi_message *message);
```
#### 1.5. APIs hỗ trợ xây dựng message
| API | Công việc |
| --- | --- |
| `spi_message_init(&message)` | Khởi tạo message và danh sách transfer |
| `spi_message_add_tail(&xfer, &message)` | Nối transfer vào cuối danh sách |
| `spi_sync_transfer(spi, xfers, n)` | Tạo message từ mảng `n` transfer và truyền đồng bộ |
| `spi_max_transfer_size(spi)` | Lấy giới hạn kích thước một transfer |
| `spi_max_message_size(spi)` | Lấy giới hạn tổng kích thước một message |

`spi_message_add_tail()` chỉ nối descriptor vào danh sách; không sao chép buffer và không bắt đầu truyền.

Các giới hạn kích thước cần được xem xét khi chia buffer pixel thành nhiều chunk.
### 2. SPI protocol driver
#### 2.1. Cấu trúc driver
#### 2.2. Đăng ký và xử lý interrupt

