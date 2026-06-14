# Device Tree
## 1. Giới thiệu chung
Device Tree (DT) là một định dạng cấu trúc dữ liệu được sử dụng để mô tả các thành phần phần cứng phần cứng (bộ vi xử lý, bộ nhớ, các ngoại vi như I2C, SPI, UART, GPIO...) của hệ thống máy tính cho hệ điều hành Linux Kernel.
- Thay vì phải biên dịch lại kernel mỗi khi phần cứng thay đổi, Device Tree cho phép hệ điều hành đọc cấu hình phần cứng một cách động khi khởi động (boot time).
- Device Tree được tách biệt hoàn toàn phần mạch (Phần cứng) ra khỏi phần mềm điều khiển (Kernel). Kernel giờ đây đóng vai trò như một bộ khung chung, khi boot lên nó sẽ đọc file Device Tree để biết bo mạch hiện tại có những gì và tự động nạp driver tương ứng.




- Linux sử dụng DT cho:
• Nhận dạng nền tảng
• Điền thông tin thiết bị: Nhân hệ điều hành phân tích dữ liệu cây thiết bị và tạo ra cấu trúc dữ liệu phần mềm cần thiết, cấu trúc này sẽ được sử dụng bởi mã nhân hệ điều hành.


- Cây thiết bị hỗ trợ cách viết mô tả phần cứng theo thứ bậc ở các cấp độ:
  - SOC level
  - Common board level 
  - Board-specific level. 
- Hầu hết thời gian, việc viết một cây thiết bị mới không khó, và bạn có thể tái sử dụng hầu hết các thông tin phần cứng chung từ tệp cây thiết bị của bo mạch tham chiếu.
## Mô tả hệ thống phân cấp phần cứng

## 2. Tổng quan cấu trúc cây thiết bị
- Cây thiết bị là một tập hợp các nút thiết bị
- Một ‘nút thiết bị’ hoặc đơn giản gọi là ‘một nút’ đại diện cho một thiết bị. Các nút được tổ chức theo một cách có hệ thống nào đó bên trong tệp cây thiết bị.
- Chúng cũng có mối quan hệ cha con, và mỗi cây thiết bị phải có một nút gốc
- Một nút tự giải thích bản thân, tức là tiết lộ dữ liệu và tài nguyên của nó bằng cách sử dụng “thuộc tính” của nó.
### Root node
![alt text](image-1.png)
## Device tree syntax
### Node name
### Node label
### Standard and non-standard property names
### Data type representation
### Example
![alt text](image.png)
``` dts

```
## Documents
https://elinux.org/Device_Tree_What_It_Is
https://www.kernel.org/doc/Documentation/devicetree/usage-model.txt
https://www.devicetree.org/
