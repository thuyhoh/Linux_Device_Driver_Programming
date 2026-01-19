# Update eMMC
## I. Giới thiệu chung 
- Trên BeagleBone Black (BBB), eMMC chính là bộ nhớ lưu trữ chính của hệ thống, đóng vai trò tương tự như ổ cứng/SSD trong máy tính.
- Chức năng chính của eMMC:
    - Lưu trữ hệ điều hành (OS)
    - Lưu trữ dữ liệu người dùng & ứng dụng
    - Cho phép cập nhật hệ điều hành
- Thông số mặc định
    - Dung lượng 4GB
    - Thiết kế hàn cố định trên board
    - Giao tiếp theo chuẩn mmc

## II. Cập nhật Debian OS trong eMMC
### 1. Boot thông qua SDcard
- Tải Image : [link](https://www.beagleboard.org/distros) đồng thời ghi dữ liệu vào trong thẻ SDcard(>= 4GB)
- Lắp SDcard > cấp nguồn > Giữ boot button(S2) đến khi uLed hiển thị
### 2. Flash Os vào eMMC
``` shell
vim /opt/scripts/tools/eMMC/init-eMMC-flasher-v3.sh
```
- Loại bỏ 1 dấu `#` trước `cmdline=init=/opt/scripts/tools/eMMC/init-eMMC-flasher-v3.sh` 
- Nếu không có chạy lệnh và chờ đến khi quá trình ghi thành công
``` shell
# ./opt/scripts/tool/eMMC/
sudo ./init-eMMC-flasher-v3.sh
```
- Tháo thẻ SDcard và loại bỏ nguồn
### 3. Nguyên lý của eMMC flasher script
- eMMC flasher script có chức năng 
### III. Kiểm tra phiên bản
- kiểm tra phiên bản Debian os trên beaglebone black thông qua lệnh:
``` shell
lsb_release -da
```
- hiển thị phiên bản của kernel (nhân Linux) mà hệ thống đang chạy
``` shell
uname -r
```

