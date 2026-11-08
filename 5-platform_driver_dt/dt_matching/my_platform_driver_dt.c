#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>

static int demo_probe(struct platform_device *pdev)
{
    dev_info(&pdev->dev, "probe: Device Tree matching successful\n");

    return 0;
}

static const struct of_device_id demo_of_match[] = {
    { .compatible = "demo-device" },
    { }
};

MODULE_DEVICE_TABLE(of, demo_of_match);

static struct platform_driver demo_driver = {
    .probe = demo_probe,
    .driver = {
        .name = "demo_platform",
        .of_match_table = demo_of_match,
    },
};


module_platform_driver(demo_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Thuy");
MODULE_DESCRIPTION("Minimal Device Tree platform driver");