obj-m := my_device.o my_driver.o
CROSS_COMPILE := /home/thuy/embedded/embedded_linux/bbb/downloads/bb-kernel/dl/gcc-8.5.0-nolibc/arm-linux-gnueabi/bin/arm-linux-gnueabi-
KERNEL := /home/thuy/embedded/embedded_linux/bbb/downloads/bb-kernel/KERNEL/
PWD  := $(shell pwd)
HOST := /lib/modules/$(shell uname -r)/build

all:
	make ARCH=arm CROSS_COMPILE=${CROSS_COMPILE} -C ${KERNEL} M=$(PWD) modules
clean:
	make -C ${KERNEL} M=$(PWD) clean
scp:
	scp *.ko debian@10.42.0.2:/home/debian/mnt
dtb:
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- -C ${KERNEL} M=$(PWD) am335x-boneblack.dtn
dtbo:
	dtc -O dtb -o ./overlay/my_device.dtbo -b 0 -@ ./overlay/my_device.dts
host:
	make ARCH=x86 CROSS_COMPILE= -C ${HOST} M=$(PWD)   modules
