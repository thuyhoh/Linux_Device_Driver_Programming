
# Setup Wifi qua USB cap cho Beaglebone black
## I. Windows
- control Panel > Network and Internet > Network and Sharing Centrel > Change adapter setting

- chọn nguồn mạng muốn share > properties 
![image](./imgs/Picture1.png)

- share > Chọn mạng share
![image](./imgs/Picture2.png)

- IP bbb > properties
![image](./imgs/Picture3.png)

- Điền giá trị như trên 
![image](./imgs/Picture4.png)
## II. Ubuntu




## III. Beaglebone black
``` shell
sudo route add đefault gw 192.168.7.1 usb0

echo -e "nameserver 8.8.8.8\nnameserver 8.8.4.4" | sudo tee /etc/resolv.conf
```

# Setup Minicom in Linux
- Install minicom packet
``` shell
sudo apt-get install -y minicom
```
- Configurarion mincom 
``` shell
sudo minicom -s
# Configuration > Serial port setup
## Serial Device: /dev/ttyUSB0
## Bps/Par/Bits : 115200 8N1
# Save setup ad dfl
```
- Open minicom
``` shell
sudo minicom
```