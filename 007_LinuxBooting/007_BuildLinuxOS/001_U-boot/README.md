# SPL and U-boot Compilation

## Download U-Boot
``` shell
git clone https://github.com/u-boot/u-boot
```
## Kiến trúc file trong U-Boot
``` 
u-boot/
├── arch/           # Mã phụ thuộc kiến trúc CPU (ARM, RISC-V, x86, PowerPC, MIPS, …)
├── board/          # Mã phụ thuộc board cụ thể (init DDR, pinmux, power, v.v.)
├── cmd/            # Các lệnh (command) của shell U-Boot (md, mmc, fatls, printenv,…)
├── common/         # Mã dùng chung, không phụ thuộc kiến trúc
├── configs/        # Các file cấu hình mặc định (defconfig) cho từng board
├── drivers/        # Driver phần cứng (UART, SPI, I2C, MMC, USB, Ethernet,…)
├── disk/           # Hỗ trợ đọc/ghi phân vùng, xử lý disk partition
├── doc/            # Tài liệu dự án (reStructuredText)
├── dts/            # Device Tree Source (FDT) – mô tả phần cứng cho U-Boot
├── env/            # Xử lý biến môi trường (environment)
├── examples/       # Mẫu ứng dụng standalone chạy trên U-Boot
├── fs/             # Filesystem driver (FAT, ext2/4, UBIFS,…)
├── include/        # Header file (.h) – định nghĩa cấu trúc, macro, API
├── lib/            # Thư viện chung (math, zlib, crypto, generic code)
├── net/            # Network stack (ARP, TFTP, DHCP, NFS,…)
├── post/           # Power-On Self-Test (kiểm tra phần cứng)
├── scripts/        # Script & tool hỗ trợ build (make rules, env setup)
├── test/           # Unit test và test script
├── tools/          # Các công cụ build như mkimage, fw_printenv, dumpimage
├── boot/           # Hỗ trợ định dạng image và boot sequence
├── Kconfig         # File cấu hình root (điều hướng hệ thống menuconfig)
├── Makefile        # File điều khiển build chính
└── README          # Thông tin tổng quan
```


<!-- kieens truc
board/ti/am335x board.c
arch/arm/cpu/armv7 start.S
configs/am335x_ 
omap -->
## Configuring and generating SPL,MLO,U-boot images
```shell
# Dọn sạch toàn bộ kết quả build trước đó để bắt đầu lại
make ARCH=arm CROSS_COMPILE= arm-linux-gnueabihf- distclean 
# Nạp file cấu hình mặc định (defconfig) tương ứng với board BeagleBone Black.
make ARCH=arm CROSS_COMPILE= arm-linux-gnueabihf- am335x_boneblack_defconfig
# Mở giao diện cấu hình tương tác (ncurses) để tùy chỉnh thêm .config. -> .Config 
make ARCH=arm CROSS_COMPILE= arm-linux-gnueabihf- menuconfig
# Biên dịch U-Boot 
make ARCH=arm CROSS_COMPILE= arm-linux-gnueabihf- j4 # 4 core
make ARCH=arm CROSS_COMPILE= arm-linux-gnueabihf- j8 # 8 core
```
### Menuconfig
- Run Menuconfig nếu muốn ghi đè cấu hình mặc định hoặc muốn thêm cấu hình không được chọn bởi mặc định 
- Chức năng: Cấu hình bootloader – chọn board, kiến trúc CPU, driver cần thiết cho khởi động, thiết bị lưu trữ, mạng, lệnh, ...
- Build tạo ra file SPL(./spl/u-boot-spl.bin), MLO(./MLO), uboot(./u-boot.img) 

