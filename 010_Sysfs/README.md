
# Sysfs
## 1. Giới thiệu về GPIO Sysfs
- GPIO Sysfs Interface là cách cũ để điều khiển GPIO từ user-space trong Linux kernel (áp dụng từ Linux 2.6.x đến 5.x).
- Đường dẫn sysfs nằm ở: ```/sys/class/gpio/```
## 2. Các file và thư mục chính trong /sys/class/gpio/ 
| File/Directory| chức năng |
|:-----|:-------|
|/sys/class/gpio/export| Yêu cầu kernel "export" một chân GPIO ra user-space |
|/sys/class/gpio/unexport |Giải phóng (unexport) một chân GPIO|
|/sys/class/gpio/gpioN/ |Thư mục chứa các file điều khiển chân GPIO số N|
|/sys/class/gpio/gpioN/direction|Chọn hướng (input/output) cho chân|
|/sys/class/gpio/gpioN/value|Đọc/ghi giá trị của chân GPIO|
|/sys/class/gpio/gpioN/edge|Cấu hình trigger interrupt (rising, falling, both)|

## 3. Quy trình điều khiển GPIO bằng command lines thông qua Sysfs
- **Sysfs**: là một virtual filesystem (hệ thống file ảo) trong Linux, dùng để cung cấp giao diện giữa kernel space và user space, giúp người dùng hoặc ứng dụng có thể truy cập và cấu hình thông tin về các thiết bị phần cứng hoặc driver.
- ví dụ điều khiển gpio60 
### i. Export chân GPIO
``` bash
echo 60 > /sys/class/gpio/export
```
- Sau khi export, hệ thống sẽ tạo thư mục:```/sys/class/gpio/gpio60/```

### ii.  Input
``` bash
echo in > /sys/class/gpio/gpio60/direction
```
#### Đọc giá trị 
``` bash
cat /sys/class/gpio/gpio60/value
```
### iii. Output
``` bash
echo out > /sys/class/gpio/gpio60/direction
```
#### Ghi giá trị
- Set HIGH (3.3V):
``` bash
echo 1 > /sys/class/gpio/gpio60/value
``` 
- Set LOW (0V):
``` bash
echo 0 > /sys/class/gpio/gpio60/value
```
### iv. Cấu hình ngắt
``` bash
echo rising > /sys/class/gpio/gpio60/edge
```
- Các tùy chọn hợp lệ: none, rising, falling, both
### v. Unexport chân GPIO
``` bash 
echo 60 > /sys/class/gpio/unexport
```


chuyeenr doi gpiox_a -> gpio(32*x + a)
sys/kernel/debug/pinctrl/44e10800...
cat pins | more



