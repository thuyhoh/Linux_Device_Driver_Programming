#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(.gnu.linkonce.this_module) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section(__versions) = {
	{ 0x63021283, "module_layout" },
	{ 0xe8a32258, "class_destroy" },
	{ 0x98c1e16f, "platform_driver_unregister" },
	{ 0x9d74ee2b, "__platform_driver_register" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x22a3332f, "__class_create" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x39fe5aac, "_dev_err" },
	{ 0x5424fab2, "device_create" },
	{ 0xb5641be2, "cdev_add" },
	{ 0x41c534c3, "cdev_init" },
	{ 0xdc259c7e, "of_match_device" },
	{ 0xd748315c, "of_property_read_variable_u32_array" },
	{ 0x74b60db8, "of_property_read_string" },
	{ 0x7c609794, "devm_kmalloc" },
	{ 0x51a910c0, "arm_copy_to_user" },
	{ 0xb39c9c8d, "_dev_info" },
	{ 0xb900ee69, "cdev_del" },
	{ 0xbd7ac7ac, "device_destroy" },
	{ 0x2cfde9a2, "warn_slowpath_fmt" },
	{ 0x5f754e5a, "memset" },
	{ 0xae353d77, "arm_copy_from_user" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0xc5850110, "printk" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
};

MODULE_INFO(depends, "");

