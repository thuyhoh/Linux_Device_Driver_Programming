# Linux Srouce Tree
## I. Giới thiệu chung

## II. Kiến trúc file trong Linux

<!-- - Cấu trúc thư mục trong Linux
./arch/arm/mach-omap2
board file
comon share code
linux/drivers
sitara va davici
config and generating linux image -->
## III. Configuring and generating linux image

### 1. Execute the Script
- Clone Linux source code
``` shell
# user@ubuntu:./$
git clone https://github.com/RobertCNelson/bb-kernel
cd bb-kernel/
git checkout origin/am33x-v5.4 -b tmp
```
- Build the first linux kernel
``` shell
# user@ubuntu:./bb-kernel$
./build_kernel.sh
```
- Rebuild linux kernel
``` shell
# user@ubuntu:./bb-kernel$
./tools/rebuild.sh
```

### 2. Execute to build Linux Source Tree
- Clone Linux source code
``` shell
# user@ubuntu:~$
git clone https://github.com/beagleboard/linux.git linux
cd linux
```
- distclean: deletes all the previously compiled/generated object file
``` shell
# user@ubuntu:./linux$
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- distclean
``` 
- apply board defaut configuration for linux kernel
``` shell
# user@ubuntu:./linux$
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- omap2plus_defconfig
```
- Run menuconfig
``` shell
# user@ubuntu:./linux$
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- menuconfig
```
- Compilating Linux source code to generate uImage and Device Tree Blobs (DTB) file
``` shell
# user@ubuntu:./linux$
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- uImage dtbs LOADADDR=0x80008000 -j$(nproc) 
```
- Compilating Linux source code to loadable kernel modules(LKM) for linux kernel
``` shell
# user@ubuntu:./linux$
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -j$(nproc)  modules
```
=> Result of compilation 

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
- Install Kernel modules into the rootfs partition of your SD card.
``` shell
# user@ubuntu:./linux$
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- INSTALL_MDD_PATH=<path_of_the_RFS> modules_install
```

