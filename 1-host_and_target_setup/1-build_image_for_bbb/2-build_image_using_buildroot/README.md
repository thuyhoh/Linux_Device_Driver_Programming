# Build image for beaglebone black using buildroot

- Buildroot là một framework đơn giản để:
    - Cross-compile toolchain
    - Build Linux kernel
    - Build root filesystem
    - Tạo image hoàn chỉnh cho thiết bị nhúng
## II. Buildroot source code 
- `configs/`: chứa các file **\*_defconfig** chỉ định cấu hình build
- `board/` : chứa cấu hình riêng cho board (kernel config, rootfs, overlay, scripts ...)
- `package/`: Nơi thêm hoặc chỉnh sửa package 
- `output/`: chứa toàn bộ kết quả build
- `toolchain/`: Infrastructure build toolchain
- `boot/`: Build bootloader như u-boot
- `linux/`: Infrastructure build Linux kernel
- `system/`: root filesystem skeleton mặc định 
## III. Configuring and building Buildroot
- Tải xuống buildroot source code từ website [`buildroot.org`](https://buildroot.org/download.html) 
- Clone source code từ github
``` shell
# user@ubuntu:$
git clone -b 2025.02.x https://gitlab.com/buildroot.org/buildroot.git buildroot-2025-02 --depth=1 
```
- Chọn cấu hình board từ folder `configs/`:
``` shell
# user@ubuntu:./buildroot$
ls configs/ # tìm và chọn cấu hình board 
make beaglebone_defconfig # beaglebone_defconfig là một cấu hình mặc định của beaglebone black
``` 
- run menuconfig. Chọn package cần thiết
``` shell
# user@ubuntu:./buildroot$
make menuconfig
```
- Compile buildroot source code tạo output folder
``` shell
# user@ubuntu:./buildroot$
make -j$(proc) V=s
```
- kiểm tra kết quả đã build
```sh
thuy@thuy-ASUS:~/buildroot-2024-5/output/images$ ls
am335x-boneblack.dtb           am335x-evm.dtb    rootfs.ext4
am335x-boneblack-wireless.dtb  am335x-evmsk.dtb  rootfs.tar
am335x-boneblue.dtb            boot.vfat         sdcard.img
am335x-bone.dtb                extlinux          u-boot.img
am335x-bonegreen.dtb           MLO               uEnv.txt
am335x-bonegreen-wireless.dtb  rootfs.ext2       zImage
```

