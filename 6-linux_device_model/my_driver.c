#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/errno.h>
#include <linux/mod_devicetable.h>

/* Trạng thái riêng của mỗi thiết bị */
struct demo_data {
    int value;
    struct mutex lock;
};

/* Được gọi khi userspace đọc file value */
static ssize_t value_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    struct demo_data *data = dev_get_drvdata(dev);
    int value;

    mutex_lock(&data->lock);
    value = data->value;
    mutex_unlock(&data->lock);

    return sysfs_emit(buf, "%d\n", value);
}

/* Được gọi khi userspace ghi file value */
static ssize_t value_store(struct device *dev,struct device_attribute *attr,
                           const char *buf,
                           size_t count)
{
    struct demo_data *data = dev_get_drvdata(dev);
    int value;
    int ret;

    ret = kstrtoint(buf, 10, &value);
    if (ret)
        return ret;

    /* Giới hạn do ví dụ này quy định */
    if (value < 0 || value > 1000)
        return -EINVAL;

    mutex_lock(&data->lock);
    data->value = value;
    mutex_unlock(&data->lock);

    return count;
}

/*
 * Tạo thuộc tính tên value, quyền 0644.
 * Macro sử dụng value_show() và value_store().
 */
static DEVICE_ATTR_RW(value);

/* Danh sách các thuộc tính của thiết bị */
static struct attribute *demo_attrs[] = {
    &dev_attr_value.attr,
    NULL,
};

/* Tạo demo_groups từ demo_attrs */
ATTRIBUTE_GROUPS(demo);

/* Kernel gọi khi device match với driver */
static int demo_probe(struct platform_device *pdev)
{
    struct demo_data *data;

    data = devm_kzalloc(&pdev->dev, sizeof(*data), GFP_KERNEL);
    if (!data)
        return -ENOMEM;

    data->value = 0;
    mutex_init(&data->lock);

    /* Gắn dữ liệu riêng vào thiết bị */
    platform_set_drvdata(pdev, data);

    dev_info(&pdev->dev, "probe successful\n");

    return 0;
}

static const struct platform_device_id demo_ids[] = {
    { .name = "demo-device" },
    { }
};

MODULE_DEVICE_TABLE(platform, demo_ids);

/* Đối tượng platform driver */
static struct platform_driver demo_driver = {
    .probe = demo_probe,
    .id_table = demo_ids,
    .driver = {
        .name = "demo_driver",
        .dev_groups = demo_groups,
    },
};

module_platform_driver(demo_driver);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Demo platform driver with sysfs");
