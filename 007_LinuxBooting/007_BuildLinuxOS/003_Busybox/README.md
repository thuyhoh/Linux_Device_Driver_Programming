# Busybox (minimalist Root file system)
## I. GIới thiệu chung
- BusyBox là một bộ công cụ dòng lệnh nhỏ gọn cho Linux/Unix, được thiết kế để tích hợp nhiều lệnh cơ bản vào trong một file thực thi duy nhất.

- BusyBox cung cấp các lệnh hệ thống quen thuộc như:
    - ls, cp, mv, rm
    - cat, echo, grep
    - ps, top, kill
    - mount, ifconfig, ping
    - sh (shell)

<br>
👉 Tất cả các lệnh này dùng chung một binary duy nhất, thay vì mỗi lệnh là một chương trình riêng như trong GNU coreutils.

- BusyBox được thiết kế để:
    - Nhẹ (rất nhỏ về dung lượng)
    - Tiết kiệm bộ nhớ
    - Chạy tốt trên hệ thống hạn chế tài nguyên

- BusyBox thường được dùng trong:
    - Hệ thống embedded (router, IoT, camera)
    - Initramfs / initrd
    - Docker container siêu nhẹ
    - Android (phiên bản hệ thống cơ bản)


## II. Busybox compilation
- Download source code: [`busybox`](https://busybox.net/) 
- Clone source code from github
``` shell
wget -c https://github.com/mirror/busybox/archive/1_30_0.tar.gz
tar -xvf 1_30_0.tar.gz
rm 1_30_0.tar.gz
```
- distclean: deletes all the previously compiled/generated object file
```shell
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- defconfig
```
- Run menuconfig
``` shell
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- menuconfig
```
- Install busybox into the rootfs partition of your SD card.
``` shell
sudo make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- CONFIG_PREFIX=/media/<USER>/rootfs/ install
```
```
cd /media/<USER>/rootfs
/media/<USER>/rootfs$ ls
bin  linuxrc  sbin  usr
```
