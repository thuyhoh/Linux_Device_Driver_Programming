# Linux kernel module

## I. Giới thiệu chung
### 1. Khái niệm cơ bản
- Linux hỗ trợ việc chèn và xóa mã động khỏi Kernel trong khi hệ thống đang hoạt động. Đoạn mã mà chúng ta thêm và xóa trong quá trình chạy được gọi là Linux Kernel Module (LKM)
- LKM mở rộng chức năng của nhân hệ điều hành một cách linh hoạt bằng cách bổ sung các tính năng mới vào nhân, chẳng hạn như bảo mật, trình điều khiển thiết bị, trình điều khiển hệ thống tập tin, các lệnh gọi hệ thống,... .
- LKM cho phép các hệ thống Linux nhúng chỉ cần kernel image tối thiểu (=>ít bộ nhớ, thời gian chạy hơn) và các trình điều khiển thiết bị tùy chọn cùng các tính năng khác được cung cấp theo yêu cầu thông qua việc chèn mô-đun.
### 2. Phân loại LKM
- Static Kernel module: là LKM được thể tích hợp kernel image(mô-đun trở thành một phần của Linux kernel image). Phương pháp này làm tăng kích thước của Linux kernel image khi hoàn tất. Nó không thể gỡ bỏ mô-đun => Static kernel module chiếm bộ nhớ vĩnh viễn trong thời gian chạy.
- Dynamic Kernel module: các mô-đun này không được tích hợp vào kernel image, mà thay vào đó chúng được biên dịch và liên kết riêng biệt để tạo ra các tệp `.ko`. Người dùng có thể tải và gỡ bỏ các module khỏi Kernel bằng cách sử dụng các lệnh như insmod, modprobe, rmmod.
### 3. Build LKM 
- Có 2 cách build linux kernel module
  - build out of tree
  - build in tree
#### build in tree
#### build out of tree

## II. Kiến trúc LKM 
### 1. header section
### 2. module initialization function
### 3. module clean-up funtion 
### 3. registration section
### 4. module description
## III. Makefile

## IV. Load LKM into target
### Install module
``` shell
insmod main.ko
```
### Remove module
``` shell
rmmod main.ko
```
- Gỡ module + dependency liên quan
``` shell
modprobe -r main
```
### Print module information
``` shell
modinfor main.ko
```
- Hiển thị: filename, license, author, description, version, vermagic, dependency
### log and debug kernel module
``` shell
dmesg
```
### remote and load module into target
``` shell
ssh debian@192.168.7.2
scp main.ko debian@192.168.7.2:/home/debian/drivers
```

## Debug using printk

