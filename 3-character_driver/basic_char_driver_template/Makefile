obj-m := my_char_driver.o

CROSS_COMPILE := /home/thuy/embedded/embedded_linux/bbb/downloads/bb-kernel/dl/gcc-8.5.0-nolibc/arm-linux-gnueabi/bin/arm-linux-gnueabi-
KERNEL := /home/thuy/embedded/embedded_linux/bbb/downloads/bb-kernel/KERNEL/
PWD  := $(shell pwd)

all:
	make ARCH=arm CROSS_COMPILE=${CROSS_COMPILE} -C ${KERNEL} M=$(PWD) modules
user_app:
	gcc -o user_app user_space.c
clean:
	make -C ${KERNEL} M=$(PWD) clean
	rm -rf user_app
scp:
	scp my_char_driver.ko debian@10.42.0.2:/home/debian/mnt
