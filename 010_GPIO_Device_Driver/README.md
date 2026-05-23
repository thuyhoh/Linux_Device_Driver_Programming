# GPIO Device Driver 
## I. Giới thiệu chung
- GPIO (General Purpose Input/Output) trên BeagleBone Black (BBB) là một tập hợp các chân (pins) có thể lập trình để thực hiện các chức năng nhập (input) hoặc xuất (output) tín hiệu số.
- BeagleBone Black có tổng cộng 65 chân GPIO khả dụng, phân bố trên hai header chính:
  - P8 header: 46 chân
  - P9 header: 46 chân
- GPIO trên BBB hoạt động với mức logic 3.3V (LVTTL).
- Dòng tối đa khuyến nghị cho mỗi chân GPIO: 4 - 8 mA.
## II. Legacy
- Legacy là một thuật ngữ ám chỉ việc viết chương trình theo cấu trúc vi điều khiển: sẽ bao gồm một file header(gpio.h) và một file c(gpio.c)
- Chương trình sẽ thao tác trực tiếp với phần cứng thông qua thanh ghi 
### 1. Địa chỉ được sử dụng trong phần cứng
- Trong các kiến trúc bảo vệ bộ nhớ hiện đại user-space không thể truy cập và sử dụng trực tiếp đến.
#### i. Ánh xạ không gian địa chỉ vật lý vào không gian địa chỉ ảo của kernel
- **ioremap** là một hàm trong Linux kernel, được dùng để ánh xạ (map) không gian địa chỉ vật lý (physical address) của một vùng thiết bị I/O vào không gian địa chỉ ảo (virtual address space) của kernel.
- **Mục đích**: Cho phép kernel hoặc driver có thể truy cập vùng bộ nhớ của thiết bị ngoại vi thông qua con trỏ địa chỉ ảo, mà không cần truy cập trực tiếp địa chỉ vật lý.
```C
void __iomem *ioremap(unsigned long phys_addr, size_t size);
/**
 * @param[in]:phys_addr: Địa chỉ vật lý bắt đầu cần ánh xạ.
 * @param[in]:size: Kích thước vùng nhớ cần ánh xạ (tính theo bytes).
 * @retval:
    * Một con trỏ kiểu void __iomem *, đại diện cho địa chỉ ảo được ánh xạ.
    * Nếu thất bại, trả về NULL.
 * */
```
#### ii. Hủy ánh xạ không gian địa chỉ ảo khỏi vùng địa chỉ vật lý 
- **iounmap**: là một hàm trong Linux kernel, được dùng để giải phóng (unmap) vùng bộ nhớ ảo đã được ánh xạ trước đó bằng ioremap().
- **Mục đích**: Nó giúp kernel hủy ánh xạ không gian địa chỉ ảo khỏi vùng địa chỉ vật lý tương ứng, tránh rò rỉ tài nguyên (memory leak) hoặc xung đột bộ nhớ.
``` C
void iounmap(void __iomem *addr);
/**
 * @param[in]:addr: Địa chỉ ảo đã được trả về từ ioremap() (vùng mà bạn muốn unmap).
*/
```
### 2. GPIO Registers
- docs: [AM335x Technical Reference manual](https://www.mouser.com/pdfdocs/spruh73h.pdf#page=4063&zoom=100,0,133)
## III. Subsystem
### 1. Interger based
``` C
int gpio_request(unsigned gpio, const char *label);
int gpio_direction_input(unsigned gpio);
int gpio_direction_output(unsigned gpio, int value);
int gpio_get_value(unsigned gpio);
void gpio_set_value(unsigned gpio, int value);
void gpio_free(unsigned gpio);
```
### 2. Descriptor based
``` C
int gpiod_direction_input(struct gpio_desc *desc);
int gpiod_direction_output(struct gpio_desc *desc, int value);
int gpiod_get_value(struct gpio_desc *desc);
void gpiod_set_value(struct gpio_desc *desc, int value);
```
