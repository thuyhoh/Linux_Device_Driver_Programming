# SPI subsystem

# Phát triển driver thiết bị SPI trên Linux


## I. Tổng quan giao tiếp SPI


## II. Kiến trúc SPI subsystem trong Linux
### 2.1. Vai trò của SPI core
### 2.2. SPI controller driver và SPI protocol driver
### 2.3. struct spi_*
#### struct spi_controller
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
#### struct spi_device
- Đây là đối tượng mô tả  một thiết bị trên bus SPI trong hệ thống thiết bị của Linux.
- Với node:
```dts
ili9341@0 {
    compatible = "thuy,ili9341";
    reg = <0>;
    spi-max-frequency = <8000000>;

    dc-gpios = <&gpio1 16 GPIO_ACTIVE_HIGH>;
    reset-gpios = <&gpio1 28 GPIO_ACTIVE_LOW>;
};
- Khi node được xử lý trên controller đã đăng ký, SPI core có thể tạo một `spi_device` tương ứng. Driver nhận con trỏ tới đối tượng đó qua:

```c
static int ili9341_probe(struct spi_device *spi)
{
    /* spi mô tả thiết bị đang được probe */
    return 0;
}
```
- struct spi_device được dùng để Cấu hình trước khi truyền


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



#### struct spi_transfer
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
| Trường | Vai trò |
| --- | --- |
| `transfers` | Danh sách liên kết chứa các transfer |
| `spi` | Thiết bị đích khi message được gửi |
| `status` | Trạng thái hoàn tất: 0 hoặc mã lỗi âm |
| `actual_length` | Tổng số byte đã truyền qua các đoạn thành công |
| `complete` | Callback hoàn tất, thường dùng với async |
| `context` | Con trỏ ngữ cảnh truyền cho callback |

## III. Khai báo thiết bị bằng Device Tree
### 3.1. Cấu hình SPI controller và pinctrl
- pinmux SPI pins
```
AM33XX_PADCONF(AM335X_PIN_SPI0_SCLK, PIN_INPUT_PULLUP, MUX_MODE0) /* P9_22: spi0_sclk, MUX mode 0, input enabled */	
AM33XX_PADCONF(AM335X_PIN_SPI0_D0, PIN_INPUT_PULLUP, MUX_MODE0) /* P9_21: spi0_d0 (MISO), MUX mode 0, input enabled */
AM33XX_PADCONF(AM335X_PIN_SPI0_D1, PIN_OUTPUT_PULLUP, MUX_MODE0) /* P9_18: spi0_d1 (MOSI), MUX mode 0 */		
AM33XX_PADCONF(AM335X_PIN_SPI0_CS0, PIN_OUTPUT_PULLUP, MUX_MODE0) /* P9_17: spi0_cs0, MUX mode 0 */	
```
### 3.2. Khai báo node thiết bị SPI

### 3.3. Property
#### cs-gpios
#### spi-cpol & spi-cpha
Mode 0 (CPOL=0, CPHA=0): không cần khai báo cả hai cờ.
Mode 1 (CPOL=0, CPHA=1): chỉ thêm spi-cpha;.
Mode 2 (CPOL=1, CPHA=0): chỉ thêm spi-cpol;.
Mode 3 (CPOL=1, CPHA=1): thêm cả spi-cpol; và spi-cpha;.
#### spi-cs-high
#### spi-lsb-first
#### spi-3wire
#### spi-max-frequency


#### Khai báo interrupt, reset GPIO và nguồn cấp

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

