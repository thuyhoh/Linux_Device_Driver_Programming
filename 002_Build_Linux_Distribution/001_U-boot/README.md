# U-boot Compilation
## I. Giới thiệu chung
- U-Boot (tên đầy đủ: Das U-Boot) là một bootloader mã nguồn mở rất phổ biến trong các hệ thống nhúng (embedded systems), đặc biệt trên các nền tảng dùng ARM, PowerPC, MIPS, RISC-V.

- U-Boot là phần mềm chạy đầu tiên sau khi CPU reset, có nhiệm vụ:
    - Khởi tạo phần cứng cơ bản (RAM, clock, GPIO, UART…)
    - Nạp và khởi động kernel (thường là Linux)
    - Cho phép người dùng cấu hình quá trình boot
    - Hỗ trợ cập nhật firmware, debug hệ thống
=> Nói ngắn gọn: U-Boot là cầu nối giữa phần cứng và hệ điều hành.

- Vị trí của U-Boot trong chuỗi khởi động. Chuỗi boot điển hình trong hệ nhúng:
    - ROM bootloader (nằm trong chip, do nhà sản xuất cung cấp)
    - U-Boot
    - Linux kernel
    - Root filesystem


## II. U-Boot source code
<!-- kieens truc
board/ti/am335x board.c chứa tập mã lệnh khởi tạo cho toàn bộ board base on AM335x SOC
arch/arm/cpu/armv7 start.S khởi tạo cpu điểm chuyern giao giữa roomboot tới spl
configs/am335x_ 
omap -->
## III. Configuring and generating SPL,MLO,U-boot images
- Install requirement linux packet
``` shell
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install build-essential
sudo apt-get install flex bison
sudo apt-get install lzop 
sudo apt-get install u-boot-tools

sudo apt-get install libssl-dev
sudo apt-get install libgtk2.0-dev
sudo apt-get install libglib2.0-dev
sudo apt-get install libglade2-dev
```
- Clone uboot source code
``` shell
# user@ubuntu:./$
git clone -b v2019.04 https://github.com/u-boot/u-boot --depth=1
cd u-boot/
git pull --no-edit https://github.com/beagleboard/u-boot v2022.04-bbb.io-am335x-am57xx
```
### 1. Execute the Script
``` shell
# user@ubuntu:./u-boot$
sudo chmode +x build_am335x.sh
sudo ./build-am335x.sh
```
### 2. Execute to build U-Boot Source Tree
- distclean: deletes all the previously compiled/generated object file
```shell
# user@ubuntu:./u-boot$
make ARCH=arm CROSS_COMPILE= arm-linux-gnueabihf- distclean 
```
- apply board defaut configuration for uboot
``` shell
# user@ubuntu:./u-boot$
make ARCH=arm CROSS_COMPILE= arm-linux-gnueabihf- am335x_boneblack_defconfig
```
``` shell
# result
  HOSTCC  scripts/basic/fixdep
  HOSTCC  scripts/kconfig/conf.o
  YACC    scripts/kconfig/zconf.tab.c
  LEX     scripts/kconfig/zconf.lex.c
  HOSTCC  scripts/kconfig/zconf.tab.o
  HOSTLD  scripts/kconfig/conf
#
# configuration written to .config
#
```
- Run menuconfig
``` shell
# user@ubuntu:./u-boot$
make ARCH=arm CROSS_COMPILE= arm-linux-gnueabihf- menuconfig
```
- Compilation U-boot source code to generate MLO,U-boot images
``` shell
# user@ubuntu:./u-boot$
make ARCH=arm CROSS_COMPILE= arm-linux-gnueabihf- -j$(nproc) 
```
<br />
=> After the `BootLoader` has been compiled, run the `ls` command to make sure that you have the executables shown below.
The ones you care about, are `MLO` and `u-boot.img`.
The `MLO` file is the **first stage BootLoader**, and `u-boot.img` is the **second stage BootLoader** also known as the **Bootstrap Loader**.

``` shell
# user@ubuntu:./u-boot$ ls
api        Documentation  lib           scripts             u-boot.img
arch       drivers        Licenses      spl                 u-boot.lds
board      dts            MAINTAINERS   System.map          u-boot.map
cmd        env            Makefile      test                u-boot-nodtb.bin
common     examples       MLO           tools               u-boot.srec
config.mk  fs             MLO.byteswap  u-boot              u-boot.sym
configs    include        net           u-boot.bin
disk       Kbuild         post          u-boot.cfg
doc        Kconfig        README        u-boot.cfg.configs
```




