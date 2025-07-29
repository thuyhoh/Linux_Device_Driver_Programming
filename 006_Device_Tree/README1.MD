# Device Tree
## I. Giới thiệu chung
### 1. Khái niệm
- Device Tree (DT) là một cấu trúc dữ liệu được sử dụng trong Embedded Linux, đặc biệt là trên các hệ thống kiến trúc ARM và RISC-V, để mô tả phần cứng cho nhân Linux (kernel) một cách độc lập với mã nguồn kernel.
### 2. Đặc điểm
- File Device Tree thường có phần mở rộng là:
    - ``.dts``: Device Tree Source — file chính mô tả phần cứng
    - ``.dtsi``: Device Tree Source Include — file dùng chung được include vào .dts
    - ``.dtb``: Device Tree Blob — dạng nhị phân được kernel sử dụng (biên dịch từ .dts bằng dtc)
- đường dẫn đến các file 
    - soc bbb-kernel\KERNEL\arch\arm\boot\dts
    - common 
    - specifi
search kernel/arch/arm/boot/dts
soc-level
ls -l | grep am335x*
grep -nr am33.dtsi

comparetable
danh sách thiết bị 
danh sách driver  cần đi đăng ký thông qua struct platform driver
trường probe đại diện cho khi trường comparetable phát hiện được trong device tree và module được insmode lên hệ thống thì sẽ map đến 
trường probe đại diện cho khi rmmode module lên phần cứng thì sẽ map đến 

.driver đại diện thông tin cho phần cứng nào
module_platform_driver(my_led_drv); khai báo cho hệ thống


trong am335x-boneblack.dts
``` 
/{
    #address-ceil = <i>
    #size-ceil = <i>
    m_led{
        conpatible = "bbb-led,dt"
        reg = <0x44D07000 0x1000>;
        mled-gpios = <&gpio0 31 GPIO_ACTIVE_HIGH>;
        status = "okay"; // disable
    };

    foo_dev{

    }
};
```
// giả thích
khai báo node trong root
/ -> thuộc root

