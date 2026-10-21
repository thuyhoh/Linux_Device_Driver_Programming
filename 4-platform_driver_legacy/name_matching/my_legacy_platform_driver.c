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

#undef pr_fmt
#define pr_fmt(fmt) "%s : " fmt, __func__

/* Permission codes */
#define RDONLY 0x01
#define WRONLY 0x10
#define RDWR   0x11

/* Platform data structure */
struct dev_data {
    int size;
    int perm;
    char *serial_number;
};

/* Device private data structure */
struct device_prv_data {
    struct dev_data pdata;
    char *buffer;
    dev_t dev_num;
    struct cdev my_cdev;
    struct device *device_file;
};

/* Driver private data structure */
struct driver_prv_data {
    int total_devices;
    dev_t dev_num_base;
    struct class *class_mychar;
};

static struct driver_prv_data drv_data;

/* Forward declarations */
static const struct file_operations fops;

int check_permission(int dev_perm, int acc_mode)
{
    if (dev_perm == RDWR)
        return 0;
    
    /* Ensures readonly access */
    if ((dev_perm == RDONLY) && ((acc_mode & FMODE_READ) && !(acc_mode & FMODE_WRITE)))
        return 0;
    
    /* Ensures writeonly access */
    if ((dev_perm == WRONLY) && ((acc_mode & FMODE_WRITE) && !(acc_mode & FMODE_READ)))
        return 0;

    return -EPERM;
}

static loff_t drv_llseek(struct file *filp, loff_t offset, int whence)
{
    struct device_prv_data *dev_data = filp->private_data;
    int max_size = dev_data->pdata.size;
    loff_t temp;

    pr_info("lseek requested \n");
    pr_info("Current value of the file position = %lld\n", filp->f_pos);

    switch (whence)
    {
        case SEEK_SET:
            if ((offset > max_size) || (offset < 0))
                return -EINVAL;
            filp->f_pos = offset;
            break;
        case SEEK_CUR:
            temp = filp->f_pos + offset;
            if ((temp > max_size) || (temp < 0))
                return -EINVAL;
            filp->f_pos = temp;
            break;
        case SEEK_END:
            temp = max_size + offset;
            if ((temp > max_size) || (temp < 0))
                return -EINVAL;
            filp->f_pos = temp;
            break;
        default:
            return -EINVAL;
    }
    
    pr_info("New value of the file position = %lld\n", filp->f_pos);
    return filp->f_pos;
}

static int drv_open(struct inode *inode, struct file *file) 
{
    struct device_prv_data *dev_data;
    int ret;

    /* Lấy con trỏ struct device_prv_data từ cdev bên trong inode */
    dev_data = container_of(inode->i_cdev, struct device_prv_data, my_cdev);
    file->private_data = dev_data;

    /* Kiểm tra quyền truy cập */
    ret = check_permission(dev_data->pdata.perm, file->f_flags);
    if (ret) {
        pr_err("Permission check failed\n");
        return ret;
    }

    pr_info("open was successful\n");
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
    int max_size = dev_data->pdata.size;

    pr_info("Read requested for %zu bytes \n", count);
    pr_info("Current file position = %lld\n", *f_pos);

    /* Adjust the 'count' */
    if ((*f_pos + count) > max_size)
        count = max_size - *f_pos;

    /* Copy to user */
    if (copy_to_user(buff, &dev_data->buffer[*f_pos], count)) {
        return -EFAULT;
    }

    /* Update the current file position */
    *f_pos += count;

    pr_info("Number of bytes successfully read = %zu\n", count);
    pr_info("Updated file position = %lld\n", *f_pos);

    return count;
}

static ssize_t drv_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos)
{
    struct device_prv_data *dev_data = filp->private_data;
    int max_size = dev_data->pdata.size;

    pr_info("Write requested for %zu bytes\n", count);
    pr_info("Current file position = %lld\n", *f_pos);

    /* Adjust the 'count' */
    if ((*f_pos + count) > max_size)
        count = max_size - *f_pos;

    if (!count) {
        pr_err("No space left on the device \n");
        return -ENOMEM;
    }

    /* Copy from user */
    if (copy_from_user(&dev_data->buffer[*f_pos], buff, count)) {
        return -EFAULT;
    }

    /* Update the current file position */
    *f_pos += count;

    pr_info("Number of bytes successfully written = %zu\n", count);
    pr_info("Updated file position = %lld\n", *f_pos);

    return count;
}

static const struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = drv_open,
    .release = drv_release,
    .read    = drv_read,
    .write   = drv_write,
    .llseek  = drv_llseek 
};

/* Called when the device is removed from the system */
static int my_plf_driver_remove(struct platform_device *pdev)
{
    struct device_prv_data *dev_data = dev_get_drvdata(&pdev->dev);

    /* 1. Remove device created with device_create() */
    device_destroy(drv_data.class_mychar, dev_data->dev_num);
    
    /* 2. Remove cdev entry from the system */
    cdev_del(&dev_data->my_cdev);

    drv_data.total_devices--;

    pr_info("A device is removed\n");
    return 0;
}

static int my_plf_driver_probe(struct platform_device *pdev)
{
    int ret;
    struct device_prv_data *dev_data;
    struct dev_data *pdata;

    pr_info("A device is detected\n");

    /* Get the platform data */
    pdata = (struct dev_data *)dev_get_platdata(&pdev->dev);
    if (!pdata) {
        pr_info("No platform data available\n");
        return -EINVAL;
    }

    /* Dynamically allocate memory for the device private data */
    dev_data = devm_kzalloc(&pdev->dev, sizeof(*dev_data), GFP_KERNEL);
    if (!dev_data) {
        pr_info("Cannot allocate memory \n");
        return -ENOMEM;
    }

    /* Save the device private data pointer in platform device structure */
    dev_set_drvdata(&pdev->dev, dev_data);
    
    dev_data->pdata.size = pdata->size;
    dev_data->pdata.perm = pdata->perm;
    dev_data->pdata.serial_number = pdata->serial_number;

    /* Dynamically allocate memory for the device buffer */
    dev_data->buffer = devm_kzalloc(&pdev->dev, dev_data->pdata.size, GFP_KERNEL);
    if (!dev_data->buffer) {
        pr_info("Cannot allocate memory \n");
        return -ENOMEM;
    }

    /* Get the device number */
    dev_data->dev_num = drv_data.dev_num_base + pdev->id;

    /* Do cdev init and cdev add */
    cdev_init(&dev_data->my_cdev, &fops);
    dev_data->my_cdev.owner = THIS_MODULE;
    
    ret = cdev_add(&dev_data->my_cdev, dev_data->dev_num, 1);
    if (ret < 0) {
        pr_err("Cdev add failed\n");
        return ret;
    }

    /* Create device file for the detected platform device */
    dev_data->device_file = device_create(drv_data.class_mychar, NULL, dev_data->dev_num, NULL, "pcdev-%d", pdev->id);
    if (IS_ERR(dev_data->device_file)) {
        pr_err("Device create failed\n");
        ret = PTR_ERR(dev_data->device_file);
        cdev_del(&dev_data->my_cdev);
        return ret;
    }

    drv_data.total_devices++;
    pr_info("Probe was successful\n");

    return 0;
}

static struct platform_driver my_platform_driver = {
    .probe = my_plf_driver_probe,
    .remove = my_plf_driver_remove,
    .driver = {
        .name = "my-platform-device"
    }
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
MODULE_DESCRIPTION("A basic platform driver kernel module");
MODULE_INFO(board, "Beaglebone black REV A5");