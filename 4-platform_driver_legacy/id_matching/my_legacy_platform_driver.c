#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/platform_device.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>
#include <linux/mod_devicetable.h>

#define DEVICE_NAME "my_char_device"
#define DEV_MEM_SIZE 512

#undef pr_fmt
#define pr_fmt(fmt) "%s : " fmt, __func__

/* permission codes */
#define RDONLY 0x01 
#define WRONLY 0x10
#define RDWR   0x11

/* platform data of the pcdev */
struct plf_data
{
    int size;
    int perm;
    const char *serial_number;
};

/* Device private data structure */
struct device_prv_data
{
    struct plf_data pdata;
    char *buffer;
    dev_t dev_num;
    struct cdev cdev;
};

/* Driver private data structure */
struct driver_prv_data
{
    int total_devices;
    dev_t device_num_base;
    struct class *class_mychar;
};

static struct driver_prv_data drv_data;

/* Kiểm tra quyền truy cập */
static int check_permission(int dev_perm, int acc_mode)
{
    if (dev_perm == RDWR)
        return 0;
    
    if ((dev_perm == RDONLY) && ((acc_mode & FMODE_READ) && !(acc_mode & FMODE_WRITE)))
        return 0;
    
    if ((dev_perm == WRONLY) && ((acc_mode & FMODE_WRITE) && !(acc_mode & FMODE_READ)))
        return 0;

    return -EPERM;
}

static int drv_open(struct inode *inode, struct file *file)
{
    struct device_prv_data *dev_data;
    int ret;

    /* Lấy con trỏ struct device_prv_data thông qua cdev container */
    dev_data = container_of(inode->i_cdev, struct device_prv_data, cdev);
    file->private_data = dev_data;

    /* Kiểm tra quyền đọc/ghi */
    ret = check_permission(dev_data->pdata.perm, file->f_flags);
    if (ret) {
        pr_err("Permission denied\n");
        return ret;
    }

    pr_info("open was successful for device with serial: %s\n", dev_data->pdata.serial_number);
    return 0;
}

static int drv_release(struct inode *inode, struct file *file)
{
    pr_info("release was successful\n");
    return 0;
}

static ssize_t drv_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos)
{
    struct device_prv_data *dev_data = filp->private_data;

    pr_info("Read requested for %zu bytes \n", count);
    pr_info("Current file position = %lld\n", *f_pos);

    if (*f_pos + count > dev_data->pdata.size)
        count = dev_data->pdata.size - *f_pos;

    if (copy_to_user(buff, &dev_data->buffer[*f_pos], count))
        return -EFAULT;

    *f_pos += count;
    pr_info("Successfully read %zu bytes\n", count);
    return count;
}

static ssize_t drv_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos)
{
    struct device_prv_data *dev_data = filp->private_data;

    pr_info("Write requested for %zu bytes\n", count);
    pr_info("Current file position = %lld\n", *f_pos);

    if (*f_pos + count > dev_data->pdata.size)
        count = dev_data->pdata.size - *f_pos;

    if (!count) {
        pr_err("No space left on the device \n");
        return -ENOMEM;
    }

    if (copy_from_user(&dev_data->buffer[*f_pos], buff, count))
        return -EFAULT;

    *f_pos += count;
    pr_info("Successfully wrote %zu bytes\n", count);
    return count;
}

static loff_t drv_llseek(struct file *filp, loff_t offset, int whence)
{
    struct device_prv_data *dev_data = filp->private_data;
    loff_t temp;

    switch (whence) {
        case SEEK_SET:
            if ((offset > dev_data->pdata.size) || (offset < 0))
                return -EINVAL;
            filp->f_pos = offset;
            break;
        case SEEK_CUR:
            temp = filp->f_pos + offset;
            if ((temp > dev_data->pdata.size) || (temp < 0))
                return -EINVAL;
            filp->f_pos = temp;
            break;
        case SEEK_END:
            temp = dev_data->pdata.size + offset;
            if ((temp > dev_data->pdata.size) || (temp < 0))
                return -EINVAL;
            filp->f_pos = temp;
            break;
        default:
            return -EINVAL;
    }
    return filp->f_pos;
}

