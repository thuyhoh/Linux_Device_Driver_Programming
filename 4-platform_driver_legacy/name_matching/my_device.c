#include<linux/module.h>
#include<linux/platform_device.h>

#undef pr_fmt
#define pr_fmt(fmt) "%s : " fmt,__func__

#define RDWR    0x11
#define RDONLY  0x01
#define WRONLY  0x10

struct prv_plf_data
{
	int size;
	int perm;
	const char *serial_number;

};

struct prv_plf_data my_plf_data = {
    .size = 512,
    .perm = RDWR,
    .serial_number = "1234567890"
};

void plf_device_release(struct device *dev)
{
	pr_info("Device released \n");
}

struct platform_device my_platform_device1 = {
    .name = "my_platform_device_1",
    .id = -1,
    .dev = {
        .platform_data = &my_plf_data,
        .release = plf_device_release
    }
};

/*Module's init entry point */
static int __init plf_device_init(void)
{
    pr_info("Hello world\n");

    platform_device_register(&my_platform_device1);
    // platform_device_register(&my_platform_device2);

    // platform_add_devices(platform_device_arr,ARRAY_SIZE(platform_device_arr));

    return 0;
}

/*Module's cleanup entry point */
static void __exit plf_device_cleanup(void)
{
    platform_device_unregister(&my_platform_device1);
    // platform_device_unregister(&my_platform_device2);   
    pr_info("Device setup module loaded \n");
}

module_init(plf_device_init);
module_exit(plf_device_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Thuy");
MODULE_DESCRIPTION("A simple hello world kernel module");
MODULE_INFO(board,"Beaglebone black REV A5");
