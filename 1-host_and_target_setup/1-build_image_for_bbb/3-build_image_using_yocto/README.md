```
sudo apt-get -y install gawk wget git-core diffstat unzip texinfo gcc-multilib \
     build-essential chrpath socat cpio python3 python3-pip python3-pexpect \
     libsdl1.2-dev xterm make xsltproc docbook-utils fop dblatex xmlto lz4
```
- Clone source code
```
git clone -b scarthgap git://git.yoctoproject.org/poky/
 
```
cd poky
git clone -b scarthgap git://git.openembedded.org/meta-openembedded
git clone -b scarthgap git://git.yoctoproject.org/meta-ti
git clone -b scarthgap https://git.yoctoproject.org/meta-arm
```

```
source oe-init-build-env build-bbb
```

