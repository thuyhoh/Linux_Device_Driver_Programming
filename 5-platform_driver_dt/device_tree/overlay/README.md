# device tree overlay
## 1. Cấu trúc file overlay
``` shell
/dts-v1/;
/plugin/;

/ {

    fragment@0 {
        # target-path = "/target";
        target = <&target>;
        __overlay__ {
            # property = value
        };
    };

    # fragment@1 2 3 ...    
};
```
| Thành phần | Ý nghĩa |
|---|---|
| `/dts-v1/;` | Khai báo cú pháp Device Tree Source |
| `/plugin/;` | Khai báo overlay, cho phép tham chiếu label nằm ngoài file |
| `/ { ... };` | Node gốc chứa nội dung file overlay |
| `compatible` ở gốc | Thông tin tương thích board; không bắt buộc cho mọi overlay |
| `fragment@0` | fragment  |
| `target = <&target>;` | Chọn node mang label `target` trong device tree cần thay đổi |
| `target-path = "/target-path"`| đường đẫn tuyệt đôi trong device tree|
| `__overlay__{...}` | Chứa những thuộc tính và node con được overide |   


## 2. Compiler overlay
- sudo apt install device-tree-compiler 
``` sh
dtc -@ -I dts -O dtb -o overlay_file.dtbo overlay_file.dts
```
- Kiểm tra các thông số 
## 2. tích hợp vào bbb
- chuyển file overlay compiler vào /lib/firmware/
- chỉnh sửa uEnv.txt 
```
enable_uboot_overlays=1
uboot_overlay_addr<n>=/lib/firmware/overlay_file.dtbo # uboot_overlay_addr<no>: file overlay thứ n(0, 1, 2, ...)
```
- reboot board
