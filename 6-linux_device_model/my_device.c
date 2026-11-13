#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/err.h>

static struct platform_device *demo_device;

static int __init demo_device_init(void)
{
    demo_device = platform_device_register_simple(
        "demo-device",
        PLATFORM_DEVID_NONE,
        NULL,
        0
    );

    if (IS_ERR(demo_device))
        return PTR_ERR(demo_device);

    pr_info("demo_device: registered\n");
    return 0;
}

static void __exit demo_device_exit(void)
{
    platform_device_unregister(demo_device);

    pr_info("demo_device: unregistered\n");
}

module_init(demo_device_init);
module_exit(demo_device_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Demo platform device");