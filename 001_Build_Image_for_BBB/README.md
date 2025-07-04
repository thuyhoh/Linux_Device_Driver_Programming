## Build image for BeagelBone Black
- document: [link](https://forum.digikey.com/t/debian-getting-started-with-the-beaglebone-black/12967)

### Tải tool chain và export biến môi trường cho tool chain
``` shell
# /~$
wget -c https://releases.linaro.org/components/toolchain/binaries/6.5-2018.12/arm-linux-gnueabihf/gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf.tar.xz
tar xf gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf.tar.xz

export CC=`pwd`/gcc-linaro-6.5.0-2018.12-x86_64_arm-linux-gnueabihf/bin/arm-linux-gnueabihf
${CC}gcc --version
```
### Tải code uboot và apply patch
``` shell
# user@ubuntu:~$
git clone -b v2019.04 https://github.com/u-boot/u-boot --depth=1
cd u-boot/
git pull --no-edit https://github.com/beagleboard/u-boot v2019.04-bbb.io-am335x
```
### build uboot
```  shell
# user@ubuntu:~/u-boot$
make ARCH=arm CROSS_COMPILE=${CC} distclean
make ARCH=arm CROSS_COMPILE=${CC} am335x_evm_defconfig
make ARCH=arm CROSS_COMPILE=${CC}
```
### Tải code linux và apply patch
``` shell
# user@ubuntu:~$
git clone https://github.com/RobertCNelson/bb-kernel
cd bb-kernel/
git checkout origin/am33x-v5.4 -b tmp
```
### Lần build kernel đầu tiên
``` shell
# user@ubuntu:~/bb-kernel$
./build_kernel.sh
```
### Tải và giải nén rootfs
``` shell
# user@ubuntu:~$
wget -c https://rcn-ee.com/rootfs/eewiki/minfs/debian-10.10-minimal-armhf-2021-06-20.tar.xz
tar xf debian-10.10-minimal-armhf-2021-06-20.tar.xz
```
### Erase sdcard
``` shell
export DISK=/dev/sdb
sudo dd if=/dev/zero of=${DISK} bs=1M count=10
``` 
### Install uboot
``` shell
sudo dd if=./u-boot/MLO of=${DISK} count=1 seek=1 bs=128k
sudo dd if=./u-boot/u-boot.img of=${DISK} count=2 seek=1 bs=384k
``` 
### Create Partition Layout
``` shell
sudo sfdisk ${DISK} <<-__EOF__
4M,,L,*
__EOF__

sudo mkfs.ext4 -L rootfs -O ^metadata_csum,^64bit ${DISK}1
``` 
### write data to sdcard
``` shell
sudo mkdir -p /media/rootfs/
sudo mount ${DISK}1 /media/rootfs/

sudo mkdir -p /media/rootfs/opt/backup/uboot/
sudo cp -v ./u-boot/MLO /media/rootfs/opt/backup/uboot/
sudo cp -v ./u-boot/u-boot.img /media/rootfs/opt/backup/uboot/

export kernel_version=5.4.129-bone55 # biến kernel_version hiển thị sau khi build chương trình thành u-boot thành công

sudo tar xfvp ./debian-*-*-armhf-*/armhf-rootfs-*.tar -C /media/rootfs/
sync
sudo chown root:root /media/rootfs/
sudo chmod 755 /media/rootfs/

sudo sh -c "echo 'uname_r=${kernel_version}' >> /media/rootfs/boot/uEnv.txt"
sudo cp -v ./bb-kernel/deploy/${kernel_version}.zImage /media/rootfs/boot/vmlinuz-${kernel_version}
sudo mkdir -p /media/rootfs/boot/dtbs/${kernel_version}/
sudo tar xfv ./bb-kernel/deploy/${kernel_version}-dtbs.tar.gz -C /media/rootfs/boot/dtbs/${kernel_version}/
sudo tar xfv ./bb-kernel/deploy/${kernel_version}-modules.tar.gz -C /media/rootfs/
sudo sh -c "echo '/dev/mmcblk0p1  /  auto  errors=remount-ro  0  1' >> /media/rootfs/etc/fstab"
``` 
### Networking
- chỉnh sửa file: /etc/network/interfaces
``` shell
sudo nano /media/rootfs/etc/network/interfaces
```
``` shell
# add to /etc/network/interfaces
auto lo
iface lo inet loopback
 
auto eth0
iface eth0 inet dhcp
```

``` shell
sync
sudo umount /media/rootfs
``` 

## Build một chương trình sau khỉ build image thành công
### lệnh Rebuild
``` shell
# user@ubuntu:~/bb-kernel$
./tools/rebuild.sh
```
