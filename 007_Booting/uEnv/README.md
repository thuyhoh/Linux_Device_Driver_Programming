# uEnv.txt and Initramfs
## Giới thiệu chung
- uEnv.txt là một bootscript, nó định nghĩa các tham số cấu hình, kernel parameters. Các tham số này mặc định đã được cấu hình trong u-boot. Tuy nhiên chúng ta có thể thêm, sửa, xóa các cấu hình này thông qua file uEnv.txt. Việc load uEnv.txt là một sự tùy chọn (Optional)
- Các tham số trong uEnv.txt có thể là các giá trị hoặc có thể là các lệnh boot. Ex: bootcmd chứa tập lệnh khởi động mặc định của u-boot, bootargs tham số của kernel
## Các lệnh boot tác động đến file uEvn.txt
- Hiển thị các biến Evn
```shell
# #u-boot
printenv (<envarg>)
```
- Thêm/Thay đổi giá trị cấc biến Env
```shell
# #u-boot
setenv <envarg> (<value>)
```
- Thực thi lệnh chứa trong biến Env
```shell
# #u-boot
run envarg
```

- Truyền file thông qua cổng serial 
```shell
loadx # Tải file qua XMODEM -> Truyền file nhỏ, đơn giản
loady # Tải file qua YMODEM -> Truyền file kernel, dtb, rootfs khi không có mạng
loadz # Tải file qua ZMODEM -> Truyền nhanh, hỗ trợ resume

CTRl+A > S chọn modem tương ứng > chọn file uEnv.txt đã tạo(2 space truy cập folder/ space + enter chọn file)
```
- Import file uEnv.txt đã tải trong memory 
```shell
# #u-boot
env import -t <memory addr> <size in byte>
```

## Example 
### Load uImage & Device tree trong eMMC qua u-boot
- Load uImage
``` shell
# #u-boot
load mmc 1:2 0x82000000 /boot/uImage
```
- Load Device tree
``` shell
# #u-boot
load mmc 1:2 0x88000000 /boot/am335x-boneblack.dtb 
```
- Thiết lập console log: cổng UART(ttyO0,115200), mount vào phân vùng chứa root filesystem: phân vùng 2(mmcblk0p2) -> rw
``` shell
# #u-boot
setenv bootargs console=ttyO0,115200 root=/dev/mmcblk0p2 rw
```
- Boot manually
```shell
# #u-boot
bootm 0x82000000 - 0x88000000
```
- `note` CÓ thể thu gọn các lệnh trên bằng cách viết vào file uEnv.txt như dưới 
### uEnv.txt
```txt
mypcip=setenv serverip 192.168.1.2
ipaddr = 192.168.27.1
bootargs=console=ttyO0,115200 root=/dev/mmcblk0p2 rw
bootcmd=echo"****************Boot from Memory****************";load mmc 1:2 0x82000000 /boot/uImage;load mmc 1:2 0x88000000 /boot/am335x-boneblack.dtb;bootm 0x82000000 - 0x88000000
```
