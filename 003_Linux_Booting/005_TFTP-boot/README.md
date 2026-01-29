# Booting BBB over TFTP
## Giới thiệu chung
## Booting over TFTP

SPL và u-boot được SDcard khởi động
- cần ethenet cap setup ethn 
``` shell
sudo ifconfig enp1s0 ethenip
```
- cp file
``` shell
cp -r uimage dtb initramfs /var/lib/tftpboot
```
board
- setup server ip
``` shell
# #U-boot
setenv serverip ethenip_host
```
- Setup client ip
``` shell
setenv ipaddr ethenip_board
```
- Ping để kiểm tra đã kết nói chưa
```shell
ping serverip
```
``` shell
tftpboot 0x82000000 uImage 
tftpboot 0x88000000 dtb
tftpboot 0x88080000 initramfs
```
- arg
``` shell
setenv bootargs console=ttyO0,115200 root=/dev/mmcblk0p2 rw initrd=0x88080000
setenv bootm 0x82000000 0x88000000 0x88080000
```

# Mở trộng load file từ host sang bbb
- host
``` shell
cp -r file /var/lib/tftpboot 
```
- board
``` shell
ifconfig eth0 ipaddr<!-- 192.168.27.1 -->
tftp -r file -g serverip
```