#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>

dev_t device_number;

cdev_t pcd_cdev;

struct class *pcd_class;

struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = pcd_open,
    .read = pcd_read,
    .write = pcd_write,
    .llseek = pcd_llseek,
    .release = pcd_release};

int (*pcd_open)(struct inode *inode, struct file *file);
int (*pcd_release)(struct inode *inode, struct file *file);
ssize_t (*pcd_read)(struct file *filp, char __user *user_buf, size_t size, loff_t *offset);
ssize_t (*pcd_write)(struct file *filp, const char __user *user_buf, size_t size, loff_t *offset);
loff_t (*pcd_llseek)(struct file *filp, loff_t offset, int whence);

static init __init pcd_driver_init(void)
{
    int ret;
    pr_info("pseudo character driver init\n");
    /* 1. Dynamically allocate device number */
    ret = alloc_chrdev_region(&device_number, 0, 1, "pcd_devices");
    if (ret < 0)
    {
        pr_err("Failed to allocate device number\n");
        return ret;
    }

    /* 2. Initialize the cdev struct with fops */
    cdev_init(&pcd_cdev, &fops);
    pcd_cdev.owner = THIS_MODULE;
    /* 3. Register a device structure with VFS */
    ret = cdev_add(&pcd_cdev, device_number, 1);
    /**/
    class_create(THIS_MODULE, "pcd_class");
    device_create(pcd_class, NULL, device_number, NULL, "pcd_device0");
    return 0;
}

static init __exit pcd_driver_exit(void)
{
    pr_info("pseudo character driver exit\n");
    device_destroy(pcd_class, device_number);
    class_destroy(pcd_class);
    cdev_del(&pcd_cdev);
    unregister_chrdev_region(device_number, 1);
}

module_init(pcd_driver_init);
module_exit(pcd_driver_exit);

int (*pcd_open)(struct inode *inode, struct file *file)
{
    pr_info("pcd_open function called\n");
    return 0;
}
int (*pcd_release)(struct inode *inode, struct file *file)
{
    pr_info("pcd_release function called\n");
    return 0;
}
ssize_t (*pcd_read)(struct file *filp, char __user *user_buf, size_t size, loff_t *offset)
{
    pr_info("pcd_read function called\n");
    return 0;
}
ssize_t (*pcd_write)(struct file *filp, const char __user *user_buf, size_t size, loff_t *offset)
{
    pr_info("pcd_write function called\n");
    return 0;
}
loff_t (*pcd_llseek)(struct file *filp, loff_t offset, int whence)
{
    pr_info("pcd_llseek function called\n");
    return 0;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("THUY");
MODULE_DESCRIPTION("A Pseudo Character Driver");
MODULE_VERSION("1.0");
MODULE_INFO(board, "BEAGLEBONE BLACK REV C");
