# Setup for bbb
## Setup eth network

### Host pc
- ktr ket noi\
```
thuy@ASUSThuy:~$ ifconfig
enp46s0: flags=4163<UP,BROADCAST,RUNNING,MULTICAST>  mtu 1500
        inet 10.42.0.1  netmask 255.255.255.0  broadcast 10.42.0.255
......................
### ip: 10.42.0.1

### Bật tính năng chuyển tiếp gói tin (IP Forwarding)
sudo sysctl -w net.ipv4.ip_forward=1

### Cấu hình NAT
sudo iptables -t nat -A POSTROUTING -o wlp45s0 -j MASQUERADE
sudo iptables -A FORWARD -i enp46s0 -o wlp45s0 -j ACCEPT
sudo iptables -A FORWARD -i wlp45s0 -o enp46s0 -m state --state RELATED,ESTABLISHED -j ACCEPT
```
### bbb
```
sudo ip addr flush dev eth0
sudo ip addr add 10.42.0.2/24 dev eth0
sudo ip link set eth0 up

sudo ip route add default via 10.42.0.1

echo "nameserver 8.8.8.8" | sudo tee /etc/resolv.conf

### Kiểm tra kết nối
sudo ping -c 3 10.42.0.1
sudo ping -c 3 8.8.8.8
```

### truyen file thong qua internet
```
scp /đường/dẫn/trên/pc/ten_file.txt debian@10.42.0.2:/home/debian/ # truyen file pc den bbb
scp debian@10.42.0.2:/home/debian/file_tren_bbb.txt /đường/dẫn/luu/tren/pc/ # keo file tuwf bbb
```



