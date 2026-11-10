obj-$(CONFIG_HELLO_LKM) += hello_kernel_module.o
CROSS_COMPILE := /home/thuy/embedded/embedded_linux/bbb/downloads/bb-kernel/dl/gcc-8.5.0-nolibc/arm-linux-gnueabi/bin/arm-linux-gnueabi-
KERNEL := /home/thuy/embedded/embedded_linux/bbb/downloads/bb-kernel/KERNEL/
CFLAGS:=-Wall
PWD  := $(shell pwd)

all:
	make ARCH=arm CROSS_COMPILE=${CROSS_COMPILE} -C ${KERNEL} M=$(PWD) modules
clean:
	make -C ${KERNEL} M=$(PWD) clean


