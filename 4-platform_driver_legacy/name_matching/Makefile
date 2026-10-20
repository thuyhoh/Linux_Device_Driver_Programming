obj-m := my_legacy_platform_driver.o my_device.o

CROSS_COMPILE := /home/thuy/embedded/embedded_linux/bbb/downloads/bb-kernel/dl/gcc-8.5.0-nolibc/arm-linux-gnueabi/bin/arm-linux-gnueabi-
KERNEL := /home/thuy/embedded/embedded_linux/bbb/downloads/bb-kernel/KERNEL/
PWD  := $(shell pwd)

all:
	make ARCH=arm CROSS_COMPILE=${CROSS_COMPILE} -C ${KERNEL} M=$(PWD) modules
clean:
	make -C ${KERNEL} M=$(PWD) clean
scp:
	scp my_platform_driver.ko debian@10.42.0.2:/home/debian/mnt
