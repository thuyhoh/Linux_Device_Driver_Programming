#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/device.h>
#include<linux/kdev_t.h>
#include<linux/uaccess.h>

#define DEV_MEM_SIZE 512

/* pseudo device's memory */
char device_buffer[DEV_MEM_SIZE];

/* This holds the device number */
dev_t device_number;

/* Cdev variable */
struct cdev_t pcd_cdev;

/* holds the class pointer */
struct class *pcd_class;

struct device *pcd_device;

int pcd_open(struct inode *inode, struct file *file);
int pcd_release(struct inode *inode, struct file *file);
ssize_t pcd_read(struct file *filp, char __user *user_buf, size_t size, loff_t *offset);
ssize_t pcd_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos);
loff_t pcd_lseek(struct file *filp, loff_t offset, int whence);

struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = pcd_open,
    .read = pcd_read,
    .write = pcd_write,
    .llseek = pcd_lseek,
    .release = pcd_release
};

static init __init pcd_driver_init(void)
{
    int ret;
    pr_info("pseudo character driver init\n");
    /* 1. Dynamically allocate device number */
    ret = alloc_chrdev_region(&device_number, 0, 1, "pcd_devices");
    if (ret < 0)
    {
        pr_err("Failed to allocate device number\n");
        goto out;
    }
    pr_info("funct: %s, <major>:<minor>=%d:%d\n",__funct__, MAJOR(device_number), MINOR(device_number));

    /* 2. Initialize the cdev struct with fops */
    cdev_init(&pcd_cdev, &fops);
    
    /* 3. Register a device structure with VFS */
    pcd_cdev.owner = THIS_MODULE;
    ret = cdev_add(&pcd_cdev, device_number, 1);
    if(ret < 0)
    {
        pr_err("Cdev add failed\n");
        goto unreg_chrdev;
    }
    /*4. create device class under /sys/class/ */
    pcd_class = class_create(THIS_MODULE, "pcd_class");
    if(IS_ERR(pcd_class))
    {
        pr_err("Class create failed\n");
        ret = PR_ERR(pcd_class);
        goto cdev_del;
    }
    pcd_device = device_create(pcd_class, NULL, device_number, NULL, "pcd_device0");
    if(IS_ERR(pcd_device))
    {
        pr_err("Class create failed\n");
        ret = PR_ERR(pcd_class);
        goto class_del;
    }
    
    pr_info("Module init was successful\n");
    return 0;

class_del:
    class_destroy(pcd_class);
cdev_del:
    cdev_del(&pcd_cdev);
unreg_chrdev:
    unregister_chrdev_region(device_number, 1);
out:
    pr_info("Module insertion failed\n");
    return ret;
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

MODULE_LICENSE("GPL");
MODULE_AUTHOR("THUY");
MODULE_DESCRIPTION("A Pseudo Character Driver");
MODULE_VERSION("1.0");
MODULE_INFO(board, "BEAGLEBONE BLACK REV C");

int pcd_open(struct inode *inode, struct file *file)
{
    pr_info("open was successfully\n");
    return 0;
}
int pcd_release(struct inode *inode, struct file *file)
{
    pr_info("release was successfully\n");
    return 0;
}
ssize_t pcd_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
    pr_info("Read request for %zu bytes\n", count);
    pr_info("Current file position = %lld\n", *f_pos);

    /* Adjust the 'count' */
    if((*f_pos + count) > DEV_MEM_SIZE)
    {
        count = DEV_MEM_SIZE - *f_pos;
    }    
    /* Copy to user */
    if(copy_to_user(buff, &device_buffer[*f_pos], count))
    {
        return -EFAULT;
    }

    /* Update the current file postion */
    *f_pos += count;

    pr_info("Number of bytes successfully read = %zu\n", count);
    pr_info("Update file position = %lld\n", *f_pos);

    return count; 

}
ssize_t pcd_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos)
{
    pr_info("Write request for %zu bytes\n", count);
    pr_info("Current file position = %lld\n", *f_pos);

    /* Adjust the 'count' */
	if((*f_pos + count) > DEV_MEM_SIZE)
		count = DEV_MEM_SIZE - *f_pos;

	if(!count){
		pr_err("No space left on the device \n");
		return -ENOMEM;
	}

    	/*copy from user */
	if(copy_from_user(&device_buffer[*f_pos],buff,count)){
		return -EFAULT;
	}

	/*update the current file postion */
	*f_pos += count;

	pr_info("Number of bytes successfully written = %zu\n",count);
	pr_info("Updated file position = %lld\n",*f_pos);

	/*Return number of bytes which have been successfully written */
	return count;
}
loff_t pcd_lseek(struct file *filp, loff_t offset, int whence)
{
    loff_t temp;

    pr_info("lssek requested \n");
    pr_info("Current value of the file position = %lld\n", filp->f_pos);

    switch (whence)
    {
    case SEEK_SET:
        if((offset > DEV_NUM_SIZE) || (offset < 0))
            return -EINVAL;
        filp->f_pos = offset;    
        break;
    
    case SEEK_CUR:
        temp = filp->f_pos + offset;
        if((temp > DEV_MEM_SIZE) || (temp < 0))
            return -EINVAL;
        filp->f_pos = temp;
        break;
    
        case SEEK_END:
        temp = DEV_MEM_SIZE + offset;
        if((temp > DEV_MEM_SIZE) || (temp < 0))
            return -EINVAL;
        filp->f_pos = temp;
        break;

    default:
        return -EINVAL;
        break;
    }

    pr_info("New value of the file position = %lld\n", filp->f_pos);
    return filp->f_pos;
}

