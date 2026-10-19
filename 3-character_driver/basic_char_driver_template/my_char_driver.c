#include<linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>

#define DEVICE_NAME "my_char_device"
#define DEV_MEM_SIZE 512

#undef pr_fmt
#define pr_fmt(fmt) "%s : " fmt,__func__

/*permission codes */
#define RDONLY 0x01
#define WRONLY 0X10
#define RDWR   0x11

char device_buffer[DEV_MEM_SIZE];

static dev_t dev_num;
static struct cdev my_cdev;
static struct class *class_mychar = NULL;
static struct device *device_mychar = NULL;

static loff_t drv_llseek(struct file *filp, loff_t offset, int whence)
{
    loff_t temp;

	pr_info("lseek requested \n");
	pr_info("Current value of the file position = %lld\n",filp->f_pos);

	switch(whence)
	{
		case SEEK_SET:
			if((offset > DEV_MEM_SIZE) || (offset < 0))
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
	}
	
	pr_info("New value of the file position = %lld\n",filp->f_pos);

	return filp->f_pos;
}

int check_permission(int dev_perm, int acc_mode)
{

	if(dev_perm == RDWR)
		return 0;
	
	//ensures readonly access
	if( (dev_perm == RDONLY) && ( (acc_mode & FMODE_READ) && !(acc_mode & FMODE_WRITE) ) )
		return 0;
	
	//ensures writeonly access
	if( (dev_perm == WRONLY) && ( (acc_mode & FMODE_WRITE) && !(acc_mode & FMODE_READ) ) )
		return 0;

	return -EPERM;

}

static int drv_open(struct inode *inode, struct file *file) {
    pr_info("open was successful\n");
    return 0;
}

static int drv_release(struct inode *inode, struct file *file) {
    pr_info("release was successful\n");
    return 0;
}

static ssize_t drv_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos)
{
	pr_info("Read requested for %zu bytes \n",count);
	pr_info("Current file position = %lld\n",*f_pos);

	
	/* Adjust the 'count' */
	if((*f_pos + count) > DEV_MEM_SIZE)
		count = DEV_MEM_SIZE - *f_pos;

	/*copy to user */
	if(copy_to_user(buff,&device_buffer[*f_pos],count)){
		return -EFAULT;
	}

	/*update the current file postion */
	*f_pos += count;

	pr_info("Number of bytes successfully read = %zu\n",count);
	pr_info("Updated file position = %lld\n",*f_pos);

	/*Return number of bytes which have been successfully read */
	return count;
}

ssize_t drv_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos)
{
	pr_info("Write requested for %zu bytes\n",count);
	pr_info("Current file position = %lld\n",*f_pos);

	
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


static const struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = drv_open,
    .release = drv_release,
    .read    = drv_read,
    .write   = drv_write,
    .llseek  = drv_llseek 
};

/*Module's init entry point */
static int __init cdrv_init(void)
{
    int ret;
    pr_info("New char driver initialized\n");

    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        pr_err("Failed to allocate char device region\n");
        goto out;
    }
    pr_info("Registered Major: %d, Minor: %d\n", MAJOR(dev_num), MINOR(dev_num));

    cdev_init(&my_cdev, &fops);
    my_cdev.owner = THIS_MODULE;

    ret = cdev_add(&my_cdev, dev_num, 1);
    if (ret < 0) {
        unregister_chrdev_region(dev_num, 1);
        pr_err("Failed to add cdev\n");
        goto unreg_chrdev;
    }

	class_mychar = class_create(THIS_MODULE,"my_char_class");
	if(IS_ERR(class_mychar)){
		pr_err("Class creation failed\n");
		ret = PTR_ERR(class_mychar);
		goto cdev_del;
	}

    device_mychar = device_create(class_mychar,NULL,dev_num,NULL,"my_char_device");
	if(IS_ERR(device_mychar)){
		pr_err("Device create failed\n");
		ret = PTR_ERR(device_mychar);
		goto class_del;
	}

    pr_info("Simple Char Driver loaded successfully\n");
    return 0;

class_del:
	class_destroy(class_mychar);
cdev_del:
	cdev_del(&my_cdev);	
unreg_chrdev:
	unregister_chrdev_region(dev_num,1);
out:
	pr_info("Module insertion failed\n");
	return ret;
}

/*Module's cleanup entry point */
static void __exit cdrv_exit(void)
{
  pr_info("Char driver cleaned up\n");
  	device_destroy(class_mychar,dev_num);
	class_destroy(class_mychar);
	cdev_del(&my_cdev);
	unregister_chrdev_region(dev_num,1);
	pr_info("module unloaded\n");
}

module_init(cdrv_init);
module_exit(cdrv_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Thuy");
MODULE_DESCRIPTION("A basic character driver kernel module");
MODULE_INFO(board,"Beaglebone black REV A5");
