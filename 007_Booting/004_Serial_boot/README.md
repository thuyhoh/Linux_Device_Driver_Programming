# Booting BBB over Serial Port
## Giới thiệu chung
- Truyền file thông qua Serial Port(UART) bằng Xmodem, Ymodem, Zmodem, kermit, etc
SPL chỉ có thể chuyển qua Xmodem
- Trong trường hợp này BBB chỉ kết nối serial port và power adapter
## Boot over Serial Port
- mở uart interface bằng tổ hợp phím `CTRL+A` + `S`
- Load file tuần tự theo spl > uboot> uImage > dtb > initranfs
- Setiup: 
``` shell
bootargs console=ttyO0,115200 root=/dev/mmcblk0p2 rw
bootm <địa chỉ giữ MLO/SPL uImage dtb> 
````

    