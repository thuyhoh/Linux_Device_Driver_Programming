obj-y:=hello_kernel_module.o

CC:=arm-linux-gnueabihf-
ARCH:=arm
CFLAGS:=-Wall
PATH:=/home/user/
#set KERN_DIR to linux source location 
KERNEL_DIR:=~/workspace/ldd/source/bb-kernel/KERNEL
HOST_KERN_DIR=/lib/modules/$(shell uname -r)/build/

all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CC) -C $(KERNEL_DIR) M=$(PWD) modules
clean:
	make -C $(HOST_KERN_DIR) M=$(PWD) clean
	make ARCH=$(ARCH) CROSS_COMPILE=$(CC) -C $(KERNEL_DIR) M=$(PWD) clean
install:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CC) -C $(KERNEL_DIR) M=$(PWD) INSTALL_MOD_PATH=$(PATH) modules_install
help:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CC) -C $(KERNEL_DIR) M=$(PWD) help

# arm-linux-gnueabihf-objectdump -h main.ko hiển thị debug bộ nhớ dưới dạng các section