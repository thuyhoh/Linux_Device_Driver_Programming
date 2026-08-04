#

# Device tree

## Introduction to the device tree
khái niệm
directory

## Syntax 
```
include /path
```
```
[label]: node-name[@unit-address] {
    [properties definitions]
    [child nodes]
};
```
### 1. Cấu trúc Cây & Tên Node (Node Naming)
![devicetree struct example](<Screenshot from 2026-08-01 21-13-29.png>)
- Devicetree được tổ chức dưới dạng danh sách liên kết phân cấp gồm các node (nút). Mỗi node đại diện cho một thiết bị phần cứng, một bus hoặc một thành phần logic.
- 
- cú pháp đặt tên Node:
``` dts
node-name@unit-address
# node-name: Tên tổng quát mô tả chức năng của thiết bị (ví dụ: cpu, memory, serial, ethernet, display)
# unit-address: Địa chỉ cơ sở (base address) dùng để truy cập thiết bị. Địa chỉ này phải tương ứng với địa chỉ đầu tiên được khai báo trong thuộc tính reg của node đó. thành phần này 
# Note: Nếu node không có thuộc tính reg (hoặc không gắn với địa chỉ cụ thể), phần @unit-address có thể được bỏ qua.
```

### Các thộc tính và kiểu dữ liệu
#### data type
```
Kiểu dữ liệu	Cú pháp trong DTS	Mô tả & Ví dụ
Empty (Flag)	read-only;	Thuộc tính dạng cờ, sự xuất hiện của nó tương đương giá trị True.
u32 / Cells	<0x1000 0x100>	Một hoặc nhiều ô số nguyên 32-bit (big-endian).
String	"ns16550"	Chuỗi ký tự kết thúc bằng byte NUL.
String list	"vendor,dev1", "vendor,dev2"	Danh sách chuỗi ký tự phân cách bằng dấu phẩy.
Byte array	[00 0b 6b 2f 80 02]	Mảng các giá trị byte (viết ở dạng Hex).
Phandle	<&label> hoặc <1>	Giá trị số 32-bit duy nhất dùng để tham chiếu chéo giữa các node.
```
#### property
- compatible
    - Chức năng: Thuộc tính quan trọng nhất dùng để ghép nối (match) thiết bị với driver tương ứng.

    - Định dạng: "vendor,model" (tên nhà sản xuất + tên dòng sản phẩm).
- \#address-cells và \#size-cells

    - Chức năng: Được đặt tại node cha để quy định cách các node con khai báo vùng địa chỉ trong thuộc tính reg.
    - \#address-cells: Số lượng ô 32-bit dùng để biểu diễn địa chỉ.

    - \#size-cells: Số lượng ô 32-bit dùng để biểu diễn kích thước (độ dài vùng nhớ).
- reg
    - Chức năng: Chứa danh sách các cặp (Địa chỉ, Độ dài) mô tả không gian thanh ghi hoặc bộ nhớ mà thiết bị chiếm dụng.
    - Định dạng: Được giải mã dựa theo #address-cells và #size-cells của node cha.

- status
    - Chức năng: Cho biết trạng thái hoạt động của thiết bị. Các giá trị tiêu chuẩn:
        - "okay": Thiết bị đang hoạt động bình thường.
        - "disabled": Thiết bị hiện bị tắt (driver sẽ bỏ qua).
        - "fail": Thiết bị gặp lỗi không thể khởi tạo.

- Thuộc tính không chuẩn 
```
[tiền tố ], tên thuộc tính = giá trị
```


### đường dẫn
/node-name-1/node-name-2/node-name-N

## Binding giữa platform dirver với device tree 
- file pcd.dtsi(giải định một node của device tree).
``` dts
/{
    pcdev1: pcdev-1 {
		compatible = "pcdev-E1x","pcdev-A1x";
		org,size = <512>;
		org,device-serial-num = "PCDEV1ABC123";
		org,perm = <0x11>;
	};
}
```
- pcd driver 
``` C
#include<linux/of.h>
#include<linux/of_device.h>

// .... code

struct of_device_id org_pcdev_dt_match[] =
{
	{.name = "pcdev-A1x",.driver_data = PCDEVA1X},
	{.name = "pcdev-B1x",.driver_data = PCDEVB1X},
    // ... node tiếp theo
	{/*Null termination */}
}

struct platform_driver pcd_platform_driver = 
{
	.probe = pcd_platform_driver_probe,
	.remove = pcd_platform_driver_remove,
	// ...
	.driver = {
        // ...
		.of_match_table = of_match_ptr(org_pcdev_dt_match) // thêm device vào table matching
	}
};

int pcd_platform_driver_probe(struct platform_device *pdev)
{
    // code ...
}

int pcd_platform_driver_remove(struct platform_device *pdev)
{
    // code ...
}

static int __init pcd_platform_driver_init(void)
{
    platform_driver_register(&pcd_platform_driver);
}
static int __init pcd_platform_driver_cleanup(void)
{
    platform_driver_unregister(&pcd_platform_driver);
}

module_init(pcd_platform_driver_init);
module_exit(pcd_platform_driver_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kiran Nayak");
MODULE_DESCRIPTION("A pseudo character platform driver which handles n platform pcdevs");

```

## Overlays



## Chia sẻ dữ liệu giữa device tree và platform driver

void *of_device_get_match_data(const struct device *dev);

of_match_device()

of_property_read_*()


CONFIG_OF
```
#ifdef CONFIG_OF
#define of_match_ptr(_ptr) (_ptr)
#else
#define of_match_ptr(_ptr) NULL
#endif
```


![of_node](<Screenshot from 2026-07-31 22-50-54.png>)

## Compile dtsi 
``` shell
# ~/bb-kernel/KERNEL
make 
```


