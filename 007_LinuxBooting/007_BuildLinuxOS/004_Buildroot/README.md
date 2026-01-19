# buildroot
## 1. Giới thiệu chung
- Buildroot là một framework đơn giản để:
    - Cross-compile toolchain
    - Build Linux kernel
    - Build root filesystem
    - Tạo image hoàn chỉnh cho thiết bị nhúng
## 2. Kiến trúc buildroot
<!-- /arch
configs
board/...
board/.../readme.md -->
## 3. Configuring and building Buildroot
- Download source code: [`buildroot`](https://buildroot.org/) 
- Clone source code from github
``` shell
# user@ubuntu:./$
git clone https://gitlab.com/buildroot.org/buildroot.git
cd buildroot
```
- Select the default configuration for the target:
``` shell
make beaglebone_defconfig
``` 
- run menuconfig
``` shell
# user@ubuntu:./buildroot$
make menuconfig
```

``` shell
> target option
> toolchain > type
            > toolchain
            > toolchain origin
            > toolchain path
> system configuration  > system hostname
                        > System banner
                        > init system
                        > root password
                        > run a getting

> Target packet> network application > openssh
> file system image
> bootloader
```

- Compilation buildroot source code to generate ouput folder
``` shell
# user@ubuntu:./buildroot$
make -j$(proc)
```
- run the `ls` command to make sure that you have the executables shown below.
``` shell
# user@ubuntu:./buildroot$ cd output/images/
# user@ubuntu:./buildroot$ ls
am335x-boneblack.dtb    am335x-boneblack-wireless.dtb   am335x-boneblue.dtb
am335x-bonegreen.dtb    am335x-bonegreen-eco.dtb        am335x-bonegreen-wireless.dtb
am335x-bone.dtb         am335x-evm.dtb                  am335x-evmsk.dtb
boot.vfat               MLO                             rootfs.ext2
rootfs.tar              sdcard.img                      u-boot.img
uEnv.txt                zImage
```