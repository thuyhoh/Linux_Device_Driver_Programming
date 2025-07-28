/**
 * @brief       GPIO sysfs driver
 * 
 * @author      
 * 
 */
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define DRIVE_AUTHOR ""
#define DRIVE_DESC   ""

struct m_foo_dev {
    struct koject *kobj_ref;
} mdev;

/********** Function Protoypes ********* */
static int __init hello_world_init(void);
static void __exit hello_world_exit(void);

/********** Sysfs function ********* */
static ssize_t sysfs_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
static ssize_t sysfs_store(struct kobject *kobj, struct kobj_attribute *attr, char *buf, size_t count);

static kobj_attribute m_attr = __ATTR(direction, 0660, sysfs_show, sysfs_store);

static ssize_t sysfs_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    pr_info("Sysfs read ...\n");
    return 0;
}
static ssize_t sysfs_store(struct kobject *kobj, struct kobj_attribute *attr, char *buf, size_t count)
{
    pr_info("Sysfs write ...\n");
    return 0;
}

static int 
__init hello_world_init(void) /* Constructor */
{
    /* 01. It will create a directory under "/sys" , [firmware_kobj, class_kobj] */
    mdev.kobj_ref = kobject_create_and_add("bbb_gpios", NULL);

    /* 02. Create sys entry under "/sys/bbb_gpios/" */
    if(sysfs_create_file(mdev.kobj_ref, &m_attr.attr))
    {
        pr_err("Cannot create sysfs file ...\n");
        goto rm_kobj;
    }

    pr_info("Hello! Initizliaze siccessfukky\n");
    return 0;

rm_kobj:
    kobject_put(mdev.kobj_ref);
    return -1;
}

static void 
__exit hello_world_exit(void)
{
    sysfs_remove_file(mdev.kobj_ref, &m_attr.attr);
    kobject_put(mdev.kobj_ref);

    pr_info("Good bye my fen");
}


