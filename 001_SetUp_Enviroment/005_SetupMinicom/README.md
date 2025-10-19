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