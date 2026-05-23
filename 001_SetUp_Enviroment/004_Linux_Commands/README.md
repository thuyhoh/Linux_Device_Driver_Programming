# Linux Commands Interface

## 1. Điều hướng thư mục (Directory Navigation)
- pwd : Hiển thị đường dẫn thư mục hiện tại
- ls : Liệt kê file và thư mục
    - ls -l (chi tiết)
    - ls -a (bao gồm file ẩn)

- cd <thư_mục> : Di chuyển vào thư mục
    - cd .. (quay lại thư mục cha)
    - cd ~ (về thư mục home)

## 2. Quản lý file và thư mục
- touch file.txt : Tạo file rỗng
- mkdir folder : Tạo thư mục
- rm file.txt : Xóa file
    - rm -r folder : Xóa thư mục (đệ quy)
- cp file1 file2 : Sao chép file
    - cp -r dir1 dir2 : Sao chép thư mục
- mv old new : Đổi tên hoặc di chuyển file/thư mục

## 3. Xem nội dung file
- cat file.txt : Hiển thị toàn bộ nội dung
- less file.txt : Xem nội dung có phân trang
- head file.txt : Xem 10 dòng đầu
- tail file.txt : Xem 10 dòng cuối
- tail -f log.txt : theo dõi log realtime

## 4. Quyền truy cập và người dùng
- chmod 755 file.sh : Thay đổi quyền file
- chown user:group file : Thay đổi chủ sở hữu
- whoami : Xem user hiện tại
- id : Thông tin user và group

## 5. Quản lý tiến trình (Process)
- ps : Danh sách tiến trình
- ps aux : Xem toàn bộ tiến trình
- top / htop : Giám sát hệ thống
- kill <PID> : Kết thúc tiến trình

## 6. Hệ thống và tài nguyên
- df -h : Dung lượng ổ đĩa
- du -sh folder : Dung lượng thư mục
- free -h : Bộ nhớ RAM
- uname -a : Thông tin hệ điều hành
- uptime : Thời gian hệ thống hoạt động

## 7. Mạng (Networking)
- ip a : Xem địa chỉ IP
- ping google.com : Kiểm tra kết nối mạng
- ss -tuln : Kiểm tra cổng đang lắng nghe
- curl url : Gửi request HTTP
- wget url : Tải file

## 8. Tìm kiếm và lọc dữ liệu
- find /path -name "file.txt" : Tìm file
- grep "text" file.txt : Tìm chuỗi trong file
- grep -r "text" folder : Tìm trong thư mục

## 9. Nén và giải nén
- tar -cvf file.tar folder : Nén tar
- tar -xvf file.tar : Giải nén
- zip -r file.zip folder
- unzip file.zip

## 10. Trợ giúp
- man ls : Hướng dẫn chi tiết lệnh
- ls --help : Trợ giúp nhanh
- history : Lịch sử lệnh đã dùng

