obj-m := bbb-spi_subsystem_template.o
CROSS_COMPILE := /home/thuy/embedded/embedded_linux/bbb/downloads/bb-kernel/dl/gcc-8.5.0-nolibc/arm-linux-gnueabi/bin/arm-linux-gnueabi-
KERNEL := /home/thuy/embedded/embedded_linux/bbb/downloads/bb-kernel/KERNEL/
PWD  := $(shell pwd)

all:
	make ARCH=arm CROSS_COMPILE=${CROSS_COMPILE} -C ${KERNEL} M=$(PWD) modules
clean:
	make -C ${KERNEL} M=$(PWD) clean
	rm -rf *.dtbo *.pre.dts
scp:
	scp *.ko *.dtbo debian@10.42.0.2:/home/debian/drivers
# dtb:
# 	make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- -C ${KERNEL} M=$(PWD) am335x-boneblack.dtb
dtbo:
	cpp -nostdinc -I "${KERNEL}/include" -I "${KERNEL}/arch/arm/boot/dts" -undef -x assembler-with-cpp bbb-ili9341-spi-overlay.dts -o bbb-ili9341-spi-overlay.pre.dts
	dtc -@ -I dts -O dtb -i "${KERNEL}/arch/arm/boot/dts" -o bbb-ili9341-spi-overlay.dtbo bbb-ili9341-spi-overlay.pre.dts
