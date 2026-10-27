obj-m += gpioctrl.o
ARCH := arm 
KERNEL=/home/thuy/embedded/embedded_linux/bbb/downloads/bb-kernel/KERNEL
CROSS := /home/thuy/embedded/embedded_linux/bbb/downloads/gcc-11.5.0-nolibc/arm-linux-gnueabi/bin/arm-linux-gnueabihf-

all:
	make ARCH=${ARCH} CROSS_COMPILE=${CROSS} -C ${KERNEL} M=$(PWD) modules
clean:
	make -C ${KERNEL} M=$(PWD) clean
