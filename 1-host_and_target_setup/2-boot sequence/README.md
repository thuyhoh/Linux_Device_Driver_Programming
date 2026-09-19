# Boot sequence
## Update essential package
``` 
sudo apt update

sudo apt install -y \
    git build-essential \
    gcc-arm-linux-gnueabihf \
    binutils-arm-linux-gnueabihf \
    u-boot-tools \
    device-tree-compiler \
    bc \
    bison \
    flex \
    libssl-dev \
    libncurses-dev \
    libelf-dev \
    rsync \
    cpio \
    wget \
    xz-utils
```
## Build uboot
```
git clone -b v2019.04 https://github.com/u-boot/u-boot --depth=1
cd u-boot/
git pull --no-edit https://github.com/beagleboard/u-boot v2019.04-bbb.io-am335x
```
```
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- distclean
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- am335x_evm_defconfig
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- menuconfig
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -j$(nproc)
```

```
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
## Build Kernel
```
# user@ubuntu:~$
git clone https://github.com/beagleboard/linux.git linux
cd linux
# user@ubuntu:./linux$
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- distclean
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- omap2plus_defconfig
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- menuconfig
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- uImage dtbs LOADADDR=0x80008000 -j$(nproc) 
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -j$(nproc)  modules
```
```
  AS      arch/arm/boot/compressed/piggy.o
  LD      arch/arm/boot/compressed/vmlinux
  OBJCOPY arch/arm/boot/zImage
  Kernel: arch/arm/boot/zImage is ready
  UIMAGE  arch/arm/boot/uImage
Image Name:   Linux-4.14.108+
Created:      Thu Dec  5 09:41:41 2019
Image Type:   ARM Linux Kernel Image (uncompressed)
Data Size:    10035712 Bytes = 9800.50 KiB = 9.57 MiB
Load Address: 80008000
Entry Point:  80008000
  Kernel: arch/arm/boot/uImage is ready
```
```
# user@ubuntu:./linux$
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- INSTALL_MDD_PATH=<path_of_the_RFS> modules_install
```
## Busybox
```
wget -c https://github.com/mirror/busybox/archive/1_30_0.tar.gz
tar -xvf 1_30_0.tar.gz
rm 1_30_0.tar.gz
``` 
```
# user@ubuntu:./busybox$
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- defconfig
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- menuconfig
sudo make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- CONFIG_PREFIX=/media/<USER>/rootfs/ install
```
```
cd /media/<USER>/rootfs
/media/<USER>/rootfs$ ls
bin  linuxrc  sbin  usr
```
