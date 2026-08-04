#include <linux/module.h> 

/* This is module initialization entry point */
static int  __init my_kernel_module_init(void)
{
    pr_info(KERN_INFO "Hello world!\n");
    return 0;
}

/* This is module clean-up entry point */
static void  __exit my_kernel_module_exit(void)
{
    pr_info(KERN_INFO "Goodbye\n");
}

/* This is registration of above entry point with kernel */
module_init(my_kernel_module_init);
module_exit(my_kernel_module_exit);

/* This is decriptive information about the module */
MODULE_LICENSE("GPL"); 
MODULE_AUTHOR("Thuy");
MODULE_INFO(board, "beaglebone black");
MODULE_DESCRIPTION("A kernel module to print some messanges");  
MODULE_VERSION("v1.1");