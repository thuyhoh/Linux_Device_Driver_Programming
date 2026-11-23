obj-m := bbb_tasklet.o
CROSS_COMPILE := /home/thuy/embedded/embedded_linux/bbb/downloads/bb-kernel/dl/gcc-8.5.0-nolibc/arm-linux-gnueabi/bin/arm-linux-gnueabi-
KERNEL := /home/thuy/embedded/embedded_linux/bbb/downloads/bb-kernel/KERNEL/
PWD  := $(shell pwd)

all:
	make ARCH=arm CROSS_COMPILE=${CROSS_COMPILE} -C ${KERNEL} M=$(PWD) modules
clean:
	make -C ${KERNEL} M=$(PWD) clean
	rm -rf *.dtbo
scp:
	scp bbb-gpio-irq-overlay.ko debian@10.42.0.2:/home/debian/mnt
dtb:
	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- -C ${KERNEL} M=$(PWD) am335x-boneblack.dtb
dtbo:
	dtc -O dtb -o bbb-gpio-irq-overlay.dtbo -b 0 -@ bbb-gpio-irq-overlay.dts