static const struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = drv_open,
    .release = drv_release,
    .read    = drv_read,
    .write   = drv_write,
    .llseek  = drv_llseek 
};

/* --- Platform Driver Probe & Remove --- */
int my_plf_driver_probe(struct platform_device *pdev)
{
    int ret;
    struct device_prv_data *my_dev_data;
    struct plf_data *pdata;
    struct device *device_mychar;

    pr_info("A device is detected\n");

    pdata = (struct plf_data *)dev_get_platdata(&pdev->dev);
    if (!pdata) {
        pr_err("No platform data available\n");
        return -EINVAL;
    }

    my_dev_data = devm_kzalloc(&pdev->dev, sizeof(struct device_prv_data), GFP_KERNEL);
    if (!my_dev_data)
        return -ENOMEM;

    my_dev_data->pdata.size = pdata->size;
    my_dev_data->pdata.perm = pdata->perm;
    my_dev_data->pdata.serial_number = pdata->serial_number;

    /* Cấp phát bộ nhớ buffer cho thiết bị dựa trên size từ platform data */
    my_dev_data->buffer = devm_kzalloc(&pdev->dev, my_dev_data->pdata.size, GFP_KERNEL);
    if (!my_dev_data->buffer)
        return -ENOMEM;

    /* Cấp phát dev_t động cho từng device được probe */
    ret = alloc_chrdev_region(&my_dev_data->dev_num, 0, 1, DEVICE_NAME);
    if (ret < 0)
        return ret;

    cdev_init(&my_dev_data->cdev, &fops);
    my_dev_data->cdev.owner = THIS_MODULE;

    ret = cdev_add(&my_dev_data->cdev, my_dev_data->dev_num, 1);
    if (ret < 0)
        goto unreg_chrdev;

    /* Tạo device node trong /dev */
    device_mychar = device_create(drv_data.class_mychar, NULL, my_dev_data->dev_num, NULL, "my_char_device");
    if (IS_ERR(device_mychar)) {
        ret = PTR_ERR(device_mychar);
        goto cdev_del;
    }

    dev_set_drvdata(&pdev->dev, my_dev_data);
    pr_info("Device probed successfully. Serial: %s, Size: %d\n", my_dev_data->pdata.serial_number, my_dev_data->pdata.size);
    return 0;

cdev_del:
    cdev_del(&my_dev_data->cdev);
unreg_chrdev:
    unregister_chrdev_region(my_dev_data->dev_num, 1);
    return ret;
}

int my_plf_driver_remove(struct platform_device *pdev)
{
    struct device_prv_data *my_dev_data = platform_get_drvdata(pdev);

    device_destroy(drv_data.class_mychar, my_dev_data->dev_num);
    cdev_del(&my_dev_data->cdev);
    unregister_chrdev_region(my_dev_data->dev_num, 1);

    pr_info("Platform driver removed successfully\n");
    return 0;
}

struct platform_device_id my_plfdevs_ids[] = {
    { .name = "my_platform_device_1", .driver_data = 0 },
    { } 
};
MODULE_DEVICE_TABLE(platform, my_plfdevs_ids);

struct platform_driver my_platform_driver = {
    .probe    = my_plf_driver_probe,
    .remove   = my_plf_driver_remove,
    .id_table = my_plfdevs_ids,
    .driver   = {
        .name = "my-platform-device"
    },
};

/* Module's init entry point */
static int __init plf_driver_init(void)
{
    int ret;
    pr_info("New platform driver initialized\n");

    drv_data.class_mychar = class_create(THIS_MODULE, "my_char_class");
    if (IS_ERR(drv_data.class_mychar)) {
        return PTR_ERR(drv_data.class_mychar);
    }

    ret = platform_driver_register(&my_platform_driver);
    if (ret < 0) {
        class_destroy(drv_data.class_mychar);
        return ret;
    }

    return 0;
}

/* Module's cleanup entry point */
static void __exit plf_driver_exit(void)
{
    platform_driver_unregister(&my_platform_driver);
    class_destroy(drv_data.class_mychar);
    pr_info("Platform driver unloaded successfully\n");
}

module_init(plf_driver_init);
module_exit(plf_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Thuy");
MODULE_DESCRIPTION("A basic platform driver kernel module with char device support");