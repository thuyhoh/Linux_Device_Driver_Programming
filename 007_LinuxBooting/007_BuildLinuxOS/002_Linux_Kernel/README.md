# Linux Srouce Tree
## Download LinuxSource tree
``` shell
git clone https://github.com/beagleboard/linux.git
```
## Kiến trúc file trong Linux
```
linux/
├── arch/           # Mã phụ thuộc kiến trúc CPU (ARM, RISC-V, x86, PowerPC, MIPS, …)
├── block/
├── drivers/          # Mã phụ thuộc board cụ thể (init DDR, pinmux, power, v.v.)
```
<!-- - Cấu trúc thư mục trong Linux
./arch/arm/mach-omap2
board file
linux/drivers
sitara va davici
config and generating linux image -->
## Configuring and generating linux image
``` shell
make ARCH=arm CROSS_COMPILE= arm-linux-gnueabihf- distclean
make ARCH=arm CROSS_COMPILE= arm-linux-gnueabihf- bb.org_defconfig
make ARCH=arm CROSS_COMPILE= arm-linux-gnueabihf- menuconfig
make ARCH=arm CROSS_COMPILE= arm-linux-gnueabihf- uImage dtbs LOADADDR=0x80008000 -j4
make ARCH=arm CROSS_COMPILE= arm-linux-gnueabihf- j4 modules
make ARCH=arm CROSS_COMPILE= arm-linux-gnueabihf- INSTALL_MDD_PATH=<path_of_the_RFS> modules_install
```
menuconfig
- Cấu hình hệ điều hành – chọn kernel features, device drivers, filesystem, networking, security, module
- Build tạo ra file uImage.img

